
//////////////////////////////////////////////////////////////////////////////////////////
///	AdminPageDisplayService class implementation.
///	@file AdminPageDisplayService.cpp
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

#include "AdminPageDisplayService.hpp"

#include "AdminParametersException.h"
#include "HTMLList.h"
#include "HTMLModule.h"
#include "HTMLPage.hpp"
#include "LoginAction.h"
#include "ModuleAdmin.h"
#include "ModuleClass.h"
#include "Profile.h"
#include "RequestException.h"
#include "Request.h"
#include "Session.h"
#include "StaticActionFunctionRequest.h"
#include "URI.hpp"
#include "User.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace html;
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,admin::AdminPageDisplayService>::FACTORY_KEY = "admin_page";
	
	namespace admin
	{
		const string AdminPageDisplayService::PARAMETER_NO_CMS = "no_cms";
		const string AdminPageDisplayService::PARAMETER_PAGE = "rub";
		const string AdminPageDisplayService::PARAMETER_TAB = "tab";
		const string AdminPageDisplayService::PARAMETER_ACTION_FAILED_PAGE = "afp";
		const string AdminPageDisplayService::PARAMETER_ACTION_FAILED_TAB = "aft";

		const string AdminPageDisplayService::DATA_CONTENT = "content";
		const string AdminPageDisplayService::DATA_TABS = "tabs";

		const string AdminPageDisplayService::TAG_TREE_NODE = "tree_node";
		const string AdminPageDisplayService::TAG_POSITION_NODE = "position_node";

		const string AdminPageDisplayService::DATA_DEPTH = "depth";
		const string AdminPageDisplayService::DATA_IS_LAST = "is_last";

		const string AdminPageDisplayService::DATA_SUBTREE = "subtree";
		const string AdminPageDisplayService::DATA_IS_OPENED = "is_opened";
		const string AdminPageDisplayService::DATA_IS_CURRENT = "is_current";


		ParametersMap AdminPageDisplayService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.merge(_page->getParametersMap());
				map.insert(PARAMETER_PAGE, _page->getFactoryKey());
				map.insert(PARAMETER_TAB, _page->getActiveTab());
			}
			if(_noCMS)
			{
				map.insert(PARAMETER_NO_CMS, _noCMS);
			}
			return map;
		}



		void AdminPageDisplayService::_setFromParametersMap(const ParametersMap& map)
		{
			// Page and tabs
			try
			{
				string pageKey(
					map.getDefault<string>(PARAMETER_PAGE)
				);
				if(!pageKey.empty())
				{
					_page.reset(
						Factory<AdminInterfaceElement>::create(pageKey)
					);
					_page->setEnv(boost::shared_ptr<Env>(new Env));
					_page->setFromParametersMap(map);
					_page->setActiveTab(map.getDefault<string>(PARAMETER_TAB));
				}
			}
			catch (FactoryException<AdminInterfaceElement> e)
			{
				throw RequestException("Admin page not found");
			}
			catch (AdminParametersException e)
			{
				throw RequestException("Admin page parameters error : " + e.getMessage());
			}

			// Display without CMS
			_noCMS = map.getDefault<bool>(PARAMETER_NO_CMS, false);
		}



		ParametersMap AdminPageDisplayService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			try
			{
				if(request.getSession() && _page.get())
				{
					_page->_buildTabs(*request.getSession()->getUser()->getProfile());

					// title, icon, url
					_page->toParametersMap(pm);

					// tree
					const AdminInterfaceElement::PageLinksTree& tree(
						_page->getTree(request)
					);
					for(vector<AdminInterfaceElement::PageLinksTree>::const_iterator it(tree.subPages.begin());
						it != tree.subPages.end();
						++it
					){
						_getTreeNode(
							pm,
							*it,
							_page,
							1,
							it+1==tree.subPages.end()
						);
					}

					// position
					const AdminInterfaceElement::PageLinks& links(_page->getTreePosition(request));
					for (AdminInterfaceElement::PageLinks::const_iterator it(links.begin()); it != links.end(); ++it)
					{
						pm.insert(
							TAG_POSITION_NODE,
							_getPositionNode(*it, it - links.begin(), it+1 == links.end())
						);
					}
					
					// tabs
					stringstream tabsStream;
					_page->displayTabs(
						tabsStream,
						request
					);
					pm.insert(DATA_TABS, tabsStream.str());

					// content
					stringstream contentStream;
					_page->display(
						contentStream,
						request
					);
					pm.insert(DATA_CONTENT, contentStream.str());

					BOOST_FOREACH(const boost::shared_ptr<ModuleClass>& module, Factory<ModuleClass>::GetNewCollection())
					{
						module->addAdminPageParameters(pm, request);
					}
				}
			}
			catch (Exception& e)
			{
				throw RequestException("Admin interface page error: " + e.getMessage());
			}

			// Display without CMS
			if(_noCMS)
			{
				HTMLPage p;
				p.setInlineCSS("body { font-family:Arial, Helvetica; }");
				p.setTitle(pm.getDefault<string>(AdminInterfaceElement::DATA_TITLE));
				
				if(request.getSession())
				{
					// No content : display sub pages list
					if(pm.getDefault<string>(DATA_CONTENT).empty())
					{
						StaticFunctionRequest<AdminPageDisplayService> r(request, true);

						// Modules list
						vector<boost::shared_ptr<ModuleClass> > modules(
							Factory<ModuleClass>::GetNewCollection()
						);
						HTMLList l;
						p << l.open();
						for(vector<boost::shared_ptr<ModuleClass> >::const_reverse_iterator it(modules.rbegin()); it != modules.rend(); ++it)
						{
							boost::shared_ptr<ModuleAdmin> link(new ModuleAdmin);
							link->setModuleClass(*it);
							if (!link->getSubPages(*link, request).empty())
							{
								ParametersMap pm;
								link->toParametersMap(pm);
								p <<
									l.element() <<
									HTMLModule::getHTMLLink(
										r.getURL() + URI::PARAMETER_SEPARATOR + pm.getDefault<string>(AdminInterfaceElement::DATA_URL),
										pm.getDefault<string>(AdminInterfaceElement::DATA_TITLE)
									)
								;
							}
						}
						p << l.close();
					}
					else
					{
						if(pm.hasSubMaps(TAG_POSITION_NODE))
						{
							StaticFunctionRequest<AdminPageDisplayService> r(request, true);
							p << "<p>| ";
							BOOST_FOREACH(const ParametersMap::SubParametersMap::mapped_type::value_type& item, pm.getSubMaps(TAG_POSITION_NODE))
							{
								p << HTMLModule::getHTMLLink(
									r.getURL() + URI::PARAMETER_SEPARATOR + item->getDefault<string>(AdminInterfaceElement::DATA_URL),
									item->getDefault<string>(AdminInterfaceElement::DATA_TITLE)
								) << " | "; 
							}
							p << "</p>";
						}

						p << pm.getDefault<string>(DATA_CONTENT);
					}
				}
				else
				{
					p << "<p>Veuillez vous connecter pour utiliser cette fonction</p>";

					StaticActionFunctionRequest<LoginAction, AdminPageDisplayService> loginRequest(request, true);
					HTMLForm f(loginRequest.getHTMLForm());
					p << f.open();
					p << "Utilisateur : " << f.getTextInput(LoginAction::PARAMETER_LOGIN, string());
					p << " Mot de passe : " << f.getPasswordInput(LoginAction::PARAMETER_PASSWORD, string());
					p << " " << f.getSubmitButton("Login");
					p << f.close();
				}

				p.output(stream);
			}


			return pm;
		}



		boost::shared_ptr<ParametersMap> AdminPageDisplayService::_getPositionNode(
			boost::shared_ptr<const AdminInterfaceElement> link,
			size_t depth,
			bool isLast
		) const {

			boost::shared_ptr<ParametersMap> pm(new ParametersMap);

			// title, icon, url
			link->toParametersMap(*pm);

			// depth
			pm->insert(DATA_DEPTH, depth);

			// is_last
			pm->insert(DATA_IS_LAST, isLast);

			return pm;
		}



		void AdminPageDisplayService::_getTreeNode(
			ParametersMap& pm,
			const AdminInterfaceElement::PageLinksTree& tree,
			boost::shared_ptr<const AdminInterfaceElement> currentCompound,
			std::size_t depth,
			bool isLast
		) const {

			boost::shared_ptr<ParametersMap> nodePM(new ParametersMap);

			// icon, title, url
			tree.page->toParametersMap(*nodePM);

			// depth
			nodePM->insert(DATA_DEPTH, depth);

			// subtree
			stringstream subtreeStream;
			for(vector<AdminInterfaceElement::PageLinksTree>::const_iterator it(tree.subPages.begin());
				it != tree.subPages.end();
				++it
			){
				_getTreeNode(
					*nodePM,
					*it,
					currentCompound,
					depth+1,
					it+1==tree.subPages.end()
				);
			}
			nodePM->insert(DATA_SUBTREE, subtreeStream.str());

			// is_opened
			nodePM->insert(DATA_IS_OPENED, tree.isNodeOpened);

			// is_last
			nodePM->insert(DATA_IS_LAST, isLast);

			// is_current
			nodePM->insert(DATA_IS_CURRENT, *tree.page == *currentCompound);

			pm.insert(TAG_TREE_NODE, nodePM);
		}

		
		
		bool AdminPageDisplayService::isAuthorized(
			const Session* session
		) const {
			return
				_noCMS ||
				(	session &&
					session->hasProfile() &&
					(	!_page.get() ||
						_page->isAuthorized(*session->getUser())
				)	)
			;
		}



		std::string AdminPageDisplayService::getOutputMimeType() const
		{
			return "text/html";
		}



		AdminPageDisplayService::AdminPageDisplayService():
			_noCMS(false)
		{}
}	}
