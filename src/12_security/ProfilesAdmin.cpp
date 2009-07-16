
/** ProfilesAdmin class implementation.
	@file ProfilesAdmin.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "01_util/Constants.h"
#include "ActionResultHTMLTable.h"
#include "SearchFormHTMLTable.h"
#include "HTMLList.h"
#include "InterfaceModule.h"
#include "ProfilesAdmin.h"
#include "ProfileAdmin.h"
#include "ProfileTableSync.h"
#include "AddProfileAction.h"
#include "DeleteProfileAction.h"
#include "Right.h"
#include "SecurityRight.h"
#include "SecurityModule.h"
#include "12_security/Constants.h"
#include "ActionFunctionRequest.h"
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
	using namespace interfaces;
	using namespace server;
	using namespace util;
	using namespace html;
	using namespace security;

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
			const ParametersMap& map,
			bool objectWillBeCreatedLater
		){
			// Profile name
			_searchName = map.getString(PARAMETER_SEARCH_NAME, false, FACTORY_KEY);

			// Profile right
			_searchRightName = map.getString(PARAMETER_SEARCH_RIGHT, false, FACTORY_KEY);

			// Parameters
			_requestParameters.setFromParametersMap(map.getMap(), PARAMETER_SEARCH_NAME, 30);
		}
		
		
		server::ParametersMap ProfilesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());
			m.insert(PARAMETER_SEARCH_NAME, _searchName);
			m.insert(PARAMETER_SEARCH_RIGHT, _searchRightName);
			return m;
		}


		void ProfilesAdmin::display(ostream& stream, interfaces::VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& _request) const
		{
			// Requests
			AdminFunctionRequest<ProfilesAdmin> searchRequest(_request);

			AdminFunctionRequest<ProfileAdmin> profileRequest(_request);

			AdminActionFunctionRequest<DeleteProfileAction, ProfilesAdmin> deleteProfileRequest(_request);
			
			AdminActionFunctionRequest<AddProfileAction, ProfileAdmin> addProfileRequest(_request);
			addProfileRequest.getFunction()->setActionFailedPage<ProfilesAdmin>();
			addProfileRequest.setActionWillCreateObject();

			
			SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
			stream << s.open();
			stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, _searchName));
			stream << s.cell("Habilitation", s.getForm().getSelectInput(PARAMETER_SEARCH_RIGHT, SecurityModule::getRightLabels(true), _searchRightName));
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
				_request.isAuthorized<SecurityRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, string())
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
				AddProfileAction::PARAMETER_TEMPLATE_ID,
				InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE)
			);
			t.getActionForm().addHiddenField(
				AddProfileAction::PARAMETER_TEMPLATE_ID,
				lexical_cast<string>(UNKNOWN_VALUE)
			);

			stream << t.open();
			
			// Profiles loop
			BOOST_FOREACH(shared_ptr<Profile> profile, profiles)
			{
				profileRequest.getPage()->setProfile(profile);
				deleteProfileRequest.getAction()->setProfile(profile);

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
					shared_ptr<Right> r(it->second);
					stream << l.element() << "Accès " << Right::getLevelLabel(r->getPublicRightLevel()) << " public et " << Right::getLevelLabel(r->getPrivateRightLevel()) << " privé pour " << r->getName();
					if (r->getParameter() != GLOBAL_PERIMETER)
						stream << "/" << r->displayParameter();
				}
				stream
					<< l.close()
					<< t.col()
					<< profileRequest.getHTMLForm().getLinkButton("Ouvrir", string(), "group_edit.png")
				;
				if(generalDeleteRight)
				{
					stream << t.col();
					if(_request.isAuthorized<SecurityRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, lexical_cast<string>(profile->getKey())))
					{
						stream << deleteProfileRequest.getHTMLForm().getLinkButton("Supprimer", "Etes-vous sûr de vouloir supprimer le profil " + profile->getName() + " ?", "group_delete.png");
			}	}	}

			stream << t.row();
			stream << t.col() << t.getActionForm().getTextInput(AddProfileAction::PARAMETER_NAME, "", "Entrez le nom du profil ici");
			stream << t.col() << "(sélectionner un profil existant duquel héritera le nouveau profil)";
			stream << t.col(2) << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.close();
		}

		bool ProfilesAdmin::isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const
		{
			return _request.isAuthorized<SecurityRight>(READ, UNKNOWN_RIGHT_LEVEL, string());
		}

		
		
		AdminInterfaceElement::PageLinks ProfilesAdmin::getSubPagesOfModule(
			const string& moduleKey,
			shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			if(moduleKey == SecurityModule::FACTORY_KEY && isAuthorized(request))
			{
				if(dynamic_cast<const ProfilesAdmin*>(currentPage.get()))
				{
					AddToLinks(links, currentPage);
				}
				else
				{
					AddToLinks(links, getNewPage());
				}
			}
			return links;
		}
	
	
		AdminInterfaceElement::PageLinks ProfilesAdmin::getSubPages(
			shared_ptr<const AdminInterfaceElement> currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
		) const	{
			AdminInterfaceElement::PageLinks links;
			
			const ProfileAdmin* pa(
				dynamic_cast<const ProfileAdmin*>(currentPage.get())
			);
			
			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(
					_getEnv(),
					0
			)	);
			BOOST_FOREACH(shared_ptr<Profile> profile, profiles)
			{
				if(	pa &&
					pa->getProfile()->getKey() == profile->getKey()
				){
					AddToLinks(links, currentPage);
				}
				else
				{
					shared_ptr<ProfileAdmin> p(getNewOtherPage<ProfileAdmin>());
					p->setProfile(profile);
					AddToLinks(links, p);
				}
			}
			
			return links;
		}
	}
}
