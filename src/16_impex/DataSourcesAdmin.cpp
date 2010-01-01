
//////////////////////////////////////////////////////////////////////////
/// DataSourcesAdmin class implementation.
///	@file DataSourcesAdmin.cpp
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

#include "DataSourcesAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "GlobalRight.h"
#include "ImpExModule.h"
#include "DataSource.h"
#include "DataSourceTableSync.h"
#include "AdminFunctionRequest.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace impex;
	using namespace security;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DataSourcesAdmin>::FACTORY_KEY("DataSourcesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DataSourcesAdmin>::ICON("database.png");
		template<> const string AdminInterfaceElementTemplate<DataSourcesAdmin>::DEFAULT_TITLE("Sources de données");
	}

	namespace impex
	{
		const string DataSourcesAdmin::PARAM_SEARCH_NAME("sn");



		DataSourcesAdmin::DataSourcesAdmin()
			: AdminInterfaceElementTemplate<DataSourcesAdmin>()
		{ }


		
		void DataSourcesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchName = map.getDefault<string>(PARAM_SEARCH_NAME);

			// Search table initialization
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_NAME, 100);
		}



		ParametersMap DataSourcesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			m.insert(PARAM_SEARCH_NAME, _searchName);

			return m;
		}


		
		bool DataSourcesAdmin::isAuthorized(
			const security::Profile& profile
		) const	{
			return profile.isAuthorized<GlobalRight>(READ);
		}



		void DataSourcesAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const AdminRequest& request
		) const	{

			AdminFunctionRequest<DataSourcesAdmin> searchRequest(request);

			DataSourceTableSync::SearchResult dataSources(
				DataSourceTableSync::Search(
					*_env,
					_searchName,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAM_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);

			ResultHTMLTable::HeaderVector c;
			c.push_back(make_pair(string(), "ID"));
			c.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			c.push_back(make_pair(string(), "Format"));
			c.push_back(make_pair(string(), "Action"));
			ResultHTMLTable t(c, searchRequest.getHTMLForm(), _requestParameters, dataSources);

			stream << t.open();

			BOOST_FOREACH(shared_ptr<DataSource> dataSource, dataSources)
			{
				stream << t.row();
				stream << t.col() << dataSource->getKey();
				stream << t.col() << dataSource->getName();
				stream << t.col() << dataSource->getFormat();
				stream << t.col();
			}

			stream << t.close();
		
		}



		AdminInterfaceElement::PageLinks DataSourcesAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if(	moduleKey == ImpExModule::FACTORY_KEY &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser()->getProfile())
			){
				links.push_back(getNewPage());
			}
			
			return links;
		}


		
		AdminInterfaceElement::PageLinks DataSourcesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;

			DataSourceTableSync::SearchResult dataSources(
				DataSourceTableSync::Search(*_env)
			);
			BOOST_FOREACH(shared_ptr<DataSource> dataSource, dataSources)
			{
//				shared_ptr<DataSourceAdmin> p(getNewOtherPage<DataSourceAdmin>());
//				p->setDataSource(dataSource);
//				links.push_back(p);
			}

			return links;
		}
	}
}
