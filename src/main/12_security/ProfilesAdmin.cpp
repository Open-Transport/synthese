
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

#include "12_security/SecurityModule.h"
#include "12_security/ProfilesAdmin.h"
#include "12_security/ProfileTableSync.h"

#include "32_admin/AdminRequest.h"

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

		void ProfilesAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
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

		void ProfilesAdmin::display(ostream& stream, const Request* request) const
		{
			AdminRequest* searchRequest = Factory<Request>::create<AdminRequest>();
			searchRequest->copy(request);
			searchRequest->setPage(Factory<AdminInterfaceElement>::create<ProfilesAdmin>());

			map<string, string>& rightsMap = SecurityModule::getRightsTemplates();
			rightsMap.insert(make_pair("", "(toutes les habilitations)"));

			stream
				<< searchRequest->getHTMLFormHeader("search")
				<< "<table id=\"searchform\"><tr>"
				<< "<td>Nom</td><td>" << Html::getTextInput(PARAMETER_SEARCH_NAME, "") << "</td>"
				<< "<td>Habilitation</td>"
				<< "<td>" << Html::getSelectInput(PARAMETER_SEARCH_RIGHT, rightsMap, string()) << "</td>"
				<< "<td>" << Html::getSubmitButton("Rechercher") << "</td></tr>"
				<< "</table></form>"
				<< Html::setFocus("search", PARAMETER_SEARCH_NAME)
				
				<< "<h1>Résultats de la recherche</h1>"
				
				<< "<table id=\"searchresult\">"
				<< "<TR><th>Sel</th><th>Nom</th><th>Résumé</th><th>Actions</th></tr>";
			
			// Profiles loop
			for (vector<Profile*>::const_iterator it = _searchResult.begin(); it != _searchResult.end(); ++it)
			{
				Profile* profile = *it;
				stream
                    << "<tr>"
					<< "<TD><INPUT type=\"checkbox\" name=\"Checkbox3\"></TD>"
					<< "<td>" << profile->getName() << "</td>"
					<< "<td><ul>";

				if (profile->getParentId())
					stream << "<li>Hérite de " << SecurityModule::getProfiles().get(profile->getParentId())->getName() << "</li>";

				// Rights loop

				stream
					<< "</td>"
					<< "<td>" << Html::getHiddenInput("", Conversion::ToString(profile->getKey())) << Html::getSubmitButton("Modifier") << "</td>"
					<< "</tr>";
			}

			stream
				<< "<TR>"
				<< "<TD>&nbsp;</TD>"
				<< "<TD><INPUT type=\"text\" name=\"Text1\" value=\"Entrez le nom du profil ici\" size=\"21\"></TD>"
				<< "<TD>(sélectionner un profil existant pour copier ses habilitations dans le nouveau profil)</P></TD>"
				<< "<TD><INPUT type=\"button\" value=\"Ajouter\" name=\"Button7\"></TD>"
				<< "</TR>"

				<< "</TABLE>"

				<< "<P align=\"right\">Profils suivants &gt;</P>"
				<< "<P>Sélection : <INPUT type=\"button\" value=\"Supprimer\" name=\"Button4\" onclick=\"alert('Etes vous sur(e) de vouloir supprimer le profil sélectionné ?')\"></P>"
				<< "<P>Cliquer sur un titre de colonne pour trier le tableau.</P>";
		}
	}
}
