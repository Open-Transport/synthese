
/** PTUseRule class header.
	@file PTUseRule.h

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

#ifndef SYNTHESE_PTUseRule_h__
#define SYNTHESE_PTUseRule_h__

#include "Object.hpp"

#include "DaysField.hpp"
#include "EnumObjectField.hpp"
#include "MinutesField.hpp"
#include "ParametersMap.h"
#include "Registrable.h"
#include "Registry.h"
#include "StringField.hpp"
#include "TimeField.hpp"
#include "UseRule.h"

#include "FrameworkTypes.hpp"

#include <string>
#include <boost/optional.hpp>
#include <set>

namespace synthese
{
	namespace fare
	{
		class Fare;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Reservation rule type.
		///	- FORBIDDEN : it is impossible to book a seat on the service
		///	- COMPULSORY : it is impossible to use the service without having booked a seat
		///	- OPTIONAL : is is possible to book a place on the service, but it is possible to use
		///   the service without having booked a seat
		///	- MIXED_BY_DEPARTURE_PLACE : it is impossible to use the service without having booked
		///   a place, except for journeys beginning at several places, defined in the path.
		typedef enum {
			RESERVATION_RULE_FORBIDDEN = 0,
			RESERVATION_RULE_COMPULSORY = 1,
			RESERVATION_RULE_OPTIONAL = 2,
			RESERVATION_RULE_MIXED_BY_DEPARTURE_PLACE = 3
		} ReservationRuleType;

		FIELD_STRING(Capacity)
		FIELD_ENUM(ReservationType, pt::ReservationRuleType)
		FIELD_BOOL(OriginIsReference)
		FIELD_MINUTES(MinDelayMinutes)
		FIELD_MINUTES(MinDelayMinutesExternal)
		FIELD_DAYS(MinDelayDays)
		FIELD_DAYS(MaxDelayDays)
		FIELD_TIME(HourDeadLine)
		FIELD_TIME(ReservationMinDepartureTime)
		FIELD_STRING(ReservationForbiddenDays)
		FIELD_POINTER(DefaultFare, fare::Fare)
		FIELD_BOOL(ForbiddenInDepartureBoards)
		FIELD_BOOL(ForbiddenInTimetables)
		FIELD_BOOL(ForbiddenInJourneyPlanning)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Capacity),
			FIELD(ReservationType),				// Type of the reservation rule.
			FIELD(OriginIsReference),			// Whether reference departure time is the line run departure time at its origin (true) or client departure time (false).
			FIELD(MinDelayMinutes),				// Minimum delay in minutes between reservation and reference moment
			FIELD(MinDelayMinutesExternal),		// Minimum external delay (used by Routeplanner if needed)
			FIELD(MinDelayDays),				//!< Minimum delay in days between reservation and reference moment
			FIELD(MaxDelayDays),				//!< Maxium number of days between reservation and departure.
			FIELD(HourDeadLine),				//!< Latest reservation hour the last day open for reservation
			FIELD(ReservationMinDepartureTime),	//!< Minimal time for reservation the day of the departure. If departure time is before, then the reservation must be done the preceding day, before _hourDeadLine or midnight if not defined
			FIELD(ReservationForbiddenDays),
			FIELD(DefaultFare),
			FIELD(ForbiddenInDepartureBoards),
			FIELD(ForbiddenInTimetables),
			FIELD(ForbiddenInJourneyPlanning)
		> PTUseRuleSchema;

		//////////////////////////////////////////////////////////////////////////
		/// Public transportation use rule.
		///	@ingroup m35
		/// @author Hugues Romain
		/// @date 2009
		//////////////////////////////////////////////////////////////////////////
		/// A use rules defines for a user profile :
		///	<ul>
		///		<li>The maximum number of customer who can use the service</li>
		///		<li>A default fare class</li>
		///		<li>Some reservation rules</li>
		///	</ul>
		///
		class PTUseRule:
			public Object<PTUseRule, PTUseRuleSchema>,
			public graph::UseRule
		{
		private:
			static const std::string DATA_RESERVATION_POSSIBLE;
			static const std::string DATA_RESERVATION_COMPULSORY;
			static const std::string DATA_RESERVATION_MIN_DELAY_MINUTES;
			static const std::string DATA_RESERVATION_MIN_DELAY_MINUTES_EXTERNAL;

		public:
			typedef std::vector<std::pair<boost::optional<ReservationRuleType>, std::string> > ReservationRuleTypesList;
			typedef std::set<boost::gregorian::date::day_of_week_type> ReservationForbiddenDays;

		private:

			//! @name Access
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Maximal person number which can be served
				/// The undefined value of the attribute seems unlimited capacity
				/// A 0 value seems that the service use is forbidden
				AccessCapacity _accessCapacity;

				/// Default fare
				const fare::Fare*	_defaultFare;
			//@}

			//! @name Reservation
			//@{
				ReservationForbiddenDays _reservationForbiddenDays;
			//@}


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param key id of the object
			/// @author Hugues Romain
			//////////////////////////////////////////////////////////////////////////
			///  - forbidden access
			///  - no reservation
			PTUseRule(
				util::RegistryKeyType key = 0
			);

			//! @name Getters
			//@{
				virtual std::string getName() const { return get<Name>(); }
				virtual AccessCapacity	getAccessCapacity()	const { return _accessCapacity; }
				bool								getOriginIsReference()	const { return get<OriginIsReference>(); }
				const boost::posix_time::time_duration&	getHourDeadLine()				const { return get<HourDeadLine>(); }
				boost::gregorian::date_duration		getMinDelayDays()				const { return get<MinDelayDays>(); }
				boost::posix_time::time_duration	getMinDelayMinutes()			const { return get<MinDelayMinutes>(); }
				boost::posix_time::time_duration	getMinDelayMinutesExternal()	const { return get<MinDelayMinutesExternal>(); }
				const boost::optional<boost::gregorian::date_duration>	getMaxDelayDays()		const;
				ReservationRuleType	getReservationType()			const { return get<ReservationType>(); }
				const fare::Fare*	getDefaultFare()				const { return get<DefaultFare>() ? get<DefaultFare>().get_ptr() : NULL; }
				bool getForbiddenInDepartureBoards ()	const { return get<ForbiddenInDepartureBoards>(); }
				bool getForbiddenInTimetables ()		const { return get<ForbiddenInTimetables>(); }
				bool getForbiddenInJourneyPlanning ()	const { return get<ForbiddenInJourneyPlanning>(); }
				const boost::posix_time::time_duration& getReservationMinDepartureTime() const { return get<ReservationMinDepartureTime>(); }
				const std::set<boost::gregorian::date::day_of_week_type>& getReservationForbiddenDays() const { return _reservationForbiddenDays; }
			//@}

			//! @name Setters
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Hour dead line setter.
				/// @param hourDeadLine value for the hour dead line. If not_a_date_time, there is no hour dead line. If 00:00 then the dead line is shifted 1 second before.
				/// @author Hugues Romain
				void setHourDeadLine (const boost::posix_time::time_duration& hourDeadLine);

				void setMinDelayMinutes (boost::posix_time::time_duration value) { set<MinDelayMinutes>(value); }
				void setMinDelayMinutesExternal (boost::posix_time::time_duration value) { set<MinDelayMinutesExternal>(value); }
				void setMinDelayDays (boost::gregorian::date_duration value) { set<MinDelayDays>(value); }
				void setMaxDelayDays (const boost::optional<boost::gregorian::date_duration> value){ set<MaxDelayDays>(value ? *value : boost::gregorian::days(0)); }
				void setOriginIsReference (bool value){ set<OriginIsReference>(value); }
				void setReservationType(ReservationRuleType value){ set<ReservationType>(value); }
				void setAccessCapacity(AccessCapacity value){ _accessCapacity = value; set<Capacity>(boost::lexical_cast<std::string>(value)); }
				void setDefaultFare(fare::Fare* value);
				void setForbiddenInDepartureBoards (bool value){ set<ForbiddenInDepartureBoards>(value); }
				void setForbiddenInJourneyPlanning (bool value){ set<ForbiddenInJourneyPlanning>(value); }
				void setForbiddenInTimetables (bool value){ set<ForbiddenInTimetables>(value); }
				void setReservationMinDepartureTime(const boost::posix_time::time_duration& value){ set<ReservationMinDepartureTime>(value); }
				void setReservationForbiddenDays(const std::set<boost::gregorian::date::day_of_week_type>& value);
				void setName(const std::string& value){ set<Name>(value); }
			//@}

			//! @name Service
			//@{
				virtual std::string getUseRuleName() const { return getName(); }

				/** Reference function for reservation dead line calculation.
					@param originTime Time of start of the corresponding service
					@param departureTime Desired departure time.

					It is done according to the following steps:
						- Choice of reference time (client departure or line run departure at origin)
						- Calculation 1 : x minutes before reference time :
						- Decrease of _minDelayMinutes before reference

						- Calculation 2 : x days before reference time :
						- Decrease of _minDelayDays before reference
						- Sets hour to _hourDeadLine

						- The smallest date time is chosen.

					If no explicit rule defines the reservation dead line,
					the actual reservation time is returned.
				*/
				virtual boost::posix_time::ptime getReservationDeadLine (
					const boost::posix_time::ptime& originTime,
					const boost::posix_time::ptime& departureTime,
					const ReservationDelayType reservationRulesDelayType = RESERVATION_INTERNAL_DELAY
				) const;



				/** Reference function for calculation of the date time of the opening of reservations.
					@param reservationTime Time when booking is done.
					@return The minimum date time to make a reservation.

					If no explicit rule defines this minimum time, the actual reservation time is returned.
				*/
				virtual boost::posix_time::ptime getReservationOpeningTime (
					const graph::ServicePointer& servicePointer
				) const;



				/** Indicates whether or not a path can be taken at a given date,
					taking into account reservation delay rules.
					@return true if the line run can be taken, false otherwise.

					This methods checks the following conditions :
						- if reservation is not compulsory, the run can be taken.
						- if reservation is compulsory, reservation time must precede reservation
					dead line and be after reservation opening time.
				*/
				virtual RunPossibilityType isRunPossible (
					const graph::ServicePointer& servicePointer,
					bool ignoreReservation,
					ReservationDelayType reservationRulesDelayType = RESERVATION_INTERNAL_DELAY
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Indicates whether or not a reservation is possible for a given run,
				///	at a certain date, taking into account delay rules.
				///	@return true if the reservation is possible, false otherwise.
				///
				///	This methods checks the following conditions :
				///		- reservation time must precede reservation dead line
				///		- reservation time must be later than reservation start time.
				///
				///	@warning If the service pointer departure time is not fully defined, then the method returns
				/// a temporary reservation availability : the departure time
				/// must be known to determinate the reservation availability definitely.
				/// In ARRIVAL_TO_DEPARTURE method, the arrival time is the only one
				/// known. The temporary result is based on the arrival time : if no
				/// reservation can be done for the arrival time, it is not possible to do
				/// one for any departure time, that is necessarily earlier. So the
				/// result of this method MUST be confirmed by a call on a full service pointer.
				virtual ReservationAvailabilityType getReservationAvailability(
					const graph::ServicePointer& servicePointer,
					bool ignoreReservationDeadline,
					ReservationDelayType reservationRulesDelayType = RESERVATION_INTERNAL_DELAY
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if the use rule is compatible with the filters of an access parameter.
				/// @param accessParameters access parameters object to compare with
				/// @return true if the use rule is compatible with the filters of the specified access parameters object
				/// @author Hugues Romain
				virtual bool isCompatibleWith(
					const graph::AccessParameters& accessParameters
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Builds the list of reservation rule types.
				/// @return the list of reservation rule types
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				//////////////////////////////////////////////////////////////////////////
				/// Designed to be used by HTMLForm::getSelectInput.
				static ReservationRuleTypesList GetTypesList();



				//////////////////////////////////////////////////////////////////////////
				/// Gets the name of a reservation rule type.
				/// @param type the type to be named
				/// @return the name of the specified reservation rule type
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				//////////////////////////////////////////////////////////////////////////
				static std::string GetTypeName(
					ReservationRuleType type
				);



				//////////////////////////////////////////////////////////////////////////
				/// PT Use rule CMS exporter.
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/
				/// TODO Wiki page
				//////////////////////////////////////////////////////////////////////////
				/// @param pm parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

			//@}

			static std::string SerializeForbiddenDays(
				const ReservationForbiddenDays& value
			);

			static ReservationForbiddenDays UnserializeForbiddenDays(
				const std::string& value
			);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_PTUseRule_h__
