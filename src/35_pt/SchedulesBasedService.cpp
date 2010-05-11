
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
#include "Edge.h"

#include <sstream>
#include <iomanip>
#include <boost/tokenizer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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



		void SchedulesBasedService::setDepartureSchedules( const Schedules& schedules )
		{
			_departureSchedules = schedules;
			_RTDepartureSchedules = schedules;
		}



		void SchedulesBasedService::setArrivalSchedules( const Schedules& schedules )
		{
			_arrivalSchedules = schedules;
			_RTArrivalSchedules = schedules;
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
			return time_duration(
				24 * lexical_cast<int>(value.substr(0, 2)) + lexical_cast<int>(value.substr(3, 2)),
				lexical_cast<int>(value.substr(6, 2)),
				0
			);
		}



		std::string SchedulesBasedService::encodeSchedules(
			boost::posix_time::time_duration shiftArrivals
		) const {
			stringstream str;
			for(size_t i(0); i<_departureSchedules.size(); ++i)
			{
				if(i)
				{
					str << ",";
				}
				str << EncodeSchedule(_arrivalSchedules[i] + shiftArrivals) << "#" << EncodeSchedule(_departureSchedules[i]);
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

			_departureSchedules = departureSchedules;
			_arrivalSchedules = arrivalSchedules;
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
				departureSchedules.push_back(i+1 == other._departureSchedules.size() ? minutes(0) : (other._departureSchedules[i].is_not_a_date_time() ? not_a_date_time : (other._departureSchedules[i] + shift)));
				arrivalSchedules.push_back(i==0 ? minutes(0) : (other._arrivalSchedules[i].is_not_a_date_time() ? not_a_date_time : (other._arrivalSchedules[i] + shift)));
			}
			_departureSchedules = departureSchedules;
			_arrivalSchedules = arrivalSchedules;
			clearRTData();
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

			_departureSchedules = departureSchedules;
			_arrivalSchedules = arrivalSchedules;
			clearRTData();
		}
	}
}
