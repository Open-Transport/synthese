
/** WebsiteConfig class header.
	@file WebsiteConfig.hpp

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

#ifndef SYNTHESE_cms_WebsiteConfig_h__
#define SYNTHESE_cms_WebsiteConfig_h__

#include "Object.hpp"

#include "CMSScriptField.hpp"
#include "NumericField.hpp"
#include "Registry.h"
#include "Webpage.h"
#include "Website.hpp"

namespace synthese
{
	FIELD_CMS_SCRIPT(ConfigScript)

	namespace cms
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Website),
			FIELD(ConfigScript)
		> WebsiteConfigRecord;


		/** Web site config class (CMS).
			@ingroup m36
		*/
		class WebsiteConfig:
			public Object<WebsiteConfig, WebsiteConfigRecord>
		{
		private:

		public:
			/// Chosen registry class.
			typedef util::Registry<WebsiteConfig>	Registry;
			
			WebsiteConfig(util::RegistryKeyType id = 0);

			//! @name Services
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_cms_WebSiteConfig
