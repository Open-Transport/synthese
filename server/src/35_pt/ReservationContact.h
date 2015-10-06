
/** ReservationRule class header.
	@file ReservationRule.h

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

#ifndef SYNTHESE_ENV_RESERVATIONRULE_H
#define SYNTHESE_ENV_RESERVATIONRULE_H

#include "Object.hpp"

#include "NumericField.hpp"
#include "StringField.hpp"
#include "ImportableTemplate.hpp"

namespace synthese
{
	FIELD_STRING(PhoneExchangeNumber) //!< Phone number for reservation
	FIELD_STRING(PhoneExchangeOpeningHours) //!< Opening hours for phone exchange
	FIELD_STRING(Description) //!< Additional info about phone exchange or reservation mode
	FIELD_STRING(WebsiteURL) //!< URL of a website allowing online reservation

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(Name),
		FIELD(PhoneExchangeNumber),
		FIELD(PhoneExchangeOpeningHours),
		FIELD(Description),
		FIELD(WebsiteURL)
	> ReservationContactSchema;

	namespace pt
	{
		/** Reservation contact.
			@ingroup m35
		*/
		class ReservationContact:
			public Object<ReservationContact, ReservationContactSchema>,
			public virtual util::Registrable,
			public impex::ImportableTemplate<ReservationContact>
		{
		 public:
			ReservationContact(
				util::RegistryKeyType key = 0
			);

			virtual bool getOnline() const;

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif
