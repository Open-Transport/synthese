
/** FareType class implementation.
	@file FareType.cpp

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

#include "FareType.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	namespace fare
	{
		FareType::FareType(
			FareTypeNumber typeNumber
		):	_typeNumber(typeNumber),
			_accessPrice(0)
		{
		}



		string FareType::GetTypeName(FareTypeNumber value)
		{
			switch(value)
			{
//				case FARE_TYPE_SECTION: return "Section";
				case FARE_TYPE_DISTANCE: return "Kilométrique";
//				case FARE_TYPE_ZONAL: return "Zonale";
				case FARE_TYPE_FLAT_RATE: return "Forfaitaire";
				default: return "Autre type";
			}
			return "Autre type";
		}



		FareType::TypesList FareType::GetTypesList()
		{
			TypesList result;
//			result.push_back(make_pair(FARE_TYPE_SECTION, GetTypeName(FARE_TYPE_SECTION)));
			result.push_back(make_pair(FARE_TYPE_DISTANCE, GetTypeName(FARE_TYPE_DISTANCE)));
//			result.push_back(make_pair(FARE_TYPE_ZONAL, GetTypeName(FARE_TYPE_ZONAL)));
			result.push_back(make_pair(FARE_TYPE_FLAT_RATE, GetTypeName(FARE_TYPE_FLAT_RATE)));
			return result;
		}
	}
}
