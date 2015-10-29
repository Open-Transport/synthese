
/** PTUseRule class implementation.
	@file PTUseRule.cpp

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

#include "PTUseRule.h"

#include "AccessParameters.h"
#include "CommercialLine.h"
#include "Fare.hpp"
#include "FareTableSync.hpp"
#include "JourneyPattern.hpp"
#include "LineStop.h"
#include "Profile.h"
#include "PTUseRuleTableSync.h"
#include "ServicePointer.h"
#include "StopArea.hpp"
#include "TransportNetworkRight.h"
#include "User.h"


using namespace boost;
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace fare;
	using namespace util;
	using namespace graph;
	using namespace pt;

	CLASS_DEFINITION(PTUseRule, "t061_pt_use_rules", 61)
	FIELD_DEFINITION_OF_OBJECT(PTUseRule, "pt_use_rule_id", "pt_use_rule_ids")

	FIELD_DEFINITION_OF_TYPE(Capacity, "capacity", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(ReservationType, "reservation_type", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(OriginIsReference, "origin_is_reference", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(MinDelayMinutes, "min_delay_minutes", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MinDelayMinutesExternal, "min_delay_minutes_external", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MinDelayDays, "min_delay_days", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MaxDelayDays, "max_delay_days", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(HourDeadLine, "hour_deadline", SQL_TIME)
	FIELD_DEFINITION_OF_TYPE(ReservationMinDepartureTime, "reservation_min_departure_time", SQL_TIME)
	FIELD_DEFINITION_OF_TYPE(ReservationForbiddenDays, "reservation_forbidden_days", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DefaultFare, "default_fare_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ForbiddenInDepartureBoards, "forbidden_in_departure_boards", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ForbiddenInTimetables, "forbidden_in_timetables", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ForbiddenInJourneyPlanning, "forbidden_in_journey_planning", SQL_BOOLEAN)

	namespace pt
	{
		const std::string PTUseRule::DATA_RESERVATION_POSSIBLE("reservation_possible");
		const std::string PTUseRule::DATA_RESERVATION_COMPULSORY("reservation_compulsory");
		const std::string PTUseRule::DATA_RESERVATION_MIN_DELAY_MINUTES("reservation_min_delay_minutes");
		const std::string PTUseRule::DATA_RESERVATION_MIN_DELAY_MINUTES_EXTERNAL("reservation_min_delay_minutes_external");



		PTUseRule::PTUseRule(
			util::RegistryKeyType key
		):	Registrable(key),
			Object<PTUseRule, PTUseRuleSchema> (
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_VALUE_CONSTRUCTOR(Capacity, "0"),
					FIELD_VALUE_CONSTRUCTOR(ReservationType, RESERVATION_RULE_FORBIDDEN),
					FIELD_VALUE_CONSTRUCTOR(OriginIsReference, false),
					FIELD_VALUE_CONSTRUCTOR(MinDelayMinutes, minutes(0)),
					FIELD_VALUE_CONSTRUCTOR(MinDelayMinutesExternal, minutes(0)),
					FIELD_VALUE_CONSTRUCTOR(MinDelayDays, days(0)),
					FIELD_DEFAULT_CONSTRUCTOR(MaxDelayDays),
					FIELD_VALUE_CONSTRUCTOR(HourDeadLine, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(ReservationMinDepartureTime, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(pt::ReservationForbiddenDays),
					FIELD_DEFAULT_CONSTRUCTOR(DefaultFare),
					FIELD_VALUE_CONSTRUCTOR(ForbiddenInDepartureBoards, false),
					FIELD_VALUE_CONSTRUCTOR(ForbiddenInTimetables, false),
					FIELD_VALUE_CONSTRUCTOR(ForbiddenInJourneyPlanning, false)
			)	),
			_accessCapacity(0)
		{}



		void PTUseRule::setHourDeadLine(
			const time_duration& hourDeadLine
		){
			if (hourDeadLine == time_duration(0,0,0))
			{
				set<HourDeadLine>(time_duration(23,59,59));
			}
			else
			{
				set<HourDeadLine>(hourDeadLine);
			}
		}




		ptime PTUseRule::getReservationDeadLine (
			const ptime& originDateTime,
			const ptime& departureTime,
			const ReservationDelayType reservationRulesDelayType
		) const {

			const ptime& referenceTime(
				get<OriginIsReference>() ?
				originDateTime :
				departureTime
			);

			// Minutes delay
			ptime minutesMoment(referenceTime);
			time_duration minDelayMinutes(minutes(0));

			if (reservationRulesDelayType == RESERVATION_INTERNAL_DELAY)
			{
				minDelayMinutes = get<MinDelayMinutes>();
			}
			else if (reservationRulesDelayType == RESERVATION_EXTERNAL_DELAY)
			{
				minDelayMinutes = get<MinDelayMinutesExternal>();
			}

			if(	!get<ReservationMinDepartureTime>().is_not_a_date_time() &&
				referenceTime.time_of_day() < get<ReservationMinDepartureTime>()
			){
				minutesMoment = minutesMoment - get<MinDelayDays>() + time_duration(23,59,59);
				if ( get<HourDeadLine>() < minutesMoment.time_of_day() )
				{
					minutesMoment = ptime(minutesMoment.date(), get<HourDeadLine>());
				}
			}
			else if (minDelayMinutes.total_seconds())
			{
				minutesMoment -= minDelayMinutes;
			}

			// Days delay
			ptime daysMoment(referenceTime);
			if(	get<MinDelayDays>().days()
			){
				daysMoment = daysMoment - get<MinDelayDays>() + time_duration(23,59,59);
				if ( get<HourDeadLine>() < daysMoment.time_of_day() )
				{
					daysMoment = ptime(daysMoment.date(), get<HourDeadLine>());
				}
			}
			else if ( get<HourDeadLine>().hours() > 0)
			{
				daysMoment = daysMoment - time_duration(23,59,59);
				daysMoment = ptime(daysMoment.date(), get<HourDeadLine>());
			}

			// Choosing worse delay
			ptime result(
				minutesMoment < daysMoment ?
				minutesMoment :
				daysMoment
			);

			// Forbidden days
			if(_reservationForbiddenDays.size() < 7)
			{
				while( _reservationForbiddenDays.find(result.date().day_of_week()) != _reservationForbiddenDays.end())
				{
					result = ptime(result.date() - days(1), result.time_of_day());
					if ( get<HourDeadLine>() < result.time_of_day() )
					{
						result = ptime(result.date(), get<HourDeadLine>());
					}
				}
			}

			return result;
		}



		UseRule::RunPossibilityType PTUseRule::isRunPossible(
			const graph::ServicePointer& servicePointer,
			bool ignoreReservation,
			ReservationDelayType reservationRulesDelayType
		) const {

			if(_accessCapacity && *_accessCapacity == 0)
			{
				return RUN_NOT_POSSIBLE;
			}

			if(ignoreReservation)
			{
				return RUN_POSSIBLE;
			}

			switch(get<ReservationType>())
			{
			case RESERVATION_RULE_FORBIDDEN:
			case RESERVATION_RULE_OPTIONAL:
				return RUN_POSSIBLE;

			case RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE:
				if(!servicePointer.getDepartureEdge())
				{
					return RUN_DEPENDING_ON_DEPARTURE_PLACE;
				}
				else
				{
					const Edge* departureEdge(servicePointer.getDepartureEdge());
					if(static_cast<const JourneyPattern*>(departureEdge->getParentPath())->getCommercialLine()->isOptionalReservationPlace(
						static_cast<const StopArea*>(departureEdge->getHub())
					)){
						return RUN_POSSIBLE;
					}
					else
					{
						return
							IsReservationPossible(getReservationAvailability(servicePointer, ignoreReservation)) ?
							RUN_POSSIBLE :
							RUN_NOT_POSSIBLE
						;
					}
				}

			case RESERVATION_RULE_COMPULSORY:
				return
					(	!servicePointer.getDepartureEdge() ||
						!servicePointer.getDepartureEdge()->getReservationNeeded() ||
						IsReservationPossible(getReservationAvailability(servicePointer, ignoreReservation, reservationRulesDelayType))
					) ?
					RUN_POSSIBLE :
					RUN_NOT_POSSIBLE
				;
			}

			return RUN_NOT_POSSIBLE;
		}



		UseRule::ReservationAvailabilityType PTUseRule::getReservationAvailability(
			const ServicePointer& servicePointer,
			bool ignoreReservationDeadline,
			ReservationDelayType reservationRulesDelayType
		) const	{
			if(!servicePointer.getDepartureEdge() && !servicePointer.getArrivalEdge())
			{
				return RESERVATION_FORBIDDEN;
			}

			switch(get<ReservationType>())
			{
			case RESERVATION_RULE_FORBIDDEN:
				return RESERVATION_FORBIDDEN;

			case RESERVATION_RULE_OPTIONAL:
				{
					if(ignoreReservationDeadline)
					{
						return RESERVATION_OPTIONAL_POSSIBLE;
					}
					ptime reservationTime(second_clock::local_time());
					if(	reservationTime < getReservationOpeningTime(servicePointer)
					){
						return RESERVATION_OPTIONAL_TOO_EARLY;
					}
					if(	servicePointer.getDepartureEdge())
					{
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(),
												servicePointer.getDepartureDateTime(), reservationRulesDelayType)
						){
							return RESERVATION_OPTIONAL_TOO_LATE;
						}
						return RESERVATION_OPTIONAL_POSSIBLE;
					}
					else
					{
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(),
												servicePointer.getArrivalDateTime(), reservationRulesDelayType)
						){
							return RESERVATION_OPTIONAL_TOO_LATE;
						}
						if(reservationTime <= getReservationDeadLine(servicePointer.getOriginDateTime(),
												servicePointer.getOriginDateTime(), reservationRulesDelayType)
						){
							return RESERVATION_OPTIONAL_POSSIBLE;
						}
						return RESERVATION_DEPENDING_ON_DEPARTURE_PLACE;
					}
				}

			case RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE:
				if(ignoreReservationDeadline)
				{
					return RESERVATION_OPTIONAL_POSSIBLE;
				}
				if(servicePointer.getDepartureEdge())
				{
					const Edge* departureEdge(servicePointer.getDepartureEdge());
					ptime reservationTime(second_clock::local_time());
					if(static_cast<const JourneyPattern*>(departureEdge->getParentPath())->getCommercialLine()->isOptionalReservationPlace(
						static_cast<const StopArea*>(departureEdge->getHub())
					)){
						if(	reservationTime < getReservationOpeningTime(servicePointer)
						){
							return RESERVATION_OPTIONAL_TOO_EARLY;
						}
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(),
												servicePointer.getDepartureDateTime(), reservationRulesDelayType)
						){
							return RESERVATION_OPTIONAL_TOO_LATE;
						}
						return RESERVATION_OPTIONAL_POSSIBLE;
					} else {
						if(	reservationTime < getReservationOpeningTime(servicePointer)
						){
							return RESERVATION_COMPULSORY_TOO_EARLY;
						}
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(),
												servicePointer.getDepartureDateTime(), reservationRulesDelayType)
						){
							return RESERVATION_COMPULSORY_TOO_LATE;
						}
						return RESERVATION_COMPULSORY_POSSIBLE;
					}
				}
				return RESERVATION_DEPENDING_ON_DEPARTURE_PLACE;

			case RESERVATION_RULE_COMPULSORY:
				{
					if(ignoreReservationDeadline)
					{
						return RESERVATION_COMPULSORY_POSSIBLE;
					}
					
					ptime reservationTime(second_clock::local_time());
					if(	reservationTime < getReservationOpeningTime(servicePointer)
					){
						return RESERVATION_COMPULSORY_TOO_EARLY;
					}

					if(servicePointer.getDepartureEdge())
					{
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(),
												servicePointer.getDepartureDateTime(), reservationRulesDelayType)
						){
							return RESERVATION_COMPULSORY_TOO_LATE;
						}
						return RESERVATION_COMPULSORY_POSSIBLE;
					}
					else
					{
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(),
												servicePointer.getArrivalDateTime(), reservationRulesDelayType)
						){
							return RESERVATION_COMPULSORY_TOO_LATE;
						}
						if(reservationTime <= getReservationDeadLine(servicePointer.getOriginDateTime(),
												servicePointer.getOriginDateTime(), reservationRulesDelayType)
						){
							return RESERVATION_COMPULSORY_POSSIBLE;
						}
						return RESERVATION_DEPENDING_ON_DEPARTURE_PLACE;
					}
				}
			}

			return RESERVATION_FORBIDDEN;
		}



		ptime PTUseRule::getReservationOpeningTime(
			const graph::ServicePointer& servicePointer
		) const {
			if(get<MaxDelayDays>() != boost::gregorian::days(0))
			{
				return ptime(servicePointer.getOriginDateTime().date(), -hours(24 * get<MaxDelayDays>().days()));
			}
			return neg_infin;
		}



		bool PTUseRule::isCompatibleWith( const AccessParameters& accessParameters ) const
		{
			if(_accessCapacity && *_accessCapacity == 0) return false;

			// 			if (_fare && complyer.getFare()->isCompliant() == logic::tribool(true) && complyer.getFare().get() != _fare)
			// 				return false;

			if (accessParameters.getDRTOnly() &&
				get<ReservationType>() == RESERVATION_RULE_FORBIDDEN
			) return false;

			if (accessParameters.getWithoutDRT() &&
				get<ReservationType>() != RESERVATION_RULE_FORBIDDEN
			) return false;

			return true;
		}



		PTUseRule::ReservationRuleTypesList PTUseRule::GetTypesList()
		{
			ReservationRuleTypesList result;
			result.push_back(make_pair(RESERVATION_RULE_FORBIDDEN, GetTypeName(RESERVATION_RULE_FORBIDDEN)));
			result.push_back(make_pair(RESERVATION_RULE_OPTIONAL, GetTypeName(RESERVATION_RULE_OPTIONAL)));
			result.push_back(make_pair(RESERVATION_RULE_COMPULSORY, GetTypeName(RESERVATION_RULE_COMPULSORY)));
			result.push_back(make_pair(RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE, GetTypeName(RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE)));
			return result;
		}



		std::string PTUseRule::GetTypeName( ReservationRuleType type )
		{
			switch(type)
			{
			case RESERVATION_RULE_FORBIDDEN: return "Réservation impossible";
			case RESERVATION_RULE_COMPULSORY: return "Réservation obligatoire";
			case RESERVATION_RULE_OPTIONAL: return "Réservation possible";
			case RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE: return "Réservation possible, obligatoire au départ de certains arrêts";
			}
			assert(false);
			return string();
		}



		void PTUseRule::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const {
			pm.insert(prefix + DATA_RESERVATION_COMPULSORY, get<ReservationType>() == RESERVATION_RULE_COMPULSORY);
			pm.insert(prefix + DATA_RESERVATION_POSSIBLE, get<ReservationType>() != RESERVATION_RULE_FORBIDDEN);
			if(get<ReservationType>() != RESERVATION_RULE_FORBIDDEN)
			{
				pm.insert(prefix + DATA_RESERVATION_MIN_DELAY_MINUTES, get<MinDelayMinutes>().minutes());
				pm.insert(prefix + DATA_RESERVATION_MIN_DELAY_MINUTES_EXTERNAL, get<MinDelayMinutesExternal>().minutes());
			}

			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(prefix + SimpleObjectFieldDefinition<Name>::FIELD.name, getName());
			pm.insert(
				prefix + Capacity::FIELD.name,
				getAccessCapacity() ? lexical_cast<string>(*getAccessCapacity()) : string()
			);
			pm.insert(
				prefix + pt::ReservationType::FIELD.name,
				static_cast<int>(getReservationType())
			);
			pm.insert(
				prefix + OriginIsReference::FIELD.name,
				getOriginIsReference()
			);
			pm.insert(
				prefix + MinDelayMinutes::FIELD.name,
				boost::lexical_cast<std::string>(getMinDelayMinutes().total_seconds() / 60)
			);
			pm.insert(
				prefix + MinDelayMinutesExternal::FIELD.name,
				boost::lexical_cast<std::string>(getMinDelayMinutesExternal().total_seconds() / 60)
			);
			pm.insert(
				prefix + MinDelayDays::FIELD.name,
				boost::lexical_cast<std::string>(static_cast<int>(getMinDelayDays().days()))
			);
			pm.insert(
				prefix + MaxDelayDays::FIELD.name,
				getMaxDelayDays() ? static_cast<int>(getMaxDelayDays()->days()) : int(0)
			);
			pm.insert(
				prefix + HourDeadLine::FIELD.name,
				getHourDeadLine().is_not_a_date_time() ? std::string() : boost::posix_time::to_simple_string(getHourDeadLine())
			);
			pm.insert(
				prefix + ReservationMinDepartureTime::FIELD.name,
				getReservationMinDepartureTime().is_not_a_date_time() ? std::string() : boost::posix_time::to_simple_string(getReservationMinDepartureTime())
			);
			pm.insert(
				prefix + pt::ReservationForbiddenDays::FIELD.name,
				PTUseRule::SerializeForbiddenDays(getReservationForbiddenDays())
			);
			pm.insert(
				prefix + DefaultFare::FIELD.name,
				getDefaultFare() ? getDefaultFare()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + ForbiddenInDepartureBoards::FIELD.name,
				getForbiddenInDepartureBoards()
			);
			pm.insert(
				prefix + ForbiddenInTimetables::FIELD.name,
				getForbiddenInTimetables()
			);
			pm.insert(
				prefix + ForbiddenInJourneyPlanning::FIELD.name,
				getForbiddenInJourneyPlanning()
			);
		}



		void PTUseRule::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			// Reservation forbidden days
			PTUseRule::ReservationForbiddenDays value(
				PTUseRule::UnserializeForbiddenDays(
					get<pt::ReservationForbiddenDays>()
			)	);
			setReservationForbiddenDays(value);

			optional<size_t> capacity;
			string text(get<Capacity>());
			if(!text.empty()) try
			{
				capacity = lexical_cast<size_t>(text);
			}
			catch(...)
			{
			}
			setAccessCapacity(capacity);
		}


		std::string PTUseRule::SerializeForbiddenDays( const PTUseRule::ReservationForbiddenDays& value )
		{
			bool first(true);
			stringstream forbiddenDays;
			BOOST_FOREACH(const date::day_of_week_type& day, value)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					forbiddenDays << ",";
				}
				forbiddenDays << int(day);
			}
			return forbiddenDays.str();
		}


		PTUseRule::ReservationForbiddenDays PTUseRule::UnserializeForbiddenDays( const std::string& value )
		{
			if(value.empty())
			{
				return PTUseRule::ReservationForbiddenDays();
			}
			else
			{
				PTUseRule::ReservationForbiddenDays days;
				vector<string> daysVec;
				split(daysVec, value, is_any_of(","));
				BOOST_FOREACH(const string& dayStr, daysVec)
				{
					try
					{
						days.insert(static_cast<date::day_of_week_type>(lexical_cast<int>(dayStr)));
					}
					catch(bad_lexical_cast&)
					{
					}
				}
				return days;
			}
		}

		void PTUseRule::setReservationForbiddenDays(const std::set<boost::gregorian::date::day_of_week_type>& value)
		{
			_reservationForbiddenDays = value;
			string strForbiddenDays = SerializeForbiddenDays(value);
			set<pt::ReservationForbiddenDays>(strForbiddenDays);
		}

		void PTUseRule::setDefaultFare(fare::Fare* value)
		{
			set<DefaultFare>(value
				? boost::optional<fare::Fare&>(*value)
				: boost::none);
		}

		const boost::optional<boost::gregorian::date_duration>	PTUseRule::getMaxDelayDays() const
		{
			boost::optional<boost::gregorian::date_duration> value;
			if (get<MaxDelayDays>() != boost::gregorian::days(0))
			{
				value = get<MaxDelayDays>();
			}
			return value;
		}

		bool PTUseRule::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool PTUseRule::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool PTUseRule::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::DELETE_RIGHT);
		}
}	}
