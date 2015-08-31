/** Fwchild1 class implementation.
	@file Fwchild1.cpp

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

#include "FwChild1.hpp"


using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{

	using namespace spike;
	using namespace util;

	CLASS_DEFINITION(FwChild1, "t991_fwchild1", 991)
	FIELD_DEFINITION_OF_TYPE(Property1, "property1", SQL_TEXT)

	namespace spike
	{

		FwChild1::FwChild1(
			RegistryKeyType id
		):	Registrable(id),
			Object<FwChild1, FwChild1Schema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Property1)
                    )
                )
		{}



		FwChild1::~FwChild1()
		{

		}
}	}
