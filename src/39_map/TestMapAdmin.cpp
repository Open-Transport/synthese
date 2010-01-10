
/** TestMapAdmin class implementation.
	@file TestMapAdmin.cpp
	@author Hugues Romain
	@date 2008

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

#include "TestMapAdmin.h"
#include "MapModule.h"
#include "MapRequest.h"
#include "JpegRenderer.h"
#include "PostscriptRenderer.h"
#include "PropertiesHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "GlobalRight.h"
#include "Profile.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace map;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, TestMapAdmin>::FACTORY_KEY("TestMapAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<TestMapAdmin>::ICON("map.png");
		template<> const string AdminInterfaceElementTemplate<TestMapAdmin>::DEFAULT_TITLE("Test de carte");
	}

	namespace map
	{
		const string TestMapAdmin::PARAMETER_DATA_XML("dx");
		const string TestMapAdmin::PARAMETER_QUERY_XML("qx");
		const string TestMapAdmin::PARAMETER_USE_ENVIRONMENT("ue");

		TestMapAdmin::TestMapAdmin()
			: AdminInterfaceElementTemplate<TestMapAdmin>(),
			_useEnvironment(true)
		{ }
		
		void TestMapAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_dataXml = map.getString(PARAMETER_DATA_XML, false, FACTORY_KEY);
			_queryXml = map.getString(PARAMETER_QUERY_XML, false, FACTORY_KEY);
			_useEnvironment = map.getBool(PARAMETER_USE_ENVIRONMENT, false, true, FACTORY_KEY);
		}
		
		
		
		server::ParametersMap TestMapAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAMETER_DATA_XML, _dataXml);
			m.insert(PARAMETER_QUERY_XML, _queryXml);
			m.insert(PARAMETER_USE_ENVIRONMENT, _useEnvironment);
			return m;
		}


		
		void TestMapAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const AdminRequest& _request
		) const	{
			// Requests
			AdminFunctionRequest<TestMapAdmin> testMapRequest(_request);
			
			
			std::string _tempFileUrl;
			std::string	_postScript;
			bool		_error(false);
			if (!_queryXml.empty())
			{
				try
				{
					StaticFunctionRequest<MapRequest> r(_request, true);
					r.getFunction()->setUseEnvironment(_useEnvironment);
					r.getFunction()->setData(_dataXml);
					r.getFunction()->setQuery(_queryXml);
					r.getFunction()->setOutput(PostscriptRenderer::FACTORY_KEY);
					stringstream s;
					r.run(s);
					_postScript = s.str();

					r.getFunction()->setOutput(JpegRenderer::FACTORY_KEY);
					stringstream t;
					r.run(t);
					_tempFileUrl = t.str();
				}
				catch(...)
				{
					_error = true;
				}
			}

			
			// Form
			PropertiesHTMLTable  st(testMapRequest.getHTMLForm("test"));
			stream << "<h1>Donn�es XML � tester</h1>";
			stream << st.open();
			stream << st.cell("Tracer environnement partag�", st.getForm().getOuiNonRadioInput(PARAMETER_USE_ENVIRONMENT, _useEnvironment));
			stream << st.cell("Donn�es temporaires (XML)", st.getForm().getTextAreaInput(PARAMETER_DATA_XML, _dataXml, 5, 50));
			stream << st.cell("Requ�te (XML)", st.getForm().getTextAreaInput(PARAMETER_QUERY_XML, _queryXml, 5, 50));
			stream << st.close();

			// Display
			stream << "<h1>Carte r�sultat</h1>";
			if (_error)
				stream << "Erreur postscript";
			else if (_tempFileUrl.empty())
				stream << "Carte vide";
			else
				stream << HTMLModule::getHTMLImage(_tempFileUrl, "Carte r�sultat");

			stream << "<h1>Postscript r�sultat</h1>";
			stream << _postScript;
		}

		bool TestMapAdmin::isAuthorized(
			const security::User& user
		) const {
			return user.getProfile()->isAuthorized<GlobalRight>(READ);;
		}
		


		AdminInterfaceElement::PageLinks TestMapAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(moduleKey == MapModule::FACTORY_KEY && request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewPage());
			}
			return links;
		}
	}
}
