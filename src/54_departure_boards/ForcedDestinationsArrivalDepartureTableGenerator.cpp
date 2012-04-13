
/** ForcedDestinationsArrivalDepartureTableGenerator class implementation.
	@file ForcedDestinationsArrivalDepartureTableGenerator.cpp

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

#include "StopPoint.hpp"
#include "StopArea.hpp"
#include "LinePhysicalStop.hpp"
#include "JourneyPattern.hpp"
#include "ForcedDestinationsArrivalDepartureTableGenerator.h"
#include "StandardArrivalDepartureTableGenerator.h"
#include "GraphConstants.h"
#include "Service.h"
#include "AccessParameters.h"

#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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

		ForcedDestinationsArrivalDepartureTableGenerator::ForcedDestinationsArrivalDepartureTableGenerator(
			const PhysicalStops& physicalStops,
			const DeparturesTableDirection& direction,
			const EndFilter& endfilter,
			const LineFilter& lineFilter,
			const DisplayedPlacesList& displayedPlacesList,
			const ForbiddenPlacesList& forbiddenPlaces,
			const ptime& startTime,
			const ptime& endDateTime,
			size_t maxSize,
			const ForcedDestinationsSet& forcedDestinations,
			time_duration persistanceDuration,
			bool allowCanceled
		):	ArrivalDepartureTableGenerator(
				physicalStops,
				direction,
				endfilter,
				lineFilter,
				displayedPlacesList,
				forbiddenPlaces,
				startTime,
				endDateTime,
				allowCanceled,
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
					const LinePhysicalStop& ls = static_cast<const LinePhysicalStop&>(*edge.second);

					if (!_allowedLineStop(ls))
						continue;

					const StopArea* place(
						ls.getLine()->getDestination()->getConnectionPlace()
					);
					_forcedDestinations.insert(make_pair(place->getKey(), place));
				}
			}
		}




		const ArrivalDepartureList& ForcedDestinationsArrivalDepartureTableGenerator::generate()
		{
			if(_physicalStops.empty()) return _result;

			/** - Search of best departure for each forced destination */
			typedef map<const StopArea*, ArrivalDepartureList::iterator> ReachedDestinationMap;
			ReachedDestinationMap reachedDestination;

			const StopArea::PhysicalStops& physicalStops(_physicalStops.begin()->second->getConnectionPlace()->getPhysicalStops());

			BOOST_FOREACH(const PhysicalStops::value_type& it, physicalStops)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}

					const LinePhysicalStop& ls = static_cast<const LinePhysicalStop&>(*edge.second);

					// Line filter compliance
					if (!_allowedLineStop(ls) || !ls.getFollowingArrivalForFineSteppingOnly())
					{
						continue;
					}

					// Max time for forced destination
					ptime maxTimeForForcedDestination(_startDateTime);
					maxTimeForForcedDestination += _persistanceDuration;
					ptime minTimeForForcedDestination(_startDateTime);
					ServicePointer serviceInstance;

					optional<Edge::DepartureServiceIndex::Value> minIndex;

					// Loop on services while all arrival stops are not reached
					set<const Edge*> nonServedEdges;
					AccessParameters ap;
					while(true)
					{
						// Next service
						AccessParameters ap(USER_PEDESTRIAN);
						serviceInstance = ls.getNextService(
							ap,
							minTimeForForcedDestination,
							maxTimeForForcedDestination,
							false,
							minIndex,
							false,
							false,
							_allowCanceled
						);

						// If no next service was found, quits the current journey pattern
						if(	serviceInstance.getService() == NULL)
						{
							break;
						}

						++*minIndex;
						minTimeForForcedDestination = serviceInstance.getDepartureDateTime();

						// If real time departure stop is forbidden, go to next service
						if(	_physicalStops.find(serviceInstance.getRealTimeDepartureVertex()->getKey()) == _physicalStops.end()
						){
							continue;
						}

						bool nonServedEdgesSearch(!nonServedEdges.empty());

						// Loop on the served edges
						for(const LinePhysicalStop* curGLA(
								static_cast<const LinePhysicalStop*>(ls.getFollowingArrivalForFineSteppingOnly())
							);
							curGLA != NULL;
							curGLA = static_cast<const LinePhysicalStop*>(curGLA->getFollowingArrivalForFineSteppingOnly())
						){
							// If the path traversal is only to find non served edges, analyse it only if
							// it belongs to the list
							if(nonServedEdgesSearch)
							{
								set<const Edge*>::iterator it(nonServedEdges.find(curGLA));
								if(it == nonServedEdges.end())
								{
									continue;
								}
								nonServedEdges.erase(it);
							}

							// Checks if the service calls at the arrival line stop
							ServicePointer completed(serviceInstance, *curGLA, ap);
							if(	completed.isUseRuleCompliant(true) == UseRule::RUN_NOT_POSSIBLE)
							{
								nonServedEdges.insert(curGLA);
								continue;
							}

							const StopArea* connectionPlace(
								curGLA->getPhysicalStop()->getConnectionPlace()
							);

							// Attempting to select the destination
							if (_forcedDestinations.find(connectionPlace->getKey()) == _forcedDestinations.end())
							{
								continue;
							}

							// If first reach
							if (reachedDestination.find(connectionPlace) == reachedDestination.end())
							{
								// Allocation
								ArrivalDepartureList::iterator itr = _insert(serviceInstance, FORCE_UNLIMITED_SIZE);

								if(itr == _result.end())
								{
									continue;
								}

								// Links
								reachedDestination[curGLA->getPhysicalStop()->getConnectionPlace()] = itr;
							}
							// Else optimizing a previously found ptd
							else if(serviceInstance.getDepartureDateTime() <
								reachedDestination[connectionPlace]->first.getDepartureDateTime()
							){
								// Allocation
								ArrivalDepartureList::iterator itr = _insert(serviceInstance, FORCE_UNLIMITED_SIZE);
								if(itr == _result.end())
								{
									continue;
								}

								ArrivalDepartureList::iterator oldIt = reachedDestination[connectionPlace];

								reachedDestination[curGLA->getPhysicalStop()->getConnectionPlace()] = itr;

								// If the preceding ptd is not used for an other place, deletion
								ReachedDestinationMap::iterator it;
								for(it	= reachedDestination.begin();
									it != reachedDestination.end();
									++it
								){
									if (it->second == oldIt)
									{
										break;
									}
								}
								if (it == reachedDestination.end())
								{
									_result.erase(oldIt);
								}
							}
						}

						// Quits the loop if there is no non served edge
						if(nonServedEdges.empty())
						{
							break;
						}
					}
				}
			}

			// In case of incomplete departure table, the serie is filled with normal algorithm
			if (_result.size() < *_maxSize)
			{
				StandardArrivalDepartureTableGenerator standardTable(_physicalStops, _direction, _endFilter
					, _lineFilter, DisplayedPlacesList(), _forbiddenPlaces, _startDateTime, _endDateTime, *_maxSize + _result.size());
				const ArrivalDepartureList& standardTableResult = standardTable.generate();

				for (ArrivalDepartureList::const_iterator itr = standardTableResult.begin();
					_result.size() < *_maxSize && itr != standardTableResult.end(); ++itr)
				{
					if (_result.find(itr->first) == _result.end())
						_insert(itr->first);
				}
			}

			if (_result.size() > *_maxSize)
			{
				ArrivalDepartureList::iterator itr = _result.begin ();
				std::advance(itr, *_maxSize);
				_result.erase (itr, _result.end ());
			}

			return _result;
		}
}	}
