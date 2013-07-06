
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
#include "Edge.h"
#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "ServicePointer.h"
#include "StopArea.hpp"
#include "AccessParameters.h"

using namespace boost;
using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const std::string Registry<pt::PTUseRule>::KEY("PTUseRule");
	}

	namespace pt
	{
		const std::string PTUseRule::DATA_RESERVATION_POSSIBLE("reservation_possible");
		const std::string PTUseRule::DATA_RESERVATION_COMPULSORY("reservation_compulsory");
		const std::string PTUseRule::DATA_RESERVATION_MIN_DELAY_MINUTES("reservation_min_delay_minutes");



		PTUseRule::PTUseRule(
			util::RegistryKeyType key
		):	Registrable(key),
			_accessCapacity(0),
			_defaultFare(NULL),
			_reservationType(RESERVATION_RULE_FORBIDDEN),
			_originIsReference(false),
			_minDelayMinutes(minutes(0)),
			_minDelayDays(days(0)),
			_maxDelayDays(optional<date_duration>()),
			_hourDeadLine(not_a_date_time),
			_reservationMinDepartureTime(not_a_date_time),
			_forbiddenInDepartureBoards(false),
			_forbiddenInTimetables(false),
			_forbiddenInJourneyPlanning(false)
		{}



		void PTUseRule::setHourDeadLine(
			const time_duration& hourDeadLine
		){
			if (hourDeadLine == time_duration(0,0,0))
			{
				_hourDeadLine = time_duration(23,59,59);
			}
			else
			{
				_hourDeadLine = hourDeadLine;
			}
		}




		ptime PTUseRule::getReservationDeadLine (
			const ptime& originDateTime,
			const ptime& departureTime
		) const {

			const ptime& referenceTime(
				_originIsReference ?
				originDateTime :
				departureTime
			);

			// Minutes delay
			ptime minutesMoment(referenceTime);
			if(	!_reservationMinDepartureTime.is_not_a_date_time() &&
				referenceTime.time_of_day() < _reservationMinDepartureTime
			){
				minutesMoment = minutesMoment - _minDelayDays + time_duration(23,59,59);
				if ( _hourDeadLine < minutesMoment.time_of_day() )
				{
					minutesMoment = ptime(minutesMoment.date(), _hourDeadLine);
				}
			}
			else if(_minDelayMinutes.total_seconds()
			){
				minutesMoment -= _minDelayMinutes;
			}

			// Days delay
			ptime daysMoment(referenceTime);
			if(	_minDelayDays.days()
			){
				daysMoment = daysMoment - _minDelayDays + time_duration(23,59,59);
				if ( _hourDeadLine < daysMoment.time_of_day() )
				{
					daysMoment = ptime(daysMoment.date(), _hourDeadLine);
				}
			}
			else if ( _hourDeadLine.hours() > 0)
			{
				daysMoment = daysMoment - time_duration(23,59,59);
				daysMoment = ptime(daysMoment.date(), _hourDeadLine);
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
					if ( _hourDeadLine < result.time_of_day() )
					{
						result = ptime(result.date(), _hourDeadLine);
					}
				}
			}

			return result;
		}



		UseRule::RunPossibilityType PTUseRule::isRunPossible(
			const graph::ServicePointer& servicePointer,
			bool ignoreReservation
		) const {

			if(_accessCapacity && *_accessCapacity == 0)
			{
				return RUN_NOT_POSSIBLE;
			}

			if(ignoreReservation)
			{
				return RUN_POSSIBLE;
			}

			switch(_reservationType)
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
					IsReservationPossible(getReservationAvailability(servicePointer, ignoreReservation)) ?
					RUN_POSSIBLE :
					RUN_NOT_POSSIBLE
				;
			}

			return RUN_NOT_POSSIBLE;
		}



		UseRule::ReservationAvailabilityType PTUseRule::getReservationAvailability(
			const ServicePointer& servicePointer,
			bool ignoreReservationDeadline
		) const	{
			if(!servicePointer.getDepartureEdge() && !servicePointer.getArrivalEdge())
			{
				return RESERVATION_FORBIDDEN;
			}

			switch(_reservationType)
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
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getDepartureDateTime())
						){
							return RESERVATION_OPTIONAL_TOO_LATE;
						}
						return RESERVATION_OPTIONAL_POSSIBLE;
					}
					else
					{
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getArrivalDateTime())
						){
							return RESERVATION_OPTIONAL_TOO_LATE;
						}
						if(reservationTime <= getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getOriginDateTime()))
						{
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
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getDepartureDateTime())
						){
							return RESERVATION_OPTIONAL_TOO_LATE;
						}
						return RESERVATION_OPTIONAL_POSSIBLE;
					} else {
						if(	reservationTime < getReservationOpeningTime(servicePointer)
						){
							return RESERVATION_COMPULSORY_TOO_EARLY;
						}
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getDepartureDateTime())
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
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getDepartureDateTime())
						){
							return RESERVATION_COMPULSORY_TOO_LATE;
						}
						return RESERVATION_COMPULSORY_POSSIBLE;
					}
					else
					{
						if(reservationTime > getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getArrivalDateTime())
						){
							return RESERVATION_COMPULSORY_TOO_LATE;
						}
						if(reservationTime <= getReservationDeadLine(servicePointer.getOriginDateTime(), servicePointer.getOriginDateTime())
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
			if(_maxDelayDays)
			{
				return ptime(servicePointer.getOriginDateTime().date(), -hours(24 * _maxDelayDays->days()));
			}
			return neg_infin;
		}



		bool PTUseRule::isCompatibleWith( const AccessParameters& accessParameters ) const
		{
			if(_accessCapacity && *_accessCapacity == 0) return false;

			// 			if (_fare && complyer.getFare()->isCompliant() == logic::tribool(true) && complyer.getFare().get() != _fare)
			// 				return false;

			if (accessParameters.getDRTOnly() &&
				_reservationType == RESERVATION_RULE_FORBIDDEN
			) return false;

			if (accessParameters.getWithoutDRT() &&
				_reservationType != RESERVATION_RULE_FORBIDDEN
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
			pm.insert(prefix + DATA_RESERVATION_COMPULSORY, _reservationType == RESERVATION_RULE_COMPULSORY);
			pm.insert(prefix + DATA_RESERVATION_POSSIBLE, _reservationType != RESERVATION_RULE_FORBIDDEN);
			if(_reservationType != RESERVATION_RULE_FORBIDDEN)
			{
				pm.insert(prefix + DATA_RESERVATION_MIN_DELAY_MINUTES, _minDelayMinutes.minutes());
			}
		}
	}
}
