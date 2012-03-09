
/** WebPageMenuFunction class implementation.
	@file WebPageMenuFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "WebPageMenuFunction.hpp"
#include "Webpage.h"
#include "StaticFunctionRequest.h"
#include "WebPageDisplayFunction.h"
#include "WebPageTableSync.h"
#include "HTMLModule.h"
#include "CMSModule.hpp"
#include "ServerModule.h"
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
		const string WebPageMenuFunction::DATA_RANK = "rank";
		const string WebPageMenuFunction::DATA_DEPTH = "depth";
		const string WebPageMenuFunction::DATA_IS_LAST_PAGE = "is_last_page";
		const string WebPageMenuFunction::DATA_IS_THE_CURRENT_PAGE = "is_the_current_page";
		const string WebPageMenuFunction::DATA_CURRENT_PAGE_IN_BRANCH = "current_page_in_branch";

		const std::string WebPageMenuFunction::PARAMETER_ROOT_ID("root");
		const std::string WebPageMenuFunction::PARAMETER_MIN_DEPTH("min_depth");
		const std::string WebPageMenuFunction::PARAMETER_MAX_DEPTH("max_depth");
		const std::string WebPageMenuFunction::PARAMETER_MAX_NUMBER("max_number");
		const std::string WebPageMenuFunction::PARAMETER_BEGINNING("beginning_");
		const std::string WebPageMenuFunction::PARAMETER_ENDING("ending_");
		const std::string WebPageMenuFunction::PARAMETER_BEGINNING_SELECTED("beginning_selected_");
		const std::string WebPageMenuFunction::PARAMETER_ENDING_SELECTED("ending_selected_");
		const std::string WebPageMenuFunction::PARAMETER_BEGINNING_BEFORE_SUBMENU("beginning_before_submenu_");
		const std::string WebPageMenuFunction::PARAMETER_ENDING_AFTER_SUBMENU("ending_after_submenu_");
		const std::string WebPageMenuFunction::PARAMETER_ITEM_PAGE_ID("item_page_id");
		const std::string WebPageMenuFunction::PARAMETER_OUTPUT_FORMAT = "output_format";
		const std::string WebPageMenuFunction::VALUE_RSS = "rss";



		ParametersMap WebPageMenuFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_ROOT_ID, _root.get() ? _root->getKey() : RegistryKeyType(0));
			map.insert(PARAMETER_MIN_DEPTH, _minDepth);
			map.insert(PARAMETER_MAX_DEPTH, _maxDepth);

			// Max number
			if(_maxNumber)
			{
				map.insert(PARAMETER_MAX_NUMBER, *_maxNumber);
			}

			// Output definition
			if(_itemPage.get())
			{
				map.insert(PARAMETER_ITEM_PAGE_ID, _itemPage->getKey());
			}
			else if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			else
			{
				for(std::map<size_t,MenuDefinition_>::const_iterator it(_menuDefinition.begin()); it!=_menuDefinition.end(); ++it)
				{
					map.insert(PARAMETER_BEGINNING + lexical_cast<string>(it->first), it->second.beginning);
					map.insert(PARAMETER_ENDING + lexical_cast<string>(it->first), it->second.ending);
					map.insert(PARAMETER_BEGINNING_SELECTED + lexical_cast<string>(it->first), it->second.beginningSelected);
					map.insert(PARAMETER_ENDING_SELECTED + lexical_cast<string>(it->first), it->second.endingSelected);
					map.insert(PARAMETER_BEGINNING_BEFORE_SUBMENU + lexical_cast<string>(it->first), it->second.beginningBeforeSubmenu);
					map.insert(PARAMETER_ENDING_AFTER_SUBMENU + lexical_cast<string>(it->first), it->second.endingAfterSubmenu);
				}
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

			// Max number
			_maxNumber = map.getOptional<size_t>(PARAMETER_MAX_NUMBER);

			// CMS output
			if(map.isDefined(PARAMETER_ITEM_PAGE_ID))
			{
				RegistryKeyType pageId(map.get<RegistryKeyType>(PARAMETER_ITEM_PAGE_ID));
				try
				{
					_itemPage = Env::GetOfficialEnv().get<Webpage>(pageId);
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such page "+ lexical_cast<string>(pageId));
				}
			}
			else if(!map.getDefault<string>(PARAMETER_OUTPUT_FORMAT).empty())
			{
				_outputFormat = map.get<string>(PARAMETER_OUTPUT_FORMAT);
			}
			else
			{
				// Old output (deprecated)
				for(size_t i(_minDepth); i<=_maxDepth; ++i)
				{
					MenuDefinition_ curMenuDefinition;
					curMenuDefinition.beginning = map.getDefault<string>(PARAMETER_BEGINNING + lexical_cast<string>(i), false);
					curMenuDefinition.ending = map.getDefault<string>(PARAMETER_ENDING + lexical_cast<string>(i), false);
					if(map.getOptional<string>(PARAMETER_BEGINNING_SELECTED + lexical_cast<string>(i), false))
					{
						curMenuDefinition.beginningSelected = map.get<string>(PARAMETER_BEGINNING_SELECTED + lexical_cast<string>(i), false);
					}
					else
					{
						curMenuDefinition.beginningSelected = curMenuDefinition.beginning;
					}
					if(map.getOptional<string>(PARAMETER_ENDING_SELECTED + lexical_cast<string>(i), false))
					{
						curMenuDefinition.endingSelected = map.getDefault<string>(PARAMETER_ENDING_SELECTED + lexical_cast<string>(i), false);
					}
					else
					{
						curMenuDefinition.endingSelected = curMenuDefinition.ending;
					}
					curMenuDefinition.beginningBeforeSubmenu = map.getDefault<string>(PARAMETER_BEGINNING_BEFORE_SUBMENU + lexical_cast<string>(i), false);
					curMenuDefinition.endingAfterSubmenu = map.getDefault<string>(PARAMETER_ENDING_AFTER_SUBMENU + lexical_cast<string>(i), false);
					_menuDefinition[i] = curMenuDefinition;
				}
			}
		}



		util::ParametersMap WebPageMenuFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Root page
			shared_ptr<const Webpage> currentPage(CMSModule::GetWebPage(request));
			shared_ptr<const Webpage> rootPage(_rootId ? _root : currentPage);

			// RSS header
			if(!_itemPage.get() && _outputFormat == VALUE_RSS)
			{
				StaticFunctionRequest<WebPageDisplayFunction> openRequest(request, false);
				openRequest.getFunction()->setPage(rootPage);

				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<rss version=\"2.0\" xmlns:atom=\"http://www.w3.org/2005/Atom\">" <<
					"<channel>" <<
					"<title>" << rootPage->getName() << "</title>" <<
					"<description><![CDATA[" << rootPage->get<Abstract>() << "]]></description>" <<
					"<link>" << HTMLModule::HTMLEncode(openRequest.getURL(true, true)) << "</link>" <<
					"<generator>SYNTHESE " << ServerModule::VERSION << "</generator>"
				;
				if(!rootPage->get<ImageURL>().empty())
				{
					stream << "<image><url>" << rootPage->get<ImageURL>() << "</url><title>" <<
						rootPage->get<Title>() << "</title><link>" <<
						HTMLModule::HTMLEncode(openRequest.getURL(true, true)) << "</link></image>"
					;
				}
				stream << "<atom:link href=\"" << HTMLModule::HTMLEncode(openRequest.getURL(true, true)) << "\" rel=\"self\" type=\"application/rss+xml\" />";
			}

			// Content
			_getMenuContentRecursive(
				stream,
				request,
				rootPage,
				0,
				currentPage,
				0,
				true
			);

			// RSS footer
			if(!_itemPage.get() && _outputFormat == VALUE_RSS)
			{
				stream << "</channel></rss>";
			}

			return util::ParametersMap();
		}



		bool WebPageMenuFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string WebPageMenuFunction::getOutputMimeType() const
		{
			if(_itemPage.get())
			{
				return _itemPage->getMimeType();
			}
			else if(_outputFormat == VALUE_RSS)
			{
				return "application/rss+xml";
			}
			return "text/html";
		}



		bool WebPageMenuFunction::_getMenuContentRecursive(
			std::ostream& stream,
			const server::Request& request /*= NULL*/,
			boost::shared_ptr<const Webpage> root,
			std::size_t depth,
			boost::shared_ptr<const Webpage> currentPage,
			size_t rank,
			bool isLastPage
		) const	{
			/** - Page in branch update */
			bool returned_page_in_branch(currentPage.get() ? (root == currentPage) : false);

			/** - Recursion attempting if :
			- the max depth is not reached
			- no masked level is defined or the masked level is superior to the current level or the current level is the submenu of the current branch
			*/
			stringstream submenu;

			size_t number(0);
			WebPageTableSync::SearchResult pages(
				WebPageTableSync::Search(
					Env::GetOfficialEnv(),
					root.get() ? optional<RegistryKeyType>() : currentPage->getRoot()->getKey(),
					root.get() ? root->getKey() : optional<RegistryKeyType>(0)
			)	);
			for(WebPageTableSync::SearchResult::const_iterator it(pages.begin()); it != pages.end(); ++it)
			{
				shared_ptr<Webpage> page(*it);

				// Avoid no displayed pages
				if(!page->mustBeDisplayed())
				{
					continue;
				}

				// Is it the last page of the menu ?
				bool isLastSubPage(
					it+1 == pages.end() ||
					(_maxNumber && number+1 == *_maxNumber)
				);

				// Recursion
				returned_page_in_branch |= _getMenuContentRecursive(
					submenu,
					request,
					const_pointer_cast<const Webpage>(page),
					depth + 1,
					currentPage,
					number,
					isLastSubPage
				);

				// End of loop if last page
				number++;
				if(isLastSubPage)
				{
					break;
				}
			}

			map<size_t, MenuDefinition_>::const_iterator it(_menuDefinition.find(depth));
			MenuDefinition_ menuDefinition(it == _menuDefinition.end() ? MenuDefinition_() : it->second);

			/** - Preparing the output of the current level if the min depth is reached */
			if (root.get() && _minDepth <= depth)
			{
				StaticFunctionRequest<WebPageDisplayFunction> subPageRequest(request, false);
				subPageRequest.getFunction()->setPage(root);

				if(_itemPage.get())
				{
					ParametersMap pm(getTemplateParameters());
					root->toParametersMap(pm);
					pm.insert(DATA_RANK, rank);
					pm.insert(DATA_DEPTH, depth);
					pm.insert(DATA_IS_THE_CURRENT_PAGE, root == currentPage);
					pm.insert(DATA_IS_LAST_PAGE, isLastPage);
					pm.insert(DATA_CURRENT_PAGE_IN_BRANCH, returned_page_in_branch);
					_itemPage->display(stream, request, pm);
				}
				else if(_outputFormat == VALUE_RSS)
				{
					stream <<
						"<item>" <<
						"<title>" << root->get<Title>() << "</title>" <<
						"<description><![CDATA[" << root->get<Abstract>() << "]]></description>" <<
						"<link>" << HTMLModule::HTMLEncode(subPageRequest.getURL(true, true)) << "</link>" <<
						"<guid isPermaLink=\"true\">" << HTMLModule::HTMLEncode(subPageRequest.getURL(true, true)) << "</guid>"
					;
					if(!root->get<StartTime>().is_not_a_date_time())
					{
						stream <<
							"<pubDate>" <<
							root->get<StartTime>().date().day_of_week() << ", " <<
							root->get<StartTime>().date().day() << " " <<
							root->get<StartTime>().date().month() << " " <<
							root->get<StartTime>().date().year() << " " <<
							root->get<StartTime>().time_of_day() << " " <<
							"+0100" <<
							"</pubDate>"
						;
					}
					stream << "</item>";
				}
				else
				{
					stream <<
						(	(root == currentPage || returned_page_in_branch) ?
							menuDefinition.beginningSelected :
							menuDefinition.beginning
						)
					;

					stream <<
						HTMLModule::getHTMLLink(
							root->get<SmartURLPath>().empty() ? subPageRequest.getURL() : root->get<SmartURLPath>(), root->get<Title>()
						)
					;

					stream <<
						(	(root == currentPage || returned_page_in_branch) ?
							menuDefinition.endingSelected :
							menuDefinition.ending
						)
					;
				}
			}

			/** - Submenu only if applicable */
			if(	!submenu.str().empty() &&
				_maxDepth > depth
			){
				if(!_itemPage.get() && _outputFormat.empty())
				{
					stream << menuDefinition.beginningBeforeSubmenu;
				}
				stream << submenu.str();
				if(!_itemPage.get() && _outputFormat.empty())
				{
					stream << menuDefinition.endingAfterSubmenu;
				}
			}

			return returned_page_in_branch;
		}



		WebPageMenuFunction::WebPageMenuFunction():
		_minDepth(0), _maxDepth(0)
		{

		}
}	}
