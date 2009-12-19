
//////////////////////////////////////////////////////////////////////////
/// PTCitiesAdmin class implementation.
///	@file PTCitiesAdmin.cpp
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

#include "PTCitiesAdmin.h"
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

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, PTCitiesAdmin>::FACTORY_KEY("PTCitiesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<PTCitiesAdmin>::ICON("building.png");
		template<> const string AdminInterfaceElementTemplate<PTCitiesAdmin>::DEFAULT_TITLE("Localités");
	}

	namespace pt
	{
		const string PTCitiesAdmin::PARAM_SEARCH_NAME("na");



		PTCitiesAdmin::PTCitiesAdmin()
			: AdminInterfaceElementTemplate<PTCitiesAdmin>()
		{ }


		
		void PTCitiesAdmin::setFromParametersMap(
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			_searchName = map.getOptional<string>(PARAM_SEARCH_NAME);

			// Search table initialization
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_NAME, 50);
		}



		ParametersMap PTCitiesAdmin::getParametersMap() const
		{
			ParametersMap m;
			// ParametersMap m(_requestParameters.getParametersMap());

			// if(_searchXxx)
			//	m.insert(PARAM_SEARCH_XXX, *_searchXxx);

			return m;
		}


		
		bool PTCitiesAdmin::isAuthorized(
			const FunctionRequest<AdminRequest>& request
		) const	{
			return request.isAuthorized<TransportNetworkRight>(READ);
		}



		void PTCitiesAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const FunctionRequest<AdminRequest>& request
		) const	{
		
			/// @todo Implement the display by streaming the output to the stream variable
		
		}



		AdminInterfaceElement::PageLinks PTCitiesAdmin::getSubPagesOfModule(
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


		
		AdminInterfaceElement::PageLinks PTCitiesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const FunctionRequest<AdminRequest>& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			// const PTCitiesAdmin* ua(
			//	dynamic_cast<const PTCitiesAdmin*>(&currentPage)
			// );
			
			// if(ua)
			// {
			//	shared_ptr<PTCitiesAdmin> p(getNewOtherPage<PTCitiesAdmin>());
			//	AddToLinks(links, p);
			// }
			
			return links;
		}
	}
}
