
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

using namespace boost;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> Registrable<uid,security::Profile>::Registry Registrable<uid,security::Profile>::_registry;
	}

	namespace security
	{


		Profile::Profile( uid id )
			: Registrable<uid, Profile>(id)
			, _parentId(0)
		{

		}

		void Profile::setName( const std::string& name )
		{
			_name = name;
		}


		void Profile::setRights( const RightsVector& rightsvector )
		{
			cleanRights();
			_rights = rightsvector; /// TODO copy of the rights
		}

		void Profile::cleanRights()
		{
			_rights.clear();
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
			_rights.erase(it);
		}

		shared_ptr<Right> Profile::getRight( const std::string key, const std::string parameter)
		{
			RightsVector::const_iterator it = _rights.find(make_pair(key, parameter));
			return (it == _rights.end()) ? shared_ptr<Right>() : it->second;
		}

		shared_ptr<const Right> Profile::getRight( const std::string key, const std::string parameter) const
		{
			RightsVector::const_iterator it = _rights.find(make_pair(key, parameter));
			return (it == _rights.end()) ? shared_ptr<Right>() : it->second;
		}

		const RightsVector& Profile::getRights() const
		{
			return _rights;
		}

		void Profile::addRight(shared_ptr<Right> right )
		{
			_rights.insert(make_pair(make_pair(right->getFactoryKey(), right->getParameter()), right));
		}

		bool Profile::isAuthorized(shared_ptr<const Right> right ) const
		{
			// 0 Default values : forbidden
			bool privateAuthorization = false;
			bool publicAuthorization = false;

			// 1 Reading of the global right
			shared_ptr<const Right> sright = getRight();
			if (sright != NULL)
			{
				privateAuthorization = (sright->getPrivateRightLevel() >= right->getPrivateRightLevel());
				publicAuthorization = (sright->getPublicRightLevel() >= right->getPublicRightLevel());
			}

			// 1 Attempting to find same right with global perimeter
			sright = getRight(right->getFactoryKey());
			if (sright != NULL)
			{
				privateAuthorization = (sright->getPrivateRightLevel() >= right->getPrivateRightLevel());
				publicAuthorization = (sright->getPublicRightLevel() >= right->getPublicRightLevel());
			}
			
			// 2 Attempting to find same right with compatible perimeter : the more favorable is selected
			RightsOfSameClassMap m = getRights(right->getFactoryKey());
			for (RightsOfSameClassMap::const_iterator it = m.begin(); it != m.end(); ++it)
			{
				if (it->second->perimeterIncludes(right->getParameter()))
				{
					if (it->second->getPrivateRightLevel() >= right->getPrivateRightLevel())
						privateAuthorization = true;
					if (it->second->getPublicRightLevel() >= right->getPublicRightLevel())
						publicAuthorization = true;
				}
			}

			return privateAuthorization && publicAuthorization;
		}

		RightsOfSameClassMap Profile::getRights( const std::string& key ) const
		{
			RightsOfSameClassMap m;
			for (RightsVector::const_iterator it = _rights.begin(); it != _rights.end(); ++it)
				if (it->first.first == key)
					m.insert(make_pair(it->first.second, it->second));
			return m;			
		}

	}
}
