
/** ReservationRule class header.
	@file ReservationRule.h

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

#ifndef SYNTHESE_ENV_RESERVATIONRULE_H
#define SYNTHESE_ENV_RESERVATIONRULE_H

#include "Registrable.h"
#include "RegistryWithAutocreationEnabled.h"
#include "Hour.h"
#include "DateTime.h"

#include "15_env/Types.h"

namespace synthese
{
	namespace env
	{
		/** Reservation rule.
			A reservation rule is seen as a compliance :
				- compliant = reservation compulsory
				- indeterminate = reservation optional
				- not compliant = reservation impossible
			@ingroup m35
		*/
		class ReservationRule
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::RegistryWithAutocreationEnabled<ReservationRule>	Registry;

		private:
			ReservationRuleType		_type;	//!< Type of the reservation rule

			bool _online;  //!< Reservation via SYNTHESE active
			
			bool _originIsReference; //!< Whether reference departure time is the line run departure time at its origin (true) or client departure time (false)

			int _minDelayMinutes;  //!< Minimum delay in minutes between reservation and reference moment
			int _minDelayDays;   //!< Minimum delay in days between reservation and reference moment
			int _maxDelayDays;  //!< Maxium number of days between reservation and departure.

			time::Hour _hourDeadLine; //!< Latest reservation hour the last day open for reservation

			std::string _phoneExchangeNumber;  //!< Phone number for reservation
			std::string _phoneExchangeOpeningHours;  //!< Opening hours for phone exchange
			std::string _description;   //!< Additional info about phone exchange or reservation mode
			std::string _webSiteUrl;    //!< URL of a website allowing online reservation

		 public:
			ReservationRule(util::RegistryKeyType key = UNKNOWN_VALUE);
			virtual ~ReservationRule();

			//! @name Getters
			//@{
				bool				getOriginIsReference()			const;
				const time::Hour&	getHourDeadLine()				const;
				const std::string&	getPhoneExchangeNumber()		const;
				const std::string&	getPhoneExchangeOpeningHours()	const;
				const std::string&	getWebSiteUrl ()				const;
				const std::string&	getDescription ()				const;
				int					getMinDelayDays()				const;
				int					getMinDelayMinutes()			const;
				int					getMaxDelayDays()				const;
				ReservationRuleType	getType()						const;
				bool				getOnline()						const;
			//@}

			//! @name Setters
			//@{
				void setHourDeadLine (const synthese::time::Hour& hourDeadLine);
				void setPhoneExchangeNumber (const std::string& phoneExchangeNumber);
				void setPhoneExchangeOpeningHours (const std::string& phoneExchangeOpeningHours);
				void setWebSiteUrl (const std::string& webSiteUrl);
				void setDescription (const std::string& description);
				void setMinDelayMinutes (int minDelayMinutes);
				void setMinDelayDays (int minDelayDays);
				void setMaxDelayDays (int maxDelayDays);
				void setOnline (bool online);
				void setOriginIsReference (bool originIsReference);
				void setType(ReservationRuleType value);
			//@}


			//! @name Query methods
			//@{


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
				time::DateTime getReservationDeadLine (
					const time::DateTime& originTime
					, const time::DateTime& departureTime
					) const;



				/** Reference function for calculation of start reservation date time.
				@param reservationTime Time when booking is done.
				@return The minimum date time to make a reservation.
				
				If no explicit rule defines this minimum time, the actual reservation time is returned.
				*/
				time::DateTime getReservationStartTime ( 
					const time::DateTime& reservationTime
					) const;


			    
				/** Indicates whether or not a path can be taken at a given date, 
					taking into account reservation delay rules.
					@param originTime Time of start of the corresponding service
					@param reservationTime Time of booking, if required.
					@param departureTime Desired departure time.
					@return true if the line run can be taken, false otherwise.

					This methods checks the following conditions :
						- if reservation is not compulsory, the run can be taken.
						- if reservation is compulsory, reservation time must precede reservation 
					dead line and be after reservation opening time.
				*/
				bool isRunPossible (
					const time::DateTime& originTime,
					bool stopBelongsToOptionalReservationPlaces,
					const synthese::time::DateTime& reservationTime,
					const synthese::time::DateTime& departureTime
				) const;
			    


				/** Indicates whether or not a reservation is possible for a given line run,
					at a certain date, taking into account delay rules.
					@param originTime Time of start of the corresponding service
					@param reservationTime Time of booking.
					@param departureTime Desired departure time.
					@return true if the reservation is possible, false otherwise.
				 
					This methods checks the following conditions :
						- reservation time must precede reservation dead line
						- reservation time must be later than reservation start time.
				*/
				bool isReservationPossible (
					const time::DateTime& originTime
					, const synthese::time::DateTime& reservationTime
					, const synthese::time::DateTime& departureTime
					) const;

			//@}

		};
	}
}

#endif
