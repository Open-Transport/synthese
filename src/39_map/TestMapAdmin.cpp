
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

#include "39_map/MapRequest.h"
#include "39_map/JpegRenderer.h"
#include "39_map/PostscriptRenderer.h"

#include "05_html/PropertiesHTMLTable.h"

#include "30_server/QueryString.h"
#include "30_server/FunctionRequest.h"
#include "30_server/Request.h"

#include "32_admin/ModuleAdmin.h"
#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace map;
	using namespace html;

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
			: AdminInterfaceElementTemplate<TestMapAdmin>()
			, _useEnvironment(true)
			, _error(false)
		{ }
		
		void TestMapAdmin::setFromParametersMap(const ParametersMap& map)
		{
			_dataXml = map.getString(PARAMETER_DATA_XML, false, FACTORY_KEY);
			_queryXml = map.getString(PARAMETER_QUERY_XML, false, FACTORY_KEY);
			_useEnvironment = map.getBool(PARAMETER_USE_ENVIRONMENT, false, true, FACTORY_KEY);

			if (!_queryXml.empty())
			{
				try
				{
					FunctionRequest<MapRequest> r;
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
		}
		
		void TestMapAdmin::display(ostream& stream, VariablesMap& variables, const FunctionRequest<AdminRequest>* request) const
		{
			// Requests
			FunctionRequest<AdminRequest> testMapRequest(request);
			testMapRequest.getFunction()->setPage<TestMapAdmin>();
			
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

		bool TestMapAdmin::isAuthorized(const FunctionRequest<AdminRequest>* request) const
		{
			return true;
		}
		
		AdminInterfaceElement::PageLinks TestMapAdmin::getSubPagesOfParent(
			const PageLink& parentLink
			, const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(parentLink.factoryKey == ModuleAdmin::FACTORY_KEY && parentLink.parameterValue == MapModule::FACTORY_KEY)
				links.push_back(getPageLink());
			return links;
		}
		
		AdminInterfaceElement::PageLinks TestMapAdmin::getSubPages(
			const AdminInterfaceElement& currentPage
			, const server::FunctionRequest<admin::AdminRequest>* request
		) const {
			AdminInterfaceElement::PageLinks links;
			return links;
		}
	}
}
