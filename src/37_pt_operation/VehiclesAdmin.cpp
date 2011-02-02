
//////////////////////////////////////////////////////////////////////////
/// VehiclesAdmin class implementation.
///	@file VehiclesAdmin.cpp
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

#include "VehiclesAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTOperationModule.hpp"
#include "VehicleAdmin.hpp"
#include "ActionResultHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "GlobalRight.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "VehicleUpdateAction.hpp"
#include "VehicleTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt_operation;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VehiclesAdmin>::FACTORY_KEY("VehiclesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VehiclesAdmin>::ICON("car.png");
		template<> const string AdminInterfaceElementTemplate<VehiclesAdmin>::DEFAULT_TITLE("Vehicules");
	}

	namespace pt_operation
	{
		const string VehiclesAdmin::PARAMETER_SEARCH_NAME("sn");



		VehiclesAdmin::VehiclesAdmin()
			: AdminInterfaceElementTemplate<VehiclesAdmin>()
		{ }


		
		void VehiclesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_NAME, 30);
		}



		ParametersMap VehiclesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_searchName)
				m.insert(PARAMETER_SEARCH_NAME, *_searchName);

			return m;
		}


		
		bool VehiclesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void VehiclesAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			AdminFunctionRequest<VehicleAdmin> openRequest(request);

			AdminFunctionRequest<VehiclesAdmin> searchRequest(request);

			AdminActionFunctionRequest<VehicleUpdateAction, VehicleAdmin> createRequest(request);
			createRequest.setActionFailedPage<VehiclesAdmin>();
			createRequest.setActionWillCreateObject();

			// Search
			VehicleTableSync::SearchResult vehicles(
				VehicleTableSync::Search(
					_getEnv(),
					_searchName
					, _requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAMETER_SEARCH_NAME
					, _requestParameters.raisingOrder
			)	);

			ActionResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Numéro"));
			h.push_back(make_pair(string(), "Actions"));
			
			ActionResultHTMLTable t(
				h,
				searchRequest.getHTMLForm(),
				_requestParameters,
				vehicles,
				createRequest.getHTMLForm("add")
			);

			stream << t.open();

			BOOST_FOREACH(shared_ptr<Vehicle> vehicle, vehicles)
			{
				stream << t.row(lexical_cast<string>(vehicle->getKey()));
				stream << t.col() << vehicle->getName();
				stream << t.col() << vehicle->getNumber();
				stream << t.col();

				openRequest.getPage()->setVehicle(const_pointer_cast<const Vehicle>(vehicle));
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), VehicleAdmin::ICON);
				
			}

			stream << t.row(string());
			stream << t.col() << t.getActionForm().getTextInput(VehicleUpdateAction::PARAMETER_NAME, "", "Entrez le nom du véhicule ici");
			stream << t.col() << t.getActionForm().getTextInput(VehicleUpdateAction::PARAMETER_NUMBER, "", "Entrez le numéro du véhicule ici");
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.close();
		}



		AdminInterfaceElement::PageLinks VehiclesAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& _request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if(	moduleKey == PTOperationModule::FACTORY_KEY &&
				_request.getUser() &&
				_request.getUser()->getProfile() &&
				isAuthorized(*_request.getUser())
			){
				links.push_back(getNewPage());
			}
			
			return links;
		}


		
		AdminInterfaceElement::PageLinks VehiclesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& _request
		) const	{
			
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const VehiclesAdmin*>(&currentPage) ||
				dynamic_cast<const VehicleAdmin*>(&currentPage)
			){

				BOOST_FOREACH(const Vehicle::Registry::value_type& vehicle, Env::GetOfficialEnv().getRegistry<Vehicle>())
				{
					shared_ptr<VehicleAdmin> p(getNewOtherPage<VehicleAdmin>());
					p->setVehicle(vehicle.second);
					links.push_back(p);
				}
			}
			
			return links;
		}
	}
}
