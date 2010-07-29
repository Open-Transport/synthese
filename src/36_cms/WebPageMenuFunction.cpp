
/** WebPageMenuFunction class implementation.
	@file WebPageMenuFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "WebPageMenuFunction.hpp"
#include "Webpage.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"
#include "WebPageTableSync.h"
#include "HTMLModule.h"
#include "CMSModule.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace html;

	template<> const string util::FactorableTemplate<Function,cms::WebPageMenuFunction>::FACTORY_KEY("menu");
	
	namespace cms
	{
		const std::string WebPageMenuFunction::PARAMETER_ROOT_ID("root");
		const std::string WebPageMenuFunction::PARAMETER_MIN_DEPTH("min_depth");
		const std::string WebPageMenuFunction::PARAMETER_MAX_DEPTH("max_depth");
		const std::string WebPageMenuFunction::PARAMETER_BEGINNING("beginning_");
		const std::string WebPageMenuFunction::PARAMETER_ENDING("ending_");
		const std::string WebPageMenuFunction::PARAMETER_BEGINNING_SELECTED("beginning_selected_");
		const std::string WebPageMenuFunction::PARAMETER_ENDING_SELECTED("ending_selected_");
		const std::string WebPageMenuFunction::PARAMETER_BEGINNING_BEFORE_SUBMENU("beginning_before_submenu_");
		const std::string WebPageMenuFunction::PARAMETER_ENDING_AFTER_SUBMENU("ending_after_submenu_");
		
		ParametersMap WebPageMenuFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_ROOT_ID, _root.get() ? _root->getKey() : RegistryKeyType(0));
			map.insert(PARAMETER_MIN_DEPTH, _minDepth);
			map.insert(PARAMETER_MAX_DEPTH, _maxDepth);
			for(std::map<size_t,MenuDefinition_>::const_iterator it(_menuDefinition.begin()); it!=_menuDefinition.end(); ++it)
			{
				map.insert(PARAMETER_BEGINNING + lexical_cast<string>(it->first), it->second.beginning);
				map.insert(PARAMETER_ENDING + lexical_cast<string>(it->first), it->second.ending);
				map.insert(PARAMETER_BEGINNING_SELECTED + lexical_cast<string>(it->first), it->second.beginningSelected);
				map.insert(PARAMETER_ENDING_SELECTED + lexical_cast<string>(it->first), it->second.endingSelected);
				map.insert(PARAMETER_BEGINNING_BEFORE_SUBMENU + lexical_cast<string>(it->first), it->second.beginningBeforeSubmenu);
				map.insert(PARAMETER_ENDING_AFTER_SUBMENU + lexical_cast<string>(it->first), it->second.endingAfterSubmenu);
			}
			return map;
		}

		void WebPageMenuFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_rootId = map.getOptional<RegistryKeyType>(PARAMETER_ROOT_ID);
			if(_rootId)
			{
				if(*_rootId > 0) try
				{
					_root = Env::GetOfficialEnv().get<Webpage>(*_rootId);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such root page");
				}
			}

			_minDepth = map.getDefault<size_t>(PARAMETER_MIN_DEPTH, 1);
			_maxDepth = map.getDefault<size_t>(PARAMETER_MAX_DEPTH, _minDepth);

			for(size_t i(_minDepth); i<=_maxDepth; ++i)
			{
				MenuDefinition_ curMenuDefinition;
				curMenuDefinition.beginning = map.getDefault<string>(PARAMETER_BEGINNING + lexical_cast<string>(i));
				curMenuDefinition.ending = map.getDefault<string>(PARAMETER_ENDING + lexical_cast<string>(i));
				if(map.getOptional<string>(PARAMETER_BEGINNING_SELECTED + lexical_cast<string>(i)))
				{
					curMenuDefinition.beginningSelected = map.get<string>(PARAMETER_BEGINNING_SELECTED + lexical_cast<string>(i));
				}
				else
				{
					curMenuDefinition.beginningSelected = curMenuDefinition.beginning;
				}
				if(map.getOptional<string>(PARAMETER_ENDING_SELECTED + lexical_cast<string>(i)))
				{
					curMenuDefinition.endingSelected = map.getDefault<string>(PARAMETER_ENDING_SELECTED + lexical_cast<string>(i));
				}
				else
				{
					curMenuDefinition.endingSelected = curMenuDefinition.ending;
				}
				curMenuDefinition.beginningBeforeSubmenu = map.getDefault<string>(PARAMETER_BEGINNING_BEFORE_SUBMENU + lexical_cast<string>(i));
				curMenuDefinition.endingAfterSubmenu = map.getDefault<string>(PARAMETER_ENDING_AFTER_SUBMENU + lexical_cast<string>(i));
				_menuDefinition[i] = curMenuDefinition;
			}
		}

		void WebPageMenuFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			shared_ptr<const Webpage> currentPage(CMSModule::GetWebPage(request));
			_getMenuContentRecursive(
				stream,
				request,
				_rootId ? _root : currentPage,
				0,
				currentPage
			);
		}
		
		
		
		bool WebPageMenuFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPageMenuFunction::getOutputMimeType() const
		{
			return "text/html";
		}



		bool WebPageMenuFunction::_getMenuContentRecursive(
			std::ostream& stream,
			const server::Request& request /*= NULL*/,
			boost::shared_ptr<const Webpage> root,
			std::size_t depth,
			boost::shared_ptr<const Webpage> currentPage
		) const	{
			/** - Page in branch update */
			bool returned_page_in_branch(currentPage.get() ? (root == currentPage) : false);

			map<size_t, MenuDefinition_>::const_iterator it(_menuDefinition.find(depth));
			MenuDefinition_ menuDefinition(it == _menuDefinition.end() ? MenuDefinition_() : it->second);

			/** - Recursion attempting if :
			- the max depth is not reached
			- no masked level is defined or the masked level is superior to the current level or the current level is the submenu of the current branch
			*/
			stringstream submenu;

			WebPageTableSync::SearchResult pages(
				WebPageTableSync::Search(
					Env::GetOfficialEnv(),
					root.get() ? optional<RegistryKeyType>() : currentPage->getRoot()->getKey(),
					root.get() ? root->getKey() : optional<RegistryKeyType>(0)
			)	);
			BOOST_FOREACH(shared_ptr<Webpage> page, pages)
			{
				if(!page->mustBeDisplayed())
				{
					continue;
				}

				returned_page_in_branch |= _getMenuContentRecursive(
					submenu,
					request,
					const_pointer_cast<const Webpage>(page),
					depth + 1,
					currentPage
				);
			}

			/** - Preparing the output of the current level if the min depth is reached */
			if (root.get() && _minDepth <= depth)
			{
				stream <<
					(	(root == currentPage || returned_page_in_branch) ?
						menuDefinition.beginningSelected :
						menuDefinition.beginning
					);

				StaticFunctionRequest<WebPageDisplayFunction> subPageRequest(request, false);
				subPageRequest.getFunction()->setPage(root);
				stream << HTMLModule::getHTMLLink(subPageRequest.getURL(), root->getName());

				stream <<
					(	(root == currentPage || returned_page_in_branch) ?
						menuDefinition.endingSelected :
						menuDefinition.ending
					);
			}

			/** - Submenu only if applicable */
			if(!submenu.str().empty() && _maxDepth > depth)
			{
				stream <<
					menuDefinition.beginningBeforeSubmenu <<
					submenu.str() <<
					menuDefinition.endingAfterSubmenu
				;
			}

			return returned_page_in_branch;
		}



		WebPageMenuFunction::WebPageMenuFunction():
		_minDepth(0), _maxDepth(0)
		{

		}
	}
}
