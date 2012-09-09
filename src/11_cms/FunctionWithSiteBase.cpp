
/** FunctionWithSiteBase class implementation.
	@file FunctionWithSiteBase.cpp

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

#include "FunctionWithSiteBase.hpp"

#include "Website.hpp"
#include "WebPageTableSync.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace cms
	{
		const string FunctionWithSiteBase::PARAMETER_SITE("si");



		FunctionWithSiteBase::FunctionWithSiteBase():
			_site(NULL)
		{}



		util::ParametersMap FunctionWithSiteBase::_getParametersMap() const
		{
			util::ParametersMap map;
			if(_site)
			{
				map.insert(PARAMETER_SITE, _site->getKey());
			}
			return map;
		}



		void FunctionWithSiteBase::_copy(const Function& function )
		{
			const FunctionWithSiteBase* fws(dynamic_cast<const FunctionWithSiteBase*>(&function));
			if(fws)
			{
				_site = fws->_site;
			}
		}



		const Webpage* FunctionWithSiteBase::getPage(
			const std::string& idOrSmartURL
		) const	{
			if(_site)
			{
				return _site->getPageByIdOrSmartURL(idOrSmartURL);
			}
			else
			{
				try
				{
					return WebPageTableSync::Get(lexical_cast<RegistryKeyType>(idOrSmartURL), *_env).get();
				}
				catch(...)
				{
				}
			}
			return NULL;
		}
}	}
