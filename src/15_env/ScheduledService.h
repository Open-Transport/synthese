
/** ScheduledService class header.
	@file ScheduledService.h

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

#ifndef SYNTHESE_ENV_SCHEDULEDSERVICE_H
#define SYNTHESE_ENV_SCHEDULEDSERVICE_H

#include "NonPermanentService.h"
#include "Types.h"
#include "Registry.h"

#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/thread/recursive_mutex.hpp>

namespace synthese
{
	namespace graph
	{
		class Vertex;
	}

	namespace env
	{
		class Line;
		class LineStop;

		/** Scheduled service.
			@ingroup m35
		*/
		class ScheduledService
		:	public NonPermanentService
		{
		public:
			typedef std::vector<time::Schedule> Schedules;
			typedef std::vector<const graph::Vertex*> ServedVertices;
			
			/// Chosen registry class.
			typedef util::Registry<ScheduledService>	Registry;

		private:
			typedef std::map<
				boost::tuples::tuple<
					std::size_t,
					std::size_t,
					graph::UserClassCode,
					boost::gregorian::date
				>, bool
			> _NonConcurrencyCache;

			mutable _NonConcurrencyCache _nonConcurrencyCache;
			mutable boost::recursive_mutex _nonConcurrencyCacheMutex;
			boost::posix_time::ptime _nextRTUpdate;

			Schedules	_departureSchedules;	//!< Departure schedules
			Schedules	_RTDepartureSchedules;
			Schedules	_arrivalSchedules;		//!< Arrival schedules
			Schedules	_RTArrivalSchedules;
			ServedVertices	_RTVertices;	//!< Real time edges
			std::string	_team;

			void _computeNextRTUpdate();

		public:

			ScheduledService(
				util::RegistryKeyType id = UNKNOWN_VALUE,
				std::string serviceNumber = std::string(),
				graph::Path* path = NULL
			);

			~ScheduledService ();

		    
			//! @name Getters
			//@{
				virtual std::string getTeam() const;
				const Schedules& getDepartureSchedules(bool RTData) const;
				const Schedules& getArrivalSchedules(bool RTData) const;
				const Line* getRoute() const;
				const boost::posix_time::ptime& getNextRTUpdate() const;
			//@}

			//! @name Setters
			//@{
				void	setPath(graph::Path* path);
				
				
				/** Departure schedules update.
					Updates both theoretical and real time data.
					@param schedules Departure schedules of the service
					@author Hugues Romain
				*/
				void	setDepartureSchedules(const Schedules& schedules);



				/** Arrival schedules update.
					Updates both theoretical and real time data.
					@param schedules Arrival schedules of the service
					@author Hugues Romain
				*/
				void	setArrivalSchedules(const Schedules& schedules);
				void	setTeam(const std::string& team);
			//@}

			//! @name Update methods
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Apply late duration on real time schedules of the service.
				/// @param rank Rank of the first edge where the late begins.
				///		rank must be inferior than the size of the schedules array
				/// @param value Duration of the late
				/// @param atArrival true if the late concerns the arrival time
				/// @param atDeparture true if the late concerns the departure time
				/// @param updateFollowingSchedules true if the method must propagate the
				///		late on each following edges (at arrivals and departures)
				void applyRealTimeLateDuration(
					std::size_t rank,
					boost::posix_time::time_duration value,
					bool atArrival,
					bool atDeparture,
					bool updateFollowingSchedules
				);



				//////////////////////////////////////////////////////////////////////////
				/// Update a served edge at real time.
				/// @param rank Rank of the edge to update
				/// @param value Served edge 
				void setRealTimeVertex(
					std::size_t rank,
					const graph::Vertex* value
				);



				//////////////////////////////////////////////////////////////////////////
				/// Restore real time data into theoretical values.
				/// Sets the next update into the next day.
				void clearRTData();
			//@}

			//! @name Query methods
			//@{
				virtual bool isContinuous () const;

				virtual bool nonConcurrencyRuleOK(
					const time::Date& date,
					const graph::Edge& departureEdge,
					const graph::Edge& arrivalEdge,
					graph::UserClassCode userClass
				) const;

				virtual void clearNonConcurrencyCache() const;

				graph::UseRule::ReservationAvailabilityType getReservationAbility(
					const time::Date& date
				) const;
				time::DateTime getReservationDeadLine(
					const time::Date& date
				) const;

				/** Generation of the next departure of a service according to a schedule and a presence date time, in the day of the presence time only, according to the compliances.
					@param method Search departure or arrival :
						- ServicePointer::DEPARTURE_TO_ARRIVAL
						- ServicePointer::ARRIVAL_TO_DEPARTURE
					@param edge Edge
					@param presenceDateTime Goal  time
					@param controlIfTheServiceIsReachable service selection method :
						- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
						- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
					@return A full ServicePointer to the service. If the service cannot be used at the specified date/time, then the ServicePointer points to a NULL service.
					@author Hugues Romain
					@date 2007
					@warning The service index is unknown in the generated ServicePointer.					
				*/
				virtual graph::ServicePointer getFromPresenceTime(
					bool RTData,
					graph::AccessDirection method,
					graph::UserClassCode userClass
					, const graph::Edge* edge
					, const time::DateTime& presenceDateTime
					, bool controlIfTheServiceIsReachable
					, bool inverted
				) const;
				
				virtual time::DateTime getLeaveTime(
					const graph::ServicePointer& servicePointer
					, const graph::Edge* edge
				) const;

				
				/** Gets a departure schedule for this service.
					@param rank Rank of the stop where to get the departure schedule
					@return time::Schedule The schedule at the specified stop rank
				*/
				virtual time::Schedule getDepartureSchedule (bool RTData, std::size_t rank) const;

				const time::Schedule& getLastDepartureSchedule(bool RTData) const;

				virtual const time::Schedule& getLastArrivalSchedule(bool RTData) const;

				virtual time::Schedule getDepartureBeginScheduleToIndex(bool RTData, std::size_t rankInPath) const;
				virtual time::Schedule getDepartureEndScheduleToIndex(bool RTData, std::size_t rankInPath) const;
				virtual time::Schedule getArrivalBeginScheduleToIndex(bool RTData, std::size_t rankInPath) const;
				virtual time::Schedule getArrivalEndScheduleToIndex(bool RTData, std::size_t rankInPath) const;

				const graph::Vertex* getRealTimeVertex(
					std::size_t rank
				) const;

			//@}

		};
	
		bool operator==(const ScheduledService& first, const ScheduledService& second);
	}
}

#endif
