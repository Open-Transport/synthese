
/** Service class header.
    @file Service.h

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

#ifndef SYNTHESE_ENV_SERVICE_H
#define SYNTHESE_ENV_SERVICE_H

#include "ServicePointer.h"
#include "Registrable.h"
#include "Registry.h"
#include "RuleUser.h"

#include <string>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace graph
	{
		class Path;
		class AccessParameters;

		/** Service abstract base class.

			A service represents the ability to follow a path
			at certain days and hours.

			The days when the service is provided are stored in a Calendar object.
			Even if a Service intrinsically corresponds to a sequence of
			(arrival schedule - departure schedule) couples, these schedules are not
			stored in Service objects but per Edge (JourneyPattern/Road). However, this is how
			Service objects are persisted.

			It is completely independent from the "vehicle" : this ability
			can be provided by an external entity (bus, train...),
			but also self-provided by the traveller himself
			(walking, cycling...).

			@warning Each constructor of Service implementations must run clearRTData().


			@ingroup m18
		*/
		class Service:
			public RuleUser,
			public virtual util::Registrable
		{
		private:
			static const std::string ATTR_SERVICE_ID;
			static const std::string ATTR_SERVICE_NUMBER;
			static const std::string ATTR_JOURNEY_PATTERN_ID;

		public:
			static const boost::posix_time::time_duration DAY_DURATION;

		protected:
			std::string				_serviceNumber;
			Path*					_path;

		public:
			Service(
				const std::string& serviceNumber,
				Path* path
			);

			Service(
				util::RegistryKeyType id = 0
			);
			~Service ();


			//! @name Getters
			//@{
				const Path*			getPath () const;
				Path*				getPath ();
				const std::string&	getServiceNumber()	const;
			//@}

			//! @name Setters
			//@{
				virtual void setPath(Path* path);
				virtual void setServiceNumber(std::string serviceNumber);
			//@}



			//! @name Services
			//@{
				virtual const RuleUser* _getParentRuleUser() const;

				virtual std::string getRuleUserName() const { return "Service " + getServiceNumber(); }

				virtual bool isContinuous () const = 0;



				virtual std::string getTeam() const;

				virtual boost::posix_time::time_duration getDepartureBeginScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;

				virtual boost::posix_time::time_duration getDepartureEndScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;

				virtual boost::posix_time::time_duration getArrivalBeginScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;

				virtual boost::posix_time::time_duration getArrivalEndScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const = 0;

				virtual bool nonConcurrencyRuleOK(
					boost::posix_time::ptime& time,
					boost::posix_time::time_duration& range,
					const graph::Edge& departureEdge,
					const graph::Edge& arrivalEdge,
					std::size_t userClassRank
				) const;

				virtual void clearNonConcurrencyCache() const;


				/** Tests if the service could be inserted in the same line than the current one, according to the line theory.
					@param other service to test
					@return bool true if the two services are compatible
					@author Hugues Romain
					@date 2008
					@warning the method must be used only at a time where the service knows its path, so use the method on the currently registered service, taking the new one as parameter
				*/
				virtual bool respectsLineTheoryWith(
					const Service& other
				) const { return true; }



				/** Generation of the next departure of a service according to a schedule and a presence date time, in the day of the presence time only, according to the compliances.
					@param THData true to base the calculation on Theoretical time,
						   false to disable them
					@param RTData true to base the calculation on Real Time values,
						   if false	the value of the THData parameter determines
						   if we use the theoretical values.
					@param getDeparture
					@param edge Edge
					@param presenceDateTime Goal  time
					@param checkIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@param inverted : indicates if the range computing must follow the same rules as method says (false) or the inverted ones (true)
					@param allowCanceled returns the service even if it is canceled at the specified edge. In this case, the _canceled attribute of the returned pointer is set to true.
					@param accessParameters access parameters to check for compatibility
					@return A full ServicePointer to the service. If the service cannot be used at the specified date/time, then the ServicePointer points to a NULL service.
					@author Hugues Romain
					@date 2007
					@warning The service index is unknown in the generated ServicePointer.
				*/
				virtual ServicePointer getFromPresenceTime(
					const AccessParameters& accessParameters,
					bool THData,
					bool RTData,
					bool getDeparture,
					const Edge& edge,
					const boost::posix_time::ptime& presenceDateTime,
					bool checkIfTheServiceIsReachable,
					bool inverted,
					bool ignoreReservation,
					bool allowCanceled,
					UseRule::ReservationDelayType reservationRulesDelayType = UseRule::RESERVATION_INTERNAL_DELAY
				) const = 0;



				virtual void completeServicePointer(
					ServicePointer& servicePointer,
					const Edge& edge,
					const AccessParameters& accessParameters
				) const = 0;

				/** Date of the departure from the origin of the service.
				@param departureDate Date of use of the service at the scheduled point
				@param departureTime Known schedule of departure in the service journey
				@return Date of the departure from the origin of the service.
				@author Hugues Romain
				@date 2007

				*/
				boost::posix_time::ptime getOriginDateTime(
					bool RTData,
					const boost::gregorian::date& departureDate,
					const boost::posix_time::time_duration& departureTime
				) const;

				/** Gets a departure schedule for this service.
					@param rank Rank of the stop where to get the departure schedule
					@return see the implementations of the method.
				*/
				virtual boost::posix_time::time_duration getDepartureSchedule(
					bool RTData,
					size_t rank
				) const = 0;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Restores real time data to theoretical value.
			/// Sets the next update into the next day.
			virtual void clearRTData();



			//////////////////////////////////////////////////////////////////////////
			/// Gets the time of day of a duration which can be greater than 24 hours
			/// Example : 25:00:00 => 1:00:00
			/// @param value the duration to transform
			/// @result the time of day duration
			static boost::posix_time::time_duration GetTimeOfDay(const boost::posix_time::time_duration& value);


			virtual void toParametersMap(
				util::ParametersMap& map,
				bool withAdditionalParameters,
				boost::logic::tribool withFiles = boost::logic::indeterminate,
				std::string prefix = std::string()
			) const;
		};
	}
}

#endif
