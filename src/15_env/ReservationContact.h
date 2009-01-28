
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
#include "Registry.h"
#include "Hour.h"
#include "DateTime.h"

#include "15_env/Types.h"

namespace synthese
{
	namespace env
	{
		/** Reservation contact.
			@ingroup m35
		*/
		class ReservationContact
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<ReservationContact>	Registry;

		private:

			bool _online;  //!< Reservation via SYNTHESE active
			
			std::string _phoneExchangeNumber;  //!< Phone number for reservation
			std::string _phoneExchangeOpeningHours;  //!< Opening hours for phone exchange
			std::string _description;   //!< Additional info about phone exchange or reservation mode
			std::string _webSiteUrl;    //!< URL of a website allowing online reservation

		 public:
			ReservationContact(
				util::RegistryKeyType key = UNKNOWN_VALUE
			);
			virtual ~ReservationContact();

			//! @name Getters
			//@{
				const std::string&	getPhoneExchangeNumber()		const;
				const std::string&	getPhoneExchangeOpeningHours()	const;
				const std::string&	getWebSiteUrl ()				const;
				const std::string&	getDescription ()				const;
				bool				getOnline()						const;
			//@}

			//! @name Setters
			//@{
				void setPhoneExchangeNumber (const std::string& phoneExchangeNumber);
				void setPhoneExchangeOpeningHours (const std::string& phoneExchangeOpeningHours);
				void setWebSiteUrl (const std::string& webSiteUrl);
				void setDescription (const std::string& description);
				void setOnline (bool online);
			//@}
		};
	}
}

#endif
