
/** FreeDRTTimeSlot class header.
	@file FreeDRTTimeSlot.hpp

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

#ifndef SYNTHESE_pt_FreeDRTTimeSlot_hpp__
#define SYNTHESE_pt_FreeDRTTimeSlot_hpp__

#include "ReservableService.hpp"

#include "Registrable.h"
#include "Registry.h"
#include "NonPermanentService.h"
#include "Journey.h"

namespace synthese
{
	namespace geography
	{
		class Place;
	}

	namespace pt
	{
		class FreeDRTArea;

		//////////////////////////////////////////////////////////////////////////
		/// Free DRT time slot.
		/// Contracts :
		///  - _commecialSpeed <= _maxSpeed
		///
		/// Notes :
		///  - _maxCapacity is available for practical reasons. The 37_pt_operation
		///    module is a better place to define how much vehicles are available
		///    and their capacity
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m35
		class FreeDRTTimeSlot:
			public virtual util::Registrable,
			public NonPermanentService,
			public ReservableService
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<FreeDRTTimeSlot>	Registry;

			typedef std::size_t Capacity;
			typedef double KMHSpeed;

		private:
			/// @name Data
			//@{
				boost::posix_time::time_duration _firstDeparture;
				boost::posix_time::time_duration _lastArrival;
				boost::optional<Capacity> _maxCapacity;
				KMHSpeed _commercialSpeed;
				KMHSpeed _maxSpeed;
			//@}

		public:
			FreeDRTTimeSlot(
				util::RegistryKeyType id = 0
			);

			/// @name Getters
			//@{
				const boost::posix_time::time_duration& getFirstDeparture() const { return _firstDeparture; }
				const boost::posix_time::time_duration& getLastArrival() const { return _lastArrival; }
				boost::optional<Capacity> getMaxCapacity() const { return _maxCapacity; }
				KMHSpeed getCommercialSpeed() const { return _commercialSpeed; }
				KMHSpeed getMaxSpeed() const { return _maxSpeed; }
			//@}

			/// @name Setters
			//@{
				void setFirstDeparture(const boost::posix_time::time_duration& value){ _firstDeparture = value; }
				void setLastArrival(const boost::posix_time::time_duration& value){ _lastArrival = value; }
				void setMaxCapacity(boost::optional<Capacity> value){ _maxCapacity = value; }
				void setCommercialSpeed(KMHSpeed value){ _commercialSpeed = value; }
				void setMaxSpeed(KMHSpeed value){ _maxSpeed = value; }
			//@}

			/// @name Modifiers
			//@{
				void setArea(FreeDRTArea* value);
			//@}

			/// @name Services
			//@{
				FreeDRTArea* getArea() const;



				//////////////////////////////////////////////////////////////////////////
				/// Commercial duration calculation between two places.
				/// @param from departure place (this method does not check if the place
				/// belongs to the area)
				/// @param to arrival place (this method does not check if the place
				/// belongs to the area)
				/// @result the commercial duration between the two places
				/// @pre departure place is located (geometry is not null)
				/// @pre arrival place is located (geometry is not null)
				boost::posix_time::time_duration getJourneyDuration(
					const geography::Place& from,
					const geography::Place& to
				) const;



				graph::Journey getJourneyDepartureToArrival(
					const boost::posix_time::ptime time,
					const geography::Place& from,
					const geography::Place& to
				) const;



				graph::Journey getJourneyArrivalToDeparture(
					const boost::posix_time::ptime time,
					const geography::Place& from,
					const geography::Place& to
				) const;


				virtual void _computeNextRTUpdate();

				virtual bool isContinuous () const;


				/** Gets a departure schedule for this service.
					@param rank Rank of the stop where to get the departure schedule
					@return The schedule at the specified stop rank
				*/
				virtual boost::posix_time::time_duration getDepartureSchedule (bool RTData, std::size_t rank) const;

				virtual boost::posix_time::time_duration getArrivalSchedule (bool RTData, std::size_t rank) const;

				virtual const boost::posix_time::time_duration& getLastArrivalSchedule(bool RTData) const;

				//////////////////////////////////////////////////////////////////////////
				/// Virtual implementation for technical purpose only.
				/// Use getJourneyDepartureToArrival or getJourneyArrivalToDeparture instead.
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
					int reservationRulesDelayType = 0
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Virtual implementation for technical purpose only.
				/// Use getJourneyDepartureToArrival or getJourneyArrivalToDeparture instead.
				virtual void completeServicePointer(
					graph::ServicePointer& servicePointer,
					const graph::Edge& edge,
					const graph::AccessParameters& accessParameters
				) const;



				virtual boost::posix_time::time_duration getDepartureBeginScheduleToIndex(bool RTData, std::size_t rankInPath) const;
				virtual boost::posix_time::time_duration getDepartureEndScheduleToIndex(bool RTData, std::size_t rankInPath) const;
				virtual boost::posix_time::time_duration getArrivalBeginScheduleToIndex(bool RTData, std::size_t rankInPath) const;
				virtual boost::posix_time::time_duration getArrivalEndScheduleToIndex(bool RTData, std::size_t rankInPath) const;



				virtual graph::UseRule::ReservationAvailabilityType getReservationAbility(
					const boost::gregorian::date& date,
					std::size_t userClassRank
				) const;

				virtual boost::posix_time::ptime getReservationDeadLine(
					const boost::gregorian::date& date,
					std::size_t userClassRank
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_pt_FreeDRTTimeSlot_hpp__
