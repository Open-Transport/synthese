
/** SettingsFunction class implementation.
	@file SettingsFunction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "SettingsFunction.h"

#include "Settings.h"

#include "RequestException.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,cms::SettingsFunction>::FACTORY_KEY("settings");

	namespace cms
	{
		const string SettingsFunction::PARAMETER_ACTION("action");
		const string SettingsFunction::PARAMETER_NAME("name");
		const string SettingsFunction::PARAMETER_MODULE("module");
		const string SettingsFunction::PARAMETER_VALUE("value");
		const string SettingsFunction::PARAMETER_NOTIFY("notify");


		ParametersMap SettingsFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_ACTION, _action);
			map.insert(PARAMETER_NAME, _name);
			map.insert(PARAMETER_MODULE, _module);
			map.insert(PARAMETER_VALUE, _value);
			map.insert(PARAMETER_NOTIFY, _value);
			return map;
		}


		void SettingsFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_action	= _getOrDie(map, PARAMETER_ACTION,	"No action given (should be 'action=get' or 'action=set')");
			_name	= _getOrDie(map, PARAMETER_NAME,	"No setting name given, please specify name=xxx");
			_module	= _getOrDie(map, PARAMETER_MODULE,	"No setting module given, please specify module=xxx");
			_value	= _getOrDie(map, PARAMETER_VALUE,	"No value or default value given, please provide a value= parameter. In case of a 'get' action, this setting will be used as the default value to return if the setting doesn't exists.");

			// Notify, by default at true
			_notify = true;
			if (map.isDefined(PARAMETER_NOTIFY))
			{
				std::string notify = map.get<std::string>(PARAMETER_NOTIFY);
				if ( notify != "false" && notify != "true" )
				{
					Log::GetInstance().warn("Invalid value for the notify parameter : " + notify + ". Should be 'true' or 'false'. Using true by default.");
				}
				else
				{
					_notify = ( notify == "true" );
				}
			}
		}


		util::ParametersMap SettingsFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			if (_action == "get" )
			{
				stream << settings::Settings::GetInstance().Get<std::string>(_module,_name,_value);
			}
			else if (_action == "set" )
			{
				settings::Settings::GetInstance().Set(_module,_name,_value,_notify);
			}
			else
			{
				throw RequestException("Invalid action given, should be 'set' or 'get'");
			}

			return util::ParametersMap();
		}


		bool SettingsFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}


		std::string SettingsFunction::getOutputMimeType() const
		{
			return "text/plain";
		}


		SettingsFunction::SettingsFunction()
		{

		}

		//------------------------------ PRIVATE ------------------------------

		// The name says it all. Ha.
		std::string SettingsFunction::_getOrDie (
			const util::ParametersMap& map,
			const std::string& parameterName,
			const std::string& errorMsg
		)
		{
			if (map.isDefined(parameterName))
			{
				return map.get<std::string>(parameterName);
			}
			else
			{
				throw RequestException(errorMsg);
			}

			return ""; // Can't see how it might happend but feels strange not to put it
		}
}	}
