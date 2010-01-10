
//////////////////////////////////////////////////////////////////////////
/// PTImportAdmin class implementation.
///	@file PTImportAdmin.cpp
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PTImportAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "ImportFunction.h"
#include "PropertiesHTMLTable.h"
#include "DataSource.h"
#include "DataSourceTableSync.h"
#include "NavteqWithProjectionFileFormat.h"
#include "TridentFileFormat.h"
#include "TransportNetworkRight.h"
#include "Profile.h"
#include "AdminFunction.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace impex;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTImportAdmin>::FACTORY_KEY("PTImportAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTImportAdmin>::ICON("database_copy.png");
		template<> const string AdminInterfaceElementTemplate<PTImportAdmin>::DEFAULT_TITLE("Imports");
	}

	namespace pt
	{
		const string PTImportAdmin::TAB_TRIDENT("tr");
		const string PTImportAdmin::TAB_NAVTEQ("nt");



		PTImportAdmin::PTImportAdmin()
			: AdminInterfaceElementTemplate<PTImportAdmin>()
		{ }


		
		void PTImportAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			/// @todo Initialize internal attributes from the map
			// 	string a = map.get<string>(PARAM_SEARCH_XXX);
			// 	string b = map.getDefault<string>(PARAM_SEARCH_XXX);
			// 	optional<string> c = map.getOptional<string>(PARAM_SEARCH_XXX);

			// Search table initialization
			// _requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_XXX, 30);
		}



		ParametersMap PTImportAdmin::getParametersMap() const
		{
			ParametersMap m;
			// ParametersMap m(_requestParameters.getParametersMap());

			// if(_searchXxx)
			//	m.insert(PARAM_SEARCH_XXX, *_searchXxx);

			return m;
		}


		
		bool PTImportAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTImportAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const AdminRequest& request
		) const	{

			DataSourceTableSync::SearchResult sources(DataSourceTableSync::Search(_getEnv()));

			////////////////////////////////////////////////////////////////////
			// TAB NAVTEQ
			if (openTabContent(stream, TAB_NAVTEQ))
			{

				StaticFunctionRequest<ImportFunction> importRequest(request, true);
				
				PropertiesHTMLTable t(importRequest.getHTMLForm());
				stream << t.open();
				stream << t.title("Propriétés");
				stream << t.cell("Source de données", t.getForm().getSelectInput(ImportFunction::PARAMETER_DATA_SOURCE, sources, optional<shared_ptr<DataSource> >()));
				stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(ImportFunction::PARAMETER_DO_IMPORT, false));
				stream << t.title("Données");
				stream << t.cell("Rues (streets)", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH + NavteqWithProjectionFileFormat::FILE_STREETS, string()));
				stream << t.cell("Noeuds (nodes)", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH + NavteqWithProjectionFileFormat::FILE_NODES, string()));
				stream << t.cell("Zones administratives (mtdarea)", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH + NavteqWithProjectionFileFormat::FILE_MTDAREA, string()));
				stream << t.close();
			}

			////////////////////////////////////////////////////////////////////
			// TAB TRIDENT
			if(openTabContent(stream, TAB_TRIDENT))
			{
				StaticFunctionRequest<ImportFunction> importRequest(request, true);

				PropertiesHTMLTable t(importRequest.getHTMLForm());
				stream << t.open();
				stream << t.title("Propriétés");
				stream << t.cell("Source de données", t.getForm().getSelectInput(ImportFunction::PARAMETER_DATA_SOURCE, sources, optional<shared_ptr<DataSource> >()));
				stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(ImportFunction::PARAMETER_DO_IMPORT, false));
				stream << t.title("Données");
				stream << t.cell("Ligne", t.getForm().getTextInput(ImportFunction::PARAMETER_PATH, string()));
				stream << t.close();
			}


			////////////////////////////////////////////////////////////////////
			// END TABS
			closeTabContent(stream);
		}



		AdminInterfaceElement::PageLinks PTImportAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if (moduleKey == PTModule::FACTORY_KEY && request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()))
			{
				links.push_back(getNewPage());
			}
			
			return links;
		}



		void PTImportAdmin::_buildTabs(
			const security::Profile& profile
		) const	{
			_tabs.clear();
			_tabs.push_back(Tab("Trident", TAB_TRIDENT, true));
			_tabs.push_back(Tab("Navteq", TAB_NAVTEQ, true));
			_tabBuilded = true;
		}
	}
}
