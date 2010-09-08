
/** SchedulesBasedService class header.
	@file SchedulesBasedService.h

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

#ifndef SYNTHESE_pt_SchedulesBasedService_h__
#define SYNTHESE_pt_SchedulesBasedService_h__

#include "NonPermanentService.h"

namespace synthese
{
	namespace pt
	{
		/** SchedulesBasedService class.
			@ingroup m35
		*/
		class SchedulesBasedService:
			public NonPermanentService
		{
		public:
			typedef std::vector<boost::posix_time::time_duration> Schedules;

			class BadSchedulesException: public util::Exception
			{
			public:
				BadSchedulesException(): util::Exception("Bad schedule") {}
			};

		protected:
			//! @name Theoretical data
			//@{
				Schedules	_departureSchedules;	//!< Departure schedules
				Schedules	_arrivalSchedules;		//!< Arrival schedules
			//@}

			//! @name Real time data
			//@{
				Schedules	_RTDepartureSchedules;
				Schedules	_RTArrivalSchedules;
			//@}

		public:
			SchedulesBasedService(
				std::string serviceNumber,
				graph::Path* path
			);


			//! @name Getters
			//@{
				const Schedules& getDepartureSchedules(bool RTData) const;
				const Schedules& getArrivalSchedules(bool RTData) const;
			//@}

			//! @name Setters
			//@{
				
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
			//@}



				/** Gets a departure schedule for this service.
					@param rank Rank of the stop where to get the departure schedule
					@return The schedule at the specified stop rank
				*/
				virtual boost::posix_time::time_duration getDepartureSchedule (bool RTData, std::size_t rank) const;

				const boost::posix_time::time_duration& getLastDepartureSchedule(bool RTData) const;

				virtual const boost::posix_time::time_duration& getLastArrivalSchedule(bool RTData) const;

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

				virtual void clearRTData();

				virtual void _computeNextRTUpdate();

				void setSchedulesFromOther(const SchedulesBasedService& other, boost::posix_time::time_duration shift);
				void generateIncrementalSchedules(boost::posix_time::time_duration firstSchedule);
			//@}


			//! @name Encoding / decoding
			//@{
				static std::string EncodeSchedule(const boost::posix_time::time_duration& value);
				static boost::posix_time::time_duration DecodeSchedule(const std::string value);
				
				
				//////////////////////////////////////////////////////////////////////////
				/// Encode schedules into a string.
				/// @param shiftArrivals duration to add to the arrival times before encoding (default 0)
				/// @author Hugues Romain
				std::string encodeSchedules(
					boost::posix_time::time_duration shiftArrivals = boost::posix_time::minutes(0)
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Reads schedules from encoded strings.
				/// @param value encoded strings
				/// @param shiftArrivals duration to add to the arrival times (default 0)
				/// @author Hugues Romain
				void decodeSchedules(
					const std::string value,
					boost::posix_time::time_duration shiftArrivals = boost::posix_time::minutes(0)
				);
			//@}
		};
	}
}

#endif // SYNTHESE_pt_SchedulesBasedService_h__
