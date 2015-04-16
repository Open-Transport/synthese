
//////////////////////////////////////////////////////////////////////////
/// HikingTrailsAdmin class implementation.
///	@file HikingTrailsAdmin.cpp
///	@author Hugues Romain
///	@date 2010
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

#include "HikingTrailsAdmin.h"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "HikingModule.h"
#include "HikingRight.h"
#include "HikingTrailTableSync.h"
#include "HTMLModule.h"
#include "SearchFormHTMLTable.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HikingTrailAddAction.h"
#include "HikingTrailAdmin.h"
#include "RemoveObjectAction.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace hiking;
	using namespace html;
	using namespace db;

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
			_requestParameters.setFromParametersMap(map, PARAM_SEARCH_NAME, 200);
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
			const server::Request& _request
		) const	{

			stream << "<h1>Recherche</h1>";

			AdminFunctionRequest<HikingTrailsAdmin> searchRequest(_request, *this);

			SearchFormHTMLTable st(searchRequest.getHTMLForm());
			stream << st.open();
			stream << st.cell("Nom", st.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName ? *_searchName : string()));
			stream << st.close();

			stream << "<h1>Résultats</h1>";

			HikingTrailTableSync::SearchResult trails(HikingTrailTableSync::Search(_getEnv()));

			AdminFunctionRequest<HikingTrailAdmin> openRequest(_request);

			AdminActionFunctionRequest<RemoveObjectAction,HikingTrailsAdmin> removeRequest(_request, *this);

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
				removeRequest.getAction()->setObjectId(trail->getKey());
				stream << t.row();
				stream << t.col() << trail->getName();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir", string(), "/admin/img/" + HikingTrailAdmin::ICON);
				stream << t.col() << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer la randonnée ?", "/admin/img/" + HikingTrailAdmin::ICON);
			}

			stream << t.row();
			stream << t.col() << f.getTextInput(HikingTrailAddAction::PARAMETER_NAME, string(), "(entrez le nom ici)");
			stream << t.col(2) << f.getSubmitButton("Créer");

			stream << t.close();
			stream << f.close();

		}



		AdminInterfaceElement::PageLinks HikingTrailsAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& _request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const HikingModule*>(&module) &&
				_request.getUser() &&
				_request.getUser()->getProfile() &&
				isAuthorized(*_request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}



		AdminInterfaceElement::PageLinks HikingTrailsAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& _request
		) const	{

			AdminInterfaceElement::PageLinks links;

			BOOST_FOREACH(Registry<HikingTrail>::value_type it, Env::GetOfficialEnv().getRegistry<HikingTrail>())
			{
				boost::shared_ptr<HikingTrailAdmin> p(getNewPage<HikingTrailAdmin>());
				p->setTrail(it.second);
				links.push_back(p);
			}
			return links;
		}
	}
}
