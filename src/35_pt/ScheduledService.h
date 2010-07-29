
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

#include "Types.h"
#include "Registry.h"
#include "SchedulesBasedService.h"

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

	namespace pt
	{
		class JourneyPattern;
		class LineStop;
	}

	namespace pt
	{
		/** Scheduled service.
			@ingroup m35
		*/
		class ScheduledService
		:	public SchedulesBasedService
		{
		public:
			
			/// Chosen registry class.
			typedef util::Registry<ScheduledService>	Registry;

		private:
			typedef std::map<
				boost::tuples::tuple<
					std::size_t,
					std::size_t,
					std::size_t,
					boost::gregorian::date
				>, bool
			> _NonConcurrencyCache;

			mutable _NonConcurrencyCache _nonConcurrencyCache;
			mutable boost::recursive_mutex _nonConcurrencyCacheMutex;

			std::string	_team;


		public:

			ScheduledService(
				util::RegistryKeyType id = 0,
				std::string serviceNumber = std::string(),
				graph::Path* path = NULL
			);

			~ScheduledService ();

		    
			//! @name Getters
			//@{
				virtual std::string getTeam() const;
				const pt::JourneyPattern* getRoute() const;
			//@}

			//! @name Setters
			//@{
				void	setTeam(const std::string& team);
			//@}

			//! @name Update methods
			//@{
			//@}

			//! @name Query methods
			//@{
				virtual bool isContinuous () const;

				virtual bool nonConcurrencyRuleOK(
					const boost::gregorian::date& date,
					const graph::Edge& departureEdge,
					const graph::Edge& arrivalEdge,
					std::size_t userClassRank
				) const;

				virtual void clearNonConcurrencyCache() const;

				graph::UseRule::ReservationAvailabilityType getReservationAbility(
					const boost::gregorian::date& date,
					std::size_t userClassRank
				) const;

				boost::posix_time::ptime getReservationDeadLine(
					const boost::gregorian::date& date,
					std::size_t userClassRank
				) const;

				/** Generation of the next departure of a service according to a schedule and a presence date time, in the day of the presence time only, according to the compliances.
					@param getDeparture
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
					bool getDeparture,
					std::size_t userClassRank
					, const graph::Edge& edge
					, const boost::posix_time::ptime& presenceDateTime
					, bool controlIfTheServiceIsReachable
					, bool inverted
				) const;
				
				virtual void completeServicePointer(
					graph::ServicePointer& servicePointer,
					const graph::Edge& edge
				) const;

				

				virtual boost::posix_time::time_duration getDepartureBeginScheduleToIndex(bool RTData, std::size_t rankInPath) const;
				virtual boost::posix_time::time_duration getDepartureEndScheduleToIndex(bool RTData, std::size_t rankInPath) const;
				virtual boost::posix_time::time_duration getArrivalBeginScheduleToIndex(bool RTData, std::size_t rankInPath) const;
				virtual boost::posix_time::time_duration getArrivalEndScheduleToIndex(bool RTData, std::size_t rankInPath) const;
			//@}

		};
	
		bool operator==(const ScheduledService& first, const ScheduledService& second);
	}
}

#endif
