
/** XMLSessionFunction class implementation.
	@file XMLSessionFunction.cpp
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

#include "RequestException.h"
#include "Request.h"
#include "XMLSessionFunction.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,server::XMLSessionFunction>::FACTORY_KEY("XMLSessionFunction");
	
	namespace server
	{
		ParametersMap XMLSessionFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void XMLSessionFunction::_setFromParametersMap(const ParametersMap& map)
		{
		}

		void XMLSessionFunction::_run( std::ostream& stream ) const
		{
			stream <<
				"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
				"<login xsi:noNamespaceSchemaLocation=\"http://rcsmobility.com/xsd/xml_Session_function.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">" <<
				"<session>"
			;
			if (_request->getSession())
				stream << _request->getSession()->getKey();

			stream << "</session>";

			if (_request->getSession())
			{
				stream <<
					"<name>" <<	_request->getUser()->getFullName() << "</name>" <<
					"<phone>" << _request->getUser()->getPhone() << "</phone>" <<
					"<email>" << _request->getUser()->getEMail() << "</email>" <<
					"<user_id>" << _request->getUser()->getKey() << "</user_id>"
				;
			}

			stream << "</login>";
		}
		
		
		
		bool XMLSessionFunction::_isAuthorized() const
		{
			return true;
		}



		std::string XMLSessionFunction::getOutputMimeType() const
		{
			return "text/xml";
		}
	}
}
