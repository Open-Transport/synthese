
/** VinciSite class implementation.
	@file VinciSite.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Rapha�l Murat - Vinci Park <rmurat@vincipark.com>

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

#include "71_vinci_bike_rental/VinciSite.h"

namespace synthese
{
	using namespace util;

	namespace vinci
	{
		VinciSite::VinciSite(uid id)
			: Registrable<uid, VinciSite>(id)
		{ }


		void VinciSite::setName(const std::string& name)
		{
			_name = name;
		}

		const std::string& VinciSite::getName() const
		{
			return _name;
		}

		void VinciSite::setAddress( const std::string& address )
		{
			_address = address;
		}

		void VinciSite::setPhone( const std::string& phone )
		{
			_phone = phone;
		}

		const std::string& VinciSite::getAddress() const
		{
			return _address;
		}

		const std::string& VinciSite::getPhone() const
		{
			return _phone;
		}
	}
}
