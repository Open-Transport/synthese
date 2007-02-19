
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

#include "15_env/PhysicalStop.h"
#include "15_env/LineStop.h"
#include "15_env/Line.h"
#include "15_env/Edge.h"

#include "34_departures_table/ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "34_departures_table/StandardArrivalDepartureTableGenerator.h"

using namespace std;

namespace synthese
{
	using namespace env;
	using namespace time;

	namespace departurestable
	{

		ForcedDestinationsArrivalDepartureTableGenerator::ForcedDestinationsArrivalDepartureTableGenerator(
			const PhysicalStopsSet& physicalStops
			, const DeparturesTableDirection& direction
			, const EndFilter& endfilter
			, const LineFilter& lineFilter
			, const DisplayedPlacesList& displayedPlacesList
			, const ForbiddenPlacesList& forbiddenPlaces
			, const DateTime& startTime
			, const DateTime& endDateTime
			, size_t maxSize
			, const ForcedDestinationsSet& forcedDestinations
			, int persistanceDuration
			, int blinkinkDelay
		) : ArrivalDepartureTableGenerator(physicalStops, direction, endfilter, lineFilter
										, displayedPlacesList, forbiddenPlaces, startTime, endDateTime, blinkingDelay, maxSize)
			, _forcedDestinations(forcedDestinations)
			, _persistanceDuration(persistanceDuration)
		{
			// Add terminuses to forced destinations
			for (PhysicalStopsSet::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
			{
				for (set<const Edge*>::const_iterator eit = (*it)->getDepartureEdges().begin(); eit != (*it)->getDepartureEdges().end(); ++eit)
				{
					const LineStop* ls = (const LineStop*) (*eit);

					if (!_allowedLineStop(ls))
						continue;

					_forcedDestinations.insert(ls->getLine()->getDestination()->getConnectionPlace());
					_displayedPlaces.insert(ls->getLine()->getDestination()->getConnectionPlace());
				}
			}
		}




		const ArrivalDepartureList& ForcedDestinationsArrivalDepartureTableGenerator::generate()
		{
			/** - Search of best departure for each forced destination */
			typedef map<const ConnectionPlace*, ArrivalDepartureList::iterator> ReachedDestinationMap;
			ReachedDestinationMap reachedDestination;
			
			for (PhysicalStopsSet::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
			{
				for (set<const Edge*>::const_iterator eit = (*it)->getDepartureEdges().begin(); eit != (*it)->getDepartureEdges().end(); ++eit)
				{
					const LineStop* ls = (const LineStop*) (*eit);

					// Selection of the line
					if (!_allowedLineStop(ls))
						continue;

					// Next service
					DateTime tempStartDateTime = _startDateTime;
					
					// Max time for forced destination
					DateTime maxTimeForForcedDestination(_startDateTime);
					maxTimeForForcedDestination += _persistanceDuration;

					int serviceNumber = ls->getNextService(tempStartDateTime, maxTimeForForcedDestination, _startDateTime);
					
					// No service
					if (serviceNumber == UNKNOWN_VALUE)
						continue;

					bool insertionIsDone = false;

					// Exploration of the line
					for (const LineStop* curGLA = (const LineStop*) ls->getFollowingArrivalForFineSteppingOnly(); curGLA != NULL; curGLA = (const LineStop*) curGLA->getFollowingArrivalForFineSteppingOnly())
					{
						// Attempting to select the destination
						if (_forcedDestinations.find(curGLA->getConnectionPlace()) == _forcedDestinations.end())
							continue;

						// If first reach
						if (reachedDestination.find(curGLA->getConnectionPlace()) == reachedDestination.end())
						{
							// Allocation
							ArrivalDepartureList::iterator itr = _insert(ls, serviceNumber, tempStartDateTime, FORCE_UNLIMITED_SIZE);

							// Links
							reachedDestination[curGLA->getConnectionPlace()] = itr;
						}
						// Else optimizing a previously founded ptd
						else if (tempStartDateTime < reachedDestination[curGLA->getConnectionPlace()]->first.realDepartureTime)
						{
							// Allocation
							ArrivalDepartureList::iterator itr = _insert(ls, serviceNumber, tempStartDateTime, FORCE_UNLIMITED_SIZE);
							ArrivalDepartureList::iterator oldIt = reachedDestination[curGLA->getConnectionPlace()];

							reachedDestination[curGLA->getConnectionPlace()] = itr;

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
					, _lineFilter, DisplayedPlacesList(), _forbiddenPlaces, _startDateTime, _endDateTime, _maxSize + _result.size());
				const ArrivalDepartureList& standardTableResult = standardTable.generate();

				for (ArrivalDepartureList::const_iterator itr = standardTableResult.begin();
					_result.size() < _maxSize && itr != standardTableResult.end(); ++itr)
				{
					if (_result.find(itr->first) == _result.end())
						_insert(itr->first.linestop, itr->first.serviceNumber, itr->first.realDepartureTime);
				}
			}

			return _result;

		}

	}
}
