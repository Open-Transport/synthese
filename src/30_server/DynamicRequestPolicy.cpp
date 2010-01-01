
/** DynamicRequestPolicy class implementation.
	@file DynamicRequestPolicy.cpp

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

#include "DynamicRequestPolicy.h"

#include "Action.h"
#include "Function.h"

namespace synthese
{
	namespace server
	{
		void DynamicRequestPolicy::_loadAction(
		){
			// Action name
			std::string actionName(_parametersMap.getDefault<std::string>(Request::PARAMETER_ACTION));
			if (!actionName.empty())
			{
				if (!util::Factory<Action>::contains(actionName))
				{
					throw RequestException("Action not found");
				}
				_action = boost::shared_ptr<Action>(util::Factory<Action>::create(actionName));
				_action->_setFromParametersMap(_parametersMap);
			}
		}



		void DynamicRequestPolicy::_loadFunction(
 			bool actionException,
 			const std::string& errorMessage,
			boost::optional<util::RegistryKeyType> actionCreatedId
		){
			// Function name
			std::string functionName(_parametersMap.getDefault<std::string>(Request::PARAMETER_FUNCTION));
			if (functionName.empty())
			{
				throw RequestException("Function not specified");
			}
			if (!util::Factory<Function>::contains(functionName))
			{
				throw RequestException("Function not found");
			}
			_function = boost::shared_ptr<Function>(util::Factory<Function>::create(functionName));

			// Last action error
			if(actionException)
			{
				_parametersMap.insert(Request::PARAMETER_ACTION_FAILED, true);
				_parametersMap.insert(Request::PARAMETER_ERROR_MESSAGE, errorMessage);
			}
			if(actionCreatedId)
			{
				_parametersMap.insert(Request::PARAMETER_OBJECT_ID, *actionCreatedId);
			}
			_parametersMap.remove(Request::PARAMETER_ACTION_WILL_CREATE_OBJECT);

			// Function parameters
			_function->_setFromParametersMap(_parametersMap);
		}
	}
}
