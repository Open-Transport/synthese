
/** NamedPlace class implementation.
	@file NamedPlace.cpp

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

#include "NamedPlace.h"
#include "City.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace geography
	{
		NamedPlace::NamedPlace(
		):	_city(NULL)
		{

		}



		const std::string&
			NamedPlace::getOfficialName () const
		{
			return getName ();
		}



		std::string NamedPlace::getFullName() const
		{
			return ((_city != NULL) ? (_city->getName() + " ") : "") + getName();
		}



		string NamedPlace::getName13OrName() const
		{
			return _name13.empty() ? _name.substr(0, 13) : _name13;
		}



		std::string NamedPlace::getName26OrName() const
		{
			return _name26.empty() ? _name.substr(0, 26) : _name26;
		}
	}
}
