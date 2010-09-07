
/** GetValueFunction class implementation.
	@file GetValueFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "GetValueFunction.hpp"
#include "FunctionWithSite.h"
#include "CMSModule.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function, GetValueFunction>::FACTORY_KEY("@");
	
	namespace cms
	{
		const string GetValueFunction::PARAMETER_PARAMETER("p");
		
		ParametersMap GetValueFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_PARAMETER, _parameter);
			return map;
		}

		void GetValueFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_parameter = map.getDefault<string>(PARAMETER_PARAMETER);
		}

		void GetValueFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			if(_parameter == "client_url")
			{
				stream << request.getClientURL();
			}
			else if(_parameter == "host_name")
			{
				stream << request.getHostName();
			}
			else if(_parameter == "site")
			{
				shared_ptr<const Website> site(CMSModule::GetSite(request));
				if(site.get())
				{
					stream << site->getKey();
				}
			}
			else
			{
				string value(_aditionnalParameters.getDefault<string>(_parameter));
				if(value.empty())
				{
					value = request.getParametersMap().getDefault<string>(_parameter);
				}
				else
				{
					stream << value;
				}
			}
		}
		
		
		
		bool GetValueFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string GetValueFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
