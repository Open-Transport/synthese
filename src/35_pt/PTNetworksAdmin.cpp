
//////////////////////////////////////////////////////////////////////////
/// PTNetworksAdmin class implementation.
///	@file PTNetworksAdmin.cpp
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

#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "PTNetworksAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "TransportNetwork.h"
#include "TransportNetworkAdmin.h"
#include "Profile.h"
#include "AdminFunction.h"
#include "TransportNetworkTableSync.h"
#include "HTMLModule.h"
#include "SearchFormHTMLTable.h"
#include "TransportNetworkAddAction.h"
#include "ActionResultHTMLTable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTNetworksAdmin>::FACTORY_KEY("PTNetworksAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTNetworksAdmin>::ICON("chart_organisation.png");
		template<> const string AdminInterfaceElementTemplate<PTNetworksAdmin>::DEFAULT_TITLE("R�seaux de transport");
	}

	namespace pt
	{
		const string PTNetworksAdmin::PARAM_SEARCH_NAME("na");



		PTNetworksAdmin::PTNetworksAdmin()
			: AdminInterfaceElementTemplate<PTNetworksAdmin>()
		{ }


		
		void PTNetworksAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchName = map.getDefault<string>(PARAM_SEARCH_NAME);

			// Search table initialization
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_NAME, 30);
		}



		ParametersMap PTNetworksAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_SEARCH_NAME, _searchName);
			return m;
		}


		
		bool PTNetworksAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void PTNetworksAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const AdminRequest& request
		) const	{

			// Search form
			stream << "<h1>Recherche</h1>";

			AdminFunctionRequest<TransportNetworkAdmin> searchRequest(request);
			SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
			stream << s.open();
			stream << s.cell("Nom", s.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName));
			HTMLForm sortedForm(s.getForm());
			stream << s.close();

			// Results display
			stream << "<h1>R�seaux</h1>";

			TransportNetworkTableSync::SearchResult networks(
				TransportNetworkTableSync::Search(
					_getEnv(),
					_searchName,
					string(),
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAM_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);

			AdminActionFunctionRequest<TransportNetworkAddAction,TransportNetworkAdmin> newRequest(request);
			newRequest.getFunction()->setActionFailedPage<PTNetworksAdmin>();
			newRequest.setActionWillCreateObject();

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			h.push_back(make_pair(string(), "Actions"));
			ActionResultHTMLTable t(h,sortedForm,_requestParameters, networks, newRequest.getHTMLForm());
		
			stream << t.open();
			AdminFunctionRequest<TransportNetworkAdmin> openRequest(request);
			BOOST_FOREACH(shared_ptr<TransportNetwork> network, networks)
			{
				openRequest.getPage()->setNetwork(const_pointer_cast<const TransportNetwork>(network));
				stream << t.row();
				stream << t.col();
				stream << network->getName();
				stream << t.col();
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), TransportNetworkAdmin::ICON);
			}
			stream << t.row();
			stream << t.col();
			stream << t.getActionForm().getTextInput(TransportNetworkAddAction::PARAMETER_NAME, string());
			stream << t.col();
			stream << t.getActionForm().getSubmitButton("Cr�er");
			stream << t.close();
		}



		AdminInterfaceElement::PageLinks PTNetworksAdmin::getSubPagesOfModule(
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


		
		AdminInterfaceElement::PageLinks PTNetworksAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			BOOST_FOREACH(const Registry<TransportNetwork>::value_type& network, Env::GetOfficialEnv().getRegistry<TransportNetwork>())
			{
				shared_ptr<TransportNetworkAdmin> link(
					getNewOtherPage<TransportNetworkAdmin>(false)
				);
				link->setNetwork(network.second);
				links.push_back(link);
			}
			
			return links;
		}
	}
}
