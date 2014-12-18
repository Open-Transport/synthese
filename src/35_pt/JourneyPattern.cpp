
/** JourneyPattern class implementation.
	@file JourneyPattern.cpp

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

#include "JourneyPattern.hpp"

#include "CommercialLineTableSync.h"
#include "DataSourceLinksField.hpp"
#include "DBConstants.h"
#include "DestinationTableSync.hpp"
#include "ImportableTableSync.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStop.h"
#include "LinePhysicalStop.hpp"
#include "Log.h"
#include "NonPermanentService.h"
#include "PTUseRuleTableSync.h"
#include "Registry.h"
#include "RollingStockTableSync.hpp"
#include "TransportNetwork.h"
#include "Service.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace geography;
	using namespace graph;
	using namespace util;
	using namespace impex;
	using namespace pt;
	using namespace calendar;
	using namespace vehicle;

	namespace util
	{
		template<> const std::string Registry<pt::JourneyPattern>::KEY("JourneyPattern");
	}

	namespace pt
	{
		const string JourneyPattern::ATTR_DIRECTION_TEXT = "direction_text";



		JourneyPattern::JourneyPattern(
			util::RegistryKeyType id,
			std::string name
		):	util::Registrable(id),
			Path(),
			_directionObj(NULL),
			_isWalkingLine (false),
			_main(false),
			_wayBack(false),
			_plannedLength(0)
		{}



		JourneyPattern::~JourneyPattern ()
		{
			boost::unique_lock<shared_recursive_mutex> lock(*sharedServicesMutex);
			BOOST_FOREACH(Service* service, getAllServices())
			{
				service->setPath(NULL);
			}
		}



		const std::string&
		JourneyPattern::getDirection () const
		{
			return _direction;
		}



		void
		JourneyPattern::setDirection (const std::string& direction)
		{
			_direction = direction;
		}



		const std::string&
		JourneyPattern::getTimetableName () const
		{
			return _timetableName;
		}



		void
		JourneyPattern::setTimetableName (const std::string& timetableName)
		{
			_timetableName = timetableName;
		}



		RollingStock*	JourneyPattern::getRollingStock () const
		{
			return static_cast<RollingStock*>(_pathClass);
		}



		TransportNetwork*   JourneyPattern::getNetwork () const
        {
            return static_cast<TransportNetwork*>(_pathNetwork);
        }



		void JourneyPattern::setRollingStock(RollingStock* rollingStock)
		{
			_pathClass = static_cast<PathClass*>(rollingStock);
		}



		void JourneyPattern::setNetwork(TransportNetwork* transportNetwork)
        {
             _pathNetwork = static_cast<PathClass*>(transportNetwork);
        }



		void JourneyPattern::setWalkingLine (bool isWalkingLine)
		{
			_isWalkingLine = isWalkingLine;
		}



		bool JourneyPattern::getWalkingLine () const
		{
			return _isWalkingLine;
		}



		void JourneyPattern::setCommercialLine(CommercialLine* commercialLine )
		{
			_pathGroup = commercialLine;
		}



		CommercialLine* JourneyPattern::getCommercialLine() const
		{
			return static_cast<CommercialLine*>(_pathGroup);
		}



		bool JourneyPattern::isPedestrianMode() const
		{
			return getWalkingLine();
		}



		bool JourneyPattern::operator==(const std::vector<StopPoint*>& stops) const
		{
			if(getEdges().size() != stops.size()) return false;

			size_t rank(0);
			BOOST_FOREACH(const Edge* edge, getEdges())
			{
				if(static_cast<const LinePhysicalStop*>(edge)->getFromVertex() != stops[rank]) return false;
				++rank;
			}

			return true;
		}



		bool JourneyPattern::operator==(
			const StopsWithDepartureArrivalAuthorization& stops
		) const	{
			if(getEdges().size() != stops.size())
			{
				return false;
			}

			size_t rank(0);
			BOOST_FOREACH(const Edge* edge, getEdges())
			{
				const StopsWithDepartureArrivalAuthorization::value_type& stop(stops[rank]);
				if( stop._stop.find(static_cast<StopPoint*>(edge->getFromVertex())) == stop._stop.end() ||
					(rank > 0 && rank+1 < stops.size() && (edge->isDeparture() != stop._departure || edge->isArrival() != stop._arrival)) ||
					((stop._withTimes && dynamic_cast<const LinePhysicalStop*>(edge) &&
					  *stop._withTimes != static_cast<const LinePhysicalStop*>(edge)->getScheduleInput())) ||
					(stop._metricOffset && stop._metricOffset != edge->getMetricOffset())
				){
					return false;
				}
				++rank;
			}

			return true;
		}



		bool JourneyPattern::operator==( const JourneyPattern& other ) const
		{
			if(_lineStops.size() != other._lineStops.size())
			{
				return false;
			}
			if(_lineStops.empty())
			{
				return true;
			}

			LineStops::const_iterator it2(other._lineStops.begin());
			for(LineStops::const_iterator it1(_lineStops.begin());
				it1 != _lineStops.end();
				++it1, ++it2
			){
				LineStop& ls1(**it1);
				LineStop& ls2(**it2);
				if( (ls1.get<LineNode>() && !ls2.get<LineNode>()) ||
					(!ls1.get<LineNode>() && ls2.get<LineNode>()) ||
					(ls1.get<LineNode>() && ls2.get<LineNode>() && &*ls1.get<LineNode>() != &*ls2.get<LineNode>()) ||
					(it1 != _lineStops.begin() && ls1.get<IsDeparture>() != ls2.get<IsDeparture>()) ||
					(*it1 != *_lineStops.rbegin() && ls1.get<IsArrival>() != ls2.get<IsArrival>()) ||
					(ls1.get<ScheduleInput>() != ls2.get<ScheduleInput>()) ||
					ls1.get<MetricOffsetField>() != ls2.get<MetricOffsetField>()
				){
					return false;
				}
			}

			return true;
		}



		const LineStop* JourneyPattern::getLineStop(
			std::size_t rank,
			bool ignoreUnscheduledStops
		) const	{
			if(ignoreUnscheduledStops)
			{
				size_t edgeRank(0);
				BOOST_FOREACH(const LineStop* edge, _lineStops)
				{
					if(	edge->get<RankInPath>() > 0 &&
						edge->get<ScheduleInput>()
					){
						++edgeRank;
					}
					if(rank == edgeRank)
					{
						return edge;
					}
				}
				return NULL;
			}
			else
			{
				BOOST_FOREACH(const LineStop* edge, _lineStops)
				{
					if(	edge->get<RankInPath>() == rank)
					{
						return edge;
					}
					if(edge->get<RankInPath>() > rank)
					{
						return NULL;
					}
				}
			}
			return NULL;
		}



		bool JourneyPattern::isActive( const boost::gregorian::date& date ) const
		{
			return getCalendarCache().isActive(date);
		}



		std::size_t JourneyPattern::getScheduledStopsNumber() const
		{
			size_t result(0);
			BOOST_FOREACH(const LineStop* edge, _lineStops)
			{
				if(	edge->get<ScheduleInput>()
				){
					++result;
				}
			}
			return result;
		}



		std::string JourneyPattern::getRuleUserName() const
		{
			return "Parcours " + getName();
		}



		bool JourneyPattern::callsAtCity( const City& city ) const
		{
			BOOST_FOREACH(Edge* edge, getEdges())
			{
				if(	static_cast<StopPoint*>(edge->getFromVertex())->getConnectionPlace()->getCity() == &city
				){
					return true;
				}
			}
			return false;
		}



		bool JourneyPattern::compareStopAreas( const StopsWithDepartureArrivalAuthorization& stops ) const
		{
			if(getEdges().size() != stops.size())
			{
				return false;
			}

			size_t rank(0);
			BOOST_FOREACH(const Edge* edge, getEdges())
			{
				const StopsWithDepartureArrivalAuthorization::value_type& stop(stops[rank]);

				// Check if a stop can be recognized
				bool ok(false);
				BOOST_FOREACH(const StopWithDepartureArrivalAuthorization::StopsSet::value_type& oStop, stop._stop)
				{
					if(oStop->getHub() == edge->getFromVertex()->getHub())
					{
						ok = true;
						break;
					}
				}
				if(!ok)
				{
					return false;
				}

				if( (rank > 0 && rank+1 < stops.size() && (edge->isDeparture() != stop._departure || edge->isArrival() != stop._arrival)) ||
					((stop._withTimes && dynamic_cast<const LinePhysicalStop*>(edge) &&
					  *stop._withTimes != static_cast<const LinePhysicalStop*>(edge)->getScheduleInput())) ||
					(stop._metricOffset && stop._metricOffset != edge->getMetricOffset())
				){
					return false;
				}
				++rank;
			}

			return true;
		}



		void JourneyPattern::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles /*= boost::logic::indeterminate*/,
			std::string prefix /*= std::string() */
		) const	{

			if(!getCommercialLine() && !getEdges().size())
			{
				return;
			}

			if(!getCommercialLine())
			{
				throw Exception("JourneyPattern save error. "
								"Missing commercial line for JourneyPattern " +
								lexical_cast<string>(getKey())
				);
			}

			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID,
				getCommercialLine()->getKey()
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_NAME,
				getName()
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_TIMETABLENAME,
				getTimetableName()
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_DIRECTION,
				getDirection()
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_DIRECTION_ID,
				getDirectionObj() ? getDirectionObj()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_ISWALKINGLINE,
				getWalkingLine()
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_ROLLINGSTOCKID,
				getRollingStock() ? getRollingStock()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_BIKECOMPLIANCEID,
				(	getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
					static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_HANDICAPPEDCOMPLIANCEID,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_PEDESTRIANCOMPLIANCEID,
				(	getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
					static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_WAYBACK,
				getWayBack()
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_DATASOURCE_ID,
				synthese::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_MAIN,
				getMain()
			);
			pm.insert(
				prefix + JourneyPatternTableSync::COL_PLANNED_LENGTH,
				getPlannedLength()
			);
			pm.insert(prefix + ATTR_DIRECTION_TEXT, _direction);
		}



		JourneyPattern::StopWithDepartureArrivalAuthorization::StopWithDepartureArrivalAuthorization(
			const std::set<StopPoint*>& stop,
			boost::optional<MetricOffset> metricOffset /*= boost::optional<MetricOffset>()*/,
			bool departure /*= true*/,
			bool arrival /*= true */,
			boost::optional<bool> withTimes,
			boost::shared_ptr<geos::geom::LineString> geometry
		):	_metricOffset(metricOffset),
			_stop(stop),
			_departure(departure),
			_arrival(arrival),
			_withTimes(withTimes),
			_geometry(geometry)
		{}



		bool JourneyPattern::loadFromRecord(
			const Record& record,
			util::Env& env
		){
			bool result(false);

			// Name
			if(record.isDefined(JourneyPatternTableSync::COL_NAME))
			{
				string name (
				    record.get<string>(JourneyPatternTableSync::COL_NAME)
				);
				if(name != _name)
				{
					_name = name;
					result = true;
				}
			}

			// Timetable name
			if(record.isDefined(JourneyPatternTableSync::COL_TIMETABLENAME))
			{
				string timetableName(
				    record.get<string>(JourneyPatternTableSync::COL_TIMETABLENAME)
				);
				if(timetableName != _timetableName)
				{
					_timetableName = timetableName;
					result = true;
				}
			}

			// Direction
			if(record.isDefined(JourneyPatternTableSync::COL_DIRECTION))
			{
				string direction(
				    record.get<string>(JourneyPatternTableSync::COL_DIRECTION)
				);
				if(direction != _direction)
				{
					_direction = direction;
					result = true;
				}
			}

			// Is walking line
			if(record.isDefined(JourneyPatternTableSync::COL_ISWALKINGLINE))
			{
				bool isWalkingLine(
					record.getDefault<bool>(
						JourneyPatternTableSync::COL_ISWALKINGLINE, false
				)	);
				if(isWalkingLine != _isWalkingLine)
				{
					_isWalkingLine = isWalkingLine;
					result = true;
				}
			}

			// Wayback
			if(record.isDefined(JourneyPatternTableSync::COL_WAYBACK))
			{
				bool value(
					record.getDefault<bool>(
						JourneyPatternTableSync::COL_WAYBACK, false
				)	);
				if(value != _wayBack)
				{
					_wayBack = value;
					result = true;
				}
			}

			// Main
			if(record.isDefined(JourneyPatternTableSync::COL_MAIN))
			{
				bool value(
					record.getDefault<bool>(
						JourneyPatternTableSync::COL_MAIN,
						false
				)	);
				if(value != _main)
				{
					_main = value;
					result = true;
				}
			}

			// Planned length
			if(record.isDefined(JourneyPatternTableSync::COL_PLANNED_LENGTH))
			{
				graph::MetricOffset value(
					record.getDefault<graph::MetricOffset>(
						JourneyPatternTableSync::COL_PLANNED_LENGTH,
						0
				)	);
				if(value != _plannedLength)
				{
					_plannedLength = value;
					result = true;
				}
			}

//			if (linkLevel >= UP_LINKS_LOAD_LEVEL)
//			{
				// Commercial line
				if(record.isDefined(JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID))
				{
					CommercialLine* cline(NULL);
					RegistryKeyType commercialLineId(
						record.getDefault<RegistryKeyType>(
							JourneyPatternTableSync::COL_COMMERCIAL_LINE_ID,
							0
					)	);
					if(commercialLineId > 0)
					try
					{
						cline = CommercialLineTableSync::GetEditable(commercialLineId, env).get();
					}
					catch(ObjectNotFoundException<CommercialLine>)
					{
						Log::GetInstance().warn("Bad value " + lexical_cast<string>(commercialLineId) + " for fare in line " + lexical_cast<string>(getKey()));
					}

					if(cline != _pathGroup)
					{
						setCommercialLine(cline);
						if(cline)
						{
							setNetwork(cline->getNetwork());
						}
						else
						{
							setNetwork(NULL);
						}
						result = true;
					}
				}

				// Data sources and operator codes
				if(record.isDefined(JourneyPatternTableSync::COL_DATASOURCE_ID))
				{
					Importable::DataSourceLinks value(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							record.get<string>(JourneyPatternTableSync::COL_DATASOURCE_ID),
							env
					)	);
					if(value != getDataSourceLinks())
					{
						setDataSourceLinksWithRegistration(value);
						result = true;
					}
				}

				// Rolling stock
				if(record.isDefined(JourneyPatternTableSync::COL_ROLLINGSTOCKID))
				{
					RollingStock* value(NULL);
					RegistryKeyType rollingStockId(
						record.getDefault<RegistryKeyType>(
							JourneyPatternTableSync::COL_ROLLINGSTOCKID,
							0
					)	);
					if(rollingStockId > 0)
					{
						try
						{
							value = RollingStockTableSync::GetEditable(rollingStockId, env).get();
						}
						catch(ObjectNotFoundException<RollingStock>&)
						{
							Log::GetInstance().warn("Bad value " + lexical_cast<string>(rollingStockId) + " for rolling stock in line " + lexical_cast<string>(getKey()));
					}	}
					if(value != getRollingStock())
					{
						setRollingStock(value);
						result = true;
					}
				}

				RuleUser::Rules rules(getRules());

				// Bike use rules
				if(record.isDefined(JourneyPatternTableSync::COL_BIKECOMPLIANCEID))
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = NULL;
					RegistryKeyType bikeComplianceId(
						record.getDefault<RegistryKeyType>(
							JourneyPatternTableSync::COL_BIKECOMPLIANCEID,
							0
					)	);
					if(bikeComplianceId > 0)
					{
						try
						{
							rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(bikeComplianceId, env).get();
						}
						catch(ObjectNotFoundException<PTUseRule>&)
						{
							Log::GetInstance().warn("Bad value " + lexical_cast<string>(bikeComplianceId) + " for bike compliance in line " + lexical_cast<string>(getKey()));
					}	}
				}

				if(record.isDefined(JourneyPatternTableSync::COL_BIKECOMPLIANCEID))
				{
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = NULL;
					RegistryKeyType handicappedComplianceId(
						record.getDefault<RegistryKeyType>(
							JourneyPatternTableSync::COL_HANDICAPPEDCOMPLIANCEID,
							0
					)	);
					if(handicappedComplianceId > 0)
					{
						try
						{
							rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(handicappedComplianceId, env).get();
						}
						catch(ObjectNotFoundException<PTUseRule>&)
						{
							Log::GetInstance().warn("Bad value " + lexical_cast<string>(handicappedComplianceId) + " for handicapped compliance in line " + lexical_cast<string>(getKey()));
					}	}
				}

				if(record.isDefined(JourneyPatternTableSync::COL_PEDESTRIANCOMPLIANCEID))
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = NULL;
					RegistryKeyType pedestrianComplianceId(
						record.getDefault<RegistryKeyType>(
							JourneyPatternTableSync::COL_PEDESTRIANCOMPLIANCEID,
							0
					)	);
					if(pedestrianComplianceId > 0)
					{
						try
						{
							rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(pedestrianComplianceId, env).get();
						}
						catch(ObjectNotFoundException<PTUseRule>&)
						{
							Log::GetInstance().warn("Bad value " + lexical_cast<string>(pedestrianComplianceId) + " for pedestrian compliance in line " + lexical_cast<string>(getKey()));
					}	}
				}

				if(rules != getRules())
				{
					setRules(rules);
					result = true;
				}

				// Direction ID
				if(record.isDefined(JourneyPatternTableSync::COL_DIRECTION_ID))
				{
					Destination* value(NULL);
					RegistryKeyType directionId(
						record.getDefault<RegistryKeyType>(
							JourneyPatternTableSync::COL_DIRECTION_ID,
							0
					)	);
					if(directionId > 0)
					{
						try
						{
							value = DestinationTableSync::GetEditable(directionId, env).get();
						}
						catch(ObjectNotFoundException<Destination>&)
						{
							Log::GetInstance().warn("Bad value " + lexical_cast<string>(directionId) + " for direction in line " + lexical_cast<string>(getKey()));
					}	}
					if(value != _directionObj)
					{
						_directionObj = value;
						result = true;
					}
				}
//			}


			return result;
		}



		synthese::SubObjects JourneyPattern::getSubObjects() const
		{
			SubObjects r;
			BOOST_FOREACH(LineStop* lineStop, getLineStops())
			{
				r.push_back(lineStop);
			}
			BOOST_FOREACH(Service* service, getAllServices())
			{
				r.push_back(service);
			}
			return r;
		}



		Calendar& JourneyPattern::getCalendarCache() const
		{
			mutex::scoped_lock lock(_calendarCacheMutex);

			if(!_calendar)
			{
				Calendar value;
				BOOST_FOREACH(const ServiceSet::value_type& service, getAllServices())
				{
					if(	dynamic_cast<Calendar*>(service) &&
						dynamic_cast<NonPermanentService*>(service)
					){
						const boost::posix_time::time_duration& lastArrivalSchedule(
							dynamic_cast<NonPermanentService*>(service)->getLastArrivalSchedule(false)
						);

						Calendar copyCalendar(*dynamic_cast<Calendar*>(service));
						for(int i(service->getDepartureSchedule(false,0).hours() / 24);
							i<= lastArrivalSchedule.hours() / 24;
							++i
						){
							value |= copyCalendar;
							copyCalendar <<= 1;
						}
					}
				}
				_calendar = value;
			}
			return *_calendar;
		}

		synthese::LinkedObjectsIds JourneyPattern::getLinkedObjectsIds( const Record& record ) const
		{
			return LinkedObjectsIds();
		}

		void JourneyPattern::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(getCommercialLine())
			{
				const_cast<CommercialLine*>(getCommercialLine())->addPath(this);
			}
		}



		size_t JourneyPattern::getRankInDefinedSchedulesVector( size_t rank ) const
		{
			size_t result(0);
			LineStops::const_iterator it(_lineStops.begin());
			size_t i(1);
			for(++it; i<=rank && it!=_lineStops.end(); ++it, ++i)
			{
				if((*it)->get<ScheduleInput>())
				{
					++result;
				}
			}
			if(it == _lineStops.end())
			{
				Log::GetInstance().warn("Bad schedules size in journey pattern "+ lexical_cast<string>(getKey()));
			}
			return result;
		}



		void JourneyPattern::removeLineStop( const LineStop& lineStop ) const
		{
			_lineStops.erase(const_cast<LineStop*>(&lineStop));
		}



		void JourneyPattern::addLineStop( const LineStop& lineStop ) const
		{
			_lineStops.insert(const_cast<LineStop*>(&lineStop));
		}



		const StopPoint* JourneyPattern::getDestination() const
		{
			return static_cast<const StopPoint*>(Path::getDestination());
		}



		const StopPoint* JourneyPattern::getOrigin() const
		{
			return static_cast<const StopPoint*>(Path::getOrigin());
		}



		bool cmpLineStop::operator()( const LineStop* s1, const LineStop* s2 ) const
		{
			// Same objects
			if(s1 == s2)
			{
				return false;
			}

			// NULL after all
			if(!s1)
			{
				assert(false); // This should not happen
				return false;
			}

			// All before NULL
			if(!s2)
			{
				assert(false); // This should not happen
				return true;
			}

			// Line stops are not null : now comparison on rank

			// Identical ranks : comparison on address
			if(s1->get<RankInPath>() == s2->get<RankInPath>())
			{
				return s1 < s2;
			}

			// Comparison on rank
			return s1->get<RankInPath>() < s2->get<RankInPath>();
		}
}	}
