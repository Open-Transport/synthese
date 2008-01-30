
/** VinciSiteUpdateAction class implementation.
	@file VinciSiteUpdateAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "VinciSiteUpdateAction.h"

#include "71_vinci_bike_rental/VinciSite.h"
#include "71_vinci_bike_rental/VinciSiteTableSync.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/QueryString.h"


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciSiteUpdateAction>::FACTORY_KEY("vincisiteupdate");
	}

	namespace vinci
	{
		const string VinciSiteUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string VinciSiteUpdateAction::PARAMETER_ADDRESS = Action_PARAMETER_PREFIX + "ad";
		const string VinciSiteUpdateAction::PARAMETER_PHONE = Action_PARAMETER_PREFIX + "ph";
		const string VinciSiteUpdateAction::PARAMETER_OPENED = Action_PARAMETER_PREFIX + "op";
		const string VinciSiteUpdateAction::PARAMETER_PARENT_SITE_ID = Action_PARAMETER_PREFIX + "ps";
		
		
		VinciSiteUpdateAction::VinciSiteUpdateAction()
			: util::FactorableTemplate<Action, VinciSiteUpdateAction>()
		{
		}
		
		
		
		ParametersMap VinciSiteUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_ADDRESS, _address);
			map.insert(PARAMETER_PHONE, _phone);
			map.insert(PARAMETER_OPENED, _opened);
			if (_parentSite.get())
				map.insert(PARAMETER_PARENT_SITE_ID, _parentSite->getKey());
			return map;
		}
		
		
		
		void VinciSiteUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(QueryString::PARAMETER_OBJECT_ID, true, FACTORY_KEY));
			try
			{
				_site = VinciSiteTableSync::GetUpdateable(id);
			}
			catch(...)
			{
				throw ActionException("Site not found");
			}

			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
			vector<shared_ptr<VinciSite> > sites(VinciSiteTableSync::search(_name, _site->getKey(), 0, 1));
			if (!sites.empty())
				throw ActionException("Ce nom est déjà utilisé par un autre site");

			_address = map.getString(PARAMETER_ADDRESS, false, FACTORY_KEY);
			_phone = map.getString(PARAMETER_PHONE, false, FACTORY_KEY);
			_opened = map.getBool(PARAMETER_OPENED, true, true, FACTORY_KEY);

			id = map.getUid(PARAMETER_PARENT_SITE_ID, true, FACTORY_KEY);
			if (id != UNKNOWN_VALUE)
			{
				try
				{
					_parentSite = VinciSiteTableSync::Get(id);
				}
				catch (...)
				{
					throw ActionException("Parent site not found");
				}
			}
		}
		
		
		
		void VinciSiteUpdateAction::run()
		{
			_site->setName(_name);
			_site->setAddress(_address);
			_site->setPhone(_phone);
			_site->setLocked(!_opened);
			_site->setParentSiteId(_parentSite.get() ? _parentSite->getKey() : UNKNOWN_VALUE);
			VinciSiteTableSync::save(_site.get());
		}
	}
}
