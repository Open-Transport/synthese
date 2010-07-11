
/** ArrivalDepartureTableGenerator class implementation.
	@file ArrivalDepartureTableGenerator.cpp

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

#include "LineStop.h"
#include "Line.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "PhysicalStop.h"
#include "Service.h"
#include "ArrivalDepartureTableGenerator.h"
#include "GraphConstants.h"
#include "PTTimeSlotRoutePlanner.h"
#include "PTRoutePlannerResult.h"

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace pt;
	using namespace graph;
	using namespace algorithm;
	using namespace ptrouteplanner;
		

	namespace departurestable
	{

		ArrivalDepartureTableGenerator::ArrivalDepartureTableGenerator(
			const ArrivalDepartureTableGenerator::PhysicalStops& physicalStops,
			const DeparturesTableDirection&			direction
				, const EndFilter&			endfilter
				, const LineFilter&			lineFilter
				, const DisplayedPlacesList&	displayedPlacesList
				, const ForbiddenPlacesList&	forbiddenPlaces,
				const TransferDestinationsList& transferDestinations
				, const ptime& startDateTime
				, const ptime& endDateTime
				, size_t maxSize
		) : _physicalStops(physicalStops), _direction(direction), _endFilter(endfilter)
			, _lineFilter(lineFilter), _displayedPlaces(displayedPlacesList), _forbiddenPlaces(forbiddenPlaces),
			_transferDestinations(transferDestinations),
			_startDateTime(startDateTime)
			, _endDateTime(endDateTime), _maxSize(maxSize)
		{}


		/** Les tests effectués sont les suivants. Ils doivent tous être positifs :
				- Ligne non explicitement interdite d'affichage sur tout tableau de départs
				- Ligne non explicitement interdite d'affichage sur le tableau de départ courant
				- Si tableau des origines seulement, ligne originaire de l'arrêt affiché sur le tableau courant
				- L'arrêt d'arrivée suivant ou la destination doivent être différent de l'arrêt courant (?? -> removed currently)
		*/
		bool ArrivalDepartureTableGenerator::_allowedLineStop(const LineStop* linestop) const
		{
			/** - If a forbidden place is served, the the line is not allowed */
			if (!_forbiddenPlaces.empty())
				for(const LineStop* curLinestop(linestop);
					curLinestop != NULL;
					curLinestop = static_cast<const LineStop*>(curLinestop->getFollowingArrivalForFineSteppingOnly())
				){
					if(	_forbiddenPlaces.find(curLinestop->getPhysicalStop()->getConnectionPlace()->getKey()) !=
						_forbiddenPlaces.end()
					){
						return false;
					}
				}

			return 	linestop->getLine()->getUseInDepartureBoards()
				&&	_lineFilter.find(linestop->getLine()->getKey()) == _lineFilter.end()
				&&	((_endFilter == WITH_PASSING) || (linestop->getPreviousDepartureForFineSteppingOnly() == NULL))
				//&&	(((linestop->getFollowingArrival() != NULL) && (linestop->getFollowingArrival()->getConnectionPlace() != _place))
				//	|| ( linestop->getLine()->getDestination()->getConnectionPlace() != _place))
			;
		}


		ArrivalDepartureList::iterator
		ArrivalDepartureTableGenerator::_insert(
			const ServicePointer& servicePointer
			, UnlimitedSize unlimitedSize
		){
			ActualDisplayedArrivalsList arrivals;
			set<const PublicTransportStopZoneConnectionPlace*> encounteredPlaces;
			const PublicTransportStopZoneConnectionPlace* destinationPlace(
				static_cast<const Line*>(
					servicePointer.getService()->getPath()
				)->getDestination()->getConnectionPlace()
			);

			const LineStop* curLinestop(static_cast<const LineStop*>(servicePointer.getDepartureEdge()));
			const LineStop* lastLineStop(NULL);
			const PublicTransportStopZoneConnectionPlace* place(curLinestop->getPhysicalStop()->getConnectionPlace());

			// Adding of the beginning place
			arrivals.push_back(IntermediateStop(place));
			encounteredPlaces.insert(place);

			for(curLinestop = static_cast<const LineStop*>(curLinestop->getFollowingArrivalForFineSteppingOnly());
				curLinestop != NULL;
				curLinestop = static_cast<const LineStop*>(curLinestop->getFollowingArrivalForFineSteppingOnly())
			){
				if(!servicePointer.getService()->nonConcurrencyRuleOK(
						servicePointer.getOriginDateTime().date(),
						*servicePointer.getDepartureEdge(),
						*curLinestop,
						USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET
				)	) continue;

				place = curLinestop->getPhysicalStop()->getConnectionPlace();
				lastLineStop = curLinestop;
				
				// If the place must be displayed according to the display rules (only once per place)
				if(	_displayedPlaces.find(place->getKey()) != _displayedPlaces.end() &&
					encounteredPlaces.find(place) == encounteredPlaces.end() &&
					place != destinationPlace
				){
					ServicePointer completed(servicePointer, *curLinestop);
					_push_back(arrivals, completed);
					encounteredPlaces.insert(place);
				}
			}
			// Add the ending stop
			if((arrivals.end()-1)->place != place || arrivals.size() <= 1)
			{
				_push_back(arrivals, ServicePointer(servicePointer, *lastLineStop));
			}

			/** - Insertion */
			pair<ArrivalDepartureList::iterator, bool> insertResult = _result.insert(make_pair(servicePointer, arrivals));

			/** - Control of size : if too long, deletion of the last element */
			if (unlimitedSize == SIZE_AS_DEFINED && _maxSize != UNLIMITED_SIZE && _result.size() > _maxSize)
			{
				_result.erase(--_result.end());
			}
			
			return insertResult.first;
		}



		void ArrivalDepartureTableGenerator::_push_back(
			ActualDisplayedArrivalsList& list,
			const graph::ServicePointer& serviceUse
		){
			IntermediateStop::TransferDestinations transfers;

			TransferDestinationsList::const_iterator it(
				_transferDestinations.find(
					dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(serviceUse.getArrivalEdge()->getFromVertex()->getHub())
			)	);
			if(it != _transferDestinations.end())
			{
				ptime routePlanningEndTime(_startDateTime);
				routePlanningEndTime += days(1);
				BOOST_FOREACH(const TransferDestinationsList::mapped_type::value_type& it2, it->second)
				{
					PTTimeSlotRoutePlanner rp(
						dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(serviceUse.getDepartureEdge()->getFromVertex()->getHub()),
						it2,
						_startDateTime,
						_endDateTime,
						_startDateTime,
						routePlanningEndTime,
						1,
						AccessParameters(
							USER_PEDESTRIAN,
							false,
							false,
							0,
							posix_time::minutes(0),
							67,
							2
						),
						DEPARTURE_FIRST
					);
					
					const PTRoutePlannerResult solution(rp.run());

					if(solution.getJourneys().empty()) continue;

					const Journey& journey(solution.getJourneys().front());

					if(	journey.size() == 2 &&
						journey.getJourneyLeg(0).getArrivalEdge()->getFromVertex()->getHub() == serviceUse.getArrivalEdge()->getFromVertex()->getHub() &&
						journey.getJourneyLeg(0).getService() == serviceUse.getService()
					){
						transfers.insert(journey.getJourneyLeg(1));
					}
				}
			}
			list.push_back(IntermediateStop(
				dynamic_cast<const PublicTransportStopZoneConnectionPlace*>(serviceUse.getArrivalEdge()->getFromVertex()->getHub()),
				serviceUse,
				transfers
			)	);
		}
	}
}
