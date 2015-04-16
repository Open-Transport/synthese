
//////////////////////////////////////////////////////////////////////////
/// GlobalVariableUpdateAction class implementation.
/// @file GlobalVariableUpdateAction.cpp
/// @author Hugues Romain
/// @date 2012
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

#include "GlobalVariableUpdateAction.hpp"

#include "ActionException.h"
#include "Conversion.h"
#include "DbModuleConfigTableSync.h"
#include "DBTransaction.hpp"
#include "GlobalRight.h"
#include "ModuleClass.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, server::GlobalVariableUpdateAction>::FACTORY_KEY = "global_variable_update";

	namespace server
	{
		const string GlobalVariableUpdateAction::PARAMETER_VARIABLE = Action_PARAMETER_PREFIX + "_variable";
		const string GlobalVariableUpdateAction::PARAMETER_VALUE = Action_PARAMETER_PREFIX + "_value";
		const string GlobalVariableUpdateAction::PARAMETER_PERSISTENT = Action_PARAMETER_PREFIX + "_persistent";



		GlobalVariableUpdateAction::GlobalVariableUpdateAction(
		):	FactorableTemplate<server::Action, GlobalVariableUpdateAction>(),
			_persistent(false)
		{}

		server::FunctionAPI GlobalVariableUpdateAction::getAPI() const
		{
			FunctionAPI api(
				"15_server",
				"Set SYNTHESE global parameters",
				"Deprecated, use the service with the same name instead."
			);
			api.openParamGroup("Variable set");
			api.addParams(GlobalVariableUpdateAction::PARAMETER_VARIABLE,
						  "The name of the variable", true);
			api.addParams(GlobalVariableUpdateAction::PARAMETER_VALUE,
						  "The value to set to this variable", true);
			api.addParams(GlobalVariableUpdateAction::PARAMETER_PERSISTENT,
						  "Set to true to make this value persistant", false);
			return api;
		}

		
		ParametersMap GlobalVariableUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void GlobalVariableUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			_variable = map.get<string>(PARAMETER_VARIABLE);
			_value = map.getDefault<string>(PARAMETER_VALUE);
			_persistent = map.getDefault<bool>(PARAMETER_PERSISTENT, false);
		}
		
		
		
		void GlobalVariableUpdateAction::run(
			Request& request
		){
			if(_persistent)
			{ // Persistent mode
				DBTransaction transaction;

				stringstream query1;
				query1 <<
					"DELETE FROM " << DbModuleConfigTableSync::TABLE.NAME << " WHERE " <<
					DbModuleConfigTableSync::COL_PARAMNAME << "=" << Conversion::ToDBString(_variable);
				transaction.addQuery(query1.str());

				stringstream query2;
				query2 <<
					"INSERT INTO " << DbModuleConfigTableSync::TABLE.NAME << " VALUES(" <<
					Conversion::ToDBString(_variable) << "," << Conversion::ToDBString(_value) <<
					")";
				transaction.addQuery(query2.str());

				transaction.run();
			}
			{ // Necessary in the two modes because the database triggers are not available in t999 table (to be fixed)
				ModuleClass::SetParameter(_variable, _value);
			}
		}
		
		
		
		bool GlobalVariableUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<GlobalRight>(DELETE_RIGHT);
		}
}	}

