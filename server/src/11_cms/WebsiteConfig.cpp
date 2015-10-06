
/** WebsiteConfig class implementation.
	@file WebsiteConfig.cpp

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

#include "WebsiteConfig.hpp"

#include "CMSRight.hpp"
#include "Profile.h"
#include "Session.h"
#include "User.h"

namespace synthese
{
	CLASS_DEFINITION(cms::WebsiteConfig, "t103_website_configs", 103)

	FIELD_DEFINITION_OF_TYPE(ConfigScript, "content1", SQL_TEXT)

	using namespace util;
		
	namespace cms
	{
		WebsiteConfig::WebsiteConfig(RegistryKeyType id):
			Registrable(id),
			Object<WebsiteConfig, WebsiteConfigRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Website),
					FIELD_DEFAULT_CONSTRUCTOR(ConfigScript)
			)	)
		{
		}



		void WebsiteConfig::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<Website>())
			{
				get<Website>()->setConfig(this);
			}
		}



		void WebsiteConfig::unlink()
		{
			if(get<Website>())
			{
				get<Website>()->setConfig(NULL);
			}
		}


		bool WebsiteConfig::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CMSRight>(security::WRITE);
		}

		bool WebsiteConfig::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CMSRight>(security::WRITE);
		}

		bool WebsiteConfig::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CMSRight>(security::DELETE_RIGHT);
		}
}	}

