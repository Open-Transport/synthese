
/** RemoveCustomerAction class implementation.
	@file RemoveCustomerAction.cpp
	@author Hugues Romain
	@date 2007

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

#include "RemoveCustomerAction.h"
#include "VinciContractTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, vinci::RemoveCustomerAction>::FACTORY_KEY("remove_customer");
	}

	namespace vinci
	{
		const string RemoveCustomerAction::PARAMETER_CONTRACT_ID = Action_PARAMETER_PREFIX + "xxx";
		
		
		
		RemoveCustomerAction::RemoveCustomerAction()
			: util::FactorableTemplate<Action, RemoveCustomerAction>()
		{
		}
		
		
		
		ParametersMap RemoveCustomerAction::getParametersMap() const
		{
			ParametersMap map;
			if (_contract.get())
				map.insert(PARAMETER_CONTRACT_ID, _contract->getKey());
			return map;
		}
		
		
		
		void RemoveCustomerAction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id = map.getUid(PARAMETER_CONTRACT_ID, true, FACTORY_KEY);
			try
			{
				_contract = VinciContractTableSync::Get(id);
			}
			catch (VinciContract::ObjectNotFoundException e)
			{
				throw ActionException("Contract not found");
			}
		}
		
		
		
		void RemoveCustomerAction::run()
		{
			VinciContractTableSync::Remove(_contract->getKey());
		}

		void RemoveCustomerAction::setContract( boost::shared_ptr<const VinciContract> contract )
		{
			_contract = contract;
		}
	}
}
