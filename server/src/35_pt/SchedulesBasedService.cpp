
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
#include "MalformedSchedulesException.hpp"
#include "LineStop.h"
#include "NonConcurrencyRule.h"
#include "Path.h"
#include "PTUseRule.h"
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
	using namespace pt;
	using namespace util;

	FIELD_DEFINITION_OF_TYPE(ServiceNumber, "service_number", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ServiceSchedules, "schedules", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ServicePath, "path_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ServiceDates, "dates", SQL_TEXT)


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
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			if(!_dataArrivalSchedules.empty())
			{
				const time_duration& lastThSchedule(*_dataArrivalSchedules.rbegin());
				const time_duration& lastRTSchedule(*(_RTArrivalSchedules.empty() ? _dataArrivalSchedules.rbegin() : _RTArrivalSchedules.rbegin()));
				const time_duration& lastSchedule = (lastThSchedule < lastRTSchedule) ? lastRTSchedule : lastThSchedule;

				boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
				_nextRTUpdate = boost::posix_time::ptime(now.date(), GetTimeOfDay(lastSchedule));
				if(now.time_of_day() > GetTimeOfDay(lastSchedule) + minutes(1))
				{
					_nextRTUpdate += boost::gregorian::days(1);
				}
			}
		}



		void SchedulesBasedService::setDataSchedules(
			const Schedules& departureSchedules,
			const Schedules& arrivalSchedules
		){
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			_dataDepartureSchedules = departureSchedules;
			_dataArrivalSchedules = arrivalSchedules;
			
			if(_path)
			{
				_path->markAllScheduleIndexesUpdateNeeded(false);
			}
			_clearGeneratedSchedules();
			clearRTData();
			_computeNextRTUpdate();
		}





		const boost::posix_time::ptime& SchedulesBasedService::getNextRTUpdate() const
		{
			return _nextRTUpdate;
		}



		void SchedulesBasedService::setRealTimeVertex( std::size_t rank, graph::Vertex* value )
		{
			// Lock the vertices
			recursive_mutex::scoped_lock lock(getVerticesMutex());

			assert(!value || value->getHub() == _path->getEdge(rank)->getHub());
			_RTVertices[rank] = value;
			if (value)
			{
				value->addDepartureEdge(const_cast<Edge*>(_path->getEdge(rank)));
			}
		}



		const graph::Vertex* SchedulesBasedService::getRealTimeVertex( std::size_t rank ) const
		{
			// Lock the vertices
			recursive_mutex::scoped_lock lock(getVerticesMutex());

			return _RTVertices[rank];
		}



		const graph::Vertex* SchedulesBasedService::getVertex(
			std::size_t rank
		) const	{
			// Lock the vertices
			recursive_mutex::scoped_lock lock(getVerticesMutex());

			return _vertices[rank];
		}



		boost::posix_time::time_duration SchedulesBasedService::getDepartureSchedule( bool RTData, std::size_t rank ) const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());
			
			if (getDataDepartureSchedules().size() == 0)
			{
				return time_duration(not_a_date_time);
			}

			if(!RTData && rank == 0)
			{
				return *getDataDepartureSchedules().begin();
			}

			return getDepartureSchedules(true, RTData).at(rank);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Last departure schedule getter (returned by reference).
		/// It is recommended to lock the _schedulesMutex before calling this method :
		/// recursive_mutex::scoped_lock lock(getSchedulesMutex());
		const boost::posix_time::time_duration& SchedulesBasedService::getLastDepartureSchedule( bool RTData ) const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			for (Path::Edges::const_reverse_iterator it(getPath()->getEdges().rbegin()); it != getPath()->getEdges().rend(); ++it)
			{
				if ((*it)->isDeparture())
				{
					return getDepartureSchedules(true, RTData).at((*it)->getRankInPath());
			}	}
			assert(false);
			return getDepartureSchedules(true, RTData).at(0);
		}



		boost::posix_time::time_duration SchedulesBasedService::getArrivalSchedule( bool RTData, std::size_t rank ) const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			return getArrivalSchedules(true, RTData).at(rank);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Last arrival schedule getter (returned by reference).
		/// It is recommended to lock the _schedulesMutex before calling this method :
		/// recursive_mutex::scoped_lock lock(getSchedulesMutex());
		const boost::posix_time::time_duration& SchedulesBasedService::getLastArrivalSchedule( bool RTData ) const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			const Schedules& schedules(
				RTData ?
				getArrivalSchedules(true, RTData) :
				getDataArrivalSchedules()
			);

			if(schedules.empty())
			{
				throw MalformedSchedulesException();
			}

			return *schedules.rbegin();
		}



		//////////////////////////////////////////////////////////////////////////
		/// Departure schedules getter.
		/// It is recommended to lock the _schedulesMutex before calling this method :
		/// recursive_mutex::scoped_lock lock(getSchedulesMutex());
		const SchedulesBasedService::Schedules& SchedulesBasedService::getDepartureSchedules( bool THData, bool RTData ) const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			if(RTData && !_RTDepartureSchedules.empty())
			{
				return _RTDepartureSchedules;
			}

			if(THData)
			{
				if(_generatedDepartureSchedules.empty())
				{
					_generateSchedules();
				}
				if(!_generatedDepartureSchedules.empty())
				{
					return _generatedDepartureSchedules;
				}
			}

			return _emptySchedules;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Arrival schedules getter.
		/// It is recommended to lock the _generatedSchedulesMutex before calling this method :
		/// recursive_mutex::scoped_lock lock(getGeneratedSchedulesMutex());
		const SchedulesBasedService::Schedules& SchedulesBasedService::getArrivalSchedules( bool THData, bool RTData ) const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			if(RTData && !_RTArrivalSchedules.empty())
			{
				return _RTArrivalSchedules;
			}

			if(THData)
			{
				if(_generatedArrivalSchedules.empty())
				{
					_generateSchedules();
				}
				if(!_generatedArrivalSchedules.empty())
				{
					return _generatedArrivalSchedules;
				}
			}

			return _emptySchedules;
		}



		void SchedulesBasedService::_initRTSchedulesFromPlanned()
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			if(_generatedArrivalSchedules.empty())
			{
				_generateSchedules();
			}
			if(_RTArrivalSchedules.empty())
			{
				_RTDepartureSchedules = _generatedDepartureSchedules;
				_RTArrivalSchedules = _generatedArrivalSchedules;
			}
		}



		void SchedulesBasedService::_applyRealTimeShiftDuration( 
			std::size_t rank, 
			boost::posix_time::time_duration arrivalShift,
			boost::posix_time::time_duration departureShift,
			bool updateFollowingSchedules
		){
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			_initRTSchedulesFromPlanned();

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
				time_duration schedule(_generatedArrivalSchedules[rank]);
				schedule += arrivalShift;
				_RTArrivalSchedules[rank] = schedule;
			}

			// Departure shift
			{
				time_duration schedule(_generatedDepartureSchedules[rank]);
				schedule += departureShift;
				_RTDepartureSchedules[rank] = schedule;
			}

			if(updateFollowingSchedules && rank + 1 < _generatedArrivalSchedules.size())
			{
				_applyRealTimeShiftDuration(
					rank + 1,
					departureShift, // Next arrival shifted after current departure
					departureShift,
					true
				);
			}
			if(rank + 1 == _generatedArrivalSchedules.size())
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
		){
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

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
				RealTimePTDataInterSYNTHESE::Content content(
					*this
				);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					boost::optional<db::DBTransaction&>(),
					this
				);
			}
		}



		void SchedulesBasedService::clearRTData()
		{
			// Real time schedules
			{
				// Lock the schedules
				recursive_mutex::scoped_lock lock(getSchedulesMutex());

				_RTArrivalSchedules.clear();
				_RTDepartureSchedules.clear();
			}
		
			if(getPath())
			{
				// Lock the vertices
				recursive_mutex::scoped_lock lock(getVerticesMutex());

				_RTTimestamps.assign(getPath()->getEdges().size(), boost::posix_time::ptime());
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
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			stringstream str;
			for(size_t i(0); i<_dataArrivalSchedules.size() && i<_dataDepartureSchedules.size(); ++i)
			{
				if(i>0)
				{
					str << ",";
				}
				str << EncodeSchedule(_dataArrivalSchedules[i] + shiftArrivals) << "#" << EncodeSchedule(_dataDepartureSchedules[i]);
			}
			return str.str();
		}



		SchedulesBasedService::SchedulesPair SchedulesBasedService::DecodeSchedules(
			const std::string value,
			boost::posix_time::time_duration shiftArrivals
		){
			typedef tokenizer<char_separator<char> > tokenizer;

//			if(!_path)
			{
				// No need to parse the data an complete our init
//				return;
			}
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

			return make_pair(departureSchedules, arrivalSchedules);
		}



		void SchedulesBasedService::setSchedulesFromOther(
			const SchedulesBasedService& other,
			boost::posix_time::time_duration shift
		){
			assert(getPath()->getKey() == other.getPath()->getKey());

			Schedules departureSchedules;
			Schedules arrivalSchedules;

			// Lock the schedules
			recursive_mutex::scoped_lock lock(other.getSchedulesMutex());

			for(size_t i(0); i<other._dataDepartureSchedules.size() && i<other._dataArrivalSchedules.size(); ++i)
			{
				departureSchedules.push_back(
					other._dataDepartureSchedules[i].is_not_a_date_time() ?
					not_a_date_time :
					(other._dataDepartureSchedules[i] + shift)
				);
				arrivalSchedules.push_back(
					other._dataArrivalSchedules[i].is_not_a_date_time() ?
					not_a_date_time :
					(other._dataArrivalSchedules[i] + shift)
				);
			}
			setDataSchedules(
				departureSchedules,
				arrivalSchedules
			);
		}



		void SchedulesBasedService::generateIncrementalSchedules(
			time_duration firstSchedule
		){
			if(!_path)
			{
				return;
			}

			Schedules departureSchedules;
			Schedules arrivalSchedules;

			size_t numberOfSchedules(static_cast<JourneyPattern*>(_path)->getScheduledStopsNumber());
			departureSchedules.assign(numberOfSchedules, firstSchedule);
			arrivalSchedules.assign(numberOfSchedules, firstSchedule);

			setDataSchedules(
				departureSchedules,
				arrivalSchedules
			);
		}



		bool SchedulesBasedService::comparePlannedSchedules( const Schedules& departure, const Schedules& arrival ) const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			return	(_dataDepartureSchedules == departure) &&
					(_dataArrivalSchedules == arrival);
		}



		graph::Edge* SchedulesBasedService::getEdgeFromStopAndTime(
			const Vertex& stopPoint,
			const boost::posix_time::time_duration& schedule,
			bool departure
		) const {

			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());
			Path::Edges edges(_path->getEdges());

			const Schedules& departureSchedules(getDepartureSchedules(true, false));
			const Schedules& arrivalSchedules(getArrivalSchedules(true, false));

			BOOST_FOREACH(Edge* edge, edges)
			{
				if(	edge->getFromVertex()->getHub() == stopPoint.getHub() &&
					(	(departure && edge->isDepartureAllowed() && schedule == departureSchedules[edge->getRankInPath()] - departureSchedules[0]) ||
						(!departure && edge->isArrivalAllowed() && schedule == arrivalSchedules[edge->getRankInPath()] - departureSchedules[0])
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
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			_initRTSchedulesFromPlanned();

			if(!departureSchedule.is_not_a_date_time())
			{
				_RTDepartureSchedules[rank] = departureSchedule;
			}
			if(!arrivalSchedule.is_not_a_date_time())
			{
				_RTArrivalSchedules[rank] = arrivalSchedule;
				if(rank + 1 == _RTArrivalSchedules.size())
				{
					_computeNextRTUpdate();
				}
			}
			_path->markAllScheduleIndexesUpdateNeeded(true);


			// Inter-SYNTHESE sync
			if(Factory<InterSYNTHESESyncTypeFactory>::size()) // Avoid in unit tests
			{
				RealTimePTDataInterSYNTHESE::Content content(*this);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					boost::optional<db::DBTransaction&>(),
					this
				);
			}
		}



		void SchedulesBasedService::setRealTimeSchedules(
			const Schedules& departureSchedules, 
			const Schedules& arrivalSchedules
		){
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			_RTDepartureSchedules = departureSchedules;
			_RTArrivalSchedules = arrivalSchedules;
			_computeNextRTUpdate();
			_path->markAllScheduleIndexesUpdateNeeded(true);


			// Inter-SYNTHESE sync
			if(Factory<InterSYNTHESESyncTypeFactory>::size()) // Avoid in unit tests
			{
				RealTimePTDataInterSYNTHESE::Content content(*this);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					boost::optional<db::DBTransaction&>(),
					this
				);
			}
		}



		void SchedulesBasedService::clearStops()
		{
			if(getPath())
			{
				// Lock the vertices
				recursive_mutex::scoped_lock lock(getVerticesMutex());

				_vertices.clear();
				_vertices.assign(getPath()->getEdges().size(), NULL);
			}
		}



		void SchedulesBasedService::setPath( Path* path )
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			NonPermanentService::setPath(path);
			_clearGeneratedSchedules();
			clearStops();
			clearRTData();
			if (path)
			{
				_emptySchedules.assign(getPath()->getEdges().size(), not_a_date_time);
			}
		}



		string SchedulesBasedService::encodeStops() const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getVerticesMutex());

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



		SchedulesBasedService::ServedVertices SchedulesBasedService::decodeStops(
			const std::string& value,
			util::Env& env
		) const {
			SchedulesBasedService::ServedVertices result;

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
								result.push_back(stop);
							}
							else
							{
								result.push_back(NULL);
							}
						}
						catch(ObjectNotFoundException<StopPoint>&)
						{
							result.push_back(NULL);
						}
					}
					else
					{
						result.push_back(NULL);
					}
					++rank;
				}

				// Size check
				if (dynamic_cast<JourneyPattern*>(_path))
				{
					const JourneyPattern* line(
						static_cast<const JourneyPattern*>(_path)
					);
					if (rank < line->getLineStops().size())
					{
						Log::GetInstance().warn("Inconsistent vertices size in service "+ lexical_cast<string>(getKey()));
					}
					for(; rank<_path->getEdges().size(); ++rank)
					{
						result.push_back(NULL);
					}
				}
				else
				{
					if(rank < _path->getEdges().size())
					{
						Log::GetInstance().warn("Inconsistent vertices size in service "+ lexical_cast<string>(getKey()));
						for(; rank<_path->getEdges().size(); ++rank)
						{
							result.push_back(NULL);
						}
					}
				}
			}
			else
			{
				for(size_t rank(0); rank<_path->getEdges().size(); ++rank)
				{
					result.push_back(NULL);
				}
			}

			return result;
		}



		SchedulesBasedService::PathBeginsWithUnscheduledStopException::PathBeginsWithUnscheduledStopException(
			const graph::Path& path
		):	Exception(
			"The path "+ boost::lexical_cast<std::string>(path.getKey()) +" begins with an unscheduled stop."
		) {}



		void SchedulesBasedService::setVertex(
			size_t rank,
			graph::Vertex* value
		){
			// Lock the vertices
			recursive_mutex::scoped_lock lock(getVerticesMutex());

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
			// Lock the vertices
			recursive_mutex::scoped_lock lock(getVerticesMutex());

			_RTVertices = value;

			// Inter-SYNTHESE sync
			if(Factory<InterSYNTHESESyncTypeFactory>::size()) // Avoid in unit tests
			{
				RealTimePTDataInterSYNTHESE::Content content(*this);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					boost::optional<db::DBTransaction&>(),
					this
				);
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Vertices getter (returned by reference).
		/// It is recommended to lock the _verticesMutex before calling this method :
		/// recursive_mutex::scoped_lock lock(getVerticesMutex());
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
			// Lock the vertices
			recursive_mutex::scoped_lock lock(getVerticesMutex());

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
			// Lock the vertices
			recursive_mutex::scoped_lock lock(getVerticesMutex());

			if(_vertices.size() != vertices.size())
			{
				_vertices = vertices;
			}
			else
			{
				for(size_t i(0); i<_path->getEdges().size(); ++i)
				{
					if (vertices[i])
					{
						setVertex(i, vertices[i]);
					}
				}
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
			const vector<time_period> &excludeRanges
		){
			BOOST_FOREACH(const time_period& period, excludeRanges)
			{
				if(period.contains(time))
				{
					if(period.end() > time + range)
					{
						// The forbidden slot goes after the whole range: not allowed
						return true;
					}
					else
					{
						// Shift after the forbidden time slot
						ptime newTime = period.end();
						ptime ptEnd(time); ptEnd += range;
						range = ptEnd - newTime;
						time = newTime;
					}
				}
				// Calc the new range
				if(time < period.begin() && range > period.begin() - time)
				{
					range = period.begin() - time;
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
			ptime baseTime(ptime(date, getDepartureBeginScheduleToIndex(false, departureEdge.getRankInPath())));

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
				return !isInTimeRange((time < baseTime ? baseTime : time), range, it->second);
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
				// Avoid non defined rules
				if(!rule->get<PriorityLine>())
				{
					continue;
				}

				CommercialLine& priorityLine(*rule->get<PriorityLine>());
				const CommercialLine::Paths& paths(priorityLine.getPaths());

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

							// Loop on services collections
							BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, path->getServiceCollections())
							{
								// Search a service at the time of the possible
								AccessParameters ap(userClassRank + USER_CLASS_CODE_OFFSET);
								optional<Edge::DepartureServiceIndex::Value> minServiceIndex;
					
								ptime minStartTime(date, getDepartureBeginScheduleToIndex(false, departureEdge.getRankInPath()));
								minStartTime -= rule->get<Delay>();
								ptime maxStartTime(date, getDepartureEndScheduleToIndex(false, departureEdge.getRankInPath()));
								maxStartTime += rule->get<Delay>();

								while(true)
								{
									ServicePointer serviceInstance(
										startEdge.getNextService(
												*itCollection,
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
												serviceInstance.getDepartureDateTime() - rule->get<Delay>(),
												serviceInstance.getDepartureDateTime() +
												serviceInstance.getServiceRange() + rule->get<Delay>()
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
			return !isInTimeRange((time < baseTime ? baseTime : time), range, excludeRanges);
		}



		void SchedulesBasedService::clearNonConcurrencyCache() const
		{
			recursive_mutex::scoped_lock serviceLock(_nonConcurrencyCacheMutex);
			_nonConcurrencyCache.clear();
		}



		void SchedulesBasedService::_clearGeneratedSchedules() const
		{
			recursive_mutex::scoped_lock lock(getSchedulesMutex());
			_generatedArrivalSchedules.clear();
			_generatedDepartureSchedules.clear();
		}



		void SchedulesBasedService::_generateSchedules() const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			// Clear existing data
			_generatedArrivalSchedules.clear();
			_generatedDepartureSchedules.clear();

			// Avoid multiple useless logs
			bool badSchedulesLogged(false);

			// Check if the data is not empty
			if(	_dataDepartureSchedules.empty() ||
				_dataArrivalSchedules.empty() ||
				_path->getEdges().empty()
			){
				return;
			}

			// Departure loop
			Schedules::const_iterator itDeparture(_dataDepartureSchedules.begin());
			Schedules::const_iterator itArrival(_dataArrivalSchedules.begin());
			if (dynamic_cast<JourneyPattern*>(_path))
			{
				JourneyPattern::LineStops::const_iterator lastDepartureScheduledLineStop(static_cast<JourneyPattern*>(_path)->getLineStops().end());
				time_duration lastDefinedDepartureSchedule;
				bool atLeastOneDepartureUnscheduledLineStop(false);
				optional<size_t> lastRankInPath;
				for(JourneyPattern::LineStops::const_iterator itLineStop(static_cast<JourneyPattern*>(_path)->getLineStops().begin()); itLineStop != static_cast<JourneyPattern*>(_path)->getLineStops().end(); ++itLineStop)
				{
					const LineStop& lineStop(**itLineStop);
					
					// Stop over DRT generated stops
					if(	(lastRankInPath && lineStop.get<RankInPath>() == *lastRankInPath))
					{
						continue;
					}

					JourneyPattern::LineStops::const_iterator itLineStop2(itLineStop);
					++itLineStop2;
					if(	lineStop.get<ScheduleInput>() &&
					(	(itDeparture != _dataDepartureSchedules.end() && !itDeparture->is_not_a_date_time()) ||
						(itLineStop2) == static_cast<JourneyPattern*>(_path)->getLineStops().end() ||
						itLineStop == static_cast<JourneyPattern*>(_path)->getLineStops().begin()
					)	){
						// In case of insufficient defined schedules number
						time_duration departureSchedule;
						if(	itDeparture == _dataDepartureSchedules.end()
						){
							if(!badSchedulesLogged)
							{
								Log::GetInstance().warn("Inconsistent schedules size in service "+ lexical_cast<string>(getKey()) +" (missing schedules)");
								badSchedulesLogged = true;
							}
							departureSchedule =
								_generatedDepartureSchedules.empty() ?
									seconds(0) :
									*_generatedDepartureSchedules.rbegin()
							;
						}
						else
						{
							departureSchedule = *itDeparture;
						}
						time_duration arrivalSchedule(not_a_date_time);
						if(	itArrival != _dataArrivalSchedules.end()
						){
							arrivalSchedule = *itArrival;
						}
						else
						{
							arrivalSchedule = departureSchedule;
						}

						// Interpolation of preceding departure schedules // RULE-214
						if(atLeastOneDepartureUnscheduledLineStop)
						{
							if(lastDepartureScheduledLineStop == static_cast<JourneyPattern*>(_path)->getLineStops().end())
							{
								throw PathBeginsWithUnscheduledStopException(*_path);
							}

							MetricOffset totalDistance(lineStop.get<MetricOffsetField>() - (*lastDepartureScheduledLineStop)->get<MetricOffsetField>());
							size_t totalRankDiff(lineStop.get<RankInPath>() - (*lastDepartureScheduledLineStop)->get<RankInPath>());
							time_duration totalTime(
								(	(	!lineStop.get<IsDeparture>() ||
								(lineStop.get<IsArrival>() && !arrivalSchedule.is_not_a_date_time() && arrivalSchedule < departureSchedule)
								) ?
									arrivalSchedule :
									departureSchedule
								) - lastDefinedDepartureSchedule
							);
							size_t lastRankInPath2((*lastDepartureScheduledLineStop)->get<RankInPath>());
							JourneyPattern::LineStops::const_iterator lastDepartureScheduledLineStop2(lastDepartureScheduledLineStop);
							++lastDepartureScheduledLineStop2;
							for(JourneyPattern::LineStops::const_iterator it(lastDepartureScheduledLineStop2); it != itLineStop && it != static_cast<JourneyPattern*>(_path)->getLineStops().end(); ++it)
							{
								// Stop over DRT generated stops
								if(	(*it)->get<RankInPath>() == lastRankInPath2
								){
									continue;
								}
								lastRankInPath2 = (*it)->get<RankInPath>();

								double minutesToAdd(0);
								if(totalDistance != 0)
								{
									MetricOffset distance((*it)->get<MetricOffsetField>() - (*lastDepartureScheduledLineStop)->get<MetricOffsetField>());
									minutesToAdd = (totalTime.total_seconds() / 60) * (distance / totalDistance);
								}
								else
								{
									assert(totalRankDiff);
									size_t rankDiff((*it)->get<RankInPath>() - (*lastDepartureScheduledLineStop)->get<RankInPath>());
									minutesToAdd = (totalTime.total_seconds() / 60) * (double(rankDiff) / double(totalRankDiff));
								}

								time_duration departureSchedule(lastDefinedDepartureSchedule);
								departureSchedule += minutes(
									static_cast<long>(floor(minutesToAdd))
								);
								_generatedDepartureSchedules.push_back(departureSchedule);
							}

							atLeastOneDepartureUnscheduledLineStop = false;
						}

						// Store the schedules
						_generatedDepartureSchedules.push_back(departureSchedule.is_not_a_date_time() ? arrivalSchedule : departureSchedule);
						lastDepartureScheduledLineStop = itLineStop;
						lastDefinedDepartureSchedule = departureSchedule;

						// Incrementation of schedules iterators
						if(itDeparture != _dataDepartureSchedules.end())
						{
							++itDeparture;
						}
						if(itArrival != _dataArrivalSchedules.end())
						{
							++itArrival;
						}
					}
					else
					{
						atLeastOneDepartureUnscheduledLineStop = true;

						// A schedule was ignored : we must jump over it
						if(lineStop.get<ScheduleInput>())
						{
							if(itDeparture != _dataDepartureSchedules.end() && itDeparture->is_not_a_date_time())
							{
								++itDeparture;
								if(itArrival != _dataArrivalSchedules.end())
								{
									++itArrival;
								}
							}
						}
					}
					lastRankInPath = lineStop.get<RankInPath>();
				}
				if(atLeastOneDepartureUnscheduledLineStop)
				{
					if(lastDepartureScheduledLineStop == static_cast<JourneyPattern*>(_path)->getLineStops().end())
					{
						throw PathBeginsWithUnscheduledStopException(*_path);
					}

					JourneyPattern::LineStops::const_iterator lastDepartureScheduledLineStop2(lastDepartureScheduledLineStop);
					++lastDepartureScheduledLineStop2;
					for(JourneyPattern::LineStops::const_iterator it(lastDepartureScheduledLineStop2); it != static_cast<JourneyPattern*>(_path)->getLineStops().end(); ++it)
					{
						_generatedDepartureSchedules.push_back(lastDefinedDepartureSchedule);
					}
				}

				// Check if there is too much schedules in the database
				if(itDeparture != _dataDepartureSchedules.end())
				{
					Log::GetInstance().warn("Inconsistent schedules size in service "+ lexical_cast<string>(getKey()) +" (too much schedules)");
				}

				// Arrival loop
				itDeparture = _dataDepartureSchedules.begin();
				itArrival = _dataArrivalSchedules.begin();
				JourneyPattern::LineStops::const_iterator lastArrivalScheduledLineStop(static_cast<JourneyPattern*>(_path)->getLineStops().begin());
				time_duration lastDefinedArrivalSchedule(*_dataDepartureSchedules.begin());
				bool atLeastOneArrivalUnscheduledLineStop(false);
				Schedules::const_iterator itGeneratedDepartureSchedules(_generatedDepartureSchedules.begin());
				bool neverSeenAScheduledLineStop(true);
				lastRankInPath = boost::none;
				for(JourneyPattern::LineStops::const_iterator itLineStop(static_cast<JourneyPattern*>(_path)->getLineStops().begin()); itLineStop != static_cast<JourneyPattern*>(_path)->getLineStops().end(); ++itLineStop)
				{
					const LineStop& lineStop(**itLineStop);

					if(	lastRankInPath && lineStop.get<RankInPath>() == *lastRankInPath
					){
						continue;
					}

					JourneyPattern::LineStops::const_iterator itLineStop2(itLineStop);
					++itLineStop2;
					if(	lineStop.get<ScheduleInput>() &&
						(	(itArrival != _dataArrivalSchedules.end() && !itArrival->is_not_a_date_time()) ||
						(itLineStop2) == static_cast<JourneyPattern*>(_path)->getLineStops().end() ||
						itLineStop == static_cast<JourneyPattern*>(_path)->getLineStops().begin()
					)	){
						neverSeenAScheduledLineStop = false;

						// In case of insufficient defined schedules number
						time_duration arrivalSchedule;
						if(	itArrival == _dataArrivalSchedules.end()
						){
							if(!badSchedulesLogged)
							{
								Log::GetInstance().warn("Inconsistent schedules size in service "+ lexical_cast<string>(getKey()) +" (missing schedules)");
								badSchedulesLogged = true;
							}
							arrivalSchedule =
								_generatedArrivalSchedules.empty() ?
									seconds(0) :
									*_generatedArrivalSchedules.rbegin()
							;
						}
						else
						{
							arrivalSchedule = *itArrival;
						}
						time_duration departureSchedule(not_a_date_time);
						if(	itDeparture != _dataDepartureSchedules.end()
						){
							departureSchedule = *itDeparture;
						}
						else
						{
							departureSchedule = arrivalSchedule;
						}

						// Interpolation of preceding schedules
						if(atLeastOneArrivalUnscheduledLineStop)
						{
							if(lastArrivalScheduledLineStop == static_cast<JourneyPattern*>(_path)->getLineStops().end())
							{
								throw PathBeginsWithUnscheduledStopException(*_path);
							}

							MetricOffset totalDistance(lineStop.get<MetricOffsetField>() - (*lastArrivalScheduledLineStop)->get<MetricOffsetField>());
							size_t totalRankDiff(lineStop.get<RankInPath>() - (*lastArrivalScheduledLineStop)->get<RankInPath>());
							time_duration totalTime(arrivalSchedule - lastDefinedArrivalSchedule);
							size_t lastRankInPath2((*lastArrivalScheduledLineStop)->get<RankInPath>());
							JourneyPattern::LineStops::const_iterator lastArrivalScheduledLineStop2(lastArrivalScheduledLineStop);
							++lastArrivalScheduledLineStop2;
							for(JourneyPattern::LineStops::const_iterator it(lastArrivalScheduledLineStop2); it != itLineStop && it != static_cast<JourneyPattern*>(_path)->getLineStops().end(); ++it)
							{
								// Stop over DRT generated stops
								if(	(*it)->get<RankInPath>() == lastRankInPath2
								){
									continue;
								}
								lastRankInPath2 = (*it)->get<RankInPath>();

								double minutesToAdd(0);
								if(totalDistance != 0)
								{
									MetricOffset distance((*it)->get<MetricOffsetField>() - (*lastArrivalScheduledLineStop)->get<MetricOffsetField>());
									minutesToAdd = (totalTime.total_seconds() / 60) * (distance / totalDistance);
								}
								else
								{
									assert(totalRankDiff);
									size_t rankDiff((*it)->get<RankInPath>() - (*lastArrivalScheduledLineStop)->get<RankInPath>());
									minutesToAdd = (totalTime.total_seconds() / 60) * (double(rankDiff) / double(totalRankDiff));
								}

								time_duration arrivalSchedule(lastDefinedArrivalSchedule);
								arrivalSchedule += minutes(
									static_cast<long>(ceil(minutesToAdd))
								);
								_generatedArrivalSchedules.push_back(arrivalSchedule);
							}

							atLeastOneArrivalUnscheduledLineStop = false;
						}

						// Store the schedules
						_generatedArrivalSchedules.push_back(arrivalSchedule.is_not_a_date_time() ? departureSchedule : arrivalSchedule);
						lastArrivalScheduledLineStop = itLineStop;
						lastDefinedArrivalSchedule =
							(lineStop.get<IsDeparture>() && !departureSchedule.is_not_a_date_time() && departureSchedule > arrivalSchedule) ?
								departureSchedule :
								arrivalSchedule
						;

						// Increment iterators
						if(itDeparture != _dataDepartureSchedules.end())
						{
							++itDeparture;
						}
						if(itArrival != _dataArrivalSchedules.end())
						{
							++itArrival;
						}
					}
					else
					{
						if(	neverSeenAScheduledLineStop)
						{
							if(itGeneratedDepartureSchedules != _generatedDepartureSchedules.end())
							{
								_generatedArrivalSchedules.push_back(*itGeneratedDepartureSchedules);
								++itGeneratedDepartureSchedules;
							}
							else
							{
								_generatedArrivalSchedules.push_back(*_generatedDepartureSchedules.rbegin());
							}
						}
						else
						{
							atLeastOneArrivalUnscheduledLineStop = true;
						}

						// A schedule was ignored : we must jump over it
						if(lineStop.get<ScheduleInput>())
						{
							if(itArrival != _dataArrivalSchedules.end() && itArrival->is_not_a_date_time())
							{
								++itArrival;
								if(itDeparture != _dataDepartureSchedules.end())
								{
									++itDeparture;
								}
							}
						}
					}

					lastRankInPath = lineStop.get<RankInPath>();
				}
				if(atLeastOneArrivalUnscheduledLineStop)
				{
					if(lastArrivalScheduledLineStop == static_cast<JourneyPattern*>(_path)->getLineStops().end())
					{
						throw PathBeginsWithUnscheduledStopException(*_path);
					}

					JourneyPattern::LineStops::const_iterator lastArrivalScheduledLineStop2(lastArrivalScheduledLineStop);
					++lastArrivalScheduledLineStop2;
					for(JourneyPattern::LineStops::const_iterator it(lastArrivalScheduledLineStop2); it != static_cast<JourneyPattern*>(_path)->getLineStops().end(); ++it)
					{
						_generatedArrivalSchedules.push_back(lastDefinedArrivalSchedule);
					}
				}

				// Check if there is too much schedules in the database
				if(itArrival != _dataArrivalSchedules.end())
				{
					Log::GetInstance().warn("Inconsistent schedules size in service "+ lexical_cast<string>(getKey()) +" (too much schedules)");
				}
			}
			else
			{
				Path::Edges::const_iterator lastDepartureScheduledEdge(_path->getEdges().end());
				time_duration lastDefinedDepartureSchedule;
				bool atLeastOneDepartureUnscheduledEdge(false);
				optional<size_t> lastRankInPath;
				for(Path::Edges::const_iterator itEdge(_path->getEdges().begin()); itEdge != _path->getEdges().end(); ++itEdge)
				{
					const Edge& edge(**itEdge);

					// Stop over DRT generated stops
					if(	(lastRankInPath && edge.getRankInPath() == *lastRankInPath)
					){
						continue;
					}

					if(	edge.getScheduleInput() &&
						(	(itDeparture != _dataDepartureSchedules.end() && !itDeparture->is_not_a_date_time()) ||
							(itEdge+1) == _path->getEdges().end() ||
							itEdge == _path->getEdges().begin()
					)	){
						// In case of insufficient defined schedules number
						time_duration departureSchedule;
						if(	itDeparture == _dataDepartureSchedules.end() 
						){
							if(!badSchedulesLogged)
							{
								Log::GetInstance().warn("Inconsistent schedules size in service "+ lexical_cast<string>(getKey()) +" (missing schedules)");
								badSchedulesLogged = true;
							}
							departureSchedule =
								_generatedDepartureSchedules.empty() ?
								seconds(0) :
								*_generatedDepartureSchedules.rbegin()
							;
						}
						else
						{
							departureSchedule = *itDeparture;
						}
						time_duration arrivalSchedule(not_a_date_time);
						if(	itArrival != _dataArrivalSchedules.end() 
						){
							arrivalSchedule = *itArrival;
						}
						else
						{
							arrivalSchedule = departureSchedule;
						}

						// Interpolation of preceding departure schedules
						if(atLeastOneDepartureUnscheduledEdge)
						{
							if(lastDepartureScheduledEdge == _path->getEdges().end())
							{
								throw PathBeginsWithUnscheduledStopException(*_path);
							}

							MetricOffset totalDistance(edge.getMetricOffset() - (*lastDepartureScheduledEdge)->getMetricOffset());
							size_t totalRankDiff(edge.getRankInPath() - (*lastDepartureScheduledEdge)->getRankInPath());
							time_duration totalTime(
								(	(	!edge.isDepartureAllowed() ||
										(edge.isArrivalAllowed() && !arrivalSchedule.is_not_a_date_time() && arrivalSchedule < departureSchedule)
									) ?
									arrivalSchedule :
									departureSchedule
								) - lastDefinedDepartureSchedule
							);
							size_t lastRankInPath2((*lastDepartureScheduledEdge)->getRankInPath());
							for(Path::Edges::const_iterator it(lastDepartureScheduledEdge+1); it != itEdge && it != _path->getEdges().end(); ++it)
							{
								// Stop over DRT generated stops
								if(	(*it)->getRankInPath() == lastRankInPath2
								){
									continue;
								}
								lastRankInPath2 = (*it)->getRankInPath();

								double minutesToAdd(0);
								if(totalDistance != 0)
								{
									MetricOffset distance((*it)->getMetricOffset() - (*lastDepartureScheduledEdge)->getMetricOffset());
									minutesToAdd = (totalTime.total_seconds() / 60) * (distance / totalDistance);
								}
								else
								{
									assert(totalRankDiff);
									size_t rankDiff((*it)->getRankInPath() - (*lastDepartureScheduledEdge)->getRankInPath());
									minutesToAdd = (totalTime.total_seconds() / 60) * (double(rankDiff) / double(totalRankDiff));
								}

								time_duration departureSchedule(lastDefinedDepartureSchedule);
								departureSchedule += minutes(
									static_cast<long>(floor(minutesToAdd))
								);
								_generatedDepartureSchedules.push_back(departureSchedule);
							}

							atLeastOneDepartureUnscheduledEdge = false;
						}

						// Store the schedules
						_generatedDepartureSchedules.push_back(departureSchedule.is_not_a_date_time() ? arrivalSchedule : departureSchedule);
						lastDepartureScheduledEdge = itEdge;
						lastDefinedDepartureSchedule = departureSchedule;

						// Incrementation of schedules iterators
						if(itDeparture != _dataDepartureSchedules.end())
						{
							++itDeparture;
					}
						if(itArrival != _dataArrivalSchedules.end())
						{
							++itArrival;
						}
					}
					else
					{
						atLeastOneDepartureUnscheduledEdge = true;

						// A schedule was ignored : we must jump over it
						if(edge.getScheduleInput())
					{
							if(itDeparture != _dataDepartureSchedules.end() && itDeparture->is_not_a_date_time())
						{
							++itDeparture;
						if(itArrival != _dataArrivalSchedules.end())
						{
							++itArrival;
						}
					}
				}
					}
					lastRankInPath = edge.getRankInPath();
				}
				if(atLeastOneDepartureUnscheduledEdge)
				{
					if(lastDepartureScheduledEdge == _path->getEdges().end())
					{
						throw PathBeginsWithUnscheduledStopException(*_path);
					}

					for(Path::Edges::const_iterator it(lastDepartureScheduledEdge+1); it != _path->getEdges().end(); ++it)
					{
						_generatedDepartureSchedules.push_back(lastDefinedDepartureSchedule);
					}
				}

				// Check if there is too much schedules in the database
				if(itDeparture != _dataDepartureSchedules.end())
				{
					Log::GetInstance().warn("Inconsistent schedules size in service "+ lexical_cast<string>(getKey()) +" (too much schedules)");
				}
				

				// Arrival loop
				itDeparture = _dataDepartureSchedules.begin();
				itArrival = _dataArrivalSchedules.begin();
				Path::Edges::const_iterator lastArrivalScheduledEdge(_path->getEdges().begin());
				time_duration lastDefinedArrivalSchedule(*_dataDepartureSchedules.begin());
				bool atLeastOneArrivalUnscheduledEdge(false);
				Schedules::const_iterator itGeneratedDepartureSchedules(_generatedDepartureSchedules.begin());
				bool neverSeenAScheduledEdge(true);
				lastRankInPath = boost::none;
				for(Path::Edges::const_iterator itEdge(_path->getEdges().begin()); itEdge != _path->getEdges().end(); ++itEdge)
				{
					const Edge& edge(**itEdge);

					if(	lastRankInPath && edge.getRankInPath() == *lastRankInPath
					){
						continue;
					}

					if(	edge.getScheduleInput() &&
						(	(itArrival != _dataArrivalSchedules.end() && !itArrival->is_not_a_date_time()) ||
							(itEdge+1) == _path->getEdges().end() ||
							itEdge == _path->getEdges().begin()
					)	){
						neverSeenAScheduledEdge = false;

						// In case of insufficient defined schedules number
						time_duration arrivalSchedule;
						if(	itArrival == _dataArrivalSchedules.end() 
						){
							if(!badSchedulesLogged)
							{
								Log::GetInstance().warn("Inconsistent schedules size in service "+ lexical_cast<string>(getKey()) +" (missing schedules)");
								badSchedulesLogged = true;
							}
							arrivalSchedule =
								_generatedArrivalSchedules.empty() ?
								seconds(0) :
								*_generatedArrivalSchedules.rbegin()
							;
				
						}
						else
						{
							arrivalSchedule = *itArrival;
						}
						time_duration departureSchedule(not_a_date_time);
						if(	itDeparture != _dataDepartureSchedules.end() 
						){
							departureSchedule = *itDeparture;
						}
						else
						{
							departureSchedule = arrivalSchedule;
						}

						// Interpolation of preceding schedules
						if(atLeastOneArrivalUnscheduledEdge)
						{
							if(lastArrivalScheduledEdge == _path->getEdges().end())
							{
								throw PathBeginsWithUnscheduledStopException(*_path);
							}

							MetricOffset totalDistance(edge.getMetricOffset() - (*lastArrivalScheduledEdge)->getMetricOffset());
							size_t totalRankDiff(edge.getRankInPath() - (*lastArrivalScheduledEdge)->getRankInPath());
							time_duration totalTime(arrivalSchedule - lastDefinedArrivalSchedule);
							size_t lastRankInPath2((*lastArrivalScheduledEdge)->getRankInPath());
							for(Path::Edges::const_iterator it(lastArrivalScheduledEdge+1); it != itEdge && it != _path->getEdges().end(); ++it)
							{
								// Stop over DRT generated stops
								if(	(*it)->getRankInPath() == lastRankInPath2
								){
									continue;
								}
								lastRankInPath2 = (*it)->getRankInPath();

								double minutesToAdd(0);
								if(totalDistance != 0)
								{
									MetricOffset distance((*it)->getMetricOffset() - (*lastArrivalScheduledEdge)->getMetricOffset());
									minutesToAdd = (totalTime.total_seconds() / 60) * (distance / totalDistance);
								}
								else
								{
									assert(totalRankDiff);
									size_t rankDiff((*it)->getRankInPath() - (*lastArrivalScheduledEdge)->getRankInPath());
									minutesToAdd = (totalTime.total_seconds() / 60) * (double(rankDiff) / double(totalRankDiff));
								}

								time_duration arrivalSchedule(lastDefinedArrivalSchedule);
								arrivalSchedule += minutes(
									static_cast<long>(ceil(minutesToAdd))
								);
								_generatedArrivalSchedules.push_back(arrivalSchedule);
							}

							atLeastOneArrivalUnscheduledEdge = false;
						}

						// Store the schedules
						_generatedArrivalSchedules.push_back(arrivalSchedule.is_not_a_date_time() ? departureSchedule : arrivalSchedule);
						lastArrivalScheduledEdge = itEdge;
						lastDefinedArrivalSchedule = 
							(edge.isDepartureAllowed() && !departureSchedule.is_not_a_date_time() && departureSchedule > arrivalSchedule) ?
							departureSchedule :
							arrivalSchedule
						;

						// Increment iterators
						if(itDeparture != _dataDepartureSchedules.end())
						{
							++itDeparture;
					}
						if(itArrival != _dataArrivalSchedules.end())
						{
							++itArrival;
						}
					}
					else
					{
						if(	neverSeenAScheduledEdge)
						{
							if(itGeneratedDepartureSchedules != _generatedDepartureSchedules.end())
							{
								_generatedArrivalSchedules.push_back(*itGeneratedDepartureSchedules);
								++itGeneratedDepartureSchedules;
							}
							else
							{
								_generatedArrivalSchedules.push_back(*_generatedDepartureSchedules.rbegin());
							}
						}
						else
						{
							atLeastOneArrivalUnscheduledEdge = true;
						}

						// A schedule was ignored : we must jump over it
						if(edge.getScheduleInput())
					{
							if(itArrival != _dataArrivalSchedules.end() && itArrival->is_not_a_date_time())
							{
								++itArrival;
						if(itDeparture != _dataDepartureSchedules.end())
						{
							++itDeparture;
						}
						}
					}
				}

					lastRankInPath = edge.getRankInPath();
				}
				if(atLeastOneArrivalUnscheduledEdge)
				{
					if(lastArrivalScheduledEdge == _path->getEdges().end())
					{
						throw PathBeginsWithUnscheduledStopException(*_path);
					}

					for(Path::Edges::const_iterator it(lastArrivalScheduledEdge+1); it != _path->getEdges().end(); ++it)
					{
						_generatedArrivalSchedules.push_back(lastDefinedArrivalSchedule);
					}
				}

				// Check if there is too much schedules in the database
				if(itArrival != _dataArrivalSchedules.end())
				{
					Log::GetInstance().warn("Inconsistent schedules size in service "+ lexical_cast<string>(getKey()) +" (too much schedules)");
				}
			}
		}



		// @FIXME: Should also return true if there is a RealTimeVertex change
		bool SchedulesBasedService::hasRealTimeData() const
		{
			return !_RTArrivalSchedules.empty();
		}



		bool SchedulesBasedService::respectsLineTheoryWith( const Service& other ) const
		{
			// Read the other service as schedules based service
			if(!dynamic_cast<const SchedulesBasedService*>(&other))
			{
				return true;
			}
			const SchedulesBasedService& sother(static_cast<const SchedulesBasedService&>(other));

			// Lock the schedules
			recursive_mutex::scoped_lock lock1(getSchedulesMutex());
			recursive_mutex::scoped_lock lock2(sother.getSchedulesMutex());

			if(	_dataDepartureSchedules.size() != sother._dataDepartureSchedules.size() ||
				_dataArrivalSchedules.size() != sother._dataArrivalSchedules.size() ||
				_dataDepartureSchedules.size() != _dataArrivalSchedules.size()
			){
				Log::GetInstance().warn("Inconsistent schedules size in "+ lexical_cast<string>(getKey()));
				return false;
			}

			// Loop on each stop
			bool timeOrder;
			bool orderDefined(false);
			
			size_t i(0);
			const Path::Edges& edges(getPath()->getEdges());
			for(Path::Edges::const_iterator it(edges.begin()); it != edges.end(); ++it)
			{
				const Edge& edge(**it);

				// Jump over stops with interpolated schedules
				if(!edge.getScheduleInput())
				{
					continue;
				}

				if(edge.isDeparture())
				{
					/// - Test 1 : Conflict between continuous service range or identical schedule
					if(	getDataFirstDepartureSchedule(i) <= sother.getDataLastDepartureSchedule(i) &&
						getDataLastDepartureSchedule(i) >= sother.getDataLastDepartureSchedule(i)
					){
						return false;
					}

					/// - Test 2 : Order of times
					if (!orderDefined)
					{
						timeOrder = (getDataFirstDepartureSchedule(i) < sother.getDataFirstDepartureSchedule(i));
						orderDefined = true;
					}
					else
					{
						if ((getDataFirstDepartureSchedule(i) < sother.getDataFirstDepartureSchedule(i)) != timeOrder)
						{
							return false;
						}
					}
				}
				if (edge.isArrival())
				{
					/// - Test 1 : Conflict between continuous service range or identical schedule
					if(	getDataFirstArrivalSchedule(i) <= sother.getDataLastArrivalSchedule(i) &&
						getDataLastArrivalSchedule(i) >= sother.getDataFirstArrivalSchedule(i)
					){
						return false;
					}

					/// - Test 2 : Order of times
					if (!orderDefined)
					{
						timeOrder = (getDataFirstArrivalSchedule(i) < sother.getDataFirstArrivalSchedule(i));
						orderDefined = true;
					}
					else
					{
						if((getDataFirstArrivalSchedule(i) < sother.getDataFirstArrivalSchedule(i)) != timeOrder)
						{
							return false;
						}
					}
				}
				++i;
			}

			// No failure : return OK
			return true;
		}



		const boost::posix_time::time_duration SchedulesBasedService::getDataFirstDepartureSchedule( size_t i ) const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			if(i >= _dataDepartureSchedules.size())
			{
				return *_dataDepartureSchedules.rbegin();
			}
			return _dataDepartureSchedules.at(i);
		}



		const boost::posix_time::time_duration SchedulesBasedService::getDataFirstArrivalSchedule( size_t i ) const
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			if(i >= _dataArrivalSchedules.size())
			{
				return *_dataArrivalSchedules.rbegin();
			}
			return _dataArrivalSchedules.at(i);
		}



		const boost::posix_time::time_duration SchedulesBasedService::getDataLastDepartureSchedule( size_t i ) const
		{
			return getDataFirstDepartureSchedule(i);
		}



		const boost::posix_time::time_duration SchedulesBasedService::getDataLastArrivalSchedule( size_t i ) const
		{
			return getDataFirstArrivalSchedule(i);
		}



		void SchedulesBasedService::regenerateDataSchedules()
		{
			if(!_path)
			{
				throw Exception("A path is needed");
			}

			// Lock the schedules
			recursive_mutex::scoped_lock lock(getSchedulesMutex());

			Schedules newDepartureSchedules;
			Schedules newArrivalSchedules;
			
			size_t rank(0);
			optional<size_t> lastRank;
			if(dynamic_cast<JourneyPattern*>(_path))
			{
				static_cast<JourneyPattern*>(getPath())->clearCalendarCache();
				BOOST_FOREACH(LineStop* lineStop, static_cast<JourneyPattern*>(_path)->getLineStops())
				{
					// Jump over edges generated by the same object
					if(lastRank && *lastRank == lineStop->get<RankInPath>())
					{
						continue;
					}
					lastRank = lineStop->get<RankInPath>();

					// Jump over stops with interpolated schedules
					if(!lineStop->get<ScheduleInput>())
					{
						++rank;
						continue;
					}

					newDepartureSchedules.push_back(getDepartureSchedule(false, rank));
					newArrivalSchedules.push_back(getArrivalSchedule(false, rank));

					++rank;
				}
			}
			else
			{
				BOOST_FOREACH(const Path::Edges::value_type& itEdge, _path->getEdges())
				{
					// Jump over edges generated by the same object
					if(lastRank && *lastRank == itEdge->getRankInPath())
					{
						continue;
					}
					lastRank = itEdge->getRankInPath();

					// Jump over stops with interpolated schedules
					if(!itEdge->getScheduleInput())
					{
						++rank;
						continue;
					}

					newDepartureSchedules.push_back(getDepartureSchedule(false, rank));
					newArrivalSchedules.push_back(getArrivalSchedule(false, rank));

					++rank;
				}
			}

			setDataSchedules(newDepartureSchedules, newArrivalSchedules);
		}



		bool operator==(const SchedulesBasedService& first, const SchedulesBasedService& second)
		{
			// Lock the schedules
			recursive_mutex::scoped_lock lock1(first.getSchedulesMutex());
			recursive_mutex::scoped_lock lock2(second.getSchedulesMutex());

			return
				first.getPath() == second.getPath() &&
				first.getServiceNumber() == second.getServiceNumber() &&
				first.getDepartureSchedules(true, false) == second.getDepartureSchedules(true, false) &&
				first.getArrivalSchedules(true, false) == second.getArrivalSchedules(true, false)
			;
		}
}	}
