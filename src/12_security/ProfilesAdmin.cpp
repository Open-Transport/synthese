
/** ProfilesAdmin class implementation.
	@file ProfilesAdmin.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "ProfilesAdmin.h"

#include "Profile.h"
#include "User.h"
#include "UtilConstants.h"
#include "ActionResultHTMLTable.h"
#include "SearchFormHTMLTable.h"
#include "HTMLList.h"
#include "ProfileAdmin.h"
#include "ProfileTableSync.h"
#include "AddProfileAction.h"
#include "RemoveObjectAction.hpp"
#include "Right.h"
#include "SecurityRight.h"
#include "SecurityModule.h"
#include "SecurityConstants.hpp"
#include "StaticActionFunctionRequest.h"
#include "AdminModule.h"
#include "AdminInterfaceElement.h"
#include "ModuleAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <map>
#include <string>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace security;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ProfilesAdmin>::FACTORY_KEY("profiles");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ProfilesAdmin>::ICON("group.png");
		template<> const string AdminInterfaceElementTemplate<ProfilesAdmin>::DEFAULT_TITLE("Profils");
	}

	namespace security
	{
		const std::string ProfilesAdmin::PARAMETER_SEARCH_NAME = "pasn";
		const std::string ProfilesAdmin::PARAMETER_SEARCH_RIGHT = "pasr";

		void ProfilesAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// Profile name
			_searchName = map.getDefault<string>(PARAMETER_SEARCH_NAME);

			// Profile right
			_searchRightName = map.getDefault<string>(PARAMETER_SEARCH_RIGHT);

			// Parameters
			_requestParameters.setFromParametersMap(map, PARAMETER_SEARCH_NAME, 30);
		}


		util::ParametersMap ProfilesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_SEARCH_NAME, _searchName);
			m.insert(PARAMETER_SEARCH_RIGHT, _searchRightName);
			return m;
		}


		void ProfilesAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			// Requests
			AdminFunctionRequest<ProfilesAdmin> searchRequest(_request, *this);

			AdminFunctionRequest<ProfileAdmin> profileRequest(_request);

			AdminActionFunctionRequest<RemoveObjectAction, ProfilesAdmin> deleteProfileRequest(_request, *this);

			AdminActionFunctionRequest<AddProfileAction, ProfileAdmin> addProfileRequest(_request);
			addProfileRequest.setActionFailedPage<ProfilesAdmin>();
			addProfileRequest.setActionWillCreateObject();


			SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
			stream << s.open();
			stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, _searchName));
			stream << s.cell(
				"Habilitation",
				s.getForm().getSelectInput(PARAMETER_SEARCH_RIGHT, SecurityModule::getRightLabels(true), optional<string>(_searchRightName)));
			stream << s.close();
			stream << s.getForm().setFocus(PARAMETER_SEARCH_NAME);

			stream << "<h1>Résultats de la recherche</h1>";

			// Search
			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(
					_getEnv(),
					"%"+_searchName+"%"
					, "%"+_searchRightName+"%"
					, _requestParameters.first
					, _requestParameters.maxSize
					, _requestParameters.orderField == PARAMETER_SEARCH_NAME
					, _requestParameters.raisingOrder
			)	);

			bool generalDeleteRight(
				_request.getUser()->getProfile()->isAuthorized<SecurityRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, string())
			);
			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_NAME, string("Nom")));
			v.push_back(make_pair(string(), string("Résumé")));
			v.push_back(make_pair(string(), string("Actions")));
			if(generalDeleteRight)
			{
				v.push_back(make_pair(string(), string("Actions")));
			}
			ActionResultHTMLTable t(
				v,
				searchRequest.getHTMLForm(),
				_requestParameters,
				profiles,
				addProfileRequest.getHTMLForm("add"),
				AddProfileAction::PARAMETER_TEMPLATE_ID
			);

			stream << t.open();

			// Profiles loop
			BOOST_FOREACH(const boost::shared_ptr<Profile>& profile, profiles)
			{
				profileRequest.getPage()->setProfile(profile);
				deleteProfileRequest.getAction()->setObjectId(profile->getKey());

				stream << t.row(lexical_cast<string>(profile->getKey()));
				stream << t.col() << profile->getName();
				stream << t.col();

				HTMLList l;
				stream << l.open();
				if (profile->getParent() != NULL)
				{
					stream << l.element() << "Fils de " << profile->getParent()->getName();
				}
				for(RightsVector::const_iterator it(profile->getRights().begin()); it != profile->getRights().end(); ++it)
				{
					boost::shared_ptr<Right> r(it->second);
					stream << l.element() << "Accès " << Right::getLevelLabel(r->getPublicRightLevel()) << " public et " << Right::getLevelLabel(r->getPrivateRightLevel()) << " privé pour " << r->getName();
					if (r->getParameter() != GLOBAL_PERIMETER)
						stream << "/" << r->displayParameter();
				}
				stream
					<< l.close()
					<< t.col()
					<< profileRequest.getHTMLForm().getLinkButton("Ouvrir", string(), "/admin/img/group_edit.png")
				;
				if(generalDeleteRight)
				{
					stream << t.col();
					if(	_request.getUser()->getProfile()->isAuthorized<SecurityRight>(
							DELETE_RIGHT,
							UNKNOWN_RIGHT_LEVEL,
							lexical_cast<string>(profile->getKey())
					)	){
						stream << deleteProfileRequest.getHTMLForm().getLinkButton("Supprimer", "Etes-vous sûr de vouloir supprimer le profil " + profile->getName() + " ?", "/admin/img/group_delete.png");
			}	}	}

			stream << t.row(string());
			stream << t.col() << t.getActionForm().getTextInput(AddProfileAction::PARAMETER_NAME, "", "Entrez le nom du profil ici");
			stream << t.col() << "(sélectionner un profil existant duquel héritera le nouveau profil)";
			stream << t.col(2) << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.close();
		}

		bool ProfilesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<SecurityRight>(READ, UNKNOWN_RIGHT_LEVEL, string());
		}



		AdminInterfaceElement::PageLinks ProfilesAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const SecurityModule*>(&module) &&
				request.getUser() &&
				request.getUser()->getProfile() &&
				isAuthorized(*request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}
			return links;
		}


		AdminInterfaceElement::PageLinks ProfilesAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const	{
			AdminInterfaceElement::PageLinks links;

			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(
					_getEnv(),
					0
			)	);
			BOOST_FOREACH(const boost::shared_ptr<Profile>& profile, profiles)
			{
				boost::shared_ptr<ProfileAdmin> p(getNewPage<ProfileAdmin>());
				p->setProfile(profile);
				links.push_back(p);
			}

			return links;
		}
	}
}
