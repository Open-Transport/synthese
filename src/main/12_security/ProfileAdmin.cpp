
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

#include "02_db/DBEmptyResultException.h"

#include "12_security/Profile.h"
#include "12_security/ProfileAdmin.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/UpdateProfileAction.h"
#include "12_security/UpdateRightAction.h"
#include "12_security/Right.h"
#include "12_security/AddRightAction.h"
#include "12_security/DeleteRightAction.h"

#include "32_admin/AdminRequest.h"
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

		void ProfileAdmin::display(std::ostream& stream, const Request* request) const
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


			stream	// UpdateProfile
				<< "<h1>Propriétés</h1>"
				<< updateRequest->getHTMLFormHeader("update")
				<< "<table>"
				<< "<tr><td>Nom</td><td>" << Html::getTextInput(UpdateProfileAction::PARAMETER_NAME, _profile->getName()) << "</td>"
				<< "<td>" << Html::getSubmitButton("Modifier") << "</td></tr>"
				<< "</table></form>"

				<< "<h1>Habilitations du profil&nbsp;:</h1>"
				<< "<table>"
				<< "<tr><th>Nature</th><th>Périmètre</th><th>Droit public</th><th>Droit privé</th><th colspan=\"2\">Actions</th></tr>";

			// Habilitations list
			for (Profile::RightsVector::const_iterator it = _profile->getRights().begin(); it != _profile->getRights().end(); ++it)
			{
				Right* right = *it;
				stream
					<< "<tr>"
					<< updateRightRequest->getHTMLFormHeader("u" + right->getFactoryKey())
					<< "<td>" << right->getFactoryKey() << "</td>"
					<< "<td>" << right->displayParameter() << "</td>"
					<< "<td>" << Right::getLevelLabel(right->getPublicRightLevel()) << "</td>"
					<< "<td>" << Right::getLevelLabel(right->getPrivateRightLevel()) << "</td>"
					<< "<td>"
					<< Html::getHiddenInput(UpdateRightAction::PARAMETER_RIGHT, right->getFactoryKey())
					<< Html::getSubmitButton("Modifier")
					<< "</td></form>"
					<< "<td>" 
					<< deleteRightRequest->getHTMLFormHeader("d" + right->getFactoryKey())
					<< Html::getHiddenInput(UpdateRightAction::PARAMETER_RIGHT, right->getFactoryKey())
					<< Html::getSubmitButton("Supprimer")
					<< "</form></td>"
					<< "</tr>";
			}
			stream
				<< "<tr>"
				<< addRightRequest->getHTMLFormHeader("add")
				<< "<td>" << "</td>"	// Rights list
				<< "<OPTION value=\"\" selected>Tableaux de départs</OPTION>"
				<< "<OPTION value=\"\">Environnement</OPTION>"
				<< "</SELECT></FONT></TD>"
				<< "<TD style=\"WIDTH: 82px\"><SELECT id=\"Select4\" name=\"Select4\">";

			// numéro lignes

			stream
				<< "</SELECT></TD>"

				<< "<TD style=\"WIDTH: 56px\"><SELECT id=\"Select5\" name=\"Select2\">"
				<< "<OPTION value=\"\" selected>Public</OPTION>"
				<< "<OPTION value=\"\">Privé</OPTION>"
				<< "</SELECT></TD>"
				<< "<TD style=\"WIDTH: 94px\"><SELECT id=\"Select3\" name=\"Select2\">"
				<< "<OPTION value=\"\" selected>Utilisation</OPTION>"
				<< "<OPTION value=\"\">Lecture</OPTION>"
				<< "<OPTION value=\"\">Modification</OPTION>"
				<< "<OPTION value=\"\">Suppression</OPTION>"
				<< "</SELECT></TD>"
				<< "<TD><INPUT id=\"Button7\" type=\"button\" value=\"Ajouter\" name=\"Button7\"></TD>"
				<< "</TR></TABLE>"
				;
		}

		void ProfileAdmin::setFromParametersMap(const server::Request::ParametersMap& map)
		{
			try
			{
				Request::ParametersMap::const_iterator it = map.find(AdminRequest::PARAMETER_OBJECT_ID);
				if (it != map.end())
					_profile = ProfileTableSync::get(Conversion::ToLongLong(it->second));
			}
			catch (DBEmptyResultException& e)
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
