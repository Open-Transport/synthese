
/** StandardArrivalDepartureTableGenerator class implementation.
	@file StandardArrivalDepartureTableGenerator.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "StandardArrivalDepartureTableGenerator.h"

#include "AccessParameters.h"
#include "LinePhysicalStop.hpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "GraphConstants.h"
#include "CommercialLine.h"
#include "JourneyPattern.hpp"
#include "Destination.hpp"
#include "Service.h"
#include "Log.h"

#ifdef WITH_SCOM
	#include "SCOMModule.h"
	#include "SCOMData.h"
#endif

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace pt;
	using namespace graph;
	using namespace pt;

	namespace departure_boards
	{

		StandardArrivalDepartureTableGenerator::StandardArrivalDepartureTableGenerator(
			const PhysicalStops& physicalStops,
			const DeparturesTableDirection& direction,
			const EndFilter& endfilter,
			const LineFilter& lineFilter,
			const DisplayedPlacesList& displayedPlacesList,
			const ForbiddenPlacesList& forbiddenPlaces,
			const ptime& startTime,
			const ptime& endDateTime,
			bool allowCanceled,
			optional<size_t> maxSize,
			bool endDateTimeConcernsTheorical
		):	ArrivalDepartureTableGenerator(
			physicalStops, direction, endfilter, lineFilter,
			displayedPlacesList, forbiddenPlaces, startTime, endDateTime, allowCanceled, maxSize, endDateTimeConcernsTheorical
		){
			_scom = false;
		}


		// Save them
		void StandardArrivalDepartureTableGenerator::setClient (
			bool useScom,
			const std::string& borne
		)
		{
			_scom = useScom;
			_borne = borne;
		}



		const ArrivalDepartureList& StandardArrivalDepartureTableGenerator::generate()
		{
			// If no stop in the current stop area, return empty result
			if(_physicalStops.empty())
			{
				return _result;
			}

			// If scom exists and is used, substract the maximum matching delay
			// Why? To enable past service to be selected that might be delayed and so adapted by SCOM
			// See the SCOM module documentation
			ptime realStartDateTime(_startDateTime);
			#ifdef WITH_SCOM
			if (_scom)
			{
				realStartDateTime -= seconds(scom::SCOMModule::GetSCOMData()->MaxTimeDiff());
			}
			#endif

			AccessParameters ap;

			// Loop on the stops
			BOOST_FOREACH(PhysicalStops::value_type it, _physicalStops)
			{
				// Loop on journey patterns calling at the stop
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					const LinePhysicalStop& ls = static_cast<const LinePhysicalStop&>(*edge.second);

					// Checks if the line stop is allowed according to the generator parameters
					if (!_allowedLineStop(ls) || !ls.getFollowingArrivalForFineSteppingOnly())
					{
						continue;
					}

					BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, ls.getParentPath()->getServiceCollections())
					{
						// Loop on services
						ptime departureDateTime = realStartDateTime;
						optional<Edge::DepartureServiceIndex::Value> index;
						size_t insertedServices(0);
						while(true)
						{
							// Tells to the journey pattern for a next service
							ServicePointer servicePointer(
								ls.getNextService(
									*itCollection,
									ap,
									departureDateTime,
									_endDateTime,
									false,
									index,
									false,
									false,
									_allowCanceled,
									true,
									true,
									UseRule::RESERVATION_INTERNAL_DELAY,
									_endDateTimeConcernsTheorical
							)	);

							// If no next service was found, then abort the search in the current journey pattern
							if(	!servicePointer.getService())
							{
								break;
							}

							// Saves local variables
							++*index;
							departureDateTime = servicePointer.getDepartureDateTime();

							// Checks if the stop area is really served and if the served stop is allowed
							if(	_physicalStops.find(servicePointer.getRealTimeDepartureVertex()->getKey()) == _physicalStops.end()
							){
								continue;
							}

							// Adapt the departure time using SCOM (if enabled)
							#ifdef WITH_SCOM
							if (_scom)
							{
								const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(servicePointer.getService()->getPath());

								// Fetch the time from SCOM
								ptime adaptedTime = scom::SCOMModule::GetSCOMData()->GetWaitingTime(
										_borne,
										journeyPattern->getCommercialLine()->getShortName(),
										journeyPattern->getDirectionObj()->getDisplayedText(),
										servicePointer.getDepartureDateTime(),
										_startDateTime
								);

								// Checks if the new time is after the _startDateTime, if not ignore this line
								if (adaptedTime < _startDateTime)
								{
									continue;
								}
							}
							#endif

							// The departure is kept in the results
							_insert(servicePointer);

							// Checks if the maximal number of results is reached
							++insertedServices;
							if(	_maxSize && insertedServices >= *_maxSize)
							{
								break;
							}
						}
					}						
				}
			}
			return _result;
		}
}	}
