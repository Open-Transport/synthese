
/** VinciBike class implementation.
	@file VinciBike.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "71_vinci_bike_rental/VinciBike.h"

namespace synthese
{
	using namespace util;

	namespace vinci
	{


		const std::string& VinciBike::getNumber() const
		{
			return _number;
		}

		void VinciBike::setNumber( const std::string& number )
		{
			_number = number;
		}

		const std::string& VinciBike::getMarkedNumber() const
		{
			return _markedNumber;
		}

		void VinciBike::setMarkedNumber( const std::string& markedNumber )
		{
			_markedNumber = markedNumber;
		}

		VinciBike::VinciBike( uid id/*=0*/ )
			: Registrable<uid, VinciBike>(id)
		{
			
		}
	}
}
