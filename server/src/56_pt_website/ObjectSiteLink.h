
/** ObjectSiteLink class header.
	@file ObjectSiteLink.h

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

#ifndef SYNTHESE_transportwebsite_ObjectSiteLink_h__
#define SYNTHESE_transportwebsite_ObjectSiteLink_h__

#include "Object.hpp"

#include "PTServiceConfig.hpp"

namespace synthese
{
	namespace pt_website
	{
		FIELD_INT(ObjectId)
		FIELD_POINTER(Site, pt_website::PTServiceConfig)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(ObjectId),
			FIELD(Site)
		> ObjectSiteLinkSchema;

		/** ObjectSiteLink class.
			@ingroup m56
		*/
		class ObjectSiteLink
		:	public virtual Object<ObjectSiteLink, ObjectSiteLinkSchema>
		{
		public:
			ObjectSiteLink(util::RegistryKeyType id = 0);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_transportwebsite_ObjectSiteLink_h__
