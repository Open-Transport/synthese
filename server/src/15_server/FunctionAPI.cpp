
/** FunctionAPI class implementation.
	@file FunctionAPI.cpp

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


#include "FunctionAPI.h"

using namespace std;

namespace synthese
{

	namespace server
	{

		// APIParam

		FunctionAPIParam::FunctionAPIParam(std::string key, std::string description, bool mandatory):
			_key(key),
			_description(description),
			_mandatory(mandatory)
		{}

		std::string FunctionAPIParam::getKey() const
		{
			return _key;
		}

		std::string FunctionAPIParam::getDescription() const
		{
			return _description;
		}

		bool FunctionAPIParam::isMandatory() const
		{
			return _mandatory;
		}

		// API description

		FunctionAPI::FunctionAPI():
		_deprecated(false)
		{}

		FunctionAPI::FunctionAPI(string group, std::string title, std::string description):
			_group(group),
			_title(title),
			_description(description),
			_deprecated(false),
			_currentParamGroupName("")
		{}

		string FunctionAPI::getGroup() const
		{
			return _group;
		}

		string FunctionAPI::getTitle() const
		{
			return _title;
		}

		string FunctionAPI::getDescription() const
		{
			return _description;
		}

		bool FunctionAPI::isDeprecated() const
		{
			return _deprecated;
		}

		void FunctionAPI::setDeprecated(bool deprecated)
		{
			_deprecated = deprecated;
		}

		void FunctionAPI::openParamGroup(std::string paramGroupName)
		{
			_currentParamGroupName = paramGroupName;
		}

		void FunctionAPI::addParams(std::string key, std::string description, bool mandatory)
		{
			if(_params.find(_currentParamGroupName) == _params.end())
			{
				_params[_currentParamGroupName] = vector<FunctionAPIParam>();
			}
			_params[_currentParamGroupName].push_back(FunctionAPIParam(key, description, mandatory));
		}

		const FunctionAPI::APIParamsMap &FunctionAPI::getParams() const
		{
			return _params;
		}

	}
}
