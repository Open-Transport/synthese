
/** RoutePlannerLogger class implementation.
	@file RoutePlannerLogger.cpp

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

#include "RoutePlannerLogger.h"
#include "ResultHTMLTable.h"
#include "NamedPlace.h"

#include <boost/date_time/posix_time/posix_time_io.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace html;
	using namespace graph;
	using namespace geography;



	namespace algorithm
	{
		RoutePlannerLogger::RoutePlannerLogger(
			std::ostream& stream,
			const JourneysResult& emptyTodo,
			const RoutePlanningIntermediateJourney& result
		):	_stream(stream),
			_t(9, ResultHTMLTable::CSS_CLASS),
			_result(result),
			_searchNumber(0)
		{
		}


		void RoutePlannerLogger::recordIntegralSearch(
			const JourneysResult& todo
		){
			++_searchNumber;

			_stream << _t.row();
			_stream << _t.col(3, string(), true) << _searchNumber;
			_stream << _t.col(1, string(), true);
			_stream << _t.col(1, string(), true);
			_stream << _t.col(6, string(), true);

			_todoBeforeClean.clear();
			BOOST_FOREACH(const JourneysResult::ResultSet::value_type& it, todo.getJourneys())
			{
				_todoBeforeClean.push_back(
					it.first
				);
			}
		}


		void RoutePlannerLogger::recordCleanup(
			const JourneysResult& todo
		){

/*							// Departure time
							Journey::ServiceUses::const_iterator its(journey->getServiceUses().begin());

												if (journey->getContinuousServiceRange() > 1)
							{
							ptime endRange(its->getDepartureDateTime());
							endRange += journey->getContinuousServiceRange();
							stream << " - Service continu jusqu'à " << endRange.;
							}
							if (journey->getReservationCompliance() == true)
							{
							stream << " - Réservation obligatoire avant le " << journey->getReservationDeadLine();
							}
							if (journey->getReservationCompliance() == boost::logic::indeterminate)
							{
							stream << " - Réservation facultative avant le " << journey->getReservationDeadLine();
							}

							stream << "<tr>";
							stream << "<td>" << its->getDepartureDateTime() << "</td>";

							// JourneyPattern
							const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
							const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
							stream << "<td";
							if (ls)
								stream << " class=\"" + ls->getLine()->getCommercialLine()->getStyle() << "\"";
							stream << ">";
							stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName()) << "</td>";

							// Transfers
							if (its == journey->getServiceUses().end() -1)
							{
								stream << "<td colspan=\"4\">(trajet direct)</td>";
							}
							else
							{
								while(true)
								{
									// Arrival
									stream << "<td>" << its->getArrivalDateTime() << "</td>";

									// Place
									stream <<
										"<td>" <<
										dynamic_cast<const NamedPlace*>(its->getArrivalEdge()->getHub())->getFullName() <<
										"</td>"
										;

									// Next service use
									++its;

									// Departure
									stream << "<td>" << its->getDepartureDateTime() << "</td>";

									// JourneyPattern
									const LineStop* ls(dynamic_cast<const LineStop*>(its->getEdge()));
									const Road* road(dynamic_cast<const Road*>(its->getEdge()->getParentPath()));
									stream << "<td";
									if (ls)
										stream << " class=\"" << ls->getLine()->getCommercialLine()->getStyle() << "\"";
									stream << ">";
									stream << (ls ? ls->getLine()->getCommercialLine()->getShortName() : road->getRoadPlace()->getName()) << "</td>";

									// Exit if last service use
									if (its == journey->getServiceUses().end() -1)
										break;

									// Empty final arrival col
									stream << "<td></td>";

									// New row and empty origin departure cols;
									stream << "</tr><tr>";
									stream << "<td></td>";
									stream << "<td></td>";
								}
							}

							// Final arrival
							stream << "<td>" << its->getArrivalDateTime() << "</td>";
*/


			_stream << _t.row();
			_stream << _t.col(1, string(), true);
			_stream << _t.col(1, string(), true) << "rk";
			_stream << _t.col(1, string(), true) << "status";
			_stream << _t.col(1, string(), true) << "place";
			_stream << _t.col(1, string(), true) << "time";
			_stream << _t.col(1, string(), true) << "jyscore";
			_stream << _t.col(1, string(), true) << "dist";
//			_stream << _t.col(1, string(), true) << "minsp / dst";
			_stream << _t.col(1, string(), true) << "plscore";
//			_stream << _t.col(1, string(), true) << "minsp";
//			_stream << _t.col(1, string(), true) << "journey";

			size_t r(1);
			BOOST_FOREACH(shared_ptr<RoutePlanningIntermediateJourney> journey, _todoBeforeClean)
			{
				const Vertex* vertex(journey->getEndEdge().getFromVertex());
				_stream << _t.row();
				if (journey->empty())
				{
					_stream << _t.col(9) << "Empty fake journey";
					continue;
				}

				bool cleaned(!todo.get(vertex).get());
				Map::const_iterator it2(_lastTodo.find(vertex));

				_stream << _t.col();
				_stream << _t.col() << r++;
				_stream << _t.col();
				if(cleaned)
				{
					_stream << "cleaned";
				}
				else if(it2 != _lastTodo.end())
				{
					if(it2->second == journey)
					{
						_stream << "existing";
					}
					else
					{
						_stream << "updated";
					}
				}
				else
				{
					_stream << "new";
				}
				_stream << _t.col();
				if(dynamic_cast<const geography::NamedPlace*>(journey->getEndEdge().getHub()))
				{
					_stream << dynamic_cast<const geography::NamedPlace*>(journey->getEndEdge().getHub())->getFullName();
				}
				_stream << _t.col() << journey->getEndTime();
				_stream << _t.col() << journey->getScore();
				_stream << _t.col() << *journey->getDistanceToEnd();
//				_stream << _t.col() << (60 * (journey->getMinSpeedToEnd() ? (journey->getDistanceToEnd() / journey->getMinSpeedToEnd()) : -1));
				_stream << _t.col() << journey->getEndEdge().getHub()->getScore();
//				_stream << _t.col() << journey->getMinSpeedToEnd();
			}

			_lastTodo.clear();
			BOOST_FOREACH(const JourneysResult::ResultSet::value_type& it, todo.getJourneys())
			{
				_lastTodo.insert(make_pair(it.first->getEndEdge().getFromVertex(), it.first));
			}
		}


		void RoutePlannerLogger::recordNewResult(
			const RoutePlanningIntermediateJourney& result
		){
			_result = result;

			if(!result.empty())
			{
				_stream << _t.row();
				_stream << _t.col(9, string(), true) << "New result : ";
				if(dynamic_cast<const NamedPlace*>(result.getEndEdge().getFromVertex()->getHub()))
				{
					_stream << dynamic_cast<const NamedPlace*>(result.getEndEdge().getFromVertex()->getHub())->getFullName();
				}
				_stream << " at " << result.getEndTime();
			}
		}



		void RoutePlannerLogger::open()
		{
			_stream << _t.open();
			recordNewResult(_result);
		}



		void RoutePlannerLogger::close()
		{
			_stream << _t.close();
		}
	}
}
