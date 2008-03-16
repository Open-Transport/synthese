
/** VinciStockAlertRemoveAction class implementation.
	@file VinciStockAlertRemoveAction.cpp
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

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "VinciStockAlertRemoveAction.h"
#include "71_vinci_bike_rental/VinciStockAlert.h"
#include "71_vinci_bike_rental/VinciStockAlertTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::VinciStockAlertRemoveAction>::FACTORY_KEY("VinciStockAlertRemoveAction");
	}

	namespace vinci
	{
		const string VinciStockAlertRemoveAction::PARAMETER_ALERT = Action_PARAMETER_PREFIX + "al";
		
		
		
		VinciStockAlertRemoveAction::VinciStockAlertRemoveAction()
			: util::FactorableTemplate<Action, VinciStockAlertRemoveAction>()
		{
		}
		
		
		
		ParametersMap VinciStockAlertRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if (_alert.get())
				map.insert(PARAMETER_ALERT, _alert->getKey());
			return map;
		}
		
		
		
		void VinciStockAlertRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id = map.getUid(PARAMETER_ALERT, true, FACTORY_KEY);
			try
			{
				_alert = VinciStockAlertTableSync::Get(id);
			}
			catch (...)
			{
				throw ActionException("No such alert");
			}
		}
		
		
		
		void VinciStockAlertRemoveAction::run()
		{
			VinciStockAlertTableSync::Remove(_alert->getKey());
		}

		void VinciStockAlertRemoveAction::setAlert( boost::shared_ptr<const VinciStockAlert> alert )
		{
			_alert = alert;
		}
	}
}
