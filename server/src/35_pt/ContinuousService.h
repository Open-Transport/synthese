
/** ContinuousService class header.
	@file ContinuousService.h

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

#ifndef SYNTHESE_ENV_CONTINUOUSSERVICE_H
#define SYNTHESE_ENV_CONTINUOUSSERVICE_H

#include "Object.hpp"

#include "SchedulesBasedService.h"
#include "Registry.h"

#include <string>

namespace synthese
{
	namespace pt
	{
		FIELD_TIME(Range)
		FIELD_TIME(MaxWaitingTime)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(ServiceNumber),
			FIELD(ServiceSchedules),
			FIELD(ServicePath),
			FIELD(Range),					//!< Continuous service range (minutes).
			FIELD(MaxWaitingTime),			//!< Max waiting waiting time before next service.
			FIELD(BikeComplianceId),
			FIELD(HandicappedComplianceId),
			FIELD(PedestrianComplianceId),
			FIELD(ServiceDates)
		> ContinuousServiceSchema;

		class JourneyPattern;

		/** Continuous service.
			@ingroup m35
		*/
		class ContinuousService:
			public Object<ContinuousService, ContinuousServiceSchema>,
			public SchedulesBasedService
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<ContinuousService>	Registry;

			ContinuousService(
				util::RegistryKeyType id = 0,
				std::string serviceNumber = std::string(),
				graph::Path* path = NULL,
				boost::posix_time::time_duration range = boost::posix_time::time_duration(0,0,0),
				boost::posix_time::time_duration maxWaitingTime = boost::posix_time::time_duration(0,0,0)
			);

			~ContinuousService ();


			//! @name Getters
			//@{
				boost::posix_time::time_duration getMaxWaitingTime () const;
				boost::posix_time::time_duration getRange () const;
			//@}

			//! @name Setters
			//@{
				void setMaxWaitingTime (boost::posix_time::time_duration maxWaitingTime);
				void setRange (boost::posix_time::time_duration range);
				virtual void setRules(const Rules& value);
				virtual void setPath(graph::Path* path);
				virtual void setDataSchedules(
					const Schedules& departureSchedules,
					const Schedules& arrivalSchedules
				);
			//@}

			//! @name Query methods
			//@{
				bool isContinuous () const;

				/** Generation of the next departure of a service according to a schedule and a presence date time, in the day of the presence time only, according to the compliances.
					@param RTData use real time data (ignored for real time vertex)
					@param getDeparture
					@param edge Edge
					@param presenceDateTime Goal  time
					@param checkIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@return A full ServicePointer to the service. If the service cannot be used at the specified date/time, then the ServicePointer points to a NULL service.
					@author Hugues Romain
					@date 2007-2010
					@warning The service index is unknown in the generated ServicePointer.
				*/
				virtual graph::ServicePointer getFromPresenceTime(
					const graph::AccessParameters& accessParameters,
					bool THData,
					bool RTData,
					bool getDeparture,
					const graph::Edge& edge,
					const boost::posix_time::ptime& presenceDateTime,
					bool checkIfTheServiceIsReachable,
					bool inverted,
					bool ignoreReservation,
					bool allowCanceled,
					graph::UseRule::ReservationDelayType reservationRulesDelayType = graph::UseRule::RESERVATION_INTERNAL_DELAY
				) const;

				virtual void completeServicePointer(
					graph::ServicePointer& servicePointer,
					const graph::Edge& edge,
					const graph::AccessParameters&
				) const;


				virtual boost::posix_time::time_duration getDepartureBeginScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const;

				virtual boost::posix_time::time_duration getDepartureEndScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const;

				virtual boost::posix_time::time_duration getArrivalBeginScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const;

				virtual boost::posix_time::time_duration getArrivalEndScheduleToIndex(
					bool RTData,
					std::size_t rankInPath
				) const;

				virtual const boost::posix_time::time_duration getDataLastDepartureSchedule(size_t i) const;
				virtual const boost::posix_time::time_duration getDataLastArrivalSchedule(size_t i) const;

				virtual void toParametersMap(
					util::ParametersMap& map,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;
			
				virtual SubObjects getSubObjects() const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;

		};
	}
}

#endif
