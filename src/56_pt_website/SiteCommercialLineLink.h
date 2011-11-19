
/** SiteCommercialLineLink class header.
	@file SiteCommercialLineLink.h

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

#ifndef SYNTHESE_routeplanner_SiteCommercialLineLink_h__
#define SYNTHESE_routeplanner_SiteCommercialLineLink_h__

#include "Registrable.h"
#include "Registry.h"
#include "UtilConstants.h"

namespace synthese
{
	namespace pt_website
	{
		/** SiteCommercialLineLink class.
			@ingroup m56
		*/
		class SiteCommercialLineLink
		:	public virtual util::Registrable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<SiteCommercialLineLink>	Registry;

		protected:
			util::RegistryKeyType _siteId;
			util::RegistryKeyType _commercialLineId;

		public:
			SiteCommercialLineLink(util::RegistryKeyType id = 0);

			void setSiteId(util::RegistryKeyType id) { _siteId = id; }
			void setCommercialLineId(util::RegistryKeyType id){ _commercialLineId = id;}

			util::RegistryKeyType getSiteId() const { return _siteId; }
			util::RegistryKeyType getCommercialLineId() const { return _commercialLineId; }
		};
	}
}

#endif // SYNTHESE_routeplanner_SiteCommercialLineLink_h__
