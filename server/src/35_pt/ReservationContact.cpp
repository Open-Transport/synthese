
/** ReservationRule class implementation.
	@file ReservationRule.cpp

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

#include "ReservationContact.h"

#include "Profile.h"
#include "TransportNetworkRight.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace pt;

	CLASS_DEFINITION(ReservationContact, "t021_reservation_contacts", 21)
	FIELD_DEFINITION_OF_OBJECT(ReservationContact, "reservation_contact_id", "reservation_contact_ids")

	FIELD_DEFINITION_OF_TYPE(PhoneExchangeNumber, "phone_exchange_number", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(PhoneExchangeOpeningHours, "phone_exchange_opening_hours", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Description, "description", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(WebsiteURL, "web_site_url", SQL_TEXT)

	namespace pt
	{
		ReservationContact::ReservationContact(
			RegistryKeyType key
		):	Registrable(key),
			Object<ReservationContact, ReservationContactSchema>(
			Schema(
				FIELD_VALUE_CONSTRUCTOR(Key, key),
				FIELD_DEFAULT_CONSTRUCTOR(Name),
				FIELD_DEFAULT_CONSTRUCTOR(PhoneExchangeNumber),
				FIELD_DEFAULT_CONSTRUCTOR(PhoneExchangeOpeningHours),
				FIELD_DEFAULT_CONSTRUCTOR(Description),
				FIELD_DEFAULT_CONSTRUCTOR(WebsiteURL)
			)	)
		{}



		bool ReservationContact::getOnline() const
		{
			return false;
		}


		bool ReservationContact::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<pt::TransportNetworkRight>(security::WRITE);
		}

		bool ReservationContact::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<pt::TransportNetworkRight>(security::WRITE);
		}

		bool ReservationContact::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<pt::TransportNetworkRight>(security::DELETE_RIGHT);
		}
}	}
