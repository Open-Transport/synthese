
/** Axis class implementation.
	@file Axis.cpp

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

#include "Axis.h"
#include "Registry.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<env::Axis>::KEY("Axis");
	}

	namespace env
	{
		Axis::Axis(
			RegistryKeyType id,
			std::string name,
			bool free,
			bool allowed
		):	Registrable(id)
			, _name (name)
			, _free (free)
			, _allowed (allowed)
		{
		}



		Axis::~Axis ()
		{
		}



		const std::string& 
		Axis::getName () const
		{
			return _name;
		}



		void 
		Axis::setName (const std::string& name)
		{
			_name = name;
		}



		bool 
		Axis::isFree () const
		{
			return _free;
		}


		bool 
		Axis::isAllowed () const
		{
			return _allowed;
		}



		void 
		Axis::setFree (bool isFree)
		{
			_free = isFree;
		}



		void 
		Axis::setAllowed (bool isAllowed)
		{
			_allowed = isAllowed;
		}



	}
}
