
/** ForEachExpression class implementation.
	@file ForEachExpression.cpp

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

#include "ForEachExpression.hpp"

#include "ParametersMap.h"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace cms
	{
		const string ForEachExpression::DATA_ITEMS_COUNT = "items_count";
		const string ForEachExpression::DATA_RANK = "rank";
		const string ForEachExpression::PARAMETER_EMPTY = "empty";
		const string ForEachExpression::PARAMETER_SORT_DOWN = "sort_down";
		const string ForEachExpression::PARAMETER_SORT_UP = "sort_up";
		const string ForEachExpression::PARAMETER_TEMPLATE = "template";
		const string ForEachExpression::PARAMETER_RECURSIVE = "recursive";
		const string ForEachExpression::DATA_RECURSIVE_CONTENT = "recursive_content";



		ForEachExpression::ForEachExpression(
			std::string::const_iterator& it,
			std::string::const_iterator end
		):	_recursive(false)
		{
			// function name
			for(;it != end && *it != '&' && *it != '}'; ++it)
			{
				_arrayCode.push_back(*it);
			}
			_arrayCode = ParametersMap::Trim(_arrayCode);

			// parameters
			if(it != end && *it == '}')
			{
				it += 2;
			}
			else
			{
				set<string> functionTermination;
				functionTermination.insert("&");
				functionTermination.insert("}>");
				while(it != end && *it == '&')
				{
					stringstream parameterName;
					++it;
					ParseText(parameterName, it, end, "=");

					if(it != end)
					{
						WebpageContent parameterNodes(it, end, functionTermination);
						string parameterNameStr(ParametersMap::Trim(parameterName.str()));
						
						if(parameterNameStr == WebPageDisplayFunction::PARAMETER_PAGE_ID)
						{
							_pageCode = parameterNodes;
						}
						else if(parameterNameStr == PARAMETER_TEMPLATE)
						{
							_inlineTemplate = parameterNodes;
						}
						else if(parameterNameStr == PARAMETER_RECURSIVE)
						{
							_recursive = true;
						}
						else if(parameterNameStr == PARAMETER_EMPTY)
						{
							_emptyTemplate = parameterNodes;
						}
						else if(parameterNameStr == PARAMETER_SORT_DOWN)
						{
							_sortDownTemplate = parameterNodes;
						}
						else if(parameterNameStr == PARAMETER_SORT_UP)
						{
							_sortUpTemplate = parameterNodes;
						}
						else
						{
							_parameters.push_back(
								make_pair(
									parameterNameStr,
									parameterNodes
							)	);
						}

						if(*(it-1) != '&')
						{
							break;
						}
						--it;
					}
			}	}
		}



		void ForEachExpression::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			// Page load
			const Webpage* templatePage(NULL);
			if(_inlineTemplate.empty())
			{
				string pageCodeStr(
					_pageCode.eval(request, additionalParametersMap, page, variables)
				);

				if(pageCodeStr.empty())
				{
					return;
				}

				const Webpage* templatePage(
					page.getRoot()->getPageByIdOrSmartURL(pageCodeStr)
				);
				if(!templatePage)
				{
					return;
				}
			}

			// Base parameters map
			ParametersMap baseParametersMap(additionalParametersMap);
			BOOST_FOREACH(const Parameters::value_type& param, _parameters)
			{
				baseParametersMap.insert(
					param.first,
					param.second.eval(request, additionalParametersMap, page, variables)
				);
			}

			// No items to display
			if(	!additionalParametersMap.hasSubMaps(_arrayCode) &&
				!variables.hasSubMaps(_arrayCode)
			){
				_emptyTemplate.display(stream, request, baseParametersMap, page, variables);
				return;
			}

			// Items read
			const ParametersMap::SubParametersMap::mapped_type& items(
				additionalParametersMap.hasSubMaps(_arrayCode) ?
				additionalParametersMap.getSubMaps(_arrayCode) :
				variables.getSubMaps(_arrayCode)
			);
			size_t itemsCount(items.size());

			// Sorting items : building the sorting key
			typedef map<string, shared_ptr<ParametersMap> > SortedItems;
			SortedItems sortedItems;
			if(!_sortUpTemplate.empty() || !_sortDownTemplate.empty())
			{
				// Loop on items
				size_t rank(0);
				string noRecursive;
				BOOST_FOREACH(const ParametersMap::SubParametersMap::mapped_type::value_type& item, items)
				{
					stringstream key;

					_displayItem(
						key,
						request,
						page,
						baseParametersMap,
						*item,
						variables,
						_sortUpTemplate.empty() ? _sortDownTemplate : _sortUpTemplate,
						NULL,
						rank,
						itemsCount,
						noRecursive
					);

					// Insertion in the map
					sortedItems.insert(make_pair(key.str(), item));
				}
			}

			size_t rank(0);
			
			// Case items are sorted ascending
			if(!_sortUpTemplate.empty())
			{
				BOOST_FOREACH(const SortedItems::value_type item, sortedItems)
				{
					stringstream recursiveContent;

					// Recursion
					if(_recursive && item.second->hasSubMaps(_arrayCode))
					{
						display(
							recursiveContent,
							request,
							*item.second,
							page,
							variables
						);
					}

					_displayItem(
						stream,
						request,
						page,
						baseParametersMap,
						*item.second,
						variables,
						optional<const WebpageContent&>(),
						templatePage,
						rank,
						itemsCount,
						recursiveContent.str()
					);
				}
			}
			// Case items are sorted descending
			else if(!_sortDownTemplate.empty())
			{
				BOOST_REVERSE_FOREACH(const SortedItems::value_type item, sortedItems)
				{
					stringstream recursiveContent;

					// Recursion
					if(_recursive && item.second->hasSubMaps(_arrayCode))
					{
						display(
							recursiveContent,
							request,
							*item.second,
							page,
							variables
						);
					}

					_displayItem(
						stream,
						request,
						page,
						baseParametersMap,
						*item.second,
						variables,
						optional<const WebpageContent&>(),
						templatePage,
						rank,
						itemsCount,
						recursiveContent.str()
					);
				}
			}
			// Case items are read in the same order as in the service result
			else
			{
				BOOST_FOREACH(const ParametersMap::SubParametersMap::mapped_type::value_type& item, items)
				{
					stringstream recursiveContent;

					// Recursion
					if(_recursive && item->hasSubMaps(_arrayCode))
					{
						display(
							recursiveContent,
							request,
							*item,
							page,
							variables
						);
					}

					_displayItem(
						stream,
						request,
						page,
						baseParametersMap,
						*item,
						variables,
						optional<const WebpageContent&>(),
						templatePage,
						rank,
						itemsCount,
						recursiveContent.str()
					);
				}
			}
		}



		void ForEachExpression::_displayItem(
			std::ostream& stream,
			const server::Request& request,
			const Webpage& page,
			const util::ParametersMap& baseParametersMap,
			const util::ParametersMap& item,
			util::ParametersMap& variables,
			optional<const WebpageContent&> templateContent,
			const Webpage* templatePage,
			size_t& rank,
			size_t itemsCount,
			const std::string& recursiveContent
		) const {
			ParametersMap pm(item);
			pm.merge(baseParametersMap);
			pm.insert(DATA_RANK, rank++);
			pm.insert(DATA_ITEMS_COUNT, itemsCount);
			if(!recursiveContent.empty())
			{
				pm.insert(DATA_RECURSIVE_CONTENT, recursiveContent);
			}

			// Display by a template page
			if(templateContent)
			{
				templateContent->display(stream, request, pm, page, variables);
			}
			else if(templatePage)
			{
				templatePage->display(stream, request, pm, variables);
			}
			else // Display by an inline defined template
			{
				_inlineTemplate.display(stream, request, pm, page, variables);
			}
		}



		std::string ForEachExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			stringstream s;
			display(s, request, additionalParametersMap, page, variables);
			return s.str();
		}
}	}

