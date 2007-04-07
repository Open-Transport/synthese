
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
#include "12_security/SecurityModule.h"
#include "12_security/ProfileAdmin.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/UpdateProfileAction.h"
#include "12_security/UpdateRightAction.h"
#include "12_security/Right.h"
#include "12_security/AddRightAction.h"
#include "12_security/DeleteRightAction.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace db;
	using namespace util;
	using namespace html;

	namespace security
	{
		const string ProfileAdmin::PARAM_PROFILE_ID = "roid";

		ProfileAdmin::ProfileAdmin()
			: AdminInterfaceElement("profiles", AdminInterfaceElement::DISPLAYED_IF_CURRENT)
			, _profile(NULL), _profileError(false)
		{

		}

		std::string ProfileAdmin::getTitle() const
		{
			return (_profile != NULL)
				? _profile->getName()
				: "";
		}

		void ProfileAdmin::display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			ActionFunctionRequest<UpdateProfileAction, AdminRequest> updateRequest(request);
			updateRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			updateRequest.setObjectId(_profile->getKey());

			ActionFunctionRequest<UpdateRightAction, AdminRequest> updateRightRequest(request);
			updateRightRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			updateRightRequest.setObjectId(_profile->getKey());
			
			ActionFunctionRequest<DeleteRightAction,AdminRequest> deleteRightRequest(request);
			deleteRightRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			deleteRightRequest.setObjectId(_profile->getKey());
			
			ActionFunctionRequest<AddRightAction,AdminRequest> addRightRequest(request);
			addRightRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			addRightRequest.setObjectId(_profile->getKey());
			
			map<int, std::string> privatePublicMap;
			privatePublicMap.insert(make_pair((int) Right::FORBIDDEN, "Interdit"));
			privatePublicMap.insert(make_pair((int) Right::USE, "Utilisation"));
			privatePublicMap.insert(make_pair((int) Right::READ, "Lecture"));
			privatePublicMap.insert(make_pair((int) Right::WRITE, "Ecriture"));
			privatePublicMap.insert(make_pair((int) Right::DELETE, "Contrôle total"));
			

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
				for (Profile::RightsVector::const_iterator it = _profile->getRights().begin(); it != _profile->getRights().end(); ++it)
				{
					Right* right = it->second;
					stream << t.row();
					stream << t.col() << right->getFactoryKey();
					stream << t.col() << right->displayParameter();
					stream << t.col();
					{
						HTMLForm form(updateRightRequest.getHTMLForm("u" + right->getFactoryKey() + right->getParameter()));
						form.addHiddenField(UpdateRightAction::PARAMETER_RIGHT_CODE, right->getFactoryKey());
						form.addHiddenField(UpdateRightAction::PARAMETER_RIGHT_PARAMETER, right->getParameter());
						stream << form.open();
						stream << "Public : " << form.getSelectInput(UpdateRightAction::PARAMETER_PUBLIC_VALUE, privatePublicMap, (int) right->getPublicRightLevel());
						stream << " Privé : " << form.getSelectInput(UpdateRightAction::PARAMETER_PRIVATE_VALUE, privatePublicMap, (int) right->getPrivateRightLevel());
						stream << form.getSubmitButton("Modifier");
						stream << form.close();
					}
					stream << t.col();
					{
						HTMLForm form(deleteRightRequest.getHTMLForm("d" + right->getFactoryKey() + right->getParameter()));
						form.addHiddenField(DeleteRightAction::PARAMETER_RIGHT, right->getFactoryKey());
						form.addHiddenField(DeleteRightAction::PARAMETER_PARAMETER, right->getParameter());
						stream << form.getSubmitButton("Supprimer");
						stream << form.close();
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
				HTMLForm form(addRightRequest.getHTMLForm("add" + it.getKey()));
				form.addHiddenField(AddRightAction::PARAMETER_RIGHT, it.getKey());
				stream
					<< "<tr>"
					<< form.open()
					<< "<td>" << it.getKey() << "</td>"
					<< "<td>" << form.getSelectInput(AddRightAction::PARAMETER_PARAMETER, it->getParametersLabels(), string("")) << "</td>"
					<< "<td>" << form.getSelectInput(AddRightAction::PARAMETER_PUBLIC_LEVEL, privatePublicMap, (int) Right::Level(Right::USE)) << "</td>"
					<< "<td>" << form.getSelectInput(AddRightAction::PARAMETER_PRIVATE_LEVEL, privatePublicMap, (int) Right::Level(Right::USE)) << "</td>"
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
				ParametersMap::const_iterator it = map.find(Request::PARAMETER_OBJECT_ID);
				if (it != map.end())
					_profile = SecurityModule::getProfiles().get(Conversion::ToLongLong(it->second));
			}
			catch (Profile::RegistryKeyException e)
			{
				throw AdminParametersException("Bad profile");
			}
		}

		ProfileAdmin::~ProfileAdmin()
		{
			delete _profile;
		}
	}
}
