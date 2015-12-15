
/** AlgorithmLogger class implementation.
	@file AlgorithmLogger.cpp

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

#include "AlgorithmLogger.hpp"

#include "CommercialLine.h" // TODO remove it
#include "JourneyPattern.hpp" // TODO remove it
#include "LinePhysicalStop.hpp" // TODO remove it
#include "ResultHTMLTable.h"
#include "Road.h" // TODO remove it
#include "RoadPath.hpp"
#include "RoadModule.h" // TODO remove it
#include "RoadPlace.h" // TODO remove it
#include "RoutePlanningIntermediateJourney.hpp"
#include "Service.h"
#include "StopArea.hpp" // TODO remove it
#include "StopPoint.hpp" // TODO remove it
#include "Vertex.h"
#include "VertexAccessMap.h"

#include <iomanip>
#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::posix_time;

namespace synthese
{
	using namespace geography; // TODO remove it
	using namespace graph;
	using namespace html;
	using namespace pt; // TODO remove it
	using namespace road; // TODO remove it

	namespace algorithm
	{
		AlgorithmLogger::AlgorithmLogger(
			path dir
		):	_directory(dir),
			_active(!dir.empty()),
			_fileNumber(0),
			_journeyPlannerStepTable(8, ResultHTMLTable::CSS_CLASS),
			_journeyPlannerSearchNumber(0),
			_journeyPlannerTable(13, ResultHTMLTable::CSS_CLASS),
			_journeyPlannerResult(NULL),
			_timeSlotJourneyPlannerStepNumber(0),
			_timeSlotJourneyPlannerTable(5, ResultHTMLTable::CSS_CLASS)
		{}



		boost::filesystem::path AlgorithmLogger::_getCurrentFilePath() const
		{
			stringstream fileName;
			fileName << "file";
			fileName << setfill('0');
			fileName << setw(10);
			fileName << _fileNumber;
			fileName << ".html";
			return path(_directory / fileName.str());
		}



		boost::shared_ptr<ofstream> AlgorithmLogger::_openNewFile() const
		{
			++_fileNumber;
			boost::shared_ptr<ofstream> result(
				new ofstream(
					_getCurrentFilePath().string().c_str()
			)	);
			return result;
		}



		void AlgorithmLogger::openIntegralSearchLog(
			PlanningPhase planningPhase,
			const boost::posix_time::ptime& desiredTime
		) const	{
			if(!_active)
			{
				return;
			}
			_stopChrono();

			_integralSearchPlanningPhase = planningPhase;
			_integralSearchDesiredTime = desiredTime;
			_integralSearchFile = _openNewFile();
			*_integralSearchFile << "<html><head><title>Integral Search File</title></head><body>";
			*_integralSearchFile << "<table class=\"adminresults\">";
			_startChrono();
		}



		void AlgorithmLogger::logIntegralSearchJourney(
			const RoutePlanningIntermediateJourney& journey
		) const {
			if(!_active)
			{
				return;
			}

			_stopChrono();

			if(journey.empty())
			{
				*_integralSearchFile << "<tr><td colspan=\"7\">Empty journey</td></tr>";
			}
			else
			{
				*_integralSearchFile
					<< "<tr>"
					<< "<th colspan=\"7\">Journey</th>"
					<< "</tr>"
					;

				// Departure time
				Journey::ServiceUses::const_iterator its(journey.getServiceUses().begin());

/*					if (journey->getContinuousServiceRange() > 1)
				{
					ptime endRange(its->getDepartureDateTime());
					endRange += journey->getContinuousServiceRange();
					*_integralSearchFile << " - Service continu jusqu'à " << endRange;
				}
				if (journey->getReservationCompliance() == true)
				{
					*_integralSearchFile << " - Réservation obligatoire avant le " << journey->getReservationDeadLine();
				}
				if (journey->getReservationCompliance() == boost::logic::indeterminate)
				{
					*_integralSearchFile << " - Réservation facultative avant le " << journey->getReservationDeadLine();
				}
*/
				*_integralSearchFile << "<tr>";
				*_integralSearchFile << "<td>";
				*_integralSearchFile << its->getDepartureDateTime();
				*_integralSearchFile << "</td>";

				// JourneyPattern
				const LinePhysicalStop* ls(
					dynamic_cast<const LinePhysicalStop*>(
						_integralSearchPlanningPhase == DEPARTURE_TO_ARRIVAL ? its->getDepartureEdge() : its->getArrivalEdge()
				)	);
				const RoadPath* road(dynamic_cast<const RoadPath*>(its->getService()->getPath()));
				*_integralSearchFile << "<td";
				if (ls)
				{
					*_integralSearchFile << " class=\"" + static_cast<JourneyPattern*>(ls->getParentPath())->getCommercialLine()->getStyle() << "\"";
				}
				*_integralSearchFile << ">";
				*_integralSearchFile << (
						ls ?
						static_cast<JourneyPattern*>(ls->getParentPath())->getCommercialLine()->getShortName() :
						road->getRoad()->getAnyRoadPlace()->getName()
					) <<
					"</td>"
				;

				// Transfers
				if (its == journey.getServiceUses().end() -1)
				{
					*_integralSearchFile << "<td colspan=\"4\">(trajet direct)</td>";
				}
				else
				{
					while(true)
					{
						// Arrival
						*_integralSearchFile << "<td>";
						*_integralSearchFile << its->getArrivalDateTime();
						*_integralSearchFile << "</td>";

						// Place
						*_integralSearchFile << "<td>";
						if(dynamic_cast<const NamedPlace*>(its->getArrivalEdge()->getHub()))
						{
							*_integralSearchFile << dynamic_cast<const NamedPlace*>(its->getArrivalEdge()->getHub())->getFullName();
						}
						*_integralSearchFile << "</td>";

						// Next service use
						++its;

						// Departure
						*_integralSearchFile << "<td>" << its->getDepartureDateTime() << "</td>";

						// JourneyPattern
						const LinePhysicalStop* ls(
							dynamic_cast<const LinePhysicalStop*>(
								_integralSearchPlanningPhase == DEPARTURE_TO_ARRIVAL ? its->getDepartureEdge() : its->getArrivalEdge()
						)	);
						const RoadPath* road(dynamic_cast<const RoadPath*>(its->getService()->getPath()));
						*_integralSearchFile << "<td";
						if (ls)
							*_integralSearchFile << " class=\"" << static_cast<JourneyPattern*>(ls->getParentPath())->getCommercialLine()->getStyle() << "\"";
						*_integralSearchFile << ">";
						*_integralSearchFile <<
							(	ls ?
								static_cast<JourneyPattern*>(ls->getParentPath())->getCommercialLine()->getShortName() :
								road->getRoad()->getAnyRoadPlace()->getName()
							) <<
							"</td>"
						;

						// Exit if last service use
						if (its == journey.getServiceUses().end() -1)
							break;

						// Empty final arrival col
						*_integralSearchFile << "<td></td>";

						// New row and empty origin departure cols;
						*_integralSearchFile << "</tr><tr>";
						*_integralSearchFile << "<td></td>";
						*_integralSearchFile << "<td></td>";
					}
				}

				// Final arrival
				*_integralSearchFile << "<td>" << its->getArrivalDateTime() << "</td>";


//					*_integralSearchFile << todo.getLog();
			}

			_startChrono();
		}



		void AlgorithmLogger::closeIntegralSearchLog() const
		{
			if(!_active)
			{
				return;
			}

			_stopChrono();

			*_integralSearchFile << "<tr><th colspan=\"7\">";
			if (_integralSearchPlanningPhase == DEPARTURE_TO_ARRIVAL)
			{
				*_integralSearchFile << "DEPARTURE_TO_ARRIVAL";
			}
			else
			{
				*_integralSearchFile << "ARRIVAL_TO_DEPARTURE   ";
			}
			*_integralSearchFile <<
				" IntegralSearch. Start "
				<< " at " << _integralSearchDesiredTime
				<< "</th></tr>"
				<< "</table></body></html>"
			;

			_integralSearchFile->close();
			_integralSearchFile.reset();

			_startChrono();
		}



		void AlgorithmLogger::openJourneyPlannerLog(
			const boost::posix_time::ptime& originDateTime,
			PlanningPhase planningPhase
		) const	{
			if(!_active)
			{
				return;
			}

			_stopChrono();

			_journeyPlanningPhase = planningPhase;
			_journeyPlannerOriginDateTime = originDateTime;
			_journeyPlannerSearchNumber = 0;
			_journeyPlannerStartChrono = _chrono;
			++_timeSlotJourneyPlannerStepNumber;
			_journeyPlannerStepStartChrono = _chrono;

			_journeyPlannerFile = _openNewFile();
			*_journeyPlannerFile << "<html><head><title>Journey Planner File</title></head><body>";
			*_journeyPlannerFile << _journeyPlannerTable.open();

			if(_timeSlotJourneyPlannerFile)
			{
				*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.row();
				*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.col() << "rank" << _timeSlotJourneyPlannerTable.col() 
				  << "direction" << _timeSlotJourneyPlannerTable.col() << "start_time" << _timeSlotJourneyPlannerTable.col() 
				  << "steps" << _timeSlotJourneyPlannerTable.col() << "duration";
				*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.row();

				*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.col() <<
					"<a href=\"" << _getCurrentFilePath().filename().string() << "\">" << _timeSlotJourneyPlannerStepNumber << "</a>";
				*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.col() << (planningPhase == DEPARTURE_TO_ARRIVAL ? "DA" : "AD");
				*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.col() << originDateTime;
			}

			_startChrono();
		}



		void AlgorithmLogger::recordJourneyPlannerLogIntegralSearch(
			boost::shared_ptr<const RoutePlanningIntermediateJourney> journey,
			const ptime& bestDateTime,
			const JourneysResult& todo
		) const {
			if(!_active || todo.empty())
			{
				return;
			}

			_stopChrono();

			++_journeyPlannerSearchNumber;

			if(_journeyPlannerStepFile.get())
			{
				_journeyPlannerStepFile->close();
			}
			_journeyPlannerStepFile = _openNewFile();
			*_journeyPlannerStepFile << "<html><head><title>Journey Planner Step File</title></head><body>";
			*_journeyPlannerStepFile << _journeyPlannerStepTable.open();

			*_journeyPlannerFile << _journeyPlannerTable.row();
			*_journeyPlannerFile << _journeyPlannerTable.col() <<
				"<a href=\"" << _getCurrentFilePath().filename().string() << "\">" <<
				_journeyPlannerSearchNumber <<
				"</a>";

			if(journey.get())
			{
				*_journeyPlannerFile << _journeyPlannerTable.col();
				if(dynamic_cast<const geography::NamedPlace*>(journey->getEndEdge().getHub()))
				{
					*_journeyPlannerFile << dynamic_cast<const geography::NamedPlace*>(journey->getEndEdge().getHub())->getFullName();
				}
				*_journeyPlannerFile << _journeyPlannerTable.col() << journey->getEndTime(false);
				*_journeyPlannerFile << _journeyPlannerTable.col() << journey->getScore();
				*_journeyPlannerFile << _journeyPlannerTable.col() << *journey->getDistanceToEnd();
				*_journeyPlannerFile << _journeyPlannerTable.col();
				if(_journeyPlanningPhase == DEPARTURE_TO_ARRIVAL)
				{
					*_journeyPlannerFile <<
						(3.6 * (*journey->getDistanceToEnd()) / (bestDateTime - journey->getEndTime()).total_seconds())
					;
				}
				else
				{
					*_journeyPlannerFile <<
						(3.6 * (*journey->getDistanceToEnd()) / (journey->getEndTime() - bestDateTime).total_seconds())
					;
				}

				*_journeyPlannerFile << _journeyPlannerTable.col() << journey->getEndEdge().getHub()->getScore();
			}
			else
			{
				*_journeyPlannerFile << _journeyPlannerTable.col() << "START";
				*_journeyPlannerFile << _journeyPlannerTable.col() << _journeyPlannerOriginDateTime;
				*_journeyPlannerFile << _journeyPlannerTable.col(4);
			}


			*_journeyPlannerFile << _journeyPlannerTable.col() << todo.getJourneys().size();

			_todoBeforeClean.clear();
			BOOST_FOREACH(const JourneysResult::ResultSet::value_type& it, todo.getJourneys())
			{
				_todoBeforeClean.push_back(
					it.first
				);
			}

			_startChrono();
		}



		void AlgorithmLogger::recordJourneyPlannerLogCleanup(
			bool resultFound,
			const ptime& bestDateTime,
			const JourneysResult& todo
		) const {
			if(!_active)
			{
				return;
			}
			_stopChrono();

			*_journeyPlannerStepFile << _journeyPlannerStepTable.row();
			*_journeyPlannerStepFile << _journeyPlannerStepTable.col(1, string(), true) << "rk";
			*_journeyPlannerStepFile << _journeyPlannerStepTable.col(1, string(), true) << "status";
			*_journeyPlannerStepFile << _journeyPlannerStepTable.col(1, string(), true) << "place";
			*_journeyPlannerStepFile << _journeyPlannerStepTable.col(1, string(), true) << "time";
			*_journeyPlannerStepFile << _journeyPlannerStepTable.col(1, string(), true) << "jyscore";
			*_journeyPlannerStepFile << _journeyPlannerStepTable.col(1, string(), true) << "dist";
			*_journeyPlannerStepFile << _journeyPlannerStepTable.col(1, string(), true) << "minsp / dst";
			*_journeyPlannerStepFile << _journeyPlannerStepTable.col(1, string(), true) << "plscore";

			size_t r(1);
			size_t cleaneds(0);
			size_t added(0);
			size_t updated(0);
			BOOST_FOREACH(const boost::shared_ptr<RoutePlanningIntermediateJourney>& journey, _todoBeforeClean)
			{
				const Vertex* vertex(journey->getEndEdge().getFromVertex());
				*_journeyPlannerStepFile << _journeyPlannerStepTable.row();
				if (journey->empty())
				{
					*_journeyPlannerStepFile << _journeyPlannerStepTable.col(8) << "Empty fake journey";
					continue;
				}

				bool cleaned(!todo.get(vertex, journey->getEndTime()).get());
				Map::const_iterator it2(_lastTodo.find(vertex));

				*_journeyPlannerStepFile << _journeyPlannerStepTable.col() << r++;
				*_journeyPlannerStepFile << _journeyPlannerStepTable.col();
				if(cleaned)
				{
					*_journeyPlannerStepFile << "cleaned";
					++cleaneds;
				}
				else if(it2 != _lastTodo.end())
				{
					if(it2->second == journey)
					{
						*_journeyPlannerStepFile << "existing";
					}
					else
					{
						*_journeyPlannerStepFile << "updated";
						++updated;
					}
				}
				else
				{
					*_journeyPlannerStepFile << "new";
					++added;
				}
				*_journeyPlannerStepFile << _journeyPlannerStepTable.col();
				if(dynamic_cast<const geography::NamedPlace*>(journey->getEndEdge().getHub()))
				{
					*_journeyPlannerStepFile << dynamic_cast<const geography::NamedPlace*>(journey->getEndEdge().getHub())->getFullName();
				}
				*_journeyPlannerStepFile << _journeyPlannerStepTable.col() << journey->getEndTime(false);
				*_journeyPlannerStepFile << _journeyPlannerStepTable.col() << journey->getScore();
				*_journeyPlannerStepFile << _journeyPlannerStepTable.col() << *journey->getDistanceToEnd();

				*_journeyPlannerStepFile << _journeyPlannerStepTable.col();
				if(_journeyPlanningPhase == DEPARTURE_TO_ARRIVAL)
				{
					*_journeyPlannerStepFile <<
						(3.6 * (*journey->getDistanceToEnd()) / (bestDateTime - journey->getEndTime()).total_seconds())
						;
				}
				else
				{
					*_journeyPlannerStepFile <<
						(3.6 * (*journey->getDistanceToEnd()) / (journey->getEndTime() - bestDateTime).total_seconds())
						;
				}

				*_journeyPlannerStepFile << _journeyPlannerStepTable.col() << journey->getEndEdge().getHub()->getScore();
			}

			_lastTodo.clear();
			BOOST_FOREACH(const JourneysResult::ResultSet::value_type& it, todo.getJourneys())
			{
				_lastTodo.insert(make_pair(it.first->getEndEdge().getFromVertex(), it.first));
			}

			*_journeyPlannerFile << _journeyPlannerTable.col() << "-" << cleaneds;
			*_journeyPlannerFile << _journeyPlannerTable.col() << "u" << updated;
			*_journeyPlannerFile << _journeyPlannerTable.col() << "+" << added;
			*_journeyPlannerFile << _journeyPlannerTable.col() << (resultFound ? "RESULT" : "");
			*_journeyPlannerFile << _journeyPlannerTable.col() << (_chrono - _journeyPlannerStepStartChrono).total_microseconds() << "μs";

			_journeyPlannerStepStartChrono = _chrono;
			_startChrono();
		}



		void AlgorithmLogger::closeJourneyPlannerLog() const
		{
			if(!_active)
			{
				return;
			}
			_stopChrono();

			time_duration jpChrono = _chrono - _journeyPlannerStartChrono;

			*_journeyPlannerFile << _journeyPlannerTable.close();
			*_journeyPlannerFile << "<p>Temps de calcul : " << jpChrono.total_microseconds() << " μs</p>";
			*_journeyPlannerFile << "</body></html>";
			_journeyPlannerFile->close();
			_journeyPlannerFile.reset();

			if(_timeSlotJourneyPlannerFile)
			{
				*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.col() << _journeyPlannerSearchNumber;
				*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.col() << jpChrono.total_microseconds() << " μs";
			}

			_startChrono();
		}



		void AlgorithmLogger::openTimeSlotJourneyPlannerLog() const
		{
			if(!_active)
			{
				return;
			}
			_stopChrono();

			if(!_timeSlotJourneyPlannerFile.get())
			{
				_timeSlotJourneyPlannerFile = _openNewFile();
				*_timeSlotJourneyPlannerFile << "<html><head><title>TimeSlot Journey Planner File</title></head><body>";
			}
			_timeSlotJourneyPlannerStepNumber = 0;

			_startChrono();
		}



		void AlgorithmLogger::logTimeSlotJourneyPlannerStep(
			const ptime& originDateTime
		) const	{
			if(!_active)
			{
				return;
			}

			_stopChrono();

			if (_timeSlotJourneyPlannerStepNumber > 0)
			{
				*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.close();
			}

			*_timeSlotJourneyPlannerFile << "<h1>Journey plannings</h1>";
			*_timeSlotJourneyPlannerFile << _timeSlotJourneyPlannerTable.open();

			_startChrono();
		}



		void AlgorithmLogger::closeTimeSlotJourneyPlannerLog() const
		{
			if(!_active)
			{
				return;
			}

			_stopChrono();

			*_timeSlotJourneyPlannerFile <<
				_timeSlotJourneyPlannerTable.close() <<
				"</body></html>"
			;
			_timeSlotJourneyPlannerFile->close();
			_timeSlotJourneyPlannerFile.reset();

			_startChrono();
		}



		void AlgorithmLogger::logTimeSlotJourneyPlannerApproachMap(
			bool isDeparture,
			const VertexAccessMap& vam
		) const	{
			if(!_active)
			{
				return;
			}

			_stopChrono();

			string od(isDeparture ? "Origin" : "Destination");

			*_timeSlotJourneyPlannerFile << "<h2>" << od << " access map calculation</h2>";

			*_timeSlotJourneyPlannerFile << "<h3>" << od << "s</h3><table class=\"adminresults\"><tr><th>Connection Place</th><th>Physical Stop</th><th>Dst.</th><th>Time</th></tr>";

			BOOST_FOREACH(VertexAccessMap::VamMap::value_type it, vam.getMap())
			{
				*_timeSlotJourneyPlannerFile	<<
					"<tr><td>" <<
					dynamic_cast<const NamedPlace*>(it.first->getHub())->getFullName() <<
					"</td><td>" <<
					static_cast<const StopPoint* const>(it.first)->getName() <<
					"</td><td>" <<
					it.second.approachDistance <<
					"</td><td>" <<
					it.second.approachTime.total_seconds() / 60 <<
					"</td></tr>"
					;
			}
			*_timeSlotJourneyPlannerFile << "</table>";

			_startChrono();
		}



		void AlgorithmLogger::_startChrono() const
		{
			if(_chronoStartTime.is_not_a_date_time())
			{
				_chronoStartTime = microsec_clock::local_time();
			}
		}



		boost::posix_time::time_duration AlgorithmLogger::_stopChrono() const
		{
			time_duration result(seconds(0));
			if(!_chronoStartTime.is_not_a_date_time())
			{
				result = microsec_clock::local_time() - _chronoStartTime;
				_chrono += result;
				_chronoStartTime = ptime(not_a_date_time);
			}
			return result;
		}
}	}
