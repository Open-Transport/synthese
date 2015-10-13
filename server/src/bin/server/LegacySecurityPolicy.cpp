/** LegacySecurityPolicy class implementation.
	@file SecurityPolicy.cpp

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
#include "LegacySecurityPolicy.h"

#include "SQLService.hpp"

namespace synthese
{

namespace server
{

using namespace security;


static
bool authorizedIfGlobalDeleteRightIsGranted(const Session* session)
{
	return session && session->hasProfile() &&
			session->getUser()->getProfile()->isAuthorized<GlobalRight>(DELETE_RIGHT);
}

LegacySecurityPolicy::LegacySecurityPolicy()
{
	_isAuthorizedFunctions.insert(std::make_pair(db::SQLService::FACTORY_KEY, authorizedIfGlobalDeleteRightIsGranted));
}

bool
LegacySecurityPolicy::isAuthorized(const std::string& key, const Session* session) const
{
	std::map<std::string, IsAuthorizedFunction>::const_iterator it = _isAuthorizedFunctions.find(key);
	if (it == _isAuthorizedFunctions.end()) return false;
	IsAuthorizedFunction isAuthorizedFunction = it->second;
	return isAuthorizedFunction(session);
}


}
}
