
//////////////////////////////////////////////////////////////////////////
/// DataSourcesAdmin class implementation.
///	@file DataSourcesAdmin.cpp
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "ObjectCreateAction.hpp"
#include "ObjectUpdateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "GlobalRight.h"
#include "ImpExModule.h"
#include "DataSource.h"
#include "DataSourceTableSync.h"
#include "AdminFunctionRequest.hpp"
#include "DataSourceAdmin.h"
#include "HTMLModule.h"
#include "AdminActionFunctionRequest.hpp"
#include "RemoveObjectAction.hpp"
#include "HTMLForm.h"
#include "ImpExModule.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace impex;
	using namespace security;
	using namespace html;
	using namespace db;

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
			_requestParameters.setFromParametersMap(map, PARAM_SEARCH_NAME, 100);
		}



		ParametersMap DataSourcesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			m.insert(PARAM_SEARCH_NAME, _searchName);

			return m;
		}



		bool DataSourcesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void DataSourcesAdmin::display(
			ostream& stream,
			const Request& request
		) const	{

			stream << "<h1>Sources de données</h1>";

			AdminFunctionRequest<DataSourcesAdmin> searchRequest(request, *this);

			AdminFunctionRequest<DataSourceAdmin> openRequest(request);

			AdminActionFunctionRequest<ObjectCreateAction, DataSourceAdmin> addRequest(request);
			addRequest.getAction()->setTable<DataSource>();
			addRequest.setActionWillCreateObject();
			addRequest.setActionFailedPage<DataSourcesAdmin>();

			AdminActionFunctionRequest<RemoveObjectAction, DataSourcesAdmin> removeRequest(request, *this);

			DataSourceTableSync::SearchResult dataSources(
				DataSourceTableSync::Search(
					*_env,
					_searchName,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAM_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);

			HTMLForm f(addRequest.getHTMLForm("add"));
			stream << f.open();

			ResultHTMLTable::HeaderVector c;
			c.push_back(make_pair(string(), "ID"));
			c.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			c.push_back(make_pair(string(), "Actions"));
			c.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(c, searchRequest.getHTMLForm(), _requestParameters, dataSources);

			stream << t.open();

			BOOST_FOREACH(const boost::shared_ptr<DataSource>& dataSource, dataSources)
			{
				openRequest.getPage()->setDataSource(const_pointer_cast<const DataSource>(dataSource));
				removeRequest.getAction()->setObjectId(dataSource->getKey());

				stream << t.row();
				stream << t.col() << dataSource->getKey();
				stream << t.col() << dataSource->get<Name>();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getHTMLForm().getURL(), "Ouvrir", string(), "/admin/img/database_edit.png");
				stream << t.col() << HTMLModule::getLinkButton(removeRequest.getHTMLForm().getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la source de données "+ dataSource->getName() +" ?", "/admin/img/database_delete.png");
			}

			stream << t.row();
			stream << t.col() << "Création :";
			stream << t.col() << f.getTextInput(ObjectCreateAction::GetInputName<Name>(), string(), "(nom)");
			stream << t.col(2) << f.getSubmitButton("Ajouter");
			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks DataSourcesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const ImpExModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks DataSourcesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			DataSourceTableSync::SearchResult dataSources(
				DataSourceTableSync::Search(*_env)
			);
			BOOST_FOREACH(const boost::shared_ptr<DataSource>& dataSource, dataSources)
			{
				boost::shared_ptr<DataSourceAdmin> p(getNewPage<DataSourceAdmin>());
				p->setDataSource(dataSource);
				links.push_back(p);
			}

			return links;
		}
	}
}
