
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

#include <map>
#include <string>

#include "01_util/Html.h"
#include "01_util/Conversion.h"
#include "01_util/Constants.h"

#include "12_security/SecurityModule.h"
#include "12_security/ProfilesAdmin.h"
#include "12_security/ProfileAdmin.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/AddProfileAction.h"
#include "12_security/DeleteProfileAction.h"

#include "32_admin/ResultHTMLTable.h"
#include "32_admin/SearchFormHTMLTable.h"

using namespace std;

namespace synthese
{
	using namespace admin;
	using namespace interfaces;
	using namespace server;
	using namespace util;

	namespace security
	{
		const std::string ProfilesAdmin::PARAMETER_SEARCH_NAME = "pasn";
		const std::string ProfilesAdmin::PARAMETER_SEARCH_RIGHT = "pasr";
		const std::string ProfilesAdmin::PARAMETER_SEARCH_FIRST = "pasf";

		ProfilesAdmin::ProfilesAdmin()
			: AdminInterfaceElement("users", AdminInterfaceElement::EVER_DISPLAYED) {}

		void ProfilesAdmin::setFromParametersMap(const AdminRequest::ParametersMap& map)
		{
			string name;
			string right;
			int first = 0;

			Request::ParametersMap::const_iterator it = map.find(PARAMETER_SEARCH_NAME);
			if (it != map.end())
			{
				name = it->second;
			}

			it = map.find(PARAMETER_SEARCH_RIGHT);
			if (it != map.end())
			{
				right = it->second;
			}

			it = map.find(PARAMETER_SEARCH_FIRST);
			if (it != map.end())
			{
				first = Conversion::ToInt(it->second);
			}

			_searchResult = ProfileTableSync::search(name, right, first, 50);
		}

		string ProfilesAdmin::getTitle() const
		{
			return "Profils";
		}

		void ProfilesAdmin::display(ostream& stream, const AdminRequest* request) const
		{
			AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<ProfilesAdmin>());

			AdminRequest* profileRequest = Factory<Request>::create<AdminRequest>();
			profileRequest->copy(request);
			profileRequest->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());

			AdminRequest* deleteProfileRequest = Factory<Request>::create<AdminRequest>();
			deleteProfileRequest->copy(request);
			deleteProfileRequest->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			deleteProfileRequest->setAction(Factory<Action>::create<DeleteProfileAction>());

			AdminRequest* addProfileRequest = Factory<Request>::create<AdminRequest>();
			addProfileRequest->copy(request);
			addProfileRequest->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			addProfileRequest->setActionFailedPage(Factory<AdminInterfaceElement>::create<ProfilesAdmin>());
			addProfileRequest->setAction(Factory<Action>::create<AddProfileAction>());

			SearchFormHTMLTable s(searchRequest);
			stream << s.open();
			stream << s.cell("Nom", Html::getTextInput(PARAMETER_SEARCH_NAME, ""));
			stream << s.cell("Habilitation", Html::getSelectInput(PARAMETER_SEARCH_RIGHT, SecurityModule::getRightLabels(true), string()));
			stream << s.close();
			stream << Html::setFocus("search", PARAMETER_SEARCH_NAME);
				
			stream << "<h1>R�sultats de la recherche</h1>";

			ResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_NAME, string("Nom")));
			v.push_back(make_pair(string(), string("R�sum�")));
			v.push_back(make_pair(string(), string("Actions")));
			ResultHTMLTable t(v, searchRequest, "", true, addProfileRequest, AddProfileAction::PARAMETER_TEMPLATE_ID);

			stream << t.open();
			
			// Profiles loop
			for (vector<Profile*>::const_iterator it = _searchResult.begin(); it != _searchResult.end(); ++it)
			{
				Profile* profile = *it;

				profileRequest->setObjectId(profile->getKey());
				deleteProfileRequest->setObjectId(profile->getKey());

				stream << t.row(Conversion::ToString(profile->getKey()));
				stream << t.col() << profile->getName();
				stream << t.col() << "<ul>";

				if (profile->getParentId())
					stream << "<li>H�rite de " << SecurityModule::getProfiles().get(profile->getParentId())->getName() << "</li>";

				// Rights loop

				stream
					<< t.col()
					<< Html::getLinkButton(profileRequest->getURL(), "Modifier")
					<< "&nbsp;"
					<< Html::getLinkButton(deleteProfileRequest->getURL(), "Supprimer");
			}

			stream << t.row();
			stream << t.col() << Html::getTextInput(AddProfileAction::PARAMETER_NAME, "", "Entrez le nom du profil ici");
			stream << t.col() << "(s�lectionner un profil existant pour copier ses habilitations dans le nouveau profil)";
			stream << t.col()
				<< Html::getHiddenInput(AddProfileAction::PARAMETER_TEMPLATE_ID, Conversion::ToString(UNKNOWN_VALUE))
				<< Html::getSubmitButton("Ajouter");
			stream << t.close();

			delete profileRequest;
			delete searchRequest;
			delete addProfileRequest;
			delete deleteProfileRequest;
		}
	}
}
