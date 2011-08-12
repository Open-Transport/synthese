
//////////////////////////////////////////////////////////////////////////
/// DestinationsAdmin class implementation.
///	@file DestinationsAdmin.cpp
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

#include "DestinationsAdmin.hpp"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "PTModule.h"
#include "TransportNetworkRight.h"
#include "DestinationTableSync.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "DestinationAdmin.hpp"
#include "DestinationUpdateAction.hpp"
#include "RemoveObjectAction.hpp"

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
		template<> const string FactorableTemplate<AdminInterfaceElement, DestinationsAdmin>::FACTORY_KEY("DestinationsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<DestinationsAdmin>::ICON("monitor.png");
		template<> const string AdminInterfaceElementTemplate<DestinationsAdmin>::DEFAULT_TITLE("Girouettes");
	}

	namespace pt
	{
		const string DestinationsAdmin::PARAM_SEARCH_NAME("name");



		DestinationsAdmin::DestinationsAdmin()
			: AdminInterfaceElementTemplate<DestinationsAdmin>()
		{ }


		
		void DestinationsAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			_searchName = map.getOptional<string>(PARAM_SEARCH_NAME);

			// Search table initialization
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_NAME, 100);
		}



		ParametersMap DestinationsAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_searchName)
			{
				m.insert(PARAM_SEARCH_NAME, *_searchName);
			}

			return m;
		}


		
		bool DestinationsAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<TransportNetworkRight>(READ);
		}



		void DestinationsAdmin::display(
			ostream& stream,
			const admin::AdminRequest& request
		) const	{

			AdminFunctionRequest<DestinationAdmin> openRequest(request);

			AdminActionFunctionRequest<RemoveObjectAction, DestinationsAdmin> deleteRequest(request);

			AdminActionFunctionRequest<DestinationUpdateAction, DestinationAdmin> addRequest(request);
			addRequest.setActionWillCreateObject();
			addRequest.setActionFailedPage<DestinationsAdmin>();

			HTMLForm f(addRequest.getHTMLForm("add"));

			HTMLTable::ColsVector c;
			c.push_back("ID");
			c.push_back("Texte affiché");
			c.push_back("Action");
			c.push_back("Action");
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);

			stream << f.open();
			stream << t.open();

			DestinationTableSync::SearchResult destinations(
				DestinationTableSync::Search(
					*_env,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == PARAM_SEARCH_NAME,
					_requestParameters.raisingOrder
			)	);
			BOOST_FOREACH(shared_ptr<Destination> destination, destinations)
			{
				// Row init
				stream << t.row();

				// ID
				stream << t.col() << destination->getKey();

				// Displayed text
				stream << t.col() << destination->getDisplayedText();

				// Open button
				openRequest.getPage()->setDestination(const_pointer_cast<const Destination>(destination));
				stream << t.col();
				stream << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir");
				
				// Delete button
				deleteRequest.getAction()->setObjectId(destination->getKey());
				stream << t.col();
				stream << HTMLModule::getLinkButton(deleteRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la destination ?");
			}

			// New destination form
			stream << t.row();
			stream << t.col();
			stream << t.col() << f.getTextInput(DestinationUpdateAction::PARAMETER_DISPLAYED_TEXT, string());
			stream << t.col() << f.getSubmitButton("Ajouter");
			stream << t.col();

			stream << t.close();
			stream << f.close();
		}



		AdminInterfaceElement::PageLinks DestinationsAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
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


		
		AdminInterfaceElement::PageLinks DestinationsAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			const DestinationsAdmin* ua(
				dynamic_cast<const DestinationsAdmin*>(&currentPage)
			);
			
			const DestinationAdmin* ua2(
				dynamic_cast<const DestinationAdmin*>(&currentPage)
			);

			if(ua || ua2)
			{

				DestinationTableSync::SearchResult destinations(
					DestinationTableSync::Search(
						*_env
				)	);
				BOOST_FOREACH(shared_ptr<Destination> destination, destinations)
				{
					shared_ptr<DestinationAdmin> p(getNewPage<DestinationAdmin>());
					p->setDestination(const_pointer_cast<const Destination>(destination));
					links.push_back(p);
				}
			}
			
			return links;
		}
}	}
