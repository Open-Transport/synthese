
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

#include "01_util/Conversion.h"
#include "01_util/Constants.h"

#include "05_html/ActionResultHTMLTable.h"
#include "05_html/SearchFormHTMLTable.h"

#include "11_interfaces/InterfaceModule.h"

#include "12_security/SecurityModule.h"
#include "12_security/ProfilesAdmin.h"
#include "12_security/ProfileAdmin.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/AddProfileAction.h"
#include "12_security/DeleteProfileAction.h"
#include "12_security/Right.h"
#include "12_security/SecurityRight.h"
#include "12_security/Constants.h"
#include "12_security/UsersAdmin.h"

#include "30_server/ActionFunctionRequest.h"

#include "32_admin/AdminModule.h"

using namespace std;
using boost::shared_ptr;

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
		template<> const AdminInterfaceElement::DisplayMode AdminInterfaceElementTemplate<ProfilesAdmin>::DISPLAY_MODE(AdminInterfaceElement::EVER_DISPLAYED);
		template<> string AdminInterfaceElementTemplate<ProfilesAdmin>::getSuperior()
		{
			return UsersAdmin::FACTORY_KEY;
		}
	}

	namespace security
	{
		const std::string ProfilesAdmin::PARAMETER_SEARCH_NAME = "pasn";
		const std::string ProfilesAdmin::PARAMETER_SEARCH_RIGHT = "pasr";
		
		void ProfilesAdmin::setFromParametersMap(const ParametersMap& map)
		{
			string name;
			string right;

			ParametersMap::const_iterator it;
			
			// Profile name
			it = map.find(PARAMETER_SEARCH_NAME);
			if (it != map.end())
			{
				name = it->second;
			}

			// Profile right
			it = map.find(PARAMETER_SEARCH_RIGHT);
			if (it != map.end())
			{
				right = it->second;
			}

			// Parameters
			_requestParameters = ActionResultHTMLTable::getParameters(map, PARAMETER_SEARCH_NAME, 30);

			_searchResult = ProfileTableSync::search(
				name
				, right
				, _requestParameters.first
				, _requestParameters.maxSize
				, _requestParameters.orderField == PARAMETER_SEARCH_NAME
				, _requestParameters.raisingOrder
			);

			_resultParameters = ActionResultHTMLTable::getParameters(_requestParameters, _searchResult);
		}

		string ProfilesAdmin::getTitle() const
		{
			return "Profils";
		}

		void ProfilesAdmin::display(ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request) const
		{
			FunctionRequest<AdminRequest> searchRequest(request);
			searchRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<ProfilesAdmin>());

			FunctionRequest<AdminRequest> profileRequest(request);
			profileRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());

			ActionFunctionRequest<DeleteProfileAction, AdminRequest> deleteProfileRequest(request);
			deleteProfileRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<ProfilesAdmin>());
			
			ActionFunctionRequest<AddProfileAction, AdminRequest> addProfileRequest(request);
			addProfileRequest.getFunction()->setPage(Factory<AdminInterfaceElement>::create<ProfileAdmin>());
			addProfileRequest.getFunction()->setActionFailedPage(Factory<AdminInterfaceElement>::create<ProfilesAdmin>());
			
			SearchFormHTMLTable s(searchRequest.getHTMLForm("search"));
			stream << s.open();
			stream << s.cell("Nom", s.getForm().getTextInput(PARAMETER_SEARCH_NAME, ""));
			stream << s.cell("Habilitation", s.getForm().getSelectInput(PARAMETER_SEARCH_RIGHT, SecurityModule::getRightLabels(true), string()));
			stream << s.close();
			stream << s.getForm().setFocus(PARAMETER_SEARCH_NAME);
				
			stream << "<h1>Résultats de la recherche</h1>";

			ActionResultHTMLTable::HeaderVector v;
			v.push_back(make_pair(PARAMETER_SEARCH_NAME, string("Nom")));
			v.push_back(make_pair(string(), string("Résumé")));
			v.push_back(make_pair(string(), string("Actions")));
			ActionResultHTMLTable t(v, s.getForm(), _requestParameters, _resultParameters, addProfileRequest.getHTMLForm("add"), AddProfileAction::PARAMETER_TEMPLATE_ID, InterfaceModule::getVariableFromMap(variables, AdminModule::ICON_PATH_INTERFACE_VARIABLE));
			t.getActionForm().addHiddenField(AddProfileAction::PARAMETER_TEMPLATE_ID, Conversion::ToString(UNKNOWN_VALUE));

			stream << t.open();
			
			// Profiles loop
			for (vector<shared_ptr<Profile> >::const_iterator it = _searchResult.begin(); it != _searchResult.end(); ++it)
			{
				shared_ptr<Profile> profile = *it;

				profileRequest.setObjectId(profile->getKey());
				deleteProfileRequest.setObjectId(profile->getKey());

				stream << t.row(Conversion::ToString(profile->getKey()));
				stream << t.col() << profile->getName();
				stream << t.col() << "<ul>";

				if (profile->getParentId())
					stream << "<li>Hérite de " << SecurityModule::getProfiles().get(profile->getParentId())->getName() << "</li>";
				for (Profile::RightsVector::const_iterator it = profile->getRights().begin(); it != profile->getRights().end(); ++it)
				{
					shared_ptr<const Right> r = it->second;
					stream << "<li>Accès " << Right::getLevelLabel(r->getPublicRightLevel()) << " public et " << Right::getLevelLabel(r->getPrivateRightLevel()) << " privé pour " << r->getName();
					if (r->getParameter() != GLOBAL_PERIMETER)
						stream << "/" << r->displayParameter();
					stream << "</li>";
				}

				stream
					<< t.col()
					<< profileRequest.getHTMLForm().getLinkButton("Modifier")
					<< "&nbsp;"
					<< deleteProfileRequest.getHTMLForm().getLinkButton("Supprimer", "Etes-vous sûr de vouloir supprimer le profil " + profile->getName() + " ?");
			}

			stream << t.row();
			stream << t.col() << t.getActionForm().getTextInput(AddProfileAction::PARAMETER_NAME, "", "Entrez le nom du profil ici");
			stream << t.col() << "(sélectionner un profil existant duquel héritera le nouveau profil)";
			stream << t.col() << t.getActionForm().getSubmitButton("Ajouter");
			stream << t.close();
		}

		bool ProfilesAdmin::isAuthorized( const server::FunctionRequest<admin::AdminRequest>* request ) const
		{
			return request->isAuthorized<SecurityRight>(Right::READ);
		}

		ProfilesAdmin::ProfilesAdmin()
			: AdminInterfaceElementTemplate<ProfilesAdmin>()
		{

		}
	}
}
