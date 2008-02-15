
/** VinciBikeSearchAdminInterfaceElement class implementation.
	@file VinciBikeSearchAdminInterfaceElement.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "71_vinci_bike_rental/VinciBikeSearchAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciAddBike.h"
#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciContract.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"
#include "71_vinci_bike_rental/VinciBikeAdminInterfaceElement.h"
#include "71_vinci_bike_rental/VinciCustomerAdminInterfaceElement.h"

#include "05_html/SearchFormHTMLTable.h"
#include "05_html/ActionResultHTMLTable.h"
#include "05_html/HTMLModule.h"

#include "57_accounting/Transaction.h"

#include "12_security/UserTableSync.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminRequest.h"
#include "32_admin/HomeAdmin.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace html;
	using namespace security;
	using namespace vinci;

	namespace util
	{
	template <> const string FactorableTemplate<AdminInterfaceElement, VinciBikeSearchAdminInterfaceElement>::FACTORY_KEY("vincibikes");
	}

	namespace admin
	{
		template <> const string AdminInterfaceElementTemplate<VinciBikeSearchAdminInterfaceElement>::ICON("tag_blue.png");
		template <> const string AdminInterfaceElementTemplate<VinciBikeSearchAdminInterfaceElement>::DEFAULT_TITLE("Parc de vélos");
	}

	namespace vinci
	{
		const std::string VinciBikeSearchAdminInterfaceElement::PARAMETER_SEARCH_NUMBER = "searchnumber";
		const std::string VinciBikeSearchAdminInterfaceElement::PARAMETER_SEARCH_CADRE = "searchcadre";

		VinciBikeSearchAdminInterfaceElement::VinciBikeSearchAdminInterfaceElement()
			: AdminInterfaceElementTemplate<VinciBikeSearchAdminInterfaceElement>() {}

		void VinciBikeSearchAdminInterfaceElement::setFromParametersMap(const ParametersMap& map)
		{
			_bikeNumber = map.getString(PARAMETER_SEARCH_NUMBER, false, FACTORY_KEY);

			_cadreNumber = map.getString(PARAMETER_SEARCH_CADRE, false, FACTORY_KEY);

			_resultRequestParameters = ResultHTMLTable::getParameters(map.getMap(), PARAMETER_SEARCH_NUMBER, 30);

			_bikes = VinciBikeTableSync::search(
				_bikeNumber
				, _cadreNumber
				, _resultRequestParameters.first
				, _resultRequestParameters.maxSize
				, _resultRequestParameters.orderField == PARAMETER_SEARCH_NUMBER
				, _resultRequestParameters.orderField == PARAMETER_SEARCH_CADRE
				, _resultRequestParameters.raisingOrder
				);

			_resultResultParameters.next = (_bikes.size() == _resultRequestParameters.maxSize + 1);
			if (_resultResultParameters.next)
				_bikes.pop_back();
			_resultResultParameters.size = _bikes.size();

		}

		bool VinciBikeSearchAdminInterfaceElement::isAuthorized( const server::FunctionRequest<AdminRequest>* request ) const
		{
			return true;
		}

		void VinciBikeSearchAdminInterfaceElement::display(ostream& stream, VariablesMap& vars, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			// AddStatus
			FunctionRequest<AdminRequest> addStatusRequest(request);
			addStatusRequest.getFunction()->setPage<VinciBikeSearchAdminInterfaceElement>();

			// AddBike
			ActionFunctionRequest<VinciAddBike,AdminRequest> addBikeRequest(request);
			addBikeRequest.getFunction()->setPage<VinciBikeAdminInterfaceElement>();
			
			// Open a bike
			FunctionRequest<AdminRequest> viewBikeRequest(request);
			viewBikeRequest.getFunction()->setPage<VinciBikeAdminInterfaceElement>();

			// Open a contract
			FunctionRequest<AdminRequest> viewContract(request);
			viewContract.getFunction()->setPage<VinciCustomerAdminInterfaceElement>();

			stream << "<h1>Recherche de vélo</h1>";

			SearchFormHTMLTable st(addStatusRequest.getHTMLForm("search"));
			stream << st.open();
			stream << st.cell("Numéro", st.getForm().getTextInput(PARAMETER_SEARCH_NUMBER, _bikeNumber));
			stream << st.cell("Cadre", st.getForm().getTextInput(PARAMETER_SEARCH_CADRE, _cadreNumber));
			stream << st.close();

			stream << "<h1>Résultat de la recherche</h1>";

			ActionResultHTMLTable::HeaderVector rh;
			rh.push_back(make_pair(PARAMETER_SEARCH_NUMBER, "Numéro"));
			rh.push_back(make_pair(PARAMETER_SEARCH_CADRE, "Cadre"));
			rh.push_back(make_pair(string(), "Statut"));
			ActionResultHTMLTable rt(rh, st.getForm(), _resultRequestParameters, _resultResultParameters, addBikeRequest.getHTMLForm("create"));
				
			stream << rt.open();
			for (vector<shared_ptr<VinciBike> >::const_iterator it = _bikes.begin(); it != _bikes.end(); ++it)
			{
				shared_ptr<const VinciBike> bike = *it;
				viewBikeRequest.setObjectId(bike->getKey());
				stream << rt.row();
				stream << rt.col() << HTMLModule::getHTMLLink(viewBikeRequest.getURL(), (*it)->getNumber());
				stream << rt.col() << HTMLModule::getHTMLLink(viewBikeRequest.getURL(), (*it)->getMarkedNumber());
				stream << rt.col();
				if (bike->getComplements().lastTransaction.get())
				{
					if (bike->getComplements().lastTransaction->getEndDateTime().isUnknown())
						stream << "EN LOCATION depuis le ";
					else
						stream << "DISPONIBLE.<br />Dernière location le ";
					stream << bike->getComplements().lastTransaction->getStartDateTime().toString();
					if (bike->getComplements().lastContract.get())
					{
						viewContract.setObjectId(bike->getComplements().lastContract->getKey());
						shared_ptr<const User> user = UserTableSync::Get(bike->getComplements().lastContract->getUserId());
						stream << " par " << HTMLModule::getHTMLLink(viewContract.getURL(), user->getFullName());
					}
				}

			}
			stream << rt.row();
			stream << rt.col() << rt.getActionForm().getTextInput(VinciAddBike::PARAMETER_NUMBER, _bikeNumber);
			stream << rt.col() << rt.getActionForm().getTextInput(VinciAddBike::PARAMETER_MARKED_NUMBER, _cadreNumber);
			stream << rt.col() << rt.getActionForm().getSubmitButton("Créer");
			stream << rt.close();
		}

		AdminInterfaceElement::PageLinks VinciBikeSearchAdminInterfaceElement::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == HomeAdmin::FACTORY_KEY)
			{
				links.push_back(_pageLink);
			}
			return links;
		}
	}
}

