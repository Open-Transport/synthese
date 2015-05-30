
/** CMSModule class implementation.
	@file CMSModule.cpp

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

#include "CMSModule.hpp"

#include "AdminActionFunctionRequest.hpp"
#include "AdminFunctionRequest.hpp"
#include "FunctionWithSiteBase.hpp"
#include "HTMLForm.h"
#include "HTMLModule.h"
#include "HTMLTable.h"
#include "ModuleAdmin.h"
#include "ObjectCreateAction.hpp"
#include "PropertiesHTMLTable.h"
#include "RemoveObjectAction.hpp"
#include "Request.h"
#include "SVNCheckoutAction.hpp"
#include "WebsiteAdmin.hpp"
#include "WebsiteTableSync.hpp"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"
#include "PythonInterpreter.hpp"

#include <boost/algorithm/string/trim.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace admin;
	using namespace cms;
	using namespace html;
	using namespace db;
	using namespace db::svn;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<>
		const string FactorableTemplate<ModuleClass,CMSModule>::FACTORY_KEY("11_cms");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<CMSModule>::NAME("CMS");

		template<> void ModuleClassTemplate<CMSModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<CMSModule>::Init()
		{
			PythonInterpreter::Initialize();
		}

		template<> void ModuleClassTemplate<CMSModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<CMSModule>::End()
		{
			PythonInterpreter::End();
		}



		template<> void ModuleClassTemplate<CMSModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<CMSModule>::CloseThread(
		){
		}
	}

	namespace cms
	{
		CMSModule::SitesByURL CMSModule::_sitesByURL;
		mutex CMSModule::_sitesByURLMutex;


		const Website* CMSModule::GetSite(
			const server::Request& request,
			const ParametersMap& pm
		){
			RegistryKeyType id(pm.getDefault<RegistryKeyType>(FunctionWithSiteBase::PARAMETER_SITE, 0));
			if(id)
			{
				return Env::GetOfficialEnv().get<Website>(id).get();
			}

			{
				boost::shared_ptr<const WebPageDisplayFunction> function(
					dynamic_pointer_cast<const WebPageDisplayFunction>(
						request.getFunction()
				)	);
				if(function.get() && function->getPage())
				{
					return Env::GetOfficialEnv().get<Website>(function->getPage()->getRoot()->getKey()).get();
				}
			}

			{
				boost::shared_ptr<const FunctionWithSiteBase> function(
						dynamic_pointer_cast<const FunctionWithSiteBase>(
						request.getFunction()
				)	);
				if(function.get())
				{
					return function->getSite();
				}
			}

			return NULL;
		}



		const Webpage* CMSModule::GetWebPage(
			const server::Request& request
		){
			boost::shared_ptr<const WebPageDisplayFunction> function(
				dynamic_pointer_cast<const WebPageDisplayFunction>(
					request.getFunction()
			)	);
			if(function.get())
			{
				if(function->getTemplateParameters().getDefault<RegistryKeyType>(WebPageDisplayFunction::PARAMETER_PAGE_ID, 0))
				{
					return Env::GetOfficialEnv().get<Webpage>(
						function->getTemplateParameters().get<RegistryKeyType>(WebPageDisplayFunction::PARAMETER_PAGE_ID)
					).get();
				}
				else
				{
					return function->getPage();
				}
			}
			return NULL;
		}



		void CMSModule::RemoveSite(
			const Website& site
		){
			mutex::scoped_lock lock(_sitesByURLMutex);
			_sitesByURL.erase(
				make_pair(
					site.get<HostName>(),
					site.get<ClientURL>()
			)	);
		}



		void CMSModule::AddSite(
			Website& site
		){
			mutex::scoped_lock lock(_sitesByURLMutex);
			_sitesByURL.insert(
				make_pair(
					make_pair(
						site.get<HostName>(),
						site.get<ClientURL>()
					), &site
			)	);
		}



		Website* CMSModule::GetSiteByURL(
			const std::string& hostName,
			const std::string& clientURL
		){
			// Avoid useless queries
			if(hostName.empty() && clientURL.empty())
			{
				return NULL;
			}

			// Thread safety
			mutex::scoped_lock lock(_sitesByURLMutex);

			// Search for host + client URL
			if(!clientURL.empty())
			{
				SitesByURL::const_iterator it(
					_sitesByURL.find(
						make_pair(
							hostName,
							clientURL
				)	)	);
				if(it != _sitesByURL.end())
				{
					return it->second;
				}

				// Search for host + client URL beginning
				BOOST_FOREACH(const SitesByURL::value_type& it, _sitesByURL)
				{
					if(	!it.first.first.empty() &&
						!it.first.second.empty() &&
						it.first.first == hostName &&
						it.first.second.size() < clientURL.size() &&
						it.first.second + "/" == clientURL.substr(0, it.first.second.size() + 1)
					){
						return it.second;
					}
				}
			}

			// Search for host
			if(!hostName.empty())
			{
				SitesByURL::const_iterator it(
					_sitesByURL.find(
					make_pair(
						hostName,
						string()
				)	)	);
				if(it != _sitesByURL.end())
				{
					return it->second;
				}
			}

			// Search for client URL
			if(!clientURL.empty())
			{
				SitesByURL::const_iterator it(
					_sitesByURL.find(
						make_pair(
							string(),
							clientURL
				)	)	);
				if(it != _sitesByURL.end())
				{
					return it->second;
				}

				// Search for client URL beginning
				BOOST_FOREACH(const SitesByURL::value_type& it, _sitesByURL)
				{
					if(	it.first.first.empty() &&
						!it.first.second.empty() &&
						it.first.second.size() < clientURL.size() &&
						it.first.second + "/" == clientURL.substr(0, it.first.second.size() + 1)
					){
						return it.second;
					}
				}
			}

			// Default site
			SitesByURL::const_iterator it(
				_sitesByURL.find(
				make_pair(
					string(),
					string()
			)	)	);
			if(it != _sitesByURL.end())
			{
				return it->second;
			}

			return NULL;
		}



		void CMSModule::displayAdmin(
			ostream& stream,
			const Request& request,
			const AdminInterfaceElement& currentPage
		) const	{

			// New site request
			AdminActionFunctionRequest<ObjectCreateAction, WebsiteAdmin> createRequest(request);
			createRequest.setActionFailedPage<ModuleAdmin>();
			static_cast<ModuleAdmin*>(createRequest.getActionFailedPage().get())->setModuleClass(boost::shared_ptr<ModuleClass>(new CMSModule));
			createRequest.getAction()->setTable<Website>();
			createRequest.setActionWillCreateObject();

			// Open site request
			AdminFunctionRequest<WebsiteAdmin> openRequest(request);

			// Remove site request
			AdminActionFunctionRequest<RemoveObjectAction, ModuleAdmin> removeRequest(request);
			removeRequest.getPage()->setModuleClass(boost::shared_ptr<ModuleClass>(new CMSModule));

			// Form and table
			HTMLForm f(createRequest.getHTMLForm("new"));
			HTMLTable::ColsVector c;
			c.push_back(string());
			c.push_back("Nom");
			c.push_back("Host name");
			c.push_back("URL");
			c.push_back(string());
			HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
			stream << "<h1>Sites</h1>";
			stream << f.open() << t.open();
			BOOST_FOREACH(const Website::Registry::value_type& item, Env::GetOfficialEnv().getRegistry<Website>())
			{
				// Declarations
				const Website& site(*item.second);
				openRequest.getPage()->setSite(item.second);
				removeRequest.getAction()->setObjectId(site.getKey());

				// Output of the row
				stream << t.row();
				stream << t.col() << HTMLModule::getLinkButton(openRequest.getURL(), "Ouvrir");
				stream << t.col() << site.get<Name>();
				stream << t.col() << site.get<HostName>();
				stream << t.col() << site.get<ClientURL>();

				// Remove button
				stream << t.col();
				stream << HTMLModule::getLinkButton(removeRequest.getURL(), "Supprimer", "Etes-vous sûr de vouloir supprimer le site "+ site.get<Name>() +" ?");
			}
			stream << t.row();
			stream << t.col();
			stream << t.col() << f.getTextInput(ObjectCreateAction::GetInputName<Name>(), string(), "(nom)");
			stream << t.col() << f.getTextInput(ObjectCreateAction::GetInputName<HostName>(), string(), "(Host name)");
			stream << t.col() << f.getTextInput(ObjectCreateAction::GetInputName<ClientURL>(), string(), "(URL)");
			stream << t.col() << f.getSubmitButton("Ajouter");
			stream << t.close() << f.close();

			// SVN Checkout
			stream << "<h1>SVN Checkout</h1>";
			AdminActionFunctionRequest<SVNCheckoutAction, WebsiteAdmin> svnRequest(request);
			svnRequest.setActionFailedPage<ModuleAdmin>();
			static_cast<ModuleAdmin*>(svnRequest.getActionFailedPage().get())->setModuleClass(boost::shared_ptr<ModuleClass>(new CMSModule));
			svnRequest.setActionWillCreateObject();
			PropertiesHTMLTable pf(svnRequest.getHTMLForm("svn"));
			stream << pf.open();
			stream << pf.cell("URL", pf.getForm().getTextInput(SVNCheckoutAction::PARAMETER_REPO_URL, string()));
			stream << pf.cell("Utilisateur", pf.getForm().getTextInput(SVNCheckoutAction::PARAMETER_USER, string()));
			stream << pf.cell("Mot de passe", pf.getForm().getPasswordInput(SVNCheckoutAction::PARAMETER_PASSWORD, string()));
			stream << pf.close();
		}
}	}
