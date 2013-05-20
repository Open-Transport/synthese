
/** Fare class implementation.
	@file Fare.cpp

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

#include "Fare.hpp"
#include "FareType.hpp"
#include "FareTypeFlatRate.hpp"
#include "FareTypeDistance.hpp"
#include "Registry.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<fare::Fare>::KEY("Fare");
	}

	namespace fare
	{
		Fare::Fare(RegistryKeyType key)
		:	Registrable(key),
			_requiredContinuity(false)
		{
		}



		void Fare::setTypeNumber(FareType::FareTypeNumber number)
		{
			switch(number)
			{
				case FareType::FARE_TYPE_FLAT_RATE:
					_type = boost::shared_ptr<FareTypeFlatRate>(new FareTypeFlatRate());
					break;
				case FareType::FARE_TYPE_DISTANCE:
					_type = boost::shared_ptr<FareTypeDistance>(new FareTypeDistance());
					break;
				default:
					_type = boost::shared_ptr<FareType>(new FareType(FareType::FARE_TYPE_UNKNOWN));
					break;
			}
		}
	}
}
