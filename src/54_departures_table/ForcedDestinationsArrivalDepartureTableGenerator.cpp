
/** ForcedDestinationsArrivalDepartureTableGenerator class implementation.
	@file ForcedDestinationsArrivalDepartureTableGenerator.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "PhysicalStop.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "LineStop.h"
#include "Line.h"
#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "StandardArrivalDepartureTableGenerator.h"
#include "GraphConstants.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace env;
	using namespace time;
	using namespace graph;

	namespace departurestable
	{

		ForcedDestinationsArrivalDepartureTableGenerator::ForcedDestinationsArrivalDepartureTableGenerator(
			const PhysicalStops& physicalStops
			, const DeparturesTableDirection& direction
			, const EndFilter& endfilter
			, const LineFilter& lineFilter
			, const DisplayedPlacesList& displayedPlacesList
			, const ForbiddenPlacesList& forbiddenPlaces,
			const TransferDestinationsList& transferDestinations
			, const DateTime& startTime
			, const DateTime& endDateTime
			, size_t maxSize
			, const ForcedDestinationsSet& forcedDestinations
			, int persistanceDuration
		):	ArrivalDepartureTableGenerator(
				physicalStops,
				direction,
				endfilter,
				lineFilter,
				displayedPlacesList,
				forbiddenPlaces,
				transferDestinations,
				startTime,
				endDateTime,
				maxSize
			),
			_forcedDestinations(forcedDestinations),
			_persistanceDuration(persistanceDuration)
		{
			// Add terminuses to forced destinations
			BOOST_FOREACH(const PhysicalStops::value_type& it, _physicalStops)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
				{
					const LineStop* ls = static_cast<const LineStop*>(edge.second);

					if (!_allowedLineStop(ls))
						continue;

					const PublicTransportStopZoneConnectionPlace* place(
						ls->getLine()->getDestination()->getConnectionPlace()
					);
					_forcedDestinations.insert(make_pair(place->getKey(), place));
				}
			}
		}




		const ArrivalDepartureList& ForcedDestinationsArrivalDepartureTableGenerator::generate()
		{
			if(_physicalStops.empty()) return _result;

			/** - Search of best departure for each forced destination */
			typedef map<const PublicTransportStopZoneConnectionPlace*, ArrivalDepartureList::iterator> ReachedDestinationMap;
			ReachedDestinationMap reachedDestination;

			const PublicTransportStopZoneConnectionPlace::PhysicalStops& physicalStops(_physicalStops.begin()->second->getConnectionPlace()->getPhysicalStops());
			
			BOOST_FOREACH(const PhysicalStops::value_type& it, physicalStops)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
				{
					const LineStop* ls = static_cast<const LineStop*>(edge.second);

					// Selection of the line
					if (!_allowedLineStop(ls))
						continue;

					// Max time for forced destination
					DateTime maxTimeForForcedDestination(_startDateTime);
					maxTimeForForcedDestination += _persistanceDuration;
					DateTime minTimeForForcedDestination(_startDateTime);
					ServicePointer serviceInstance;

					while(true)
					{
						// Next service
						optional<Edge::DepartureServiceIndex::Value> minIndex;
						serviceInstance = ls->getNextService(
							USER_PEDESTRIAN,
							_startDateTime,
							minTimeForForcedDestination,
							false,
							minIndex
						);

						if(	serviceInstance.getService() == NULL ||
							_physicalStops.find(serviceInstance.getRealTimeVertex()->getKey()) != _physicalStops.end()
						){
							break;
						}

						++*minIndex;
						minTimeForForcedDestination = serviceInstance.getActualDateTime();
					}

					if (serviceInstance.getService() == NULL)
						continue;

					bool insertionIsDone = false;

					// Exploration of the line
					for(const LineStop* curGLA(
							static_cast<const LineStop*>(ls->getFollowingArrivalForFineSteppingOnly())
						);
						curGLA != NULL;
						curGLA = static_cast<const LineStop*>(curGLA->getFollowingArrivalForFineSteppingOnly())
					){
						const PublicTransportStopZoneConnectionPlace* connectionPlace(
							curGLA->getPhysicalStop()->getConnectionPlace()
						);

						// Attempting to select the destination
						if (_forcedDestinations.find(connectionPlace->getKey()) == _forcedDestinations.end())
							continue;

						// If first reach
						if (reachedDestination.find(connectionPlace) == reachedDestination.end())
						{
							// Allocation
							ArrivalDepartureList::iterator itr = _insert(serviceInstance, FORCE_UNLIMITED_SIZE);

							// Links
							reachedDestination[curGLA->getPhysicalStop()->getConnectionPlace()] = itr;
						}
						// Else optimizing a previously founded ptd
						else if(serviceInstance.getActualDateTime() <
							reachedDestination[connectionPlace]->first.getActualDateTime()
						){
							// Allocation
							ArrivalDepartureList::iterator itr = _insert(serviceInstance, FORCE_UNLIMITED_SIZE);
							ArrivalDepartureList::iterator oldIt = reachedDestination[connectionPlace];

							reachedDestination[curGLA->getPhysicalStop()->getConnectionPlace()] = itr;

								// If the preceding ptd is not used for an other place, deletion
							ReachedDestinationMap::iterator it;
							for (it	= reachedDestination.begin();
								it != reachedDestination.end(); ++it)
								if (it->second == oldIt)
									break;
							if (it == reachedDestination.end())
								_result.erase(oldIt);
						}
					}
				}
			}

			// In case of incomplete departure table, the serie is filled with normal algorithm
			if (_result.size() < _maxSize)
			{
				StandardArrivalDepartureTableGenerator standardTable(_physicalStops, _direction, _endFilter
					, _lineFilter, DisplayedPlacesList(), _forbiddenPlaces, _transferDestinations, _startDateTime, _endDateTime, _maxSize + _result.size());
				const ArrivalDepartureList& standardTableResult = standardTable.generate();

				for (ArrivalDepartureList::const_iterator itr = standardTableResult.begin();
					_result.size() < _maxSize && itr != standardTableResult.end(); ++itr)
				{
					if (_result.find(itr->first) == _result.end())
						_insert(itr->first);
				}
			}


			if (_result.size() > _maxSize)
			{
			    ArrivalDepartureList::iterator itr = _result.begin ();
			    std::advance(itr, _maxSize);
			    _result.erase (itr, _result.end ());
			}


			return _result;

		}

	}
}
