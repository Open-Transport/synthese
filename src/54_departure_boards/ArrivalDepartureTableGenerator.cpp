
/** ArrivalDepartureTableGenerator class implementation.
	@file ArrivalDepartureTableGenerator.cpp

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

#include "LinePhysicalStop.hpp"
#include "JourneyPattern.hpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Service.h"
#include "ArrivalDepartureTableGenerator.h"
#include "GraphConstants.h"
#include "AccessParameters.h"
#include "PTUseRule.h"

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


	namespace departure_boards
	{

		ArrivalDepartureTableGenerator::ArrivalDepartureTableGenerator(
			const ArrivalDepartureTableGenerator::PhysicalStops& physicalStops,
			const DeparturesTableDirection&			direction,
			const EndFilter&			endfilter,
			const LineFilter&			lineFilter,
			const DisplayedPlacesList&	displayedPlacesList,
			const ForbiddenPlacesList&	forbiddenPlaces,
			const ptime& startDateTime,
			const ptime& endDateTime,
			bool allowCanceled,
			optional<size_t> maxSize,
			bool endDateTimeConcernsTheorical
		) : _displayedPlaces(displayedPlacesList),
			_physicalStops(physicalStops),
			_direction(direction),
			_endFilter(endfilter),
			_lineFilter(lineFilter),
			_forbiddenPlaces(forbiddenPlaces),
			_startDateTime(startDateTime),
			_endDateTime(endDateTime),
			_allowCanceled(allowCanceled),
			_maxSize(maxSize),
			_endDateTimeConcernsTheorical(endDateTimeConcernsTheorical)
		{}


		/** Les tests effectués sont les suivants. Ils doivent tous être positifs :
				- Ligne non explicitement interdite d'affichage sur tout tableau de départs
				- Ligne non explicitement interdite d'affichage sur le tableau de départ courant
				- Si tableau des origines seulement, ligne originaire de l'arrêt affiché sur le tableau courant
				- L'arrêt d'arrivée suivant ou la destination doivent être différent de l'arrêt courant (?? -> removed currently)
		*/
		bool ArrivalDepartureTableGenerator::_allowedLineStop(const LinePhysicalStop& linestop) const
		{
			/** - If a forbidden place is served, the the line is not allowed */
			if (!_forbiddenPlaces.empty())
				for(const LinePhysicalStop* curLinestop(&linestop);
					curLinestop != NULL;
					curLinestop = static_cast<const LinePhysicalStop*>(curLinestop->getFollowingArrivalForFineSteppingOnly())
				){
					if(	_forbiddenPlaces.find(curLinestop->getPhysicalStop()->getConnectionPlace()->getKey()) !=
						_forbiddenPlaces.end()
					){
						return false;
					}
				}

			// Tests if the line is forbidden in departure boards according to tue uUse rule
			const UseRule& useRule(linestop.getJourneyPattern()->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
			if(dynamic_cast<const PTUseRule*>(&useRule) && static_cast<const PTUseRule&>(useRule).getForbiddenInDepartureBoards())
			{
				return false;
			}

			// Line filter : if non empty, select only lines present in the filter
			if(!_lineFilter.empty())
			{
				LineFilter::const_iterator it(_lineFilter.find(linestop.getJourneyPattern()->getCommercialLine()));

				// Line was not found
				if(it == _lineFilter.end())
				{
					return false;
				}

				// Way back filter
				if(it->second && *it->second != linestop.getJourneyPattern()->getWayBack())
				{
					return false;
				}
			}

			return
				((_endFilter == WITH_PASSING) || (linestop.getPreviousDepartureForFineSteppingOnly() == NULL))
				//&&	(((linestop.getFollowingArrival() != NULL) && (linestop.getFollowingArrival()->getConnectionPlace() != _place))
				//	|| ( linestop.getLine()->getDestination()->getConnectionPlace() != _place))
			;
		}


		ArrivalDepartureList::iterator
		ArrivalDepartureTableGenerator::_insert(
			const ServicePointer& servicePointer
			, UnlimitedSize unlimitedSize
		){
			ActualDisplayedArrivalsList arrivals;
			set<const StopArea*> encounteredPlaces;
			const StopArea* destinationPlace(
				static_cast<const JourneyPattern*>(
					servicePointer.getService()->getPath()
				)->getDestination()->getConnectionPlace()
			);

			const LinePhysicalStop* curLinestop(static_cast<const LinePhysicalStop*>(servicePointer.getDepartureEdge()));
			const LinePhysicalStop* lastLineStop(NULL);
			const StopArea* place(curLinestop->getPhysicalStop()->getConnectionPlace());

			// Adding of the beginning place
			arrivals.push_back(IntermediateStop(place, servicePointer));
			encounteredPlaces.insert(place);
			AccessParameters ap;

			for(curLinestop = static_cast<const LinePhysicalStop*>(curLinestop->getFollowingArrivalForFineSteppingOnly());
				curLinestop != NULL;
				curLinestop = static_cast<const LinePhysicalStop*>(curLinestop->getFollowingArrivalForFineSteppingOnly())
			){
				ServicePointer completed(servicePointer, *curLinestop, ap);

				// Checks if the service calls at the arrival stop
				if(	completed.isUseRuleCompliant(true) == UseRule::RUN_NOT_POSSIBLE
				){
					continue;
				}

				place = curLinestop->getPhysicalStop()->getConnectionPlace();
				lastLineStop = curLinestop;

				// If the place must be displayed according to the display rules (only once per place)
				if(	_displayedPlaces.find(place->getKey()) != _displayedPlaces.end() &&
					encounteredPlaces.find(place) == encounteredPlaces.end() &&
					place != destinationPlace
				){
					_push_back(arrivals, completed);
					encounteredPlaces.insert(place);
				}
			}

			if(!lastLineStop)
			{
				return _result.end();
			}

			// Add the ending stop
			if((arrivals.end()-1)->place != place || arrivals.size() <= 1)
			{
				_push_back(arrivals, ServicePointer(servicePointer, *lastLineStop, ap));
			}

			/** - Insertion */
			pair<ArrivalDepartureList::iterator, bool> insertResult = _result.insert(make_pair(servicePointer, arrivals));

			/** - Control of size : if too long, deletion of the last element */
			if (unlimitedSize == SIZE_AS_DEFINED && _maxSize && _result.size() > *_maxSize)
			{
				_result.erase(--_result.end());
			}

			return insertResult.first;
		}



		void ArrivalDepartureTableGenerator::_push_back(
			ActualDisplayedArrivalsList& list,
			const graph::ServicePointer& serviceUse
		){
			list.push_back(IntermediateStop(
				dynamic_cast<const StopArea*>(serviceUse.getArrivalEdge()->getFromVertex()->getHub()),
				serviceUse
			)	);
		}
}	}
