
/** ProfileAdmin class implementation.
	@file ProfileAdmin.cpp

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

#include "05_html/HTMLTable.h"
#include "05_html/HTMLForm.h"

#include "12_security/Profile.h"
#include "12_security/ProfileAdmin.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/UpdateProfileAction.h"
#include "12_security/UpdateRightAction.h"
#include "12_security/Right.h"
#include "12_security/AddRightAction.h"
#include "12_security/DeleteRightAction.h"
#include "12_security/Constants.h"
#include "12_security/ProfilesAdmin.h"

#include "30_server/ActionFunctionRequest.h"
#include "30_server/QueryString.h"

#include "32_admin/AdminParametersException.h"
#include "32_admin/AdminRequest.h"


using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace db;
	using namespace util;
	using namespace html;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ProfileAdmin>::FACTORY_KEY("profile");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ProfileAdmin>::ICON("group.png");
		template<> const string AdminInterfaceElementTemplate<ProfileAdmin>::DEFAULT_TITLE("Profil inconnu");
	}

	namespace security
	{
		const string ProfileAdmin::PARAM_PROFILE_ID = "roid";

		ProfileAdmin::ProfileAdmin()
			: AdminInterfaceElementTemplate<ProfileAdmin>()
			, _profileError(false)
		{

		}


		void ProfileAdmin::display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<UpdateProfileAction, AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage<ProfileAdmin>();
			updateRequest.setObjectId(_profile->getKey());

			ActionFunctionRequest<UpdateRightAction, AdminRequest> updateRightRequest(request);
			updateRightRequest.getFunction()->setPage<ProfileAdmin>();
			updateRightRequest.setObjectId(_profile->getKey());
			
			ActionFunctionRequest<DeleteRightAction,AdminRequest> deleteRightRequest(request);
			deleteRightRequest.getFunction()->setPage<ProfileAdmin>();
			deleteRightRequest.setObjectId(_profile->getKey());
			
			ActionFunctionRequest<AddRightAction,AdminRequest> addRightRequest(request);
			addRightRequest.getFunction()->setPage<ProfileAdmin>();
			addRightRequest.setObjectId(_profile->getKey());
			
			vector<pair<int, string> > privatePublicMap;
			privatePublicMap.push_back(make_pair((int) FORBIDDEN, "Interdit"));
			privatePublicMap.push_back(make_pair((int) USE, "Utilisation"));
			privatePublicMap.push_back(make_pair((int) READ, "Lecture"));
			privatePublicMap.push_back(make_pair((int) WRITE, "Ecriture"));
			privatePublicMap.push_back(make_pair((int) DELETE_RIGHT, "Contrôle total"));
			

			stream	<< "<h1>Propriétés</h1>";

			HTMLForm form(updateRequest.getHTMLForm("update"));
			stream << form.open();

			{
				HTMLTable t;
				stream << t.open();
				stream << t.row();
				stream << t.col() << "Nom";
				stream << t.col() << form.getTextInput(UpdateProfileAction::PARAMETER_NAME, _profile->getName());
				stream << t.col() << form.getSubmitButton("Modifier");
				stream << t.close();
				stream << form.close();
			}
	
			stream << "<h1>Habilitations du profil</h1>";

			if (_profile->getRights().empty())
				stream << "Aucune habilitation";
			else
			{
				HTMLTable::ColsVector v;
				v.push_back("Nature");
				v.push_back("Périmètre");
				v.push_back("Droits");
				v.push_back("Actions");
				HTMLTable t(v);
				stream << t.open();

				// Habilitations list
				for (RightsVector::const_iterator it = _profile->getRights().begin(); it != _profile->getRights().end(); ++it)
				{
					shared_ptr<const Right> right = it->second;
					stream << t.row();
					stream << t.col() << right->getName();
					stream << t.col() << right->displayParameter();
					stream << t.col();
					{
						HTMLForm form(updateRightRequest.getHTMLForm("u" + right->getFactoryKey() + right->getParameter()));
						form.addHiddenField(UpdateRightAction::PARAMETER_RIGHT_CODE, right->getFactoryKey());
						form.addHiddenField(UpdateRightAction::PARAMETER_RIGHT_PARAMETER, right->getParameter());
						stream << form.open();
						stream << "Public : " << form.getSelectInput(UpdateRightAction::PARAMETER_PUBLIC_VALUE, privatePublicMap, (int) right->getPublicRightLevel());
						if (right->getUsePrivateRights())
							stream << " Privé : " << form.getSelectInput(UpdateRightAction::PARAMETER_PRIVATE_VALUE, privatePublicMap, (int) right->getPrivateRightLevel());
						stream << form.getSubmitButton("Modifier");
						stream << form.close();
					}
					stream << t.col();
					{
						HTMLForm form(deleteRightRequest.getHTMLForm("d" + right->getFactoryKey() + right->getParameter()));
						form.addHiddenField(DeleteRightAction::PARAMETER_RIGHT, right->getFactoryKey());
						form.addHiddenField(DeleteRightAction::PARAMETER_PARAMETER, right->getParameter());
						stream << form.getLinkButton("Supprimer","Etes-vous sûr(e) de vouloir supprimer l\\'habilitation sélectionnée ?");
					}
				}
				stream << t.close();
			}

			stream << "<h1>Ajout d'habilitation au profil</h1>";

			HTMLTable::ColsVector v;
			v.push_back("Nature");
			v.push_back("Périmètre");
			v.push_back("Droit public");
			v.push_back("Droit privé");
			v.push_back("Action");
			HTMLTable t(v);

			stream << t.open();

			for (Factory<Right>::Iterator it = Factory<Right>::begin(); it != Factory<Right>::end(); ++it)
			{
				ParameterLabelsVector pl = it->getParametersLabels();
				HTMLForm form(addRightRequest.getHTMLForm("add" + it.getKey()));
				form.addHiddenField(AddRightAction::PARAMETER_RIGHT, it.getKey());
				stream
					<< "<tr>"
					<< form.open()
					<< "<td>" << it->getName() << "</td>"
					<< "<td>";
				if (pl.size() == 1)
				{
					stream << pl.at(0).second;
					form.addHiddenField(AddRightAction::PARAMETER_PARAMETER, pl.at(0).first);
				}
				else
					stream << form.getSelectInput(AddRightAction::PARAMETER_PARAMETER, pl, GLOBAL_PERIMETER);
				stream
					<< "</td>"
					<< "<td>" << form.getSelectInput(AddRightAction::PARAMETER_PUBLIC_LEVEL, privatePublicMap, (int) USE) << "</td>"
					<< "<td>";
				if ((*it)->getUsePrivateRights())
					stream << form.getSelectInput(AddRightAction::PARAMETER_PRIVATE_LEVEL, privatePublicMap, (int) USE);
				stream
					<< "</td>"
					<< "<td>" 
					<< form.getSubmitButton("Ajouter") << "</td>"
					<< form.close()
					<< "</tr>";
			}
			stream << "</table>";
		}

		void ProfileAdmin::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				uid id = map.getUid(QueryString::PARAMETER_OBJECT_ID, false, FACTORY_KEY);
				if (id != UNKNOWN_VALUE && id != QueryString::UID_WILL_BE_GENERATED_BY_THE_ACTION)
				{
					_profile = ProfileTableSync::Get(id);
					_pageLink.name = _profile->getName();
				}
			}
			catch (Profile::ObjectNotFoundException& e)
			{
				throw AdminParametersException("Bad profile" + e.getMessage());
			}
		}

		bool ProfileAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return true;
		}

		AdminInterfaceElement::PageLinks ProfileAdmin::getSubPagesOfParent( const PageLink& parentLink , const AdminInterfaceElement& currentPage 		, const server::FunctionRequest<admin::AdminRequest>* request
			) const
		{
			AdminInterfaceElement::PageLinks links;
			if (parentLink.factoryKey == ProfilesAdmin::FACTORY_KEY && currentPage.getFactoryKey() == FACTORY_KEY)
			{
				links.push_back(currentPage.getPageLink());
			}
			return links;
		}
	}
}
