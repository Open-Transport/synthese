
/** ScenarioFolder class implementation.
	@file ScenarioFolder.cpp

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

#include "ScenarioFolder.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<messages::ScenarioFolder>::KEY("ScenarioFolder");
	}

	namespace messages
	{


		ScenarioFolder* ScenarioFolder::getParent() const
		{
			return _parent;
		}

		const std::string& ScenarioFolder::getName() const
		{
			return _name;
		}

		void ScenarioFolder::setName( const std::string& value )
		{
			_name = value;
		}

		void ScenarioFolder::setParent(ScenarioFolder* value)
		{
			_parent = value;
		}

		ScenarioFolder::ScenarioFolder(RegistryKeyType key)
		:	Registrable(key),
			_parent(NULL)
		{
	
		}



		std::string ScenarioFolder::getFullName() const
		{
			string result;
			for(const ScenarioFolder* folder(this); folder != NULL; folder = folder->getParent())
			{
				result = "/" + folder->getName() + result;
			}
			return result;
		}
	}
}
