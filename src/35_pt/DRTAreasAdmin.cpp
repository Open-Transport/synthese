
//////////////////////////////////////////////////////////////////////////
/// DRTAreasAdmin class implementation.
///	@file DRTAreasAdmin.cpp
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

#include "DRTAreasAdmin.hpp"

#include "AdminParametersException.h"
#include "ObjectCreateAction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "DRTArea.hpp"
#include "DRTAreaAdmin.hpp"
#include "ResultHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "HTMLModule.h"
#include "RemoveObjectAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "LineArea.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace html;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, DRTAreasAdmin>::FACTORY_KEY("DRTAreasAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DRTAreasAdmin>::ICON("shape_ungroup.png");
		template<> const string AdminInterfaceElementTemplate<DRTAreasAdmin>::DEFAULT_TITLE("Zones TAD");
	}

	namespace pt
	{
		// const string DRTAreasAdmin::PARAM_SEARCH_XXX("xx");



		DRTAreasAdmin::DRTAreasAdmin()
			: AdminInterfaceElementTemplate<DRTAreasAdmin>()
		{ }



		void DRTAreasAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			/// @todo Initialize internal attributes from the map
			// 	string a = map.get<string>(PARAM_SEARCH_XXX);
			// 	string b = map.getDefault<string>(PARAM_SEARCH_XXX);
			// 	optional<string> c = map.getOptional<string>(PARAM_SEARCH_XXX);

			// Search table initialization
			// _requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_XXX, 30);
		}



		ParametersMap DRTAreasAdmin::getParametersMap() const
		{
			ParametersMap m;
			// ParametersMap m(_requestParameters.getParametersMap());

			// if(_searchXxx)
			//	m.insert(PARAM_SEARCH_XXX, *_searchXxx);

			return m;
		}



		bool DRTAreasAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void DRTAreasAdmin::display(
			ostream& stream,
			const server::Request& request
		) const	{

			AdminFunctionRequest<DRTAreaAdmin> openRequest(request);

			AdminActionFunctionRequest<RemoveObjectAction, DRTAreasAdmin> deleteRequest(request, *this);

			AdminActionFunctionRequest<ObjectCreateAction, DRTAreaAdmin> addRequest(request);
			addRequest.getAction()->setTable<DRTArea>();
			addRequest.setActionWillCreateObject();
			addRequest.setActionFailedPage<DRTAreasAdmin>();

			HTMLForm f(addRequest.getHTMLForm("add"));

			HTMLTable::ColsVector c;
			c.push_back("ID");
			c.push_back("Nom");
			c.push_back("Action");
			c.push_back("Action");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

			stream << f.open();
			stream << t.open();

			BOOST_FOREACH(const DRTArea::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<DRTArea>())
			{
				stream << t.row();

				stream << t.col();
				stream << it.first;

				stream << t.col();
				stream << it.second->get<Name>();

				stream << t.col();
				openRequest.getPage()->setArea(it.second);
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + DRTAreaAdmin::ICON);

				stream << t.col();

				bool hasLine = false;
				BOOST_FOREACH(Registry<LineStop>::value_type lineArea, Env::GetOfficialEnv().getRegistry<LineStop>())
				{
					if(!dynamic_cast<LineArea*>(lineArea.second.get()))
					{
						continue;
					}
					LineArea& la(static_cast<LineArea&>(*lineArea.second));
					if(la.getArea()->getKey() == it.first && la.getLine())
					{
						hasLine = true;
						break;
					}
				}
				// Remove button only if the DRT area is not used by any route
				if(!hasLine)
				{
					deleteRequest.getAction()->setObjectId(it.first);
					stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la zone ?", "/admin/img/delete.png");
				}
				else
				{
					stream << HTMLModule::getLinkButton("alert('Impossible, zone utilisée dans un itinéraire.');", "Supprimer", string(), "/admin/img/delete.png", true);
				}
			}

			stream << t.row();
			stream << t.col();
			stream << t.col();
			stream << f.getTextInput(ObjectCreateAction::GetInputName<Name>(), string(), "(nom)");
			stream << t.col(2);
			stream << f.getSubmitButton("Ajouter");

			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks DRTAreasAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const PTModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks DRTAreasAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{

			AdminInterfaceElement::PageLinks links;

			const DRTAreaAdmin* ua(
				dynamic_cast<const DRTAreaAdmin*>(&currentPage)
			);

			if(ua)
			{
				boost::shared_ptr<DRTAreaAdmin> p(getNewPage<DRTAreaAdmin>());
				p->setArea(ua->getArea());
				links.push_back(p);
			}

			return links;
		}
}	}
