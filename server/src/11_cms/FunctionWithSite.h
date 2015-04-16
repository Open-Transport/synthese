
/** FunctionWithSite class header.
	@file FunctionWithSite.h
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_FunctionWithSite_H__
#define SYNTHESE_FunctionWithSite_H__

#include "FunctionWithSiteBase.hpp"

#include "Env.h"
#include "RequestException.h"
#include "WebsiteTableSync.hpp"

namespace synthese
{
	namespace cms
	{
		/** Function With website class.
			@author Hugues Romain
			@date 2007
			@ingroup m36
		*/
		template<bool compulsory>
		class FunctionWithSite:
			public FunctionWithSiteBase
		{
		public:

			typedef FunctionWithSite<compulsory> _FunctionWithSite;

		protected:
			/** Conversion from generic parameters map to attributes.
				@param map Parameters map to interpret
			*/
			void _setFromParametersMap(const util::ParametersMap& map)
			{
				util::RegistryKeyType id(
					compulsory ?
					map.get<util::RegistryKeyType>(PARAMETER_SITE) :
					map.getDefault<util::RegistryKeyType>(PARAMETER_SITE, 0)
				);
				if(id <= 0 && compulsory)
				{
					throw server::RequestException("Website id must be specified");
				}
				if(id > 0)
				{
					try
					{
						_site = WebsiteTableSync::Get(id, *_env).get();
					}
					catch (util::ObjectNotFoundException<Website>&)
					{
						throw server::RequestException("Specified website not found");
					}
				}
			}
		};
}	}

#endif // SYNTHESE_FunctionWithSite_H__
