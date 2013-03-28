
//////////////////////////////////////////////////////////////////////////////////////////
///	ActionService class implementation.
///	@file ActionService.cpp
///	@author hromain
///	@date 2013
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ActionService.hpp"

#include "ActionException.h"
#include "RequestException.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,server::ActionService>::FACTORY_KEY = "action";
	
	namespace server
	{
		const string ActionService::ATTR_ERROR_MESSAGE = "error_message";
		const string ActionService::ATTR_GENERATED_ID = "generated_id";



		ParametersMap ActionService::_getParametersMap() const
		{
			ParametersMap map;

			// The action key
			if(_action.get())
			{
				map.insert(Request::PARAMETER_ACTION, _action->getFactoryKey());
			}

			return map;
		}



		void ActionService::_setFromParametersMap(const ParametersMap& map)
		{
			// The action
			try
			{
				_action.reset(
					Factory<Action>::create(
						map.get<string>(Request::PARAMETER_ACTION)
				)	);
			}
			catch(FactoryException<Action>&)
			{
				throw RequestException("No such action");
			}

			// Action parameters
			try
			{
				_action->_setFromParametersMap(map);
			}
			catch(ActionException& e)
			{
				throw RequestException("Action load error : "+ e.getMessage());
			}
		}



		ParametersMap ActionService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			
			// Right check
			if(	!_action->isAuthorized(request.getSession().get())
			){
				map.insert(ATTR_ERROR_MESSAGE, "Forbidden Action");
			}

			try
			{
				// Run
				_action->run(const_cast<Request&>(request)); // cheating const_cast !!

				// Get the generated id
				if(request.getActionCreatedId())
				{
					map.insert(ATTR_GENERATED_ID, *request.getActionCreatedId());
				}
			}
			catch(ActionException& e)
			{
				map.insert(ATTR_ERROR_MESSAGE, e.getMessage());
				Log::GetInstance().debug("Action error : " + e.getMessage());
			}

			return map;
		}
		
		
		
		bool ActionService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ActionService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
