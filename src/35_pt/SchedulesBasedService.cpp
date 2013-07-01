
/** SchedulesBasedService class implementation.
	@file SchedulesBasedService.cpp

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

#include "SchedulesBasedService.h"

#include "AccessParameters.h"
#include "CommercialLine.h"
#include "InterSYNTHESEContent.hpp"
#include "InterSYNTHESEModule.hpp"
#include "LineStop.h"
#include "NonConcurrencyRule.h"
#include "Path.h"
#include "RealTimePTDataInterSYNTHESE.hpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "Vertex.h"

#include <sstream>
#include <iomanip>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace graph;
	using namespace inter_synthese;
	using namespace util;


	namespace pt
	{
		const string SchedulesBasedService::STOP_SEPARATOR = ",";


		SchedulesBasedService::SchedulesBasedService(
			std::string serviceNumber,
			graph::Path* path
		):	NonPermanentService(serviceNumber, path),
			_nextRTUpdate(posix_time::second_clock::local_time() + gregorian::days(1))
		{
			clearStops();
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
			const Schedules& arrivalSchedules,
			bool onlyScheduledEdges
		){
			if(!_path)
			{
				throw BadSchedulesException();
			}

			if(onlyScheduledEdges)
			{
				_departureSchedules.clear();
				_arrivalSchedules.clear();
				Schedules::const_iterator itDeparture(departureSchedules.begin());
				Schedules::const_iterator itArrival(arrivalSchedules.begin());
				Path::Edges::const_iterator lastScheduledEdge(_path->getEdges().end());
				bool atLeastOneUnscheduledEdge(false);
				for(Path::Edges::const_iterator itEdge(_path->getEdges().begin()); itEdge != _path->getEdges().end(); ++itEdge)
				{
					const LineStop* lineStop(dynamic_cast<const LineStop*>(*itEdge));
					if(	!lineStop || lineStop->getScheduleInput())
					{
						const Edge& edge(**itEdge);

						// Interpolation of preceding schedules
						if(atLeastOneUnscheduledEdge)
						{
							if(lastScheduledEdge == _path->getEdges().end())
							{
								throw PathBeginsWithUnscheduledStopException(*_path);
							}

							MetricOffset totalDistance(edge.getMetricOffset() - (*lastScheduledEdge)->getMetricOffset());
							size_t totalRankDiff(edge.getRankInPath() - (*lastScheduledEdge)->getRankInPath());
							time_duration originDepartureSchedule(*_departureSchedules.rbegin());
							time_duration totalTime(*itArrival - originDepartureSchedule);
							for(Path::Edges::const_iterator it(lastScheduledEdge+1); it != itEdge && it != _path->getEdges().end(); ++it)
							{
								double minutesToAdd(0);
								if(totalDistance != 0)
								{
									MetricOffset distance((*it)->getMetricOffset() - (*lastScheduledEdge)->getMetricOffset());
									minutesToAdd = (totalTime.total_seconds() / 60) * (distance / totalDistance);
								}
								else
								{
									assert(totalRankDiff);
									size_t rankDiff((*it)->getRankInPath() - (*lastScheduledEdge)->getRankInPath());
									minutesToAdd = (totalTime.total_seconds() / 60) * (double(rankDiff) / double(totalRankDiff));
								}

								time_duration departureSchedule(originDepartureSchedule);
								time_duration arrivalSchedule(originDepartureSchedule);
								departureSchedule += minutes(
									static_cast<long>(floor(minutesToAdd))
								);
								arrivalSchedule += minutes(
									static_cast<long>(ceil(minutesToAdd))
								);

								_departureSchedules.push_back(departureSchedule);
								_arrivalSchedules.push_back(arrivalSchedule);
							}
						}

						// Check
						if(	itDeparture == departureSchedules.end() ||
							itArrival == arrivalSchedules.end()
						){
							throw BadSchedulesException();
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
			}	}
			else
			{
				_departureSchedules = departureSchedules;
				_arrivalSchedules = arrivalSchedules;
			}

			_path->markScheduleIndexesUpdateNeeded(false);
			clearRTData();
			_computeNextRTUpdate();
		}





		const boost::posix_time::ptime& SchedulesBasedService::getNextRTUpdate() const
		{
			return _nextRTUpdate;
		}



		void SchedulesBasedService::setRealTimeVertex( std::size_t rank, const graph::Vertex* value )
		{
			assert(!value || value->getHub() == _path->getEdge(rank)->getHub());
			_RTVertices[rank] = value;
		}



		const graph::Vertex* SchedulesBasedService::getRealTimeVertex( std::size_t rank ) const
		{
			return _RTVertices[rank];
		}



		const graph::Vertex* SchedulesBasedService::getVertex(
			std::size_t rank
		) const	{
			return _vertices[rank];
		}



		boost::posix_time::time_duration SchedulesBasedService::getDepartureSchedule( bool RTData, std::size_t rank ) const
		{
			return getDepartureSchedules(true, RTData).at(rank);
		}



		const boost::posix_time::time_duration& SchedulesBasedService::getLastDepartureSchedule( bool RTData ) const
		{
			for (Path::Edges::const_reverse_iterator it(getPath()->getEdges().rbegin()); it != getPath()->getEdges().rend(); ++it)
				if ((*it)->isDeparture())
					return getDepartureSchedules(true, RTData).at((*it)->getRankInPath());
			assert(false);
			return getDepartureSchedules(true, RTData).at(0);
		}



		boost::posix_time::time_duration SchedulesBasedService::getArrivalSchedule( bool RTData, std::size_t rank ) const
		{
			return getArrivalSchedules(true, RTData).at(rank);
		}



		const boost::posix_time::time_duration& SchedulesBasedService::getLastArrivalSchedule( bool RTData ) const
		{
			Schedules::const_iterator it(getArrivalSchedules(true, RTData).end() - 1);
			return *it;
		}



		const SchedulesBasedService::Schedules& SchedulesBasedService::getDepartureSchedules( bool THData, bool RTData ) const
		{
			if(RTData && _hasRealTimeData)
			{
				return _RTDepartureSchedules;
			}

			return THData ? _departureSchedules : _emptySchedules;
		}



		const SchedulesBasedService::Schedules& SchedulesBasedService::getArrivalSchedules( bool THData, bool RTData ) const
		{
			if(RTData && _hasRealTimeData)
			{
				return _RTArrivalSchedules;
			}

			return THData ? _arrivalSchedules : _emptySchedules;
		}


		void SchedulesBasedService::_applyRealTimeShiftDuration( 
			std::size_t rank, 
			boost::posix_time::time_duration arrivalShift,
			boost::posix_time::time_duration departureShift,
			bool updateFollowingSchedules
		)
		{
			_hasRealTimeData = true;
			// TODO Use a constant or a param for the accepted delay
			if(!_RTTimestamps[rank].is_not_a_date_time() &&
				_RTTimestamps[rank] > second_clock::local_time() - boost::posix_time::minutes(5))
			{
				// We are trying to update a data but we already got a specific
				// request on this rank and it is soon enough to consider that
				// we have a better one.
				return;
			}

			// Arrival shift
			{
				time_duration schedule(_arrivalSchedules[rank]);
				schedule += arrivalShift;
				_RTArrivalSchedules[rank] = schedule;
			}

			// Departure shift
			{
				time_duration schedule(_departureSchedules[rank]);
				schedule += departureShift;
				_RTDepartureSchedules[rank] = schedule;
			}

			if(updateFollowingSchedules && rank + 1 < _arrivalSchedules.size())
			{
				_applyRealTimeShiftDuration(
					rank + 1,
					departureShift, // Next arrival shifted after current departure
					departureShift,
					true
				);
			}
			if(rank + 1 == _arrivalSchedules.size())
			{
				_computeNextRTUpdate();
			}
		}



		void SchedulesBasedService::applyRealTimeShiftDuration( 
			std::size_t rank, 
			boost::posix_time::time_duration arrivalShift,
			boost::posix_time::time_duration departureShift,
			bool updateFollowingSchedules,
			bool recordTimeStamp
		)
		{
			// Clear the time stamp in all cases
			// If we had a timestamp here, we assume that the
			// new update should be processed so we clear it.
			_RTTimestamps[rank] = boost::posix_time::ptime();

			_applyRealTimeShiftDuration(
				rank,
				arrivalShift, departureShift,
				updateFollowingSchedules
			);
			// Record the time stamp
			if (recordTimeStamp)
			{
				_RTTimestamps[rank] = second_clock::local_time();
			}



			// Inter-SYNTHESE sync
			if(Factory<InterSYNTHESESyncTypeFactory>::size()) // Avoid in unit tests
			{
				inter_synthese::InterSYNTHESEContent content(
					RealTimePTDataInterSYNTHESE::FACTORY_KEY,
					lexical_cast<string>(getRoute()->getCommercialLine()->getKey()),
					RealTimePTDataInterSYNTHESE::GetContent(*this)
				);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					boost::optional<db::DBTransaction&>()
				);
			}
		}

		void SchedulesBasedService::clearRTData()
		{
			_hasRealTimeData = false;
			_RTDepartureSchedules = _departureSchedules;
			_RTArrivalSchedules = _arrivalSchedules;
			// Assuming arrival and departure schedules have the same size
			_RTTimestamps.assign(_departureSchedules.size(), boost::posix_time::ptime());
			_emptySchedules.assign(_departureSchedules.size(), not_a_date_time);

			if(getPath())
			{
				_RTVertices.clear();
				size_t i(0);
				BOOST_FOREACH(const Edge* edge, getPath()->getEdges())
				{
					if(i < _vertices.size())
					{
						if(_vertices[i])
						{
							_RTVertices.push_back(_vertices[i]);
						}
						else
						{
							_RTVertices.push_back(edge->getFromVertex());
						}
						++i;
					}
				}
			}
			_computeNextRTUpdate();
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
				if(	dynamic_cast<const LineStop*>(edge) &&
					!static_cast<const LineStop*>(edge)->getScheduleInput()
				){
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
				arrivalSchedules,
				true
			);
		}



		void SchedulesBasedService::setSchedulesFromOther(
			const SchedulesBasedService& other,
			boost::posix_time::time_duration shift
		){
			assert(getPath()->getKey() == other.getPath()->getKey());

			Schedules departureSchedules;
			Schedules arrivalSchedules;

			for(size_t i(0); i<other._departureSchedules.size(); ++i)
			{
				departureSchedules.push_back(other._departureSchedules[i].is_not_a_date_time() ? not_a_date_time : (other._departureSchedules[i] + shift));
				arrivalSchedules.push_back(other._arrivalSchedules[i].is_not_a_date_time() ? not_a_date_time : (other._arrivalSchedules[i] + shift));
			}
			setSchedules(
				departureSchedules,
				arrivalSchedules,
				false
			);
		}



		void SchedulesBasedService::generateIncrementalSchedules(
			time_duration firstSchedule
		){

			Schedules departureSchedules;
			Schedules arrivalSchedules;

			departureSchedules.assign(_path->getEdges().size(), firstSchedule);
			arrivalSchedules.assign(_path->getEdges().size(), firstSchedule);

			setSchedules(
				departureSchedules,
				arrivalSchedules,
				false
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
			_hasRealTimeData = true;


			// Inter-SYNTHESE sync
			if(Factory<InterSYNTHESESyncTypeFactory>::size()) // Avoid in unit tests
			{
				inter_synthese::InterSYNTHESEContent content(
					RealTimePTDataInterSYNTHESE::FACTORY_KEY,
					lexical_cast<string>(getRoute()->getCommercialLine()->getKey()),
					RealTimePTDataInterSYNTHESE::GetContent(*this)
				);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					boost::optional<db::DBTransaction&>()
				);
			}
		}



		void SchedulesBasedService::setRealTimeSchedules(
			const Schedules& departureSchedules, 
			const Schedules& arrivalSchedules
		){
			_RTDepartureSchedules = departureSchedules;
			_RTArrivalSchedules = arrivalSchedules;
			_computeNextRTUpdate();
			_path->markScheduleIndexesUpdateNeeded(true);
			_hasRealTimeData = true;


			// Inter-SYNTHESE sync
			if(Factory<InterSYNTHESESyncTypeFactory>::size()) // Avoid in unit tests
			{
				inter_synthese::InterSYNTHESEContent content(
					RealTimePTDataInterSYNTHESE::FACTORY_KEY,
					lexical_cast<string>(getRoute()->getCommercialLine()->getKey()),
					RealTimePTDataInterSYNTHESE::GetContent(*this)
				);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					boost::optional<db::DBTransaction&>()
				);
			}
		}



		void SchedulesBasedService::clearStops()
		{
			if(getPath())
			{
				_vertices.clear();
				_vertices.assign(getPath()->getEdges().size(), NULL);
			}
		}



		void SchedulesBasedService::setPath( Path* path )
		{
			NonPermanentService::setPath(path);
			clearStops();
			clearRTData();
		}



		string SchedulesBasedService::encodeStops() const
		{
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const SchedulesBasedService::ServedVertices::value_type& stop, _vertices)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << (stop ? lexical_cast<string>(stop->getKey()) : string());
			}
			return s.str();
		}



		void SchedulesBasedService::decodeStops(
			const std::string& value,
			util::Env& env
		){
			_vertices.clear();
			if(!value.empty())
			{
				vector<string> stops;
				split(stops, value, is_any_of(","));
				size_t rank(0);
				BOOST_FOREACH(const string& stop, stops)
				{
					if(!stop.empty())
					{
						try
						{
							RegistryKeyType stopId(lexical_cast<RegistryKeyType>(stop));
							StopPoint* stop(StopPointTableSync::GetEditable(stopId, env).get());
							if(stop->getHub() == _path->getEdge(rank)->getHub())
							{
								_vertices.push_back(stop);
							}
							else
							{
								_vertices.push_back(NULL);
							}
						}
						catch(ObjectNotFoundException<StopPoint>&)
						{
							_vertices.push_back(NULL);
						}
					}
					else
					{
						_vertices.push_back(NULL);
					}
					++rank;
				}

				// Size check
				if(rank < _path->getEdges().size())
				{
					Log::GetInstance().warn("Inconsistent vertices size in service "+ lexical_cast<string>(getKey()));
					for(; rank<_path->getEdges().size(); ++rank)
					{
						_vertices.push_back(NULL);
					}
				}
			}
			else
			{
				for(size_t rank(0); rank<_path->getEdges().size(); ++rank)
				{
					_vertices.push_back(NULL);
				}
			}
		}



		SchedulesBasedService::PathBeginsWithUnscheduledStopException::PathBeginsWithUnscheduledStopException(
			const graph::Path& path
		):	Exception(
			"The path "+ boost::lexical_cast<std::string>(path.getKey()) +" begins with an unscheduled stop."
		) {}



		void SchedulesBasedService::setVertex(
			size_t rank,
			const graph::Vertex* value
		){
			assert(!value || value->getHub() == _path->getEdge(rank)->getHub());
			if(value != _path->getEdge(rank)->getFromVertex())
			{
				_vertices[rank] = value;
			}
			else
			{
				_vertices[rank] = NULL;
			}
		}



		void SchedulesBasedService::setRealTimeVertices( const ServedVertices& value )
		{
			_RTVertices = value;

			// Inter-SYNTHESE sync
			if(Factory<InterSYNTHESESyncTypeFactory>::size()) // Avoid in unit tests
			{
				inter_synthese::InterSYNTHESEContent content(
					RealTimePTDataInterSYNTHESE::FACTORY_KEY,
					lexical_cast<string>(getRoute()->getCommercialLine()->getKey()),
					RealTimePTDataInterSYNTHESE::GetContent(*this)
				);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					boost::optional<db::DBTransaction&>()
				);
			}
		}



		const SchedulesBasedService::ServedVertices& SchedulesBasedService::getVertices(
			bool RTData
		) const	{
			if(RTData)
			{
				return _RTVertices;
			}
			else
			{
				return _vertices;
			}
		}



		bool SchedulesBasedService::comparePlannedStops(
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& servedVertices
		) const	{
			for(size_t i(0); i<_vertices.size(); ++i)
			{
				if(	_vertices[i] &&
					servedVertices[i]._stop.find(const_cast<StopPoint*>(static_cast<const StopPoint*>(_vertices[i]))) == servedVertices[i]._stop.end()
				){
					return false;
				}
			}
			return true;
		}



		void SchedulesBasedService::setVertices( const ServedVertices& vertices )
		{
			for(size_t i(0); i<_path->getEdges().size(); ++i)
			{
				setVertex(i, vertices[i]);
			}
		}

		/**
		 * @brief SchedulesBasedService::isInTimeRange checks if the given time is
		 * within one of the ranges in excludesRanges
		 * @param time is the time to search
		 * @param range is the range defined by the servicePointer calling us
		 * @param exludeRanges is a vector of time_period that are invalid.
		 * @return true if time is in one of the ranges
		 */
		bool SchedulesBasedService::isInTimeRange(
				ptime &time,
				time_duration &range,
				const vector<time_period> &excludeRanges) const
		{
			BOOST_FOREACH(const time_period& period, excludeRanges)
			{
				if(period.contains(time))
				{
					if(period.end() > time + range)
					{
						// The unallowed slot goes after the whole range: not allowed
						return true;
					}
					else
					{
						// Shift after the unallowed time slot
						ptime newTime = period.end();
						ptime ptEnd(time); ptEnd += range;
						range = ptEnd - newTime;
						time = newTime;
						return false;
					}
				}
				// Calc the new range
				if(time < period.begin() && range > period.begin() - time)
				{
					range = period.begin() - time;
					return false;
				}
			}
			return false;
		}

		bool SchedulesBasedService::nonConcurrencyRuleOK(
			ptime& time,
			time_duration &range,
			const Edge& departureEdge,
			const Edge& arrivalEdge,
			size_t userClassRank
		) const {
			const date date(time.date());
			const CommercialLine* line(getRoute()->getCommercialLine());
			if(line->getNonConcurrencyRules().empty()) return true;

			boost::recursive_mutex::scoped_lock serviceLock(_nonConcurrencyCacheMutex);

			_NonConcurrencyCache::const_iterator it(
				_nonConcurrencyCache.find(
					_NonConcurrencyCache::key_type(
						&departureEdge,
						&arrivalEdge,
						userClassRank,
						date
			)	)	);
			if(it != _nonConcurrencyCache.end())
			{
				return !isInTimeRange(time, range, it->second);
			}
			recursive_mutex::scoped_lock lineLock(line->getNonConcurrencyRulesMutex());

			vector<time_period> excludeRanges;
			const CommercialLine::NonConcurrencyRules& rules(line->getNonConcurrencyRules());
			const StopArea::PhysicalStops& startStops(
				static_cast<const StopPoint*>(departureEdge.getFromVertex())->getConnectionPlace()->getPhysicalStops()
			);
			const Hub* arrivalHub(
				arrivalEdge.getFromVertex()->getHub()
			);

			typedef graph::Edge* (graph::Edge::*PtrEdgeStep) () const;
			PtrEdgeStep step(
				arrivalHub->isUsefulTransfer(arrivalEdge.getFromVertex()->getGraphType())
				? (&Edge::getFollowingConnectionArrival)
				: (&Edge::getFollowingArrivalForFineSteppingOnly)
			);


			BOOST_FOREACH(const NonConcurrencyRule* rule, rules)
			{
				CommercialLine* priorityLine(rule->getPriorityLine());
				const CommercialLine::Paths& paths(priorityLine->getPaths());
				ptime minStartTime(date, getDepartureBeginScheduleToIndex(false, departureEdge.getRankInPath()));
				minStartTime -= rule->getDelay();
				ptime maxStartTime(date, getDepartureEndScheduleToIndex(false, departureEdge.getRankInPath()));
				maxStartTime += rule->getDelay();

				// Loop on all vertices of the starting place
				BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStartStop, startStops)
				{
					// Loop on all non concurrent paths
					BOOST_FOREACH(const Path* path, paths)
					{
						if(path == getPath()) continue;

						const Vertex::Edges& departureEdges(itStartStop.second->getDepartureEdges());
						pair<Vertex::Edges::const_iterator, Vertex::Edges::const_iterator> range(departureEdges.equal_range(path));
						if(range.first == departureEdges.end() || range.first->first != path)
						{
							continue;
						}

						for(Vertex::Edges::const_iterator its(range.first); its != range.second; ++its)
						{
							const Edge& startEdge(*its->second);
							// Search a service at the time of the possible
							AccessParameters ap(userClassRank + USER_CLASS_CODE_OFFSET);
							optional<Edge::DepartureServiceIndex::Value> minServiceIndex;
					
							while(true)
							{
								ServicePointer serviceInstance(
									startEdge.getNextService(
										ap,
										minStartTime,
										maxStartTime,
										true,
										minServiceIndex,
										false,
										true
								)	);
								// If no service, advance to the next path
								if (!serviceInstance.getService()) break;
								++*minServiceIndex;
								// This is needed because getNextService is broken if we keep calling
								// it with a minStartTime on a non active day (infinite loop)
								minStartTime = serviceInstance.getDepartureDateTime();
								// Path traversal
								for (const Edge* endEdge = (startEdge.*step) ();
									 endEdge != NULL; endEdge = (endEdge->*step) ())
								{
									// Found eligible arrival place
									if(endEdge->getHub() == arrivalHub)
									{
										time_period timePeriod(
													serviceInstance.getDepartureDateTime() - rule->getDelay(),
													serviceInstance.getDepartureDateTime() +
													serviceInstance.getServiceRange() + rule->getDelay()
													);
										if(excludeRanges.size() && timePeriod.intersects(excludeRanges.back()))
										{
											// Merge the last period and the new one. We assume we are always called
											// in incremental time so the time_periods are sorted
											excludeRanges[excludeRanges.size()-1] =
													timePeriod.merge(excludeRanges.back());
										}
										else
										{
											excludeRanges.push_back(timePeriod);
										}

										if ( isContinuous() )
										{
											// There maybe some more non concurent services to record
											break;
										} else
										{
											// No need to search further for non continuous services
											_nonConcurrencyCache.insert(
														make_pair(
															_NonConcurrencyCache::key_type(
																&departureEdge,
																&arrivalEdge,
																userClassRank,
																date
																), excludeRanges
															)	);
											return false;
										}
									}
								}
							}
						}
					}
				}
			}

			_nonConcurrencyCache.insert(
				make_pair(
					_NonConcurrencyCache::key_type(
						&departureEdge,
						&arrivalEdge,
						userClassRank,
						date
					), excludeRanges
			)	);
			return !isInTimeRange(time, range, excludeRanges);
		}


		void SchedulesBasedService::clearNonConcurrencyCache() const
		{
			recursive_mutex::scoped_lock serviceLock(_nonConcurrencyCacheMutex);
			_nonConcurrencyCache.clear();
		}

		bool operator==(const SchedulesBasedService& first, const SchedulesBasedService& second)
		{
			return
				first.getPath() == second.getPath() &&
				first.getServiceNumber() == second.getServiceNumber() &&
				first.getDepartureSchedules(true, false) == second.getDepartureSchedules(true, false) &&
				first.getArrivalSchedules(true, false) == second.getArrivalSchedules(true, false)
			;
		}


}	}
