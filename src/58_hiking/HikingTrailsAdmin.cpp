
//////////////////////////////////////////////////////////////////////////
/// HikingTrailsAdmin class implementation.
///	@file HikingTrailsAdmin.cpp
///	@author Hugues Romain
///	@date 2010
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

#include "HikingTrailsAdmin.h"
#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "HikingModule.h"
#include "HikingRight.h"
#include "HikingTrailTableSync.h"
#include "HTMLModule.h"
#include "SearchFormHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HikingTrailAddAction.h"
#include "HikingTrailRemoveAction.h"
#include "HikingTrailAdmin.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace hiking;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, HikingTrailsAdmin>::FACTORY_KEY("HikingTrailsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<HikingTrailsAdmin>::ICON("map.png");
		template<> const string AdminInterfaceElementTemplate<HikingTrailsAdmin>::DEFAULT_TITLE("Randonnées");
	}

	namespace hiking
	{
		const string HikingTrailsAdmin::PARAM_SEARCH_NAME("na");



		HikingTrailsAdmin::HikingTrailsAdmin()
			: AdminInterfaceElementTemplate<HikingTrailsAdmin>()
		{ }


		
		void HikingTrailsAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Search table initialization
			_searchName = map.getOptional<string>(PARAM_SEARCH_NAME);
			_requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_NAME, 200);
		}



		ParametersMap HikingTrailsAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			if(_searchName)
			{
				m.insert(PARAM_SEARCH_NAME, *_searchName);
			}

			return m;
		}


		
		bool HikingTrailsAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<HikingRight>(READ);
		}



		void HikingTrailsAdmin::display(
			ostream& stream,
			VariablesMap& variables,
			const admin::AdminRequest& _request
		) const	{

			stream << "<h1>Recherche</h1>";

			AdminFunctionRequest<HikingTrailsAdmin> searchRequest(_request);

			SearchFormHTMLTable st(searchRequest.getHTMLForm());
			stream << st.open();
			stream << st.cell("Nom", st.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName ? *_searchName : string()));
			stream << st.close();

			stream << "<h1>Résultats</h1>";

			HikingTrailTableSync::SearchResult trails(HikingTrailTableSync::Search(_getEnv()));

			AdminFunctionRequest<HikingTrailAdmin> openRequest(_request);

			AdminActionFunctionRequest<HikingTrailRemoveAction,HikingTrailsAdmin> removeRequest(_request);

			AdminActionFunctionRequest<HikingTrailAddAction,HikingTrailAdmin> addRequest(_request);
			addRequest.setActionWillCreateObject();
			HTMLForm f(addRequest.getHTMLForm());
			stream << f.open();

			ResultHTMLTable::HeaderVector c;
			c.push_back(make_pair(PARAM_SEARCH_NAME, "Nom"));
			c.push_back(make_pair(string(), "Actions"));
			c.push_back(make_pair(string(), "Actions"));
			ResultHTMLTable t(c, st.getForm(), _requestParameters, trails);
			stream << t.open();

			BOOST_FOREACH(HikingTrailTableSync::SearchResult::value_type trail, trails)
			{
				openRequest.getPage()->setTrail(trail);
				removeRequest.getAction()->setTrail(trail);
				stream << t.row();
				stream << t.col() << trail->getName();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), HikingTrailAdmin::ICON);
				stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la randonnée ?", HikingTrailAdmin::ICON);
			}

			stream << t.row();
			stream << t.col() << f.getTextInput(HikingTrailAddAction::PARAMETER_NAME, string(), "(entrez le nom ici)");
			stream << t.col(2) << f.getSubmitButton("Créer");
			
			stream << t.close();
			stream << f.close();

		}



		AdminInterfaceElement::PageLinks HikingTrailsAdmin::getSubPagesOfModule(
			const std::string& moduleKey,
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& _request
		) const	{
			
			AdminInterfaceElement::PageLinks links;
			
			if(	moduleKey == HikingModule::FACTORY_KEY &&
				_request.getUser() &&
				_request.getUser()->getProfile() &&
				isAuthorized(*_request.getUser())
			){
				links.push_back(getNewPage());
			}
			
			return links;
		}


		
		AdminInterfaceElement::PageLinks HikingTrailsAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const admin::AdminRequest& _request
		) const	{
			
			AdminInterfaceElement::PageLinks links;

			BOOST_FOREACH(Registry<HikingTrail>::value_type it, Env::GetOfficialEnv().getRegistry<HikingTrail>())
			{
				shared_ptr<HikingTrailAdmin> p(getNewOtherPage<HikingTrailAdmin>());
				p->setTrail(it.second);
				links.push_back(p);
			}
			return links;
		}
	}
}
