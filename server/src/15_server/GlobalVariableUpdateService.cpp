
//////////////////////////////////////////////////////////////////////////
/// GlobalVariableUpdateService class implementation.
/// @file GlobalVariableUpdateService.cpp
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

#include "GlobalVariableUpdateService.hpp"

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
	const string FactorableTemplate<Function, server::GlobalVariableUpdateService>::FACTORY_KEY = "global_variable_update";

	namespace server
	{
		const string GlobalVariableUpdateService::PARAMETER_VARIABLE("variable");
		const string GlobalVariableUpdateService::PARAMETER_VALUE("value");
		const string GlobalVariableUpdateService::PARAMETER_PERSISTENT("persistent");
		const string GlobalVariableUpdateService::TAG_VARIABLES = "variables";
		const string GlobalVariableUpdateService::TAG_KEY = "key";
		const string GlobalVariableUpdateService::TAG_VALUE = "value";


		server::FunctionAPI GlobalVariableUpdateService::getAPI() const
		{
			FunctionAPI api(
				"15_server",
				"Query or set SYNTHESE global parameters",
				"If a variable and a value are provided in the request, then "
				"this variable is set to this value. Instead the list of \n"
				"all the SYNTHESE variables are returned as a parameters map.\n"
				"This example returns the list of all the variables:\n"
				"  <?global_variable_update&\n"
				"    template=<{variables&template=<@key@>=<@value@><br>}>\n"
				"  ?>"
			);
			api.openParamGroup("Variable set");
			api.addParams(GlobalVariableUpdateService::PARAMETER_VARIABLE,
						  "The name of the variable", true);
			api.addParams(GlobalVariableUpdateService::PARAMETER_VALUE,
						  "The value to set to this variable", true);
			api.addParams(GlobalVariableUpdateService::PARAMETER_PERSISTENT,
						  "Set to true to make this value persistant", false);
			api.openParamGroup("Get the variable list");
			api.addParams("", "No parameters required in this mode", false);
			return api;
		}

		GlobalVariableUpdateService::GlobalVariableUpdateService(
		):	FactorableTemplate<server::Function, GlobalVariableUpdateService>(),
			_persistent(false)
		{}

		
		
		ParametersMap GlobalVariableUpdateService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void GlobalVariableUpdateService::_setFromParametersMap(const ParametersMap& map)
		{
			_variable = map.getDefault<string>(PARAMETER_VARIABLE);
			_value = map.getDefault<string>(PARAMETER_VALUE);
			_persistent = map.getDefault<bool>(PARAMETER_PERSISTENT, false);
		}
		
		
		
		ParametersMap GlobalVariableUpdateService::run(
			ostream &stream,
			const Request &request
		) const
		{
			ParametersMap map;

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
			if(!_variable.empty())
			{
				// Necessary in the two modes because the database triggers are not available in t999 table (to be fixed)
				ModuleClass::SetParameter(_variable, _value);
			}
			else
			{
				// No variable given, return the list of all the variables
				BOOST_FOREACH(const ModuleClass::Parameters::value_type& item, ModuleClass::GetParameters())
				{
					// Insert a submap for each screen
					boost::shared_ptr<ParametersMap> variableMap(new ParametersMap);
					variableMap->insert(TAG_KEY, item.first);
					variableMap->insert(TAG_VALUE, item.second);
					map.insert(TAG_VARIABLES, variableMap);
				}
			}
			return map;
		}
		
		
		
		bool GlobalVariableUpdateService::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<GlobalRight>(DELETE_RIGHT);
		}

		std::string GlobalVariableUpdateService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}

