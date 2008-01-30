
/** VinciSiteAddAction class implementation.
	@file VinciSiteAddAction.cpp
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

#include "71_vinci_bike_rental/VinciSiteTableSync.h"
#include "71_vinci_bike_rental/VinciSite.h"

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"
#include "30_server/Request.h"
#include "30_server/QueryString.h"

#include "VinciSiteAddAction.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciSiteAddAction>::FACTORY_KEY("vincisiteadd");
	}

	namespace vinci
	{
		const string VinciSiteAddAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		
		
		
		VinciSiteAddAction::VinciSiteAddAction()
			: util::FactorableTemplate<Action, VinciSiteAddAction>()
		{
		}
		
		
		
		ParametersMap VinciSiteAddAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NAME, _name);
			return map;
		}
		
		
		
		void VinciSiteAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
			if (_name.empty())
				throw ActionException("Le nom ne peut être vide.");	
			
			// Unicity
			vector<shared_ptr<VinciSite> > sites = VinciSiteTableSync::search(_name, 0, 1);
			if (!sites.empty())
				throw ActionException("Le site "+ _name +" existe déjà.");

			_request->setObjectId(QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION);
		}
		
		
		
		void VinciSiteAddAction::run()
		{
			VinciSite s;
			s.setName(_name);
			VinciSiteTableSync::save(&s);
			
			_request->setObjectId(s.getKey());
		}
	}
}
