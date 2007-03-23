
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

#include "01_util/Html.h"
#include "01_util/HtmlTable.h"

#include "12_security/Profile.h"
#include "12_security/SecurityModule.h"
#include "12_security/ProfileAdmin.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/UpdateProfileAction.h"
#include "12_security/UpdateRightAction.h"
#include "12_security/Right.h"
#include "12_security/AddRightAction.h"
#include "12_security/DeleteRightAction.h"

#include "32_admin/AdminParametersException.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace admin;
	using namespace db;
	using namespace util;

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

		void ProfileAdmin::display(std::ostream& stream, interfaces::VariablesMap& variables, const AdminRequest* request) const
		{
			AdminRequest* updateRequest = Factory<Request>::create<AdminRequest>();
			updateRequest->copy(request);
			updateRequest->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			updateRequest->setObjectId(_profile->getKey());
			updateRequest->setAction(Factory<Action>::create<UpdateProfileAction>());

			AdminRequest* updateRightRequest = Factory<Request>::create<AdminRequest>();
			updateRightRequest->copy(request);
			updateRightRequest->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			updateRightRequest->setObjectId(_profile->getKey());
			updateRightRequest->setAction(Factory<Action>::create<UpdateRightAction>());

			AdminRequest* deleteRightRequest = Factory<Request>::create<AdminRequest>();
			deleteRightRequest->copy(request);
			deleteRightRequest->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			deleteRightRequest->setObjectId(_profile->getKey());
			deleteRightRequest->setAction(Factory<Action>::create<DeleteRightAction>());

			AdminRequest* addRightRequest = Factory<Request>::create<AdminRequest>();
			addRightRequest->copy(request);
			addRightRequest->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			addRightRequest->setObjectId(_profile->getKey());
			addRightRequest->setAction(Factory<Action>::create<AddRightAction>());

			map<int, std::string> privatePublicMap;
			privatePublicMap.insert(make_pair((int) Right::FORBIDDEN, "Interdit"));
			privatePublicMap.insert(make_pair((int) Right::USE, "Utilisation"));
			privatePublicMap.insert(make_pair((int) Right::READ, "Lecture"));
			privatePublicMap.insert(make_pair((int) Right::WRITE, "Ecriture"));
			privatePublicMap.insert(make_pair((int) Right::DELETE, "Contrôle total"));
			


			stream	// UpdateProfile
				<< "<h1>Propriétés</h1>"
				<< updateRequest->getHTMLFormHeader("update")
				<< "<table>"
				<< "<tr><td>Nom</td><td>" << Html::getTextInput(UpdateProfileAction::PARAMETER_NAME, _profile->getName()) << "</td>"
				<< "<td>" << Html::getSubmitButton("Modifier") << "</td></tr>"
				<< "</table></form>"

				<< "<h1>Habilitations du profil</h1>";

			if (_profile->getRights().empty())
				stream << "Aucune habilitation";
			else
			{
				vector<string> v;
				v.push_back("Nature");
				v.push_back("Périmètre");
				v.push_back("Droits");
				v.push_back("Actions");
				HtmlTable t(v);
				stream << t.open();

				// Habilitations list
				for (Profile::RightsVector::const_iterator it = _profile->getRights().begin(); it != _profile->getRights().end(); ++it)
				{
					Right* right = it->second;
					stream << t.row();
					stream << t.col() << right->getFactoryKey();
					stream << t.col() << right->displayParameter();
					stream << t.col()
						<< updateRightRequest->getHTMLFormHeader("u" + right->getFactoryKey())
						<< "Public : " << Html::getSelectInput(AddRightAction::PARAMETER_PUBLIC_LEVEL, privatePublicMap, (int) right->getPublicRightLevel())
						<< " Privé : " << Html::getSelectInput(AddRightAction::PARAMETER_PRIVATE_LEVEL, privatePublicMap, (int) right->getPrivateRightLevel())
						<< Html::getHiddenInput(UpdateRightAction::PARAMETER_RIGHT, right->getFactoryKey())
						<< Html::getSubmitButton("Modifier")
						<< "</form>";
					stream << t.col()
						<< deleteRightRequest->getHTMLFormHeader("d" + right->getFactoryKey())
						<< Html::getHiddenInput(UpdateRightAction::PARAMETER_RIGHT, right->getFactoryKey())
						<< Html::getSubmitButton("Supprimer")
						<< "</form>";
				}
				stream << t.close();
			}

			stream << "<h1>Ajout d'habilitation au profil</h1>";

			vector<string> v;
			v.push_back("Nature");
			v.push_back("Périmètre");
			v.push_back("Droit public");
			v.push_back("Droit privé");
			v.push_back("Action");
			HtmlTable t(v);

			stream << t.open();

			for (Factory<Right>::Iterator it = Factory<Right>::begin(); it != Factory<Right>::end(); ++it)
			{
				stream
					<< "<tr>"
					<< addRightRequest->getHTMLFormHeader("add" + it.getKey())
					<< "<td>" << it.getKey() << "</td>"
					<< "<td>" << Html::getSelectInput(AddRightAction::PARAMETER_PARAMETER, it->getParametersLabels(), string("")) << "</td>"
					<< "<td>" << Html::getSelectInput(AddRightAction::PARAMETER_PUBLIC_LEVEL, privatePublicMap, (int) Right::Level(Right::USE)) << "</td>"
					<< "<td>" << Html::getSelectInput(AddRightAction::PARAMETER_PRIVATE_LEVEL, privatePublicMap, (int) Right::Level(Right::USE)) << "</td>"
					<< "<td>" 
					<< Html::getHiddenInput(AddRightAction::PARAMETER_RIGHT, it.getKey())
					<< Html::getSubmitButton("Ajouter") << "</td>"
					<< "</form></tr>";
			}
			stream << "</table>";

			delete updateRightRequest;
			delete updateRequest;
			delete addRightRequest;
			delete deleteRightRequest;
		}

		void ProfileAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			try
			{
				Request::ParametersMap::const_iterator it = map.find(AdminRequest::PARAMETER_OBJECT_ID);
				if (it == map.end())
					throw AdminParametersException("Profile not specified");
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
