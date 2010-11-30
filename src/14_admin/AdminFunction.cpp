
/** AdminRequest class implementation.
	@file AdminRequest.cpp

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

#include <sstream>
#include <assert.h>

#include "AdminFunction.h"
#include "FactoryException.h"
#include "HTMLForm.h"
#include "RequestException.h"
#include "Action.h"
#include "HomeAdmin.h"
#include "AdminInterfaceElement.h"
#include "AdminParametersException.h"
#include "LoginAdmin.hpp"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"
#include "StaticActionFunctionRequest.h"
#include "LogoutAction.h"
#include "ServerModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace html;
	using namespace server;
	using namespace cms;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,admin::AdminFunction>::FACTORY_KEY("admin");

	namespace admin
	{
		const string AdminFunction::PARAMETER_PAGE("rub");
		const string AdminFunction::PARAMETER_TAB("tab");
		const string AdminFunction::PARAMETER_ACTION_FAILED_PAGE("afp");
		const string AdminFunction::PARAMETER_ACTION_FAILED_TAB("aft");
		const string AdminFunction::PARAMETER_MAIN_CMS_TEMPLATE("mt");
		const string AdminFunction::PARAMETER_POSITION_ELEMENT_CMS_TEMPLATE("pt");
		const string AdminFunction::PARAMETER_TREE_ELEMENT_CMS_TEMPLATE("tt");

		const string AdminFunction::DATA_CONTENT("content");
		const string AdminFunction::DATA_ERROR_MESSAGE("error_message");
		const string AdminFunction::DATA_LOGOUT_URL("logout_url");
		const string AdminFunction::DATA_POSITION("position");
		const string AdminFunction::DATA_TABS("tabs");
		const string AdminFunction::DATA_TREE("tree");
		const string AdminFunction::DATA_USER_FULL_NAME("user_full_name");
		const string AdminFunction::DATA_ICON("icon");
		const string AdminFunction::DATA_TITLE("title");
		const string AdminFunction::DATA_URL("url");
		const string AdminFunction::DATA_DEPTH("depth");
		const string AdminFunction::DATA_IS_LAST("is_last");
		const string AdminFunction::DATA_IS_CURRENT("is_current");
		const string AdminFunction::DATA_IS_OPENED("is_opened");
		const string AdminFunction::DATA_SUBTREE("subtree");
		const string AdminFunction::DATA_VERSION("version");

		
		ParametersMap AdminFunction::_getParametersMap() const
		{
			ParametersMap result;

			if (_page.get())
			{
				result.insert(PARAMETER_PAGE, _page->getFactoryKey());
				result.insert(PARAMETER_TAB, _page->getActiveTab());
				const map<string,string> adminMap(_page->getParametersMap().getMap());
				for (map<string,string>::const_iterator it(adminMap.begin()); it != adminMap.end(); ++it)
				{
					result.insert(it->first,it->second);
				}
			}
			if (_actionFailedPage.get())
			{
				result.insert(PARAMETER_ACTION_FAILED_PAGE, _actionFailedPage->getFactoryKey());
				result.insert(PARAMETER_ACTION_FAILED_TAB, _actionFailedPage->getActiveTab());
			}
			if(_mainTemplate.get())
			{
				result.insert(PARAMETER_MAIN_CMS_TEMPLATE, _mainTemplate->getKey());
			}
			if(_treeNodeTemplate.get())
			{
				result.insert(PARAMETER_TREE_ELEMENT_CMS_TEMPLATE, _treeNodeTemplate->getKey());
			}
			if(_positionElementTemplate.get())
			{
				result.insert(PARAMETER_POSITION_ELEMENT_CMS_TEMPLATE, _positionElementTemplate->getKey());
			}
			return result;
		}



		void AdminFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				// Page
				string pageKey;
				if (map.getDefault<bool>(Request::PARAMETER_ACTION_FAILED, false))
				{	// Prepare the KO page
					pageKey = map.getDefault<string>(PARAMETER_ACTION_FAILED_PAGE);
					if (pageKey.empty())
						pageKey = map.getDefault<string>(PARAMETER_PAGE);
				}
				else
				{	// Prepare the OK page

					// Saving of the action failed page for url output purposes
					pageKey = map.getDefault<string>(PARAMETER_ACTION_FAILED_PAGE);
					if (!pageKey.empty())
					{
						_actionFailedPage.reset(Factory<AdminInterfaceElement>::create(pageKey));
						_actionFailedPage->setActiveTab(
							map.getDefault<string>(PARAMETER_ACTION_FAILED_TAB)
						);
					}

					pageKey = map.getDefault<string>(PARAMETER_PAGE);
				}
				_page.reset(pageKey.empty()
					? new HomeAdmin
					: Factory<AdminInterfaceElement>::create(pageKey)
				);
				_page->setEnv(shared_ptr<Env>(new Env));
				_errorMessage = map.getOptional<string>(Request::PARAMETER_ERROR_MESSAGE);
				_page->setFromParametersMap(map);
				_page->setActiveTab(map.getDefault<string>(PARAMETER_TAB));
			}
			catch (FactoryException<AdminInterfaceElement> e)
			{
				throw RequestException("Admin page not found");
			}
			catch (AdminParametersException e)
			{
				throw RequestException("Admin page parameters error : " + e.getMessage());
			}

			// CMS templates
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_MAIN_CMS_TEMPLATE));
				if(id)
				{
					_mainTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such main CMS template : "+ e.getMessage());
			}

			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_POSITION_ELEMENT_CMS_TEMPLATE));
				if(id)
				{
					_positionElementTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such position element CMS template : "+ e.getMessage());
			}

			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_TREE_ELEMENT_CMS_TEMPLATE));
				if(id)
				{
					_treeNodeTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such tree element CMS template : "+ e.getMessage());
			}
		}



		void AdminFunction::run(
			std::ostream& stream,
			const Request& request
		) const	{
			try
			{
				shared_ptr<LoginAdmin> loginPage(new LoginAdmin);
				if(request.getSession())
				{
					_page->_buildTabs(*request.getSession()->getUser()->getProfile());
				}
				else
				{
					loginPage->setURLToLaunch(request.getURL());
				}

				if(_mainTemplate.get())
				{
					_display(
						stream,
						request,
						request.getSession() ? const_pointer_cast<const AdminInterfaceElement>(_page) : static_pointer_cast<const AdminInterfaceElement, LoginAdmin>(loginPage),
						_errorMessage
					);
				}
				else
				{
					stream <<
						"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" <<
						"<html xmlns=\"http://www.w3.org/1999/xhtml\"><head><title>" <<
						(request.getSession() ? _page->getTitle() : loginPage->getTitle()) <<
						"</head><body>"
					;
					if(request.getSession())
					{
						_page->display(stream, StaticFunctionRequest<AdminFunction>(request, true));
					}
					else
					{
						loginPage->display(stream, StaticFunctionRequest<AdminFunction>(request, true));
					}
					stream << "</body></html>";
				}
			}
			catch (Exception& e)
			{
				throw RequestException("Admin interface page not implemented in database"+ e.getMessage());
			}
		}



		bool AdminFunction::isAuthorized(const server::Session* session) const
		{
			return
				_page.get() &&
				(	!session ||
					session->hasProfile() &&
					_page->isAuthorized(
						*session->getUser()
				)	)
			;
		}



		std::string AdminFunction::getOutputMimeType() const
		{
			return "text/html";
		}



		void AdminFunction::_copy( boost::shared_ptr<const Function> function )
		{
			const AdminFunction& adminFunction(static_cast<const AdminFunction&>(*function));
			_page = adminFunction._page;
			_mainTemplate = adminFunction._mainTemplate;
			_positionElementTemplate = adminFunction._positionElementTemplate;
			_treeNodeTemplate = adminFunction._treeNodeTemplate;
		}



		void AdminFunction::_display(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const AdminInterfaceElement> compound,
			optional<string> errorMessage
		) const {
			AdminRequest adminRequest(request, true);

			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(_mainTemplate);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			// error_message
			if(errorMessage)
			{
				pm.insert(DATA_ERROR_MESSAGE, *errorMessage);
			}
			
			// title, icon, url
			ExportAdminCompound(pm, request, compound);

			// tree
			if(_treeNodeTemplate.get())
			{
				stringstream treeStream;
				_displayAdminTreeNode(
					treeStream,
					request,
					compound->getTree(adminRequest),
					compound,
					0,
					true
				);
				pm.insert(DATA_TREE, treeStream.str());
			}

			// position
			if(_positionElementTemplate.get())
			{
				const AdminInterfaceElement::PageLinks& links(compound->getTreePosition(adminRequest));
				stringstream positionStream;
				for (AdminInterfaceElement::PageLinks::const_iterator it(links.begin()); it != links.end(); ++it)
				{
					_displayAdminPosElement(positionStream, request, *it, it - links.begin(), it+1 == links.end());
				}
				pm.insert(DATA_POSITION, positionStream.str());
			}

			// tabs
			stringstream tabsStream;
			compound->displayTabs(
				tabsStream,
				adminRequest
			);
			pm.insert(DATA_TABS, tabsStream.str());

			// content
			stringstream contentStream;
			compound->display(
				contentStream,
				adminRequest
			);
			pm.insert(DATA_CONTENT, contentStream.str());

			// logout_url
			StaticActionFunctionRequest<LogoutAction,AdminFunction> logoutRequest(request, true);
			pm.insert(DATA_LOGOUT_URL, logoutRequest.getURL());

			// user full name
			if(request.getUser())
			{
				pm.insert(DATA_USER_FULL_NAME, request.getUser()->getFullName());
			}

			// version
			pm.insert(DATA_VERSION, ServerModule::VERSION);

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void AdminFunction::_displayAdminPosElement(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const AdminInterfaceElement> link,
			size_t depth,
			bool isLast
		) const {

			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(_positionElementTemplate);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			// title, icon, url
			ExportAdminCompound(pm, request, link);

			// depth
			pm.insert(DATA_DEPTH, depth);

			// is_last
			pm.insert(DATA_IS_LAST, isLast);

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void AdminFunction::_displayAdminTreeNode(
			std::ostream& stream,
			const server::Request& request,
			const AdminInterfaceElement::PageLinksTree& tree,
			boost::shared_ptr<const AdminInterfaceElement> currentCompound,
			std::size_t depth,
			bool isLast
		) const {

			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(_treeNodeTemplate);
			displayRequest.getFunction()->setUseTemplate(false);
			ParametersMap pm(
				dynamic_cast<const WebPageDisplayFunction*>(request.getFunction().get()) ?
				dynamic_cast<const WebPageDisplayFunction&>(*request.getFunction()).getAditionnalParametersMap() :
				ParametersMap()
			);

			// icon, title, url
			ExportAdminCompound(pm, request, tree.page);

			// depth
			pm.insert(DATA_DEPTH, depth);

			// subtree
			stringstream subtreeStream;
			for(vector<AdminInterfaceElement::PageLinksTree>::const_iterator it(tree.subPages.begin());
				it != tree.subPages.end();
				++it
			){
				_displayAdminTreeNode(
					subtreeStream,
					request,
					*it,
					currentCompound,
					depth+1,
					it+1==tree.subPages.end()
				);
			}
			pm.insert(DATA_SUBTREE, subtreeStream.str());

			// is_opened
			pm.insert(DATA_IS_OPENED, tree.isNodeOpened);

			// is_last
			pm.insert(DATA_IS_LAST, isLast);

			// is_current
			pm.insert(DATA_IS_CURRENT, *tree.page == *currentCompound);

			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}



		void AdminFunction::ExportAdminCompound(
			server::ParametersMap& pm,
			const server::Request& request,
			boost::shared_ptr<const AdminInterfaceElement> compound,
			std::string prefix /*= std::string() */
		){
			// icon
			pm.insert(prefix + DATA_ICON, compound->getIcon());
			
			// title
			pm.insert(prefix + DATA_TITLE, compound->getTitle());
			
			// url
			AdminRequest r(request, true);
			r.getFunction()->setPage(const_pointer_cast<AdminInterfaceElement>(compound));
			pm.insert(prefix + DATA_URL, r.getURL());
		}
}	}
