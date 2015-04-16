
/** NamedPlace class implementation.
	@file NamedPlace.cpp

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

#include "NamedPlace.h"

#include "City.h"
#include "Env.h"
#include "GeographyModule.h"
#include "ObjectBase.hpp"
#include "ParametersMap.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace geography;
	using namespace util;

	namespace geography
	{
		NamedPlace::NamedPlace(
		):	_city(NULL)
		{}



		const std::string NamedPlace::getOfficialName() const
		{
			return getName();
		}



		string NamedPlace::getFullName() const
		{
			if(_city == NULL)
			{
				return getName();
			}
			else if(_getCityNameBeforePlaceName())
			{
				return _city->getName() + " " + getName();
			}
			else
			{
				return getName() + " " + _city->getName();
			}
		}



		string NamedPlace::getName13OrName() const
		{
			return _name13.empty() ? _name.substr(0, 13) : _name13;
		}



		string NamedPlace::getName26OrName() const
		{
			return _name26.empty() ? _name.substr(0, 26) : _name26;
		}



		bool NamedPlace::_getCityNameBeforePlaceName() const
		{
			return GeographyModule::GetCityNameBeforePlaceName();
		}
}	}
