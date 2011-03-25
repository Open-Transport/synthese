
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
#include "Webpage.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,XMLSessionIsValidFunction>::FACTORY_KEY("XMLSessionIsValidFunction");
	
	namespace server
	{
		const string XMLSessionIsValidFunction::PARAMETER_SESSION_ID_TO_CONTROL("si");
		const string XMLSessionIsValidFunction::PARAMETER_CMS_TEMPLATE_ID("ti");

		const string XMLSessionIsValidFunction::TAG_VALID_SESSION("validSession");
		const string XMLSessionIsValidFunction::ATTR_ID("id");
		const string XMLSessionIsValidFunction::ATTR_VALID("valid");
		
		ParametersMap XMLSessionIsValidFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(!_sessionIdToControl.empty())
			{
				map.insert(PARAMETER_SESSION_ID_TO_CONTROL, _sessionIdToControl);
			}
			if(_cmsTemplate.get())
			{
				map.insert(PARAMETER_CMS_TEMPLATE_ID, _cmsTemplate->getKey());
			}
			return map;
		}

		void XMLSessionIsValidFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Session number
			_sessionIdToControl = map.get<string>(PARAMETER_SESSION_ID_TO_CONTROL);

			// CMS template
			optional<RegistryKeyType> tid(map.getOptional<RegistryKeyType>(PARAMETER_CMS_TEMPLATE_ID));
			if(tid) try
			{
				_cmsTemplate = Env::GetOfficialEnv().get<Webpage>(*tid);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such main page");
			}
			_savedParameters.remove(PARAMETER_CMS_TEMPLATE_ID);
		}

		void XMLSessionIsValidFunction::run( std::ostream& stream, const Request& request ) const
		{
			// Service
			ServerModule::SessionMap::iterator sit = ServerModule::getSessions().find(_sessionIdToControl);
			bool value (sit != ServerModule::getSessions().end());
			if(value) try
			{
				sit->second->controlAndRefresh(request.getIP());
			}
			catch(SessionException)
			{
				value = false;
			}

			// CMS response
			if(_cmsTemplate.get())
			{
				ParametersMap pm;
				pm.insert(ATTR_ID, _sessionIdToControl);
				pm.insert(ATTR_VALID, value);
				_cmsTemplate->display(stream, request, pm);
			}
			else // XML response
			{
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<" << TAG_VALID_SESSION << " xsi:noNamespaceSchemaLocation=\"https://extranet-rcsmobility.com/projects/synthese/repository/raw/src/15_server/xml_session_is_valid_function.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" <<
					" " << ATTR_ID << "=\"" << _sessionIdToControl << "\" " << ATTR_VALID << "=\"" << (value ? "true" : "false") << "\" />"
				;
			}
		}
		
		
		
		bool XMLSessionIsValidFunction::isAuthorized(const Session*) const
		{
			return true;
		}



		std::string XMLSessionIsValidFunction::getOutputMimeType() const
		{
			return _cmsTemplate.get() ? _cmsTemplate->getMimeType() : "text/xml";
		}
}	}
