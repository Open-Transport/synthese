
/** XMLSessionFunction class implementation.
	@file XMLSessionFunction.cpp
	@author Hugues
	@date 2009

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

#include "XMLSessionFunction.h"

#include "Request.h"
#include "RequestException.h"
#include "Session.h"
#include "User.h"
#include "Webpage.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,server::XMLSessionFunction>::FACTORY_KEY("XMLSessionFunction");

	namespace server
	{
		const string XMLSessionFunction::PARAMETER_CMS_TEMPLATE_ID("ti");

		const string XMLSessionFunction::ATTR_SESSION_ID("session_id");
		const string XMLSessionFunction::TAG_USER("user");

		ParametersMap XMLSessionFunction::_getParametersMap() const
		{
			ParametersMap map;

			if(_cmsTemplate.get())
			{
				map.insert(PARAMETER_CMS_TEMPLATE_ID, _cmsTemplate->getKey());
			}

			return map;
		}

		void XMLSessionFunction::_setFromParametersMap(const ParametersMap& map)
		{
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
		}



		ParametersMap XMLSessionFunction::run( std::ostream& stream, const Request& request ) const
		{
			ParametersMap pm;

			// CMS response
			if(_cmsTemplate.get())
			{
				if (request.getSession())
				{
					pm.insert(ATTR_SESSION_ID, request.getSession()->getKey());
					if(request.getSession()->getUser())
					{
						boost::shared_ptr<ParametersMap> userPM(new ParametersMap);
						request.getSession()->getUser()->toParametersMap(*userPM, true);
						pm.insert(TAG_USER, userPM);
					}
				}
				_cmsTemplate->display(stream, request, pm);
			}
			else // XML response
			{
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<login xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/xml_Session_function.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">" <<
					"<session>"
				;
				if (request.getSession())
					stream << request.getSession()->getKey();

				stream << "</session>";

				if (request.getSession())
				{
					stream <<
						"<name>" <<	request.getUser()->getFullName() << "</name>" <<
						"<phone>" << request.getUser()->getPhone() << "</phone>" <<
						"<email>" << request.getUser()->getEMail() << "</email>" <<
						"<user_id>" << request.getUser()->getKey() << "</user_id>"
					;
				}

				stream << "</login>";
			}

			return pm;
		}



		bool XMLSessionFunction::isAuthorized(const Session*) const
		{
			return true;
		}



		std::string XMLSessionFunction::getOutputMimeType() const
		{
			return _cmsTemplate.get() ? _cmsTemplate->getMimeType() : "text/xml";
		}
	}
}
