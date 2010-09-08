
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
#include "PTStopsImportWizardAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "ImportFunction.h"
#include "PropertiesHTMLTable.h"
#include "DataSource.h"
#include "DataSourceTableSync.h"
#include "TransportNetworkRight.h"
#include "Profile.h"
#include "AdminFunction.h"
#include "AdminFunctionRequest.hpp"

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
		template<> const string AdminInterfaceElementTemplate<PTImportAdmin>::DEFAULT_TITLE("Sources de données");
	}

	namespace pt
	{
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

			stream << "<h1>Sources de données</h1>";

			DataSourceTableSync::SearchResult datasources(
				DataSourceTableSync::Search(
					*_env
			)	);

			HTMLTable::ColsVector c;
			c.push_back("ID");
			c.push_back("Ic");
			c.push_back("Nom");
			c.push_back("Format");
			c.push_back("Actions");
			c.push_back("Actions");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

			stream << t.open();

			BOOST_FOREACH(const DataSourceTableSync::SearchResult::value_type& datasource, datasources)
			{
				stream << t.row();
				stream << t.col();
				stream << datasource->getKey();

				stream << t.col();
				stream << HTMLModule::getHTMLImage(
					datasource->getIcon().empty() ? "note.png" : datasource->getIcon(),
					datasource->getName()
				);

				stream << t.col();
				stream << datasource->getName();

				stream << t.col();
				stream << datasource->getFormat();

				stream << t.col();
				AdminFunctionRequest<PTStopsImportWizardAdmin> openRequest(request);
				openRequest.getPage()->setDataSource(const_pointer_cast<const DataSource>(datasource));
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir");

				stream << t.col();
				stream << "Supprimer";
			}
			stream << t.close();
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



		PTImportAdmin::PageLinks PTImportAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			if(	dynamic_cast<const PTImportAdmin*>(&currentPage) ||
				dynamic_cast<const PTStopsImportWizardAdmin*>(&currentPage) 
			){
				DataSourceTableSync::SearchResult datasources(
					DataSourceTableSync::Search(
						*_env
				)	);
				BOOST_FOREACH(const DataSourceTableSync::SearchResult::value_type& datasource, datasources)
				{
					shared_ptr<PTStopsImportWizardAdmin> page(getNewOtherPage<PTStopsImportWizardAdmin>());
					page->setDataSource(datasource);
					links.push_back(page);
				}
			}
			return links;
		}
	}
}
