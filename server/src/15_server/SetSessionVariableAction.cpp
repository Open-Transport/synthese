
//////////////////////////////////////////////////////////////////////////
/// SetSessionVariableAction class implementation.
/// @file SetSessionVariableAction.cpp
/// @author Hugues Romain
/// @date 2011
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

#include "SetSessionVariableAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "Session.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, server::SetSessionVariableAction>::FACTORY_KEY("SetSessionVariable");
	}

	namespace server
	{
		const string SetSessionVariableAction::PARAMETER_VARIABLE = Action_PARAMETER_PREFIX + "variable";
		const string SetSessionVariableAction::PARAMETER_VALUE = Action_PARAMETER_PREFIX + "value";



		ParametersMap SetSessionVariableAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_VARIABLE, _variable);
			map.insert(PARAMETER_VALUE, _value);
			return map;
		}



		void SetSessionVariableAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Variable
			_variable = map.get<string>(PARAMETER_VARIABLE);
			if(_variable.empty())
			{
				throw ActionException("The variable must be non empty");
			}

			// Value
			_value = map.get<string>(PARAMETER_VALUE);
		}



		void SetSessionVariableAction::run(
			Request& request
		){
			if(!request.getSession().get())
			{
				throw ActionException("No active session right now");
			}

			request.getSession()->setSessionVariable(_variable, _value);
		}



		bool SetSessionVariableAction::isAuthorized(
			const Session* session
		) const {
			return session != NULL;
		}
	}
}
