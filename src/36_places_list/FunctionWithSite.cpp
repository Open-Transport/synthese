
/** FunctionWithSite class implementation.
	@file FunctionWithSite.cpp

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

#include "FunctionWithSite.h"
#include "Site.h"
#include "RequestException.h"
#include "Conversion.h"
#include "Env.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace transportwebsite
	{
		const string FunctionWithSite::PARAMETER_SITE("si");
		
		ParametersMap FunctionWithSite::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_SITE, _site->getKey());
			return map;
		}

		void FunctionWithSite::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_site = Env::GetOfficialEnv()->getRegistry<Site>().get(map.getUid(PARAMETER_SITE, true, "fws"));
			}
			catch (ObjectNotFoundException<Site>& e)
			{
				throw RequestException("Specified site not found");
			}
		}

		void FunctionWithSite::_copy( boost::shared_ptr<const Function> function )
		{
			shared_ptr<const FunctionWithSite> rwi = static_pointer_cast<const FunctionWithSite, const Function>(function);
			_site = rwi->_site;
		}

		boost::shared_ptr<const Site> FunctionWithSite::getSite() const
		{
			return _site;
		}

		void FunctionWithSite::setSite( boost::shared_ptr<const Site> site)
		{
			_site = site;
		}

		synthese::server::ParametersMap FunctionWithSite::getFixedParametersMap() const
		{
			return _getParametersMap();
		}
	}
}
