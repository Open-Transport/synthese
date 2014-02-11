
//////////////////////////////////////////////////////////////////////////
/// VehiclesAdmin class implementation.
///	@file VehiclesAdmin.cpp
///	@author Hugues Romain
///	@date 2011
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

#include "VehiclesAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "VehicleModule.hpp"
#include "User.h"
#include "VehicleAdmin.hpp"
#include "ActionResultHTMLTable.h"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "GlobalRight.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "VehicleUpdateAction.hpp"
#include "VehicleTableSync.hpp"
#include "RemoveObjectAction.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace vehicle;
	using namespace html;
	using namespace db;


	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, VehiclesAdmin>::FACTORY_KEY("VehiclesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<VehiclesAdmin>::ICON("car.png");
		template<> const string AdminInterfaceElementTemplate<VehiclesAdmin>::DEFAULT_TITLE("Vehicules");
	}

	namespace vehicle
	{
		const string VehiclesAdmin::PARAMETER_SEARCH_NAME("sn");
		const string VehiclesAdmin::PARAMETER_SEARCH_NUMBER("su");
		const string VehiclesAdmin::PARAMETER_SEARCH_REGISTRATION("sr");



		VehiclesAdmin::VehiclesAdmin()
			: AdminInterfaceElementTemplate<VehiclesAdmin>()
		{ }



		void VehiclesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_requestParameters.setFromParametersMap(map, PARAMETER_SEARCH_NAME, 30);
			if(!map.getDefault<string>(PARAMETER_SEARCH_NAME).empty())
			{
				_searchName = map.get<string>(PARAMETER_SEARCH_NAME);
			}
		}



		ParametersMap VehiclesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_searchName)
			{
				m.insert(PARAMETER_SEARCH_NAME, *_searchName);
			}

			return m;
		}



		bool VehiclesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<GlobalRight>(READ);
		}



		void VehiclesAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			AdminFunctionRequest<VehicleAdmin> openRequest(request);

			AdminFunctionRequest<VehiclesAdmin> searchRequest(request, *this);

			AdminActionFunctionRequest<VehicleUpdateAction, VehicleAdmin> createRequest(request);
			createRequest.setActionFailedPage<VehiclesAdmin>();
			createRequest.setActionWillCreateObject();

			AdminActionFunctionRequest<RemoveObjectAction, VehiclesAdmin> removeRequest(request, *this);

			// Search
			VehicleTableSync::SearchResult vehicles(
				VehicleTableSync::Search(
					_getEnv(),
					_searchName,
					_searchNumber,
					_searchRegistration,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAMETER_SEARCH_NAME,
					_requestParameters.orderField == PARAMETER_SEARCH_NUMBER,
					_requestParameters.orderField == PARAMETER_SEARCH_REGISTRATION,
					_requestParameters.raisingOrder
			)	);

			ActionResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAMETER_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(PARAMETER_SEARCH_NUMBER, "Numéro"));
			h.push_back(make_pair(PARAMETER_SEARCH_REGISTRATION, "Immatriculation"));
			h.push_back(make_pair(string(), "Lien"));
			h.push_back(make_pair(string(), "Actions"));
			h.push_back(make_pair(string(), "Actions"));

			ActionResultHTMLTable t(
				h,
				searchRequest.getHTMLForm(),
				_requestParameters,
				vehicles,
				createRequest.getHTMLForm("add")
			);

			stream << t.open();

			BOOST_FOREACH(const boost::shared_ptr<Vehicle>& vehicle, vehicles)
			{
				stream << t.row(lexical_cast<string>(vehicle->getKey()));
				stream << t.col() << vehicle->getName();
				stream << t.col() << vehicle->getNumber();
				stream << t.col() << vehicle->getRegistrationNumbers();

				// Link
				stream << t.col();
				if(!vehicle->getURL().empty())
				{
					stream << HTMLModule::getLinkButton(vehicle->getURL(), "Lien");
				}

				stream << t.col();

				openRequest.getPage()->setVehicle(const_pointer_cast<const Vehicle>(vehicle));
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + VehicleAdmin::ICON);

				stream << t.col();
				removeRequest.getAction()->setObjectId(vehicle->getKey());
				stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le véhicule "+ vehicle->getName() + " ?");
			}

			stream << t.row(string());
			stream << t.col() << t.getActionForm().getTextInput(VehicleUpdateAction::PARAMETER_NAME, "", "Entrez le nom du véhicule ici");
			stream << t.col() << t.getActionForm().getTextInput(VehicleUpdateAction::PARAMETER_NUMBER, "", "Entrez le numéro du véhicule ici");
			stream << t.col() << t.getActionForm().getTextInput(VehicleUpdateAction::PARAMETER_REGISTRATION_NUMBERS, "", "Entrez l'immatriculation ici");
			stream << t.col();
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.col();
			stream << t.close();
		}



		AdminInterfaceElement::PageLinks VehiclesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& _request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const VehicleModule*>(&module) &&
				_request.getUser() &&
				_request.getUser()->getProfile() &&
				isAuthorized(*_request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks VehiclesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& _request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const VehiclesAdmin*>(&currentPage) ||
				dynamic_cast<const VehicleAdmin*>(&currentPage)
			){

				BOOST_FOREACH(const Vehicle::Registry::value_type& vehicle, Env::GetOfficialEnv().getRegistry<Vehicle>())
				{
					boost::shared_ptr<VehicleAdmin> p(getNewPage<VehicleAdmin>());
					p->setVehicle(vehicle.second);
					links.push_back(p);
				}
			}

			return links;
		}
	}
}
