
/** Profile class implementation.
	@file Profile.cpp

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

#include <sstream>

#include "Exception.h"
#include "Registry.h"

#include "ParametersMap.h"
#include "Profile.h"
#include "Right.h"
#include "SecurityRight.h"
#include "Session.h"
#include "User.h"

#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace security;
	using namespace util;

	CLASS_DEFINITION(Profile, "t027_profiles", 27)
	FIELD_DEFINITION_OF_OBJECT(Profile, "profile_id", "profile_ids")

	FIELD_DEFINITION_OF_TYPE(ParentProfile, "parent", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Rights, "rights", SQL_TEXT)

	namespace security
	{
		const string Profile::RIGHT_SEPARATOR = "|";
		const string Profile::RIGHT_VALUE_SEPARATOR = ",";


		Profile::Profile(
			RegistryKeyType id
		):	Registrable(id),
			Object<Profile, ProfileSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(ParentProfile),
					FIELD_DEFAULT_CONSTRUCTOR(Rights)
			)	)
		{
		}

		void Profile::setName( const std::string& name )
		{
			set<Name>(name);
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

		void Profile::setParent(const Profile* value)
		{
			set<ParentProfile>(value
				? boost::optional<Profile&>(*const_cast<Profile*>(value))
				: boost::none);
		}

		std::string Profile::getName() const
		{
			return get<Name>();
		}

		const Profile* Profile::getParent() const
		{
			if (get<ParentProfile>())
			{
				return get<ParentProfile>().get_ptr();
			}
			return NULL;
		}

		void Profile::removeRight( const std::string& key, const std::string& parameter)
		{
			RightsVector::iterator it = _rights.find(make_pair(key, parameter));
			if (it == _rights.end())
				throw Exception("Right not found");
			_rights.erase(it);

			set<Rights>(getRightsString());
		}

		boost::shared_ptr<Right> Profile::getRight( const std::string key, const std::string parameter)
		{
			RightsVector::const_iterator it = _rights.find(make_pair(key, parameter));
			return (it == _rights.end()) ? boost::shared_ptr<Right>() : it->second;
		}

		boost::shared_ptr<const Right> Profile::getRight( const std::string key, const std::string parameter) const
		{
			RightsVector::const_iterator it = _rights.find(make_pair(key, parameter));
			return (it == _rights.end()) ? boost::shared_ptr<Right>() : it->second;
		}

		const RightsVector& Profile::getRights() const
		{
			return _rights;
		}

		void Profile::addRight(boost::shared_ptr<Right> right )
		{
			_rights.insert(make_pair(make_pair(right->getFactoryKey(), right->getParameter()), right));
			set<Rights>(getRightsString());
		}

		bool Profile::isAuthorized(
			const Right& right
		) const	{
			// 0 Default values : forbidden
			bool privateAuthorization = false;
			bool publicAuthorization = false;

			// 1 Reading of the global right
			boost::shared_ptr<const Right> sright = getRight();
			if (sright != NULL)
			{
				privateAuthorization = (sright->getPrivateRightLevel() >= right.getPrivateRightLevel());
				publicAuthorization = (sright->getPublicRightLevel() >= right.getPublicRightLevel());
			}

			// 1 Attempting to find same right with global perimeter
			sright = getRight(right.getFactoryKey());
			if (sright != NULL)
			{
				privateAuthorization = (sright->getPrivateRightLevel() >= right.getPrivateRightLevel());
				publicAuthorization = (sright->getPublicRightLevel() >= right.getPublicRightLevel());
			}

			// 2 Attempting to find same right with compatible perimeter : the more favorable is selected
			RightsOfSameClassMap m = getRights(right.getFactoryKey());
			BOOST_FOREACH(const RightsOfSameClassMap::value_type& it, m)
			{
				if (right.getParameter().empty() || it.second->perimeterIncludes(right.getParameter()))
				{
					if (it.second->getPrivateRightLevel() >= right.getPrivateRightLevel())
						privateAuthorization = true;
					if (it.second->getPublicRightLevel() >= right.getPublicRightLevel())
						publicAuthorization = true;
				}
			}

			return privateAuthorization && publicAuthorization;
		}

		RightsOfSameClassMap Profile::getRights( const std::string& key ) const
		{
			RightsOfSameClassMap m;
			BOOST_FOREACH(RightsVector::value_type it, _rights)
			{
				if (it.first.first == key)
					m.insert(make_pair(it.first.second, it.second));
			}
			return m;
		}

		void Profile::link(util::Env& env, bool withAlgorithmOptimizations)
		{
			string rights(get<Rights>()); // string copy because Rights will be cleaned during setRightsFromString
			setRightsFromString(rights);
		}

		string Profile::getRightsString()
		{
			stringstream s;

			for (RightsVector::const_iterator it = getRights().begin(); it != getRights().end(); ++it)
			{
				boost::shared_ptr<const Right> right = it->second;
				if (it != getRights().begin())
					s	<< RIGHT_SEPARATOR;
				s	<< right->getFactoryKey()
					<< RIGHT_VALUE_SEPARATOR << right->getParameter()
					<< RIGHT_VALUE_SEPARATOR << (static_cast<int>(right->getPrivateRightLevel()))
					<< RIGHT_VALUE_SEPARATOR << (static_cast<int>(right->getPublicRightLevel()))
				;
			}
			return s.str();
		}

		void Profile::setRightsFromString(const string& text )
		{
			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep(RIGHT_SEPARATOR.c_str ());

			// CLeaning the profile
			cleanRights();

			// Parsing
			tokenizer parametersTokens (text, sep);
			for (tokenizer::iterator parameterToken = parametersTokens.begin();
				parameterToken != parametersTokens.end (); ++ parameterToken)
			{
				tokenizer valuesToken(*parameterToken, boost::char_separator<char>(RIGHT_VALUE_SEPARATOR.c_str()));
				tokenizer::iterator it = valuesToken.begin();

				try
				{
					boost::shared_ptr<Right> right(Factory<Right>::create(*it));

					++it;
					right->setParameter(*it);

					++it;
					right->setPrivateLevel(static_cast<RightLevel>(lexical_cast<int>(*it)));

					++it;
					right->setPublicLevel(static_cast<RightLevel>(lexical_cast<int>(*it)));

					addRight(right);
				}
				catch (FactoryException<Right> e)
				{
					continue;
				}
			}
		}

		bool Profile::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(security::WRITE);
		}

		bool Profile::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(security::WRITE);
		}

		bool Profile::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<SecurityRight>(security::DELETE_RIGHT);
		}

	}
}
