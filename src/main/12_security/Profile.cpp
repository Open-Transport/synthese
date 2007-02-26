
/** Profile class implementation.
	@file Profile.cpp

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

#include <sstream>

#include "01_util/Exception.h"

#include "12_security/Profile.h"
#include "12_security/Right.h"

namespace synthese
{
	using namespace util;

	namespace security
	{


		Profile::Profile( uid id )
			: Registrable<uid, Profile>(id)
			, _parentId(0)
		{

		}

		Profile::~Profile()
		{
			cleanRights();
		}

		void Profile::setName( const std::string& name )
		{
			_name = name;
		}


		void Profile::setRights( const RightsVector& rightsvector )
		{
			cleanRights();
			_rights = rightsvector;
		}

		void Profile::cleanRights()
		{
			for (RightsVector::iterator it = _rights.begin(); it != _rights.end(); ++it)
			{
				delete it->second;
				_rights.erase(it);
			}
		}

		void Profile::setParent(uid id)
		{
			_parentId = id;
		}

		const std::string& Profile::getName() const
		{
			return _name;
		}

		const uid Profile::getParentId() const
		{
			return _parentId;
		}

		void Profile::removeRight( const std::string& key, const std::string& parameter)
		{
			RightsVector::iterator it = _rights.find(make_pair(key, parameter));
			if (it == _rights.end())
				throw Exception("Right not found");
			delete it->second;
			_rights.erase(it);
		}

		Right* Profile::getRight( const std::string& key, const std::string& parameter) const
		{
			RightsVector::const_iterator it = _rights.find(make_pair(key, parameter));
			if (it == _rights.end())
				throw Exception("Right not found");
			return it->second;
		}

		const Profile::RightsVector& Profile::getRights() const
		{
			return _rights;
		}

		void Profile::addRight( Right* right )
		{
			_rights.insert(make_pair(make_pair(right->getFactoryKey(), right->getParameter()), right));
		}

		void Profile::setPrivateRight( const Right::Level& level )
		{
			_privateGeneralLevel = level;
		}

		void Profile::setPublicRight( const Right::Level& level )
		{
			_publicGeneralLevel = level;
		}

		const Right::Level& Profile::getPrivateRight() const
		{
			return _privateGeneralLevel;
		}

		const Right::Level& Profile::getPublicRight() const
		{
			return _privateGeneralLevel;
		}
	}
}
