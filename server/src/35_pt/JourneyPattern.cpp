
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
#include "Fare.hpp"
#include "ImportableTableSync.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStop.h"
#include "LinePhysicalStop.hpp"
#include "Log.h"
#include "NonPermanentService.h"
#include "Profile.h"
#include "PTUseRuleTableSync.h"
#include "Registry.h"
#include "RollingStockTableSync.hpp"
#include "Service.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "TransportNetwork.h"
#include "TransportNetworkRight.h"
#include "User.h"

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

	CLASS_DEFINITION(JourneyPattern, "t009_lines", 9)
	FIELD_DEFINITION_OF_OBJECT(JourneyPattern, "journey_pattern_id", "journey_pattern_ids")

	FIELD_DEFINITION_OF_TYPE(JourneyPatternCommercialLine, "commercial_line_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(TimetableName, "timetable_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Direction, "direction", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(LineDestination, "direction_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(IsWalkingLine, "is_walking_line", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(LineRollingStock, "rolling_stock_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(WayBack, "wayback", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(LineDataSource, "data_source", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Main, "main", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(PlannedLength, "planned_length", SQL_DOUBLE)

	namespace pt
	{
		const string JourneyPattern::ATTR_DIRECTION_TEXT = "direction_text";



		JourneyPattern::JourneyPattern(
			util::RegistryKeyType id,
			std::string name
		):	util::Registrable(id),
			Object<JourneyPattern, JourneyPatternSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(JourneyPatternCommercialLine),
					FIELD_DEFAULT_CONSTRUCTOR(TimetableName),
					FIELD_DEFAULT_CONSTRUCTOR(Direction),
					FIELD_DEFAULT_CONSTRUCTOR(LineDestination),
					FIELD_VALUE_CONSTRUCTOR(IsWalkingLine, false),
					FIELD_DEFAULT_CONSTRUCTOR(LineRollingStock),
					FIELD_DEFAULT_CONSTRUCTOR(BikeComplianceId),
					FIELD_DEFAULT_CONSTRUCTOR(HandicappedComplianceId),
					FIELD_DEFAULT_CONSTRUCTOR(PedestrianComplianceId),
					FIELD_VALUE_CONSTRUCTOR(WayBack, false),
					FIELD_DEFAULT_CONSTRUCTOR(LineDataSource),
					FIELD_VALUE_CONSTRUCTOR(Main, false),
					FIELD_VALUE_CONSTRUCTOR(PlannedLength, 0)
			)	),
			Path()
		{}



		JourneyPattern::~JourneyPattern ()
		{
			boost::unique_lock<shared_recursive_mutex> lock(*sharedServicesMutex);
			BOOST_FOREACH(Service* service, getAllServices())
			{
				service->setPath(NULL);
			}

			BOOST_FOREACH(LineStop* edge, _lineStops)
			{
				BOOST_FOREACH(const LineStop::GeneratedLineStops::value_type& itGeneratedLineStop, edge->getGeneratedLineStops())
				{
					itGeneratedLineStop->setParentPath(NULL);
				}

				edge->set<Line>(boost::none);
			}
		}



		const std::string&
		JourneyPattern::getDirection () const
		{
			return get<Direction>();
		}



		void
		JourneyPattern::setDirection (const std::string& direction)
		{
			set<Direction>(direction);
		}



		const std::string&
		JourneyPattern::getTimetableName () const
		{
			return get<TimetableName>();
		}



		void
		JourneyPattern::setTimetableName (const std::string& timetableName)
		{
			set<TimetableName>(timetableName);
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
			set<LineRollingStock>(rollingStock
				? boost::optional<RollingStock&>(*rollingStock)
				: boost::none);
		}



		void JourneyPattern::setNetwork(TransportNetwork* transportNetwork)
        {
             _pathNetwork = static_cast<PathClass*>(transportNetwork);
        }



		void JourneyPattern::setWalkingLine (bool isWalkingLine)
		{
			set<IsWalkingLine>(isWalkingLine);
		}



		bool JourneyPattern::getWalkingLine () const
		{
			return get<IsWalkingLine>();
		}



		void JourneyPattern::setCommercialLine(CommercialLine* commercialLine )
		{
			_pathGroup = commercialLine;
			set<JourneyPatternCommercialLine>(commercialLine
				? boost::optional<CommercialLine&>(*commercialLine)
				: boost::none);
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
				prefix + JourneyPatternCommercialLine::FIELD.name,
				getCommercialLine()->getKey()
			);
			pm.insert(
				prefix + SimpleObjectFieldDefinition<Name>::FIELD.name,
				getName()
			);
			pm.insert(
				prefix + TimetableName::FIELD.name,
				getTimetableName()
			);
			pm.insert(
				prefix + Direction::FIELD.name,
				getDirection()
			);
			pm.insert(
				prefix + LineDestination::FIELD.name,
				getDirectionObj() ? getDirectionObj()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + IsWalkingLine::FIELD.name,
				getWalkingLine()
			);
			pm.insert(
				prefix + LineRollingStock::FIELD.name,
				getRollingStock() ? getRollingStock()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + BikeComplianceId::FIELD.name,
				(	getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
					static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + HandicappedComplianceId::FIELD.name,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + PedestrianComplianceId::FIELD.name,
				(	getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
					static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey() :
					RegistryKeyType(0)
			)	);
			pm.insert(
				prefix + WayBack::FIELD.name,
				getWayBack()
			);
			pm.insert(
				prefix + LineDataSource::FIELD.name,
				synthese::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			pm.insert(
				prefix + Main::FIELD.name,
				getMain()
			);
			pm.insert(
				prefix + PlannedLength::FIELD.name,
				getPlannedLength()
			);
			pm.insert(prefix + ATTR_DIRECTION_TEXT, get<Direction>());
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



		void JourneyPattern::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<JourneyPatternCommercialLine>())
			{
				_pathGroup = get<JourneyPatternCommercialLine>().get_ptr();
			}
			else
			{
				_pathGroup = NULL;
			}
			if(getCommercialLine())
			{
				const_cast<CommercialLine*>(getCommercialLine())->addPath(this);
			}
			if(get<JourneyPatternCommercialLine>())
			{
				setNetwork(get<JourneyPatternCommercialLine>()->getNetwork());
			}
			else
			{
				setNetwork(NULL);
			}

			if (get<LineRollingStock>())
			{
				_pathClass = get<LineRollingStock>().get_ptr();
			}
			else
			{
				_pathClass = NULL;
			}

			// Use rules
			RuleUser::Rules rules(getRules());

			// Bike compliance
			if(get<BikeComplianceId>())
			{
				if(get<BikeComplianceId>() > 0)
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(get<BikeComplianceId>(), env).get();
				}
				else
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = NULL;
				}
			}

			// Handicapped compliance
			if(get<HandicappedComplianceId>())
			{
				if(get<HandicappedComplianceId>() > 0)
				{
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(get<HandicappedComplianceId>(), env).get();
				}
				else
				{
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = NULL;
				}
			}

			// Pedestrian compliance
			if(get<PedestrianComplianceId>())
			{
				if(get<PedestrianComplianceId>() > 0)
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(get<PedestrianComplianceId>(), env).get();
				}
				else
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = NULL;
				}
			}
			setRules(rules);
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

		void JourneyPattern::setRules(const Rules& value)
		{
			RuleUser::setRules(value);
			getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
				set<BikeComplianceId>(static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey()) :
				set<BikeComplianceId>(RegistryKeyType(0));
			getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
				set<HandicappedComplianceId>(static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey()) :
				set<HandicappedComplianceId>(RegistryKeyType(0));
			getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
				set<PedestrianComplianceId>(static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey()) :
				set<PedestrianComplianceId>(RegistryKeyType(0));
		}

		bool JourneyPattern::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool JourneyPattern::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool JourneyPattern::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::DELETE_RIGHT);
		}
}	}
