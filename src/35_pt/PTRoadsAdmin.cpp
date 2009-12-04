
//////////////////////////////////////////////////////////////////////////
/// PTRoadsAdmin class implementation.
///	@file PTRoadsAdmin.cpp
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

#include "PTRoadsAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace env;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTRoadsAdmin>::FACTORY_KEY("PTRoadsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTRoadsAdmin>::ICON("pause_blue.png");
		template<> const string AdminInterfaceElementTemplate<PTRoadsAdmin>::DEFAULT_TITLE("Routes");
	}

	namespace pt
	{
		const string PTRoadsAdmin::PARAM_SEARCH_CITY_NAME("cn");
		const string PTRoadsAdmin::PARAM_SEARCH_NAME("na");



		PTRoadsAdmin::PTRoadsAdmin()
			: AdminInterfaceElementTemplate<PTRoadsAdmin>()
		{ }


		
		void PTRoadsAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			/// @todo Initialize internal attributes from the map
			// 	string a = map.get<string>(PARAM_SEARCH_XXX);
			// 	string b = map.getDefault<string>(PARAM_SEARCH_XXX);
			// 	optional<string> c = map.getOptional<string>(PARAM_SEARCH_XXX);

			// Search table initialization
			// _requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_XXX, 30);
		}



		ParametersMap PTRoadsAdmin::getParametersMap() const
		{
			ParametersMap m;
			// ParametersMap m(_requestParameters.getParametersMap());

			// if(_searchXxx)
			//	m.insert(PARAM_SEARCH_XXX, *_searchXxx);

			return m;
		}


		
		bool PTRoadsAdmin::isAuthorized(
			const FunctionRequest<AdminRequest>& request
		) const	{
			return request.isAuthorized<TransportNetworkRight>(READ);
		}



		void PTRoadsAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const FunctionRequest<AdminRequest>& request
		) const	{
		
			/// @todo Implement the display by streaming the output to the stream variable
		
		}



		AdminInterfaceElement::PageLinks PTRoadsAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const FunctionRequest<AdminRequest>& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if (moduleKey == PTModule::FACTORY_KEY && isAuthorized(request))
			{
				AddToLinks(links, getNewPage());
			}
			
			return links;
		}


		
		AdminInterfaceElement::PageLinks PTRoadsAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const FunctionRequest<AdminRequest>& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			// const PTRoadsAdmin* ua(
			//	dynamic_cast<const PTRoadsAdmin*>(&currentPage)
			// );
			
			// if(ua)
			// {
			//	shared_ptr<PTRoadsAdmin> p(getNewOtherPage<PTRoadsAdmin>());
			//	AddToLinks(links, p);
			// }
			
			return links;
		}
	}
}
