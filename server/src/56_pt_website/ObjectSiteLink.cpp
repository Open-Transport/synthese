
/** ObjectSiteLink class implementation.
	@file ObjectSiteLink.cpp

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

#include "ObjectSiteLink.h"

#include "Profile.h"
#include "Session.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace pt_website;
	using namespace util;

	CLASS_DEFINITION(ObjectSiteLink, "t001_object_site_links", 1)
	FIELD_DEFINITION_OF_OBJECT(ObjectSiteLink, "object_site_link_id", "object_site_link_ids")

	FIELD_DEFINITION_OF_TYPE(ObjectId, "object_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Site, "site_id", SQL_INTEGER)

	namespace pt_website
	{
		ObjectSiteLink::ObjectSiteLink(
			RegistryKeyType id
		):	Registrable(id),
			Object<ObjectSiteLink, ObjectSiteLinkSchema> (
				Schema (
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(ObjectId),
					FIELD_DEFAULT_CONSTRUCTOR(Site)
		)	) {}


		bool ObjectSiteLink::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool ObjectSiteLink::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool ObjectSiteLink::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
	}
}
