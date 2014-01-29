
/** ResaCustomerMergeAdmin class implementation.
	@file ResaCustomerMergeAdmin.cpp
	@date 2014

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

#include "ResaCustomerMergeAdmin.hpp"

#include "Profile.h"
#include "User.h"
#include "UserTableSync.h"
#include "ResaModule.h"
#include "ResaRight.h"
#include "ResaCustomerAdmin.h"
#include "ResaCustomersAdmin.h"
#include "ReservationUserMergeAction.hpp"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "Request.h"
#include "ModuleAdmin.h"
#include "AdminParametersException.h"
#include "AdminInterfaceElement.h"
#include "SearchFormHTMLTable.h"
#include "PropertiesHTMLTable.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace resa;
	using namespace admin;
	using namespace security;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ResaCustomerMergeAdmin>::FACTORY_KEY("ResaCustomerMergeAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ResaCustomerMergeAdmin>::ICON("user.png");
		template<> const string AdminInterfaceElementTemplate<ResaCustomerMergeAdmin>::DEFAULT_TITLE("Fusion clients");
	}

	namespace resa
	{
		const string ResaCustomerMergeAdmin::PARAM_SEARCH_LOGIN("login");
		const string ResaCustomerMergeAdmin::PARAM_SEARCH_NAME("name");
		const string ResaCustomerMergeAdmin::PARAM_SEARCH_SURNAME("surname");
		const string ResaCustomerMergeAdmin::PARAM_USER_TO_DELETE("user_to_delete");


		
		ResaCustomerMergeAdmin::ResaCustomerMergeAdmin(
		): AdminInterfaceElementTemplate<ResaCustomerMergeAdmin>() {}



		void ResaCustomerMergeAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			// If userToDelete isn't accessible in URL yet, check the
			// search form data has been posted in order to get it
			if (!map.getDefault<RegistryKeyType>(PARAM_USER_TO_DELETE))
			{
				if (!map.getDefault<string>(PARAM_SEARCH_NAME).empty())
				{
					_searchName = map.getOptional<string>(PARAM_SEARCH_NAME);
				}
				if (!map.getDefault<string>(PARAM_SEARCH_SURNAME).empty())
				{
					_searchSurname = map.getOptional<string>(PARAM_SEARCH_SURNAME);
				}
				if (!map.getDefault<string>(PARAM_SEARCH_LOGIN).empty())
				{
					_searchLogin = map.getOptional<string>(PARAM_SEARCH_LOGIN);
				}
				if (_searchName || _searchSurname || _searchLogin) 
				{
                    UserTableSync::SearchResult	user(
						UserTableSync::Search(
							_getEnv(),
							_searchLogin ? "%" + *_searchLogin + "%" : _searchLogin,
							_searchName ? "%" + *_searchName + "%" : _searchName,
							_searchSurname ? "%" + *_searchSurname + "%" : _searchSurname,
							optional<string>(),
							optional<RegistryKeyType>(),
							logic::indeterminate,
							logic::indeterminate,
							optional<RegistryKeyType>()
						)
					);
					if (user.empty())
					{
                        _search = true;
					}
					else
					{
                        if (user.front()->getKey() == map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
                        {
                            _search = true;
                        }
                        else
                        {
                            _userToDelete = user.front();
                            _search = false;
                        }
					}
				}
				else
				{
					_search = true;
				}
			}
			// If userToDelete is accessible in URL, this page doesn't have to display
			// the search form and purpose merging operation instead
			else
			{
				_search = false;
				if (map.get<RegistryKeyType>(PARAM_USER_TO_DELETE) != 0)
				{
					try
					{
						_userToDelete = UserTableSync::Get(
							map.get<RegistryKeyType>(PARAM_USER_TO_DELETE),
							_getEnv()
						);
					}
					catch (...)
					{
						throw AdminParametersException("Bad user id");
					}
				}
			}
			// Get userToMerge from the request call object ID present in URL
			if (map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID) != 0)
			{
				try
				{
					_userToMerge = UserTableSync::Get(
						map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID),
						_getEnv()
					);
				}
				catch (...)
				{
					throw AdminParametersException("Bad user id");
				}
			}
		}



		util::ParametersMap ResaCustomerMergeAdmin::getParametersMap() const
		{
			ParametersMap m("rcmmap");
			if (_searchName)
				m.insert(PARAM_SEARCH_NAME, *_searchName);
			if (_searchSurname)
				m.insert(PARAM_SEARCH_SURNAME, *_searchSurname);
			if (_searchLogin)
				m.insert(PARAM_SEARCH_LOGIN, *_searchLogin);
			if (_userToMerge.get())
			{
				m.insert(Request::PARAMETER_OBJECT_ID, _userToMerge->getKey());
			}
			if (_userToDelete.get())
			{
				m.insert(PARAM_USER_TO_DELETE, _userToDelete->getKey());
			}
			return m;
		}


		void ResaCustomerMergeAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{
			// If only one user found, display search form to find userToDelete
			if (_search)
			{
				// Build search request
				AdminFunctionRequest<ResaCustomerMergeAdmin> searchRequest(_request,*this);

				// Display search form
				SearchFormHTMLTable st(searchRequest.getHTMLForm("search"));
				stream << "<h1>Recherche</h1>";
				stream << "Veuillez renseigner l'utilisateur à fusionner avec " 
						<< _userToMerge->getSurname() << " " << _userToMerge->getName() 
						<< " en utilisant le formulaire de recherche.";
				stream << st.open();
				stream << st.cell("Nom", st.getForm().getTextInput(PARAM_SEARCH_NAME, _searchName ? *_searchName : string()));
				stream << st.cell("Prénom", st.getForm().getTextInput(PARAM_SEARCH_SURNAME, _searchSurname ? *_searchSurname : string()));
				stream << st.cell("Login", st.getForm().getTextInput(PARAM_SEARCH_LOGIN, _searchLogin ? *_searchLogin : string()));
				stream << st.close();
				stream << st.getForm().setFocus(PARAM_SEARCH_NAME);
			}
			else
			{
				// Build merge request
				AdminActionFunctionRequest<ReservationUserMergeAction,ResaCustomerAdmin> mergeRequest(_request);
				mergeRequest.getAction()->setUserToDelete(_userToDelete);
				mergeRequest.getPage()->setUser(_userToMerge);
				mergeRequest.setActionFailedPage<ResaCustomersAdmin>();

				// Build permutation request
				AdminFunctionRequest<ResaCustomerMergeAdmin> permutationRequest(
					_request,
					*this
				);
				permutationRequest.getPage()->setUserToDelete(_userToMerge);
				permutationRequest.getPage()->setUserToMerge(_userToDelete);

				// Display merging form
				PropertiesHTMLTable t(mergeRequest.getHTMLForm());

				stream << "<script type=\"text/javascript\" src=\"/lib/synthese/js/merge.js\"></script>";
				stream << "<h1 style='text-align:center;'>FUSION DE COMPTE CLIENT</h1>";
                stream << "<div style='text-align:center;'><br><br>" << HTMLModule::getLinkButton(permutationRequest.getURL(), "Permuter", "Vous vous apprêtez à permuter les deux clients : toutes vos modifications seront perdues. Voulez vous continuer ?") << "</div><br><br>";
				stream << t.getForm().open() << "<table style=\"margin:auto;border:none;\" class=\"propertysheet\"><colgroup>";
				stream << "<col class=\"property\"><col class=\"value\"><col class=\"switch\"><col class=\"property\"><col class=\"value\">";
				stream << "<thead><th colspan=\"2\" style=\"background-color:#fff;text-align:center;border:none;\">";
				stream << "<span style=\"color:red;\">CE CLIENT SERA CONSERVE</span><br />";
				stream << "<span style=\"color:#000;font-weight:normal;\">Vous pouvez modifier les champs";
				stream << "<br>ou les permuter avec le client supprimé</span>";
				stream << "</th><th style=\"background-color:#fff;border:none;\"></th>";
				stream << "<th colspan=\"2\" style=\"background-color:#fff;text-align:center;border:none;\">";
				stream << "<span style=\"color:red;\">CE CLIENT SERA SUPPRIME</span><br>";
				stream << "<span style=\"color:red;font-weight:normal;\">Son historique sera transféré<br>au client conservé</span>";
				stream << "</th></thead>";
				stream << t.title("Connexion") << "<th></th><th colspan=\"2\">Connexion</th></tr>";
				stream << t.cell("Login", t.getForm().getTextInput(ReservationUserMergeAction::PARAMETER_LOGIN, _userToMerge->getLogin()));
				stream << "<td></td><td>Login</td><td>" << _userToDelete->getLogin() << "</td></tr>";
				stream << t.title("Coordonnées") << "<th></th><th colspan=\"2\">Coordonnées</th></tr>";
				stream << t.cell("Prénom", t.getForm().getTextInput(ReservationUserMergeAction::PARAMETER_SURNAME, _userToMerge->getSurname()));
				stream << "<td></td><td>Prénom</td><td>" << _userToDelete->getSurname() << "</td></tr>";
				stream << t.cell("Nom", t.getForm().getTextInput(ReservationUserMergeAction::PARAMETER_NAME, _userToMerge->getName()));
				stream << "<td></td><td>Nom</td><td>" << _userToDelete->getName() << "</td></tr>";
				stream << t.cell("Adresse", t.getForm().getTextAreaInput(ReservationUserMergeAction::PARAMETER_ADDRESS, _userToMerge->getAddress(), 3, 30, false));
				stream << "<td></td><td>Adresse</td><td>" << _userToDelete->getAddress() << "</td></tr>";
				stream << t.cell("Code postal", t.getForm().getTextInput(ReservationUserMergeAction::PARAMETER_POSTAL_CODE, _userToMerge->getPostCode()));
				stream << "<td></td><td>Code postal</td><td>" << _userToDelete->getPostCode() << "</td></tr>";
				stream << t.cell("Ville", t.getForm().getTextInput(ReservationUserMergeAction::PARAMETER_CITY, _userToMerge->getCityText()));
				stream << "<td></td><td>Ville</td><td>" << _userToDelete->getCityText() << "</td></tr>";
				stream << t.cell("Téléphone", t.getForm().getTextInput(ReservationUserMergeAction::PARAMETER_PHONE, _userToMerge->getPhone()));
				stream << "<td></td><td>Téléphone</td><td>" << _userToDelete->getPhone() << "</td></tr>";
				stream << t.cell("E-mail", t.getForm().getTextInput(ReservationUserMergeAction::PARAMETER_EMAIL, _userToMerge->getEMail()));
				stream << "<td></td><td>E-mail</td><td>" << _userToDelete->getEMail() << "</td></tr>";
				stream << t.title("Droits") << "<th></th><th colspan=\"2\">Droits</th></tr>";
				stream << t.cell("Accès site web", t.getForm().getOuiNonRadioInput(ReservationUserMergeAction::PARAMETER_AUTHORIZED_LOGIN, _userToMerge->getConnectionAllowed()));
				stream << "<td></td><td>Accès site web</td><td>" << (_userToDelete->getConnectionAllowed() ? "oui" : "non") << "</td></tr>";
				stream << "<tr><th colspan=\"5\" style=\"text-align:center;height:60px;background-color:#fff;border:none;\">";
				stream << t.getForm().getSubmitButton("Fusionner") << "</th></tr>";
				stream << "</tbody></table>";
                stream << "<input type=\"hidden\" name=\"actionParamu\" value=\"" << lexical_cast<string>(_userToMerge->getKey()) << "\" id=\"_u_AUTOGENERATEDFIELDID\">";
                stream << t.getForm().close();
			}
		}



		bool ResaCustomerMergeAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ResaRight>(WRITE);
		}
	}
}
