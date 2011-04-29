
/** ReservationRule class implementation.
	@file ReservationRule.cpp

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

#include "ReservationContact.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace pt;

	namespace util
	{
		template<> const string Registry<ReservationContact>::KEY("ReservationRule");
	}

	namespace pt
	{
		ReservationContact::ReservationContact(
			RegistryKeyType key
		):	Registrable(key)
		{
		}

		ReservationContact::~ReservationContact()
		{
		}



		const std::string&
		ReservationContact::getPhoneExchangeOpeningHours () const
		{
			return _phoneExchangeOpeningHours;
		}



		void
		ReservationContact::setPhoneExchangeOpeningHours (const std::string& phoneExchangeOpeningHours)
		{
			_phoneExchangeOpeningHours = phoneExchangeOpeningHours;
		}




		const std::string&
		ReservationContact::getWebSiteUrl () const
		{
			return _webSiteUrl;
		}


		void
		ReservationContact::setWebSiteUrl (const std::string& webSiteUrl)
		{
			_webSiteUrl = webSiteUrl;
		}




		void
		ReservationContact::setPhoneExchangeNumber (const std::string& phoneExchangeNumber)
		{
			_phoneExchangeNumber = phoneExchangeNumber;
		}




		const std::string&
		ReservationContact::getPhoneExchangeNumber () const
		{
			return _phoneExchangeNumber;
		}




		const std::string& ReservationContact::getDescription() const
		{
			return _description;
		}



		void
		ReservationContact::setDescription (const std::string& description)
		{
			_description = description;
		}




		bool ReservationContact::getOnline() const
		{
			return false;
		}
	}
}
