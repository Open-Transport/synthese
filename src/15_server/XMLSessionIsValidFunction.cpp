
/** XMLSessionIsValidFunction class implementation.
	@file XMLSessionIsValidFunction.cpp
	@author Hugues
	@date 2009

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

#include "ServerModule.h"
#include "Request.h"
#include "RequestException.h"
#include "XMLSessionIsValidFunction.h"
#include "SessionException.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,XMLSessionIsValidFunction>::FACTORY_KEY("XMLSessionIsValidFunction");
	
	namespace server
	{
		const string XMLSessionIsValidFunction::PARAMETER_SESSION_ID_TO_CONTROL("si");
		
		ParametersMap XMLSessionIsValidFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(!_sessionIdToControl.empty())
			{
				map.insert(PARAMETER_SESSION_ID_TO_CONTROL, _sessionIdToControl);
			}
			return map;
		}

		void XMLSessionIsValidFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_sessionIdToControl = map.get<string>(PARAMETER_SESSION_ID_TO_CONTROL);
		}

		void XMLSessionIsValidFunction::run( std::ostream& stream, const Request& request ) const
		{
			ServerModule::SessionMap::iterator sit = ServerModule::getSessions().find(_sessionIdToControl);
			stream <<
				"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
				"<validSession xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/xml_session_is_valid_function.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" <<
				" id=\"" << _sessionIdToControl << "\" valid=\"";
			if(sit != ServerModule::getSessions().end())
			{
				try
				{
					sit->second->controlAndRefresh(request.getIP());
					stream << "true";
				}
				catch(SessionException)
				{
					stream << "false";
				}
			}
			else
			{
				stream << "false";
			}
			stream << "\" />";
		}
		
		
		
		bool XMLSessionIsValidFunction::isAuthorized(const Session*) const
		{
			return true;
		}



		std::string XMLSessionIsValidFunction::getOutputMimeType() const
		{
			return "text/xml";
		}
	}
}
