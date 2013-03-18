
/** PermanentService class header.
	@file PermanentService.h

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

#ifndef SYNTHESE_env_PermanentService_h__
#define SYNTHESE_env_PermanentService_h__

#include "Service.h"

namespace synthese
{
	namespace graph
	{
		class AccessParameters;

		/** PermanentService class.
			@ingroup m18
		*/
		class PermanentService
		:	public graph::Service
		{
		private:
			boost::optional<boost::posix_time::time_duration> _duration;

		public:
			PermanentService(
				util::RegistryKeyType id = 0,
				graph::Path* path = NULL,
				boost::optional<boost::posix_time::time_duration> duration = boost::optional<boost::posix_time::time_duration>()
			);

			//! @name Getters
			//@{
				boost::optional<boost::posix_time::time_duration> getDuration() const;
			//@}

			/** Gets a departure schedule for this service.
				@param rank Rank of the stop where to get the departure schedule.
				@warning Only a 0 value is allowed for the rank.
				@return time::Schedule The schedule at the specified stop rank
			*/
			virtual boost::posix_time::time_duration getDepartureSchedule(bool RTData, std::size_t rank = 0) const;

			virtual bool isContinuous () const;

			virtual boost::posix_time::time_duration getDepartureBeginScheduleToIndex(bool RTData, std::size_t rankInPath) const;
			virtual boost::posix_time::time_duration getDepartureEndScheduleToIndex(bool RTData, std::size_t rankInPath) const;
			virtual boost::posix_time::time_duration getArrivalBeginScheduleToIndex(bool RTData, std::size_t rankInPath) const;
			virtual boost::posix_time::time_duration getArrivalEndScheduleToIndex(bool RTData, std::size_t rankInPath) const;

			/** Generation of the next departure of a service according to a schedule and a presence date time, in the day of the presence time only, according to the compliances.
				@param THData ignored parameter
				@param RTData ignored parameter
				@param edge Edge
				@param presenceDateTime Goal  time
				@param controlIfTheServiceIsReachable service selection method :
					- true : the result is a usable service : its departure time must be in the future, and the reservation rules must be followed
					- false : the result is a runnable service : if the reservation on it is compulsory, then there must bu at least one reservation for the service
				@param allowCanceled ignored parameter
				@param accessParameters access parameters to check for compatibility
				@return A full ServicePointer to the service. If the service cannot be used at the specified date/time, then the ServicePointer points to a NULL service.
				@author Hugues Romain
				@date 2007
				@warning The service index is unknown in the generated ServicePointer.
			*/
			virtual graph::ServicePointer getFromPresenceTime(
				const AccessParameters& accessParameters,
				bool THData,
				bool RTData,
				bool getDeparture,
				const graph::Edge& edge,
				const boost::posix_time::ptime& presenceDateTime,
				bool controlIfTheServiceIsReachable,
				bool inverted,
				bool ingoreReservation,
				bool allowCanceled
			) const;

			virtual void completeServicePointer(
				graph::ServicePointer& servicePointer,
				const graph::Edge& edge,
				const AccessParameters& accessParameters
			) const;

		};
}	}

#endif // SYNTHESE_env_PermanentService_h__
