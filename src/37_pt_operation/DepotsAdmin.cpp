
//////////////////////////////////////////////////////////////////////////
/// DepotsAdmin class implementation.
///	@file DepotsAdmin.cpp
///	@author RCSobility
///	@date 2011
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

#include "DepotsAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTOperationModule.hpp"
#include "GlobalRight.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "DepotUpdateAction.hpp"
#include "DepotTableSync.hpp"
#include "RemoveObjectAction.hpp"
#include "ActionResultHTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace db;
	using namespace html;
	
	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DepotsAdmin>::FACTORY_KEY("DepotsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DepotsAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<DepotsAdmin>::DEFAULT_TITLE("Dépôts");
	}

	namespace pt_operation
	{
		const string DepotsAdmin::PARAMETER_SEARCH_NAME("sn");



		DepotsAdmin::DepotsAdmin()
			: AdminInterfaceElementTemplate<DepotsAdmin>()
		{ }


		
		void DepotsAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_NAME, 30);
			if(!map.getDefault<string>(PARAMETER_SEARCH_NAME).empty())
			{
				_searchName = map.get<string>(PARAMETER_SEARCH_NAME);
			}
		}



		ParametersMap DepotsAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_searchName)
			{
				m.insert(PARAMETER_SEARCH_NAME, *_searchName);
			}

			return m;
		}


		
		bool DepotsAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void DepotsAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

//			AdminFunctionRequest<VehicleAdmin> openRequest(request);

			AdminFunctionRequest<DepotsAdmin> searchRequest(request);

			AdminActionFunctionRequest<DepotUpdateAction, DepotsAdmin> createRequest(request);
			//createRequest.setActionFailedPage<VehiclesAdmin>();
			createRequest.setActionWillCreateObject();

			AdminActionFunctionRequest<RemoveObjectAction, DepotsAdmin> removeRequest(request);

			// Search
			DepotTableSync::SearchResult depots(
				DepotTableSync::Search(
					_getEnv(),
					_searchName
					, _requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAMETER_SEARCH_NAME
					, _requestParameters.raisingOrder
			)	);

			ActionResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Actions"));
			h.push_back(make_pair(string(), "Actions"));
			
			ActionResultHTMLTable t(
				h,
				searchRequest.getHTMLForm(),
				_requestParameters,
				depots,
				createRequest.getHTMLForm("add")
			);

			stream << t.open();

			BOOST_FOREACH(shared_ptr<Depot> depot, depots)
			{
				stream << t.row(lexical_cast<string>(depot->getKey()));
				stream << t.col() << depot->getName();

				stream << t.col();
//				openRequest.getPage()->setVehicle(const_pointer_cast<const Vehicle>(vehicle));
//				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), VehicleAdmin::ICON);
				
				stream << t.col();
				removeRequest.getAction()->setObjectId(depot->getKey());
				stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le dépôt "+ depot->getName() + " ?");
			}

			stream << t.row(string());
			stream << t.col() << t.getActionForm().getTextInput(DepotUpdateAction::PARAMETER_NAME, "", "Entrez le nom du dépôt ici");
			stream << t.col();
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.col();
			stream << t.close();

		}



		AdminInterfaceElement::PageLinks DepotsAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if(	dynamic_cast<const PTOperationModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}
			
			return links;
		}


		
		AdminInterfaceElement::PageLinks DepotsAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			// const DepotsAdmin* ua(
			//	dynamic_cast<const DepotsAdmin*>(&currentPage)
			// );
			
			// if(ua)
			// {
			//	shared_ptr<DepotsAdmin> p(getNewOtherPage<DepotsAdmin>());
			//	links.push_back(p);
			// }
			
			return links;
		}
	}
}


