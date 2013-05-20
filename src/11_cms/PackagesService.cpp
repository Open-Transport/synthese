
//////////////////////////////////////////////////////////////////////////////////////////
///	PackagesService class implementation.
///	@file PackagesService.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PackagesService.hpp"

#include "CMSInstallRight.hpp"
#include "HTMLForm.h"
#include "HTMLPage.hpp"
#include "HTMLTable.h"
#include "HTMLModule.h"
#include "LoginAction.h"
#include "Profile.h"
#include "RequestException.h"
#include "Request.h"
#include "Session.h"
#include "StaticActionFunctionRequest.h"
#include "StaticFunctionRequest.h"
#include "SVNCheckoutAction.hpp"
#include "SVNUpdateAction.hpp"
#include "User.h"
#include "Webpage.h"
#include "Website.hpp"

using namespace boost;
using namespace boost::algorithm;
using namespace std;

namespace synthese
{
	using namespace html;
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace db::svn;

	template<>
	const string FactorableTemplate<Function,cms::PackagesService>::FACTORY_KEY = "packages";
	
	namespace cms
	{
		const string PackagesService::PARAMETER_SVN_URL = "svn_url";
		const string PackagesService::PARAMETER_SVN_USER = "svn_user";
		const string PackagesService::PARAMETER_SVN_PASSWORD = "svn_password";
		


		ParametersMap PackagesService::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_SVN_URL, _repo.getURL());
			map.insert(PARAMETER_SVN_USER, _svnUser);
			map.insert(PARAMETER_SVN_PASSWORD, _svnPassword);
			return map;
		}



		void PackagesService::_setFromParametersMap(const ParametersMap& map)
		{
			_repo = SVNRepository(
				map.getDefault<string>(
					PARAMETER_SVN_URL,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/sites"
			)	);
			_svnUser = map.getDefault<string>(PARAMETER_SVN_USER);
			_svnPassword = map.getDefault<string>(PARAMETER_SVN_PASSWORD);
		}



		ParametersMap PackagesService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			HTMLPage p;
			p.setDefaultInlineCSS();
			
			if(request.getSession() &&
				request.getSession()->hasProfile() &&
				request.getSession()->getUser()->getProfile()->isAuthorized<CMSInstallRight>(READ)
			){
				// User right
				bool installRight(request.getSession()->getUser()->getProfile()->isAuthorized<CMSInstallRight>(WRITE));

				// SVN URL change form
				StaticFunctionRequest<PackagesService> svnRequest(request, true);
				HTMLForm f(svnRequest.getHTMLForm());
				p << f.open();
				p << "Chemin SVN : " << f.getTextInput(PARAMETER_SVN_URL, _repo.getURL());
				p << " Utilisateur : " << f.getTextInput(PARAMETER_SVN_USER, _svnUser);
				p << " Mot de passe : " << f.getPasswordInput(PARAMETER_SVN_PASSWORD, _svnPassword);
				p << " " << f.getSubmitButton("Changer");
				p << f.close();

				// Get installable packages
				SVNCommands::LsResult packages(_repo.ls(_svnUser, _svnPassword));
				if(packages.empty())
				{
					p << "<p>Pas de package à installer à cette URL.</p>";
				}
				else
				{
					// Installation request
					StaticActionFunctionRequest<SVNCheckoutAction, PackagesService> checkoutRequest(request, true);
					checkoutRequest.getAction()->setUser(_svnUser);
					checkoutRequest.getAction()->setPassword(_svnPassword);

					// Update request
					StaticActionFunctionRequest<SVNUpdateAction, PackagesService> updateRequest(request, true);
					updateRequest.getAction()->setUser(_svnUser);
					updateRequest.getAction()->setPassword(_svnPassword);

					// Draw the table
					HTMLTable::ColsVector c;
					c.push_back("Package");
					c.push_back("Installé");
					if(installRight)
					{
						c.push_back("Actions");
					}
					HTMLTable t(c);
					p << t.open();
					BOOST_FOREACH(const string& package, packages)
					{
						// Jump over empty strings
						if(trim_copy(package).empty())
						{
							continue;
						}

						// Check of the id of the site
						SVNCommands::LsResult files(
							_repo.ls("/"+ package, _svnUser, _svnPassword)
						);
						RegistryKeyType siteId(0);
						BOOST_FOREACH(const string& file, files)
						{
							if(	file.size() > 5 &&
								file.substr(file.size() - 5, 5) == ".dump"
							){
								try
								{
									siteId = lexical_cast<RegistryKeyType>(
										file.substr(0, file.size() - 5)
									);
								}
								catch(bad_lexical_cast&)
								{
								}
							}
						}

						// No id was found : corrupted package
						if(!siteId)
						{
							continue;
						}

						// Is the package currently installed ?
						bool isInstalled(
							Env::GetOfficialEnv().getRegistry<Website>().contains(siteId)
						);

						// HTML output
						p << t.row();

						// Package name
						p << t.col() << package;

						// Package is installed =
						p << t.col() <<
							(isInstalled ? "OUI" : "NON");

						// Action cell
						if(installRight)
						{
							p << t.col();
							if(isInstalled)
							{
								boost::shared_ptr<ObjectBase> site(
									Env::GetOfficialEnv().getCastEditable<ObjectBase, Website>(siteId)
								);
								updateRequest.getAction()->setObject(site);
								p << HTMLModule::getLinkButton(
									updateRequest.getURL(),
									"Mettre à jour",
									"Etes-vous sûr de vouloir mettre à jour le package "+ package +" ?"
								);
							}
							else
							{
								SVNRepository packageRepo(_repo.getURL() +"/"+ package);
								checkoutRequest.getAction()->setRepo(packageRepo);
								p << HTMLModule::getLinkButton(
									checkoutRequest.getURL(),
									"Installer",
									"Etes-vous sûr de vouloir installer le package "+ package +" ?"
								);
							}
						}
					}
					p << t.close();
				}
			}
			else
			{
				p << "<p>Veuillez vous connecter pour utiliser cette fonction</p>";

				StaticActionFunctionRequest<LoginAction, PackagesService> loginRequest(request, true);
				HTMLForm f(loginRequest.getHTMLForm());
				p << f.open();
				p << "Utilisateur : " << f.getTextInput(LoginAction::PARAMETER_LOGIN, string());
				p << " Mot de passe : " << f.getPasswordInput(LoginAction::PARAMETER_PASSWORD, string());
				p << " " << f.getSubmitButton("Login");
				p << f.close();
			}

			p.output(stream);

			return map;
		}
		
		
		
		bool PackagesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PackagesService::getOutputMimeType() const
		{
			return MimeTypes::HTML;
		}
}	}
