
/** VariableExpression class implementation.
	@file VariableExpression.cpp

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

#include "VariableExpression.hpp"

#include "ParametersMap.h"
#include "Request.h"
#include "Webpage.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace cms
	{
		VariableExpression::VariableExpression(
			const std::string& variable
		):	_variable(variable)
		{}



		std::string VariableExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			if(_variable == "client_url")
			{
				return request.getClientURL();
			}
			else if(_variable == "host_name")
			{
				return request.getHostName();
			}
			if(page.getRoot() && _variable == "site")
			{
				return lexical_cast<string>(page.getRoot()->getKey());
			}

			string value(variables.getDefault<string>(_variable));
			if(value.empty())
			{
				value = additionalParametersMap.getDefault<string>(_variable);
			}
			if(value.empty())
			{
				value = request.getParametersMap().getDefault<string>(_variable);
			}
			return value;
		}
}	}
