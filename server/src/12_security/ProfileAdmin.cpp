////////////////////////////////////////////////////////////////////////////////
/// ProfileAdmin class implementation.
///	@file ProfileAdmin.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ProfileAdmin.h"

#include "Profile.h"
#include "PropertiesHTMLTable.h"
#include "User.h"
#include "HTMLTable.h"
#include "HTMLForm.h"
#include "Profile.h"
#include "ProfileTableSync.h"
#include "UpdateProfileAction.h"
#include "UpdateRightAction.h"
#include "Right.h"
#include "AddRightAction.h"
#include "DeleteRightAction.h"
#include "Constants.h"
#include "ProfilesAdmin.h"
#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "SecurityRight.h"
#include "ResaRight.h"

#include <boost/foreach.hpp>

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
	using namespace resa;

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



		void ProfileAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			try
			{
				RegistryKeyType id = map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID);
				_profile = ProfileTableSync::Get(id,_getEnv());
			}
			catch (ObjectNotFoundException<Profile>& e)
			{
				throw AdminParametersException("Bad profile" + e.getMessage());
			}
		}



		util::ParametersMap ProfileAdmin::getParametersMap() const
		{
			ParametersMap m;
			if(_profile.get()) m.insert(Request::PARAMETER_OBJECT_ID, _profile->getKey());
			return m;
		}



		void ProfileAdmin::display(
			std::ostream& stream,
			const server::Request& _request
		) const	{

			AdminActionFunctionRequest<UpdateProfileAction, ProfileAdmin> updateRequest(_request, *this);
			updateRequest.getAction()->setProfile(_profile);

			AdminActionFunctionRequest<UpdateRightAction, ProfileAdmin> updateRightRequest(_request, *this);
			updateRightRequest.getAction()->setProfile(_profile);

			AdminActionFunctionRequest<DeleteRightAction, ProfileAdmin> deleteRightRequest(_request, *this);
			deleteRightRequest.getAction()->setProfile(_profile);

			AdminActionFunctionRequest<AddRightAction, ProfileAdmin> addRightRequest(_request, *this);
			addRightRequest.getAction()->setProfile(_profile);

			vector<pair<optional<int>, string> > privatePublicMap;
			privatePublicMap.push_back(make_pair((int) FORBIDDEN, "Interdit"));
			privatePublicMap.push_back(make_pair((int) USE, "Utilisation"));
			privatePublicMap.push_back(make_pair((int) READ, "Lecture"));
			privatePublicMap.push_back(make_pair((int) WRITE, "Ecriture"));
			privatePublicMap.push_back(make_pair((int) DELETE_RIGHT, "Contrôle total"));

			vector<pair<optional<int>, string> > privatePublicMapResa;
			privatePublicMapResa.push_back(make_pair((int) FORBIDDEN, "Interdit"));
			privatePublicMapResa.push_back(make_pair((int) USE, "Utilisation"));
			privatePublicMapResa.push_back(make_pair((int) READ, "Lecture"));
			privatePublicMapResa.push_back(make_pair((int) CANCEL, "Annulation"));
			privatePublicMapResa.push_back(make_pair((int) WRITE, "Ecriture"));
			privatePublicMapResa.push_back(make_pair((int) DELETE_RIGHT, "Contrôle total"));


			stream	<< "<h1>Propriétés</h1>";

			PropertiesHTMLTable pt(updateRequest.getHTMLForm("update"));
			stream << pt.open();
			stream << pt.cell("Nom", pt.getForm().getTextInput(UpdateProfileAction::PARAMETER_NAME, _profile->getName()));
			stream << pt.close();

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
				HTMLTable t(v, "adminresults");
				stream << t.open();

				// Habilitations list
				for (RightsVector::const_iterator it = _profile->getRights().begin(); it != _profile->getRights().end(); ++it)
				{
					boost::shared_ptr<const Right> right = it->second;
					stream << t.row();
					stream << t.col() << right->getName();
					stream << t.col() << right->displayParameter();
					stream << t.col();
					{
						HTMLForm form(updateRightRequest.getHTMLForm("u" + right->getFactoryKey() + right->getParameter()));
						form.addHiddenField(UpdateRightAction::PARAMETER_RIGHT_CODE, right->getFactoryKey());
						form.addHiddenField(UpdateRightAction::PARAMETER_RIGHT_PARAMETER, right->getParameter());
						stream << form.open();
						if (dynamic_cast<const ResaRight*>(right.get()))
						{
							stream << "Public : " <<
								form.getSelectInput(
									UpdateRightAction::PARAMETER_PUBLIC_VALUE,
									privatePublicMapResa,
									optional<int>((int) right->getPublicRightLevel())
							);
						}
						else
						{
							stream << "Public : " <<
								form.getSelectInput(
									UpdateRightAction::PARAMETER_PUBLIC_VALUE,
									privatePublicMap,
									optional<int>((int) right->getPublicRightLevel())
								);
						}
						if (right->getUsePrivateRights())
							stream << " Privé : " <<
							form.getSelectInput(
								UpdateRightAction::PARAMETER_PRIVATE_VALUE,
								privatePublicMap,
								optional<int>((int) right->getPrivateRightLevel())
							);
						stream << form.getSubmitButton("Modifier");
						stream << form.close();
					}
					stream << t.col();
					{
						HTMLForm form(deleteRightRequest.getHTMLForm("d" + right->getFactoryKey() + right->getParameter()));
						form.addHiddenField(DeleteRightAction::PARAMETER_RIGHT, right->getFactoryKey());
						form.addHiddenField(DeleteRightAction::PARAMETER_PARAMETER, right->getParameter());
						stream << form.getLinkButton("Supprimer","Etes-vous sûr(e) de vouloir supprimer l'habilitation sélectionnée ?");
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
			HTMLTable t(v, "adminresults");

			stream << t.open();

			vector<boost::shared_ptr<Right> > rights(Factory<Right>::GetNewCollection());
			BOOST_FOREACH(const boost::shared_ptr<Right> right, rights)
			{
				ParameterLabelsVector pl(right->getParametersLabels());
				HTMLForm form(addRightRequest.getHTMLForm("add" + right->getFactoryKey()));
				form.addHiddenField(AddRightAction::PARAMETER_RIGHT, right->getFactoryKey());
				stream
					<< "<tr>"
					<< form.open()
					<< "<td>" << right->getName() << "</td>"
					<< "<td>";
				if (pl.size() == 1)
				{
					stream << pl.at(0).second;
					form.addHiddenField(AddRightAction::PARAMETER_PARAMETER, *pl.at(0).first);
				}
				else
					stream << form.getSelectInput(AddRightAction::PARAMETER_PARAMETER, pl, optional<string>(GLOBAL_PERIMETER));
				if (dynamic_cast<const ResaRight*>(right.get()))
				{
					stream
						<< "</td>"
						<< "<td>" << form.getSelectInput(AddRightAction::PARAMETER_PUBLIC_LEVEL, privatePublicMapResa, optional<int>((int) USE)) << "</td>"
						<< "<td>";
				}
				else
				{
					stream
						<< "</td>"
						<< "<td>" << form.getSelectInput(AddRightAction::PARAMETER_PUBLIC_LEVEL, privatePublicMap, optional<int>((int) USE)) << "</td>"
						<< "<td>";
				}
				if (right->getUsePrivateRights())
					stream << form.getSelectInput(AddRightAction::PARAMETER_PRIVATE_LEVEL, privatePublicMap, optional<int>((int) USE));
				stream
					<< "</td>"
					<< "<td>"
					<< form.getSubmitButton("Ajouter") << "</td>"
					<< form.close()
					<< "</tr>";
			}
			stream << "</table>";
		}



		bool ProfileAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<SecurityRight>(READ, UNKNOWN_RIGHT_LEVEL, string());
		}

		std::string ProfileAdmin::getTitle() const
		{
			return _profile.get() ? _profile->getName() : DEFAULT_TITLE;
		}

		void ProfileAdmin::setProfile(boost::shared_ptr<Profile> value)
		{
			_profile = const_pointer_cast<const Profile>(value);
		}

		boost::shared_ptr<const Profile> ProfileAdmin::getProfile() const
		{
			return _profile;
		}


		AdminInterfaceElement::PageLinks ProfileAdmin::getSubPages(
			const AdminInterfaceElement& currentPage,
			const server::Request& request
		) const {
			AdminInterfaceElement::PageLinks links;

			ProfileTableSync::SearchResult profiles(
				ProfileTableSync::Search(
					_getEnv(),
					_profile->getKey()
			)	);
			BOOST_FOREACH(const boost::shared_ptr<Profile>& profile, profiles)
			{
				boost::shared_ptr<ProfileAdmin> p(getNewPage<ProfileAdmin>());
				p->setProfile(profile);
				links.push_back(p);
			}

			return links;
		}


		bool ProfileAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			return _profile == static_cast<const ProfileAdmin&>(other)._profile;
		}

	}
}
