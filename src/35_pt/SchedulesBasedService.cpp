
/** SchedulesBasedService class implementation.
	@file SchedulesBasedService.cpp

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

#include "SchedulesBasedService.h"
#include "Path.h"
#include "LineStop.h"
#include "Vertex.h"

#include <sstream>
#include <iomanip>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;

	namespace pt
	{
		SchedulesBasedService::SchedulesBasedService(
			std::string serviceNumber,
			graph::Path* path
		):
			NonPermanentService(serviceNumber, path)
		{
			clearRTData();
		}



		void SchedulesBasedService::_computeNextRTUpdate()
		{
			if(!_arrivalSchedules.empty())
			{
				const time_duration& lastThSchedule(*(_arrivalSchedules.end() - 1));
				const time_duration& lastRTSchedule(*(_RTArrivalSchedules.end() - 1));
				const time_duration& lastSchedule = (lastThSchedule < lastRTSchedule) ? lastRTSchedule : lastThSchedule;

				boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
				_nextRTUpdate = boost::posix_time::ptime(now.date(), GetTimeOfDay(lastSchedule));
				if(now.time_of_day() > GetTimeOfDay(lastSchedule))
				{
					_nextRTUpdate += boost::gregorian::days(1);
				}
			}
		}



		void SchedulesBasedService::setSchedules(
			const Schedules& departureSchedules,
			const Schedules& arrivalSchedules
		){
			if(!_path)
			{
				throw BadSchedulesException();
			}

			_departureSchedules.clear();
			_arrivalSchedules.clear();
			Schedules::const_iterator itDeparture(departureSchedules.begin());
			Schedules::const_iterator itArrival(arrivalSchedules.begin());
			Path::Edges::const_iterator lastScheduledEdge(_path->getEdges().end());
			bool atLeastOneUnscheduledEdge(false);
			for(Path::Edges::const_iterator itEdge(_path->getEdges().begin()); itEdge != _path->getEdges().end(); ++itEdge)
			{
				const LineStop& lineStop(static_cast<const LineStop&>(**itEdge));
				if(	lineStop.getScheduleInput())
				{
					// Interpolation of preceding schedules
					if(atLeastOneUnscheduledEdge)
					{
						if(lastScheduledEdge == _path->getEdges().end())
						{
							throw PathBeginsWithUnscheduledStopException(*_path);
						}
						MetricOffset totalDistance(lineStop.getMetricOffset() - (*lastScheduledEdge)->getMetricOffset());
						time_duration originDepartureSchedule(*_departureSchedules.rbegin());
						time_duration totalTime(*itArrival - originDepartureSchedule);
						for(Path::Edges::const_iterator it(lastScheduledEdge+1); it != itEdge && it != _path->getEdges().end(); ++it)
						{
							MetricOffset distance((*it)->getMetricOffset() - (*lastScheduledEdge)->getMetricOffset());

							time_duration departureSchedule(originDepartureSchedule);
							time_duration arrivalSchedule(originDepartureSchedule);
							departureSchedule += minutes(
								static_cast<long>(floor( (totalTime.total_seconds() / 60) * (distance / totalDistance)))
							);
							arrivalSchedule += minutes(
								static_cast<long>(ceil( (totalTime.total_seconds() / 60) * (distance / totalDistance)))
							);

							_departureSchedules.push_back(departureSchedule);
							_arrivalSchedules.push_back(arrivalSchedule);
						}
					}

					// Store the schedules
					_departureSchedules.push_back(*itDeparture);
					_arrivalSchedules.push_back(*itArrival);

					// Store the last scheduled edge
					lastScheduledEdge = itEdge;
					atLeastOneUnscheduledEdge = false;

					// Increment iterators
					++itDeparture;
					++itArrival;
				}
				else
				{
					atLeastOneUnscheduledEdge = true;
				}
			}

			_path->markScheduleIndexesUpdateNeeded(false);
			clearRTData();
			_computeNextRTUpdate();
		}



		boost::posix_time::time_duration SchedulesBasedService::getDepartureSchedule( bool RTData, std::size_t rank ) const
		{
			return getDepartureSchedules(RTData).at(rank);
		}



		const boost::posix_time::time_duration& SchedulesBasedService::getLastDepartureSchedule( bool RTData ) const
		{
			for (Path::Edges::const_reverse_iterator it(getPath()->getEdges().rbegin()); it != getPath()->getEdges().rend(); ++it)
				if ((*it)->isDeparture())
					return getDepartureSchedules(RTData).at((*it)->getRankInPath());
			assert(false);
			return getDepartureSchedules(RTData).at(0);
		}



		boost::posix_time::time_duration SchedulesBasedService::getArrivalSchedule( bool RTData, std::size_t rank ) const
		{
			return getArrivalSchedules(RTData).at(rank);
		}



		const boost::posix_time::time_duration& SchedulesBasedService::getLastArrivalSchedule( bool RTData ) const
		{
			Schedules::const_iterator it(getArrivalSchedules(RTData).end() - 1);
			return *it;
		}



		const SchedulesBasedService::Schedules& SchedulesBasedService::getDepartureSchedules( bool RTData ) const
		{
			return RTData ? _RTDepartureSchedules : _departureSchedules;
		}



		const SchedulesBasedService::Schedules& SchedulesBasedService::getArrivalSchedules( bool RTData ) const
		{
			return RTData ? _RTArrivalSchedules : _arrivalSchedules;
		}



		void SchedulesBasedService::applyRealTimeLateDuration( std::size_t rank, boost::posix_time::time_duration value, bool atArrival, bool atDeparture, bool updateFollowingSchedules )
		{
			if(atArrival)
			{
				time_duration schedule(_arrivalSchedules[rank]);
				schedule += value;
				_RTArrivalSchedules[rank] = schedule;
			}
			if(atDeparture)
			{
				time_duration schedule(_departureSchedules[rank]);
				schedule += value;
				_RTDepartureSchedules[rank] = schedule;
			}
			if(updateFollowingSchedules && rank + 1 < _arrivalSchedules.size())
			{
				applyRealTimeLateDuration(
					rank + 1,
					value,
					true, true, true
				);
			}
			if(atArrival && rank + 1 == _arrivalSchedules.size())
			{
				_computeNextRTUpdate();
			}
		}



		void SchedulesBasedService::clearRTData()
		{
			_RTDepartureSchedules = _departureSchedules;
			_RTArrivalSchedules = _arrivalSchedules;
			Service::clearRTData();
		}



		std::string SchedulesBasedService::EncodeSchedule( const boost::posix_time::time_duration& value )
		{
			if(value.is_not_a_date_time())
			{
				return string();
			}

			std::stringstream os;

			os << std::setw( 2 ) << std::setfill ( '0' )
				<< (value.hours() / 24) << ":"
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< (value.hours() % 24) << ":"
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< value.minutes()
				;

			return os.str ();
		}



		boost::posix_time::time_duration SchedulesBasedService::DecodeSchedule( const std::string value )
		{
			int days(lexical_cast<int>(value.substr(0, 2)));
			int hours(lexical_cast<int>(value.substr(3, 2)));
			int minutes(lexical_cast<int>(value.substr(6, 2)));
			return time_duration(
				(days > 0 ? 24 * days : 0) + (hours > 0 ? hours : 0),
				(minutes > 0 ? minutes : 0),
				0
			);
		}



		std::string SchedulesBasedService::encodeSchedules(
			boost::posix_time::time_duration shiftArrivals
		) const {
			stringstream str;
			size_t i(0);
			BOOST_FOREACH(const Edge* edge, _path->getEdges())
			{
				if(!static_cast<const LineStop*>(edge)->getScheduleInput())
				{
					++i;
					continue;
				}
				if(i)
				{
					str << ",";
				}
				str << EncodeSchedule(_arrivalSchedules[i] + shiftArrivals) << "#" << EncodeSchedule(_departureSchedules[i]);
				++i;
			}
			return str.str();
		}



		void SchedulesBasedService::decodeSchedules(
			const std::string value,
			boost::posix_time::time_duration shiftArrivals
		){
			typedef tokenizer<char_separator<char> > tokenizer;

			// Parse all schedules arrival#departure,arrival#departure...
			tokenizer schedulesTokens (value, char_separator<char>(","));

			Schedules departureSchedules;
			Schedules arrivalSchedules;

			for(tokenizer::iterator schedulesIter = schedulesTokens.begin();
				schedulesIter != schedulesTokens.end ();
				++schedulesIter
			){
				string arrDep (*schedulesIter);
				size_t sepPos = arrDep.find ("#");
				assert (sepPos != string::npos);

				string arrivalScheduleStr (arrDep.substr (0, sepPos));
				string departureScheduleStr (arrDep.substr (sepPos+1));

				// unnecessary : boost::trim (departureScheduleStr);
				// unnecessary : boost::trim (arrivalScheduleStr);

				if (departureScheduleStr.empty ())
				{
					assert (arrivalScheduleStr.empty () == false);
					departureScheduleStr = arrivalScheduleStr;
				}
				if (arrivalScheduleStr.empty ())
				{
					assert (departureScheduleStr.empty () == false);
					arrivalScheduleStr = departureScheduleStr;
				}

				time_duration departureSchedule (DecodeSchedule(departureScheduleStr));
				time_duration arrivalSchedule (DecodeSchedule(arrivalScheduleStr) + shiftArrivals);

				departureSchedules.push_back (departureSchedule);
				arrivalSchedules.push_back (arrivalSchedule);
			}

			if(	departureSchedules.size () <= 0 ||
				arrivalSchedules.size () <= 0 ||
				departureSchedules.size() != arrivalSchedules.size ()
			){
				throw BadSchedulesException();
			}

			setSchedules(
				departureSchedules,
				arrivalSchedules
			);
		}



		void SchedulesBasedService::setSchedulesFromOther(
			const SchedulesBasedService& other,
			boost::posix_time::time_duration shift
		){
			assert(getPathId() == other.getPathId());

			Schedules departureSchedules;
			Schedules arrivalSchedules;

			for(size_t i(0); i<other._departureSchedules.size(); ++i)
			{
				departureSchedules.push_back(other._departureSchedules[i].is_not_a_date_time() ? not_a_date_time : (other._departureSchedules[i] + shift));
				arrivalSchedules.push_back(other._arrivalSchedules[i].is_not_a_date_time() ? not_a_date_time : (other._arrivalSchedules[i] + shift));
			}
			setSchedules(
				departureSchedules,
				arrivalSchedules
			);
		}



		void SchedulesBasedService::generateIncrementalSchedules(
			time_duration firstSchedule
		){

			Schedules departureSchedules;
			Schedules arrivalSchedules;

			for(size_t i(0); i<_path->getEdges().size(); ++i)
			{
				departureSchedules.push_back(i+1 == _path->getEdges().size() ? not_a_date_time : firstSchedule);
				arrivalSchedules.push_back(i == 0 ? not_a_date_time : firstSchedule);
				firstSchedule += minutes(1);
			}

			setSchedules(
				departureSchedules,
				arrivalSchedules
			);
		}



		bool SchedulesBasedService::comparePlannedSchedules( const Schedules& departure, const Schedules& arrival ) const
		{
			size_t i(0);
			Schedules::const_iterator itd(departure.begin());
			Schedules::const_iterator ita(arrival.begin());
			BOOST_FOREACH(const Edge* edge, _path->getEdges())
			{
				if(!static_cast<const LineStop*>(edge)->getScheduleInput())
				{
					++i;
					continue;
				}
				if(	_departureSchedules[i] != *itd ||
					_arrivalSchedules[i] != *ita
				){
					return false;
				}
				++i;
				++itd;
				++ita;
			}
			return true;
		}



		graph::Edge* SchedulesBasedService::getEdgeFromStopAndTime(
			const Vertex& stopPoint,
			const boost::posix_time::time_duration& schedule,
			bool departure
		) const {

			Path::Edges edges(_path->getAllEdges());

			BOOST_FOREACH(Edge* edge, edges)
			{
				if(	edge->getFromVertex()->getHub() == stopPoint.getHub() &&
					(	(departure && edge->isDepartureAllowed() && schedule == _departureSchedules[edge->getRankInPath()] - _departureSchedules[0]) ||
						(!departure && edge->isArrivalAllowed() && schedule == _arrivalSchedules[edge->getRankInPath()] - _departureSchedules[0])
				)	){
					return edge;
				}
			}
			return NULL;
		}



		void SchedulesBasedService::setRealTimeSchedules(
			size_t rank,
			boost::posix_time::time_duration departureSchedule,
			boost::posix_time::time_duration arrivalSchedule
		){
			if(!departureSchedule.is_not_a_date_time())
			{
				_RTDepartureSchedules[rank] = departureSchedule;
			}
			if(!arrivalSchedule.is_not_a_date_time())
			{
				_RTArrivalSchedules[rank] = arrivalSchedule;
				if(rank + 1 == _arrivalSchedules.size())
				{
					_computeNextRTUpdate();
				}
			}
			_path->markScheduleIndexesUpdateNeeded(true);
		}



		SchedulesBasedService::PathBeginsWithUnscheduledStopException::PathBeginsWithUnscheduledStopException(
			const graph::Path& path
		):	Exception(
			"The path "+ boost::lexical_cast<std::string>(path.getKey()) +" begins with an unscheduled stop."
		) {}
}	}
