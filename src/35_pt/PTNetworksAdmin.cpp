
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

#include "PTNetworksAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "TransportNetwork.h"
#include "TransportNetworkAdmin.h"

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

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTNetworksAdmin>::FACTORY_KEY("PTNetworksAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTNetworksAdmin>::ICON("chart_organisation.png");
		template<> const string AdminInterfaceElementTemplate<PTNetworksAdmin>::DEFAULT_TITLE("Réseaux de transport");
	}

	namespace pt
	{
		const string PTNetworksAdmin::PARAM_SEARCH_NAME("na");



		PTNetworksAdmin::PTNetworksAdmin()
			: AdminInterfaceElementTemplate<PTNetworksAdmin>()
		{ }


		
		void PTNetworksAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			_searchName = map.getDefault<string>(PARAM_SEARCH_NAME);

			// Search table initialization
			// _requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_XXX, 30);
		}



		ParametersMap PTNetworksAdmin::getParametersMap() const
		{
			ParametersMap m;
			m.insert(PARAM_SEARCH_NAME, _searchName);
			return m;
		}


		
		bool PTNetworksAdmin::isAuthorized(
			const FunctionRequest<AdminRequest>& request
		) const	{
			return request.isAuthorized<TransportNetworkRight>(READ);
		}



		void PTNetworksAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const FunctionRequest<AdminRequest>& request
		) const	{
		
			/// @todo Implement the display by streaming the output to the stream variable
		
		}



		AdminInterfaceElement::PageLinks PTNetworksAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const FunctionRequest<AdminRequest>& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if (moduleKey == PTModule::FACTORY_KEY && isAuthorized(request))
			{
				links.push_back(getNewPage());
			}
			
			return links;
		}


		
		AdminInterfaceElement::PageLinks PTNetworksAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const FunctionRequest<AdminRequest>& request
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
