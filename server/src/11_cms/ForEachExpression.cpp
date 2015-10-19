
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
#include "VariableExpression.hpp"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"
#include "alphanum.hpp"

#include <sstream>
#include <boost/foreach.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace cms
	{
		const string ForEachExpression::DATA_ITEMS_COUNT = "items_count";
		const string ForEachExpression::DATA_RANK = "item_rank";
		const string ForEachExpression::DATA_DEPTH = "depth";
		const string ForEachExpression::PARAMETER_EMPTY = "empty";
		const string ForEachExpression::PARAMETER_SORT_DOWN = "sort_down";
		const string ForEachExpression::PARAMETER_SORT_UP = "sort_up";
		const string ForEachExpression::PARAMETER_SORT_ALGO = "sort_algo";
		const string ForEachExpression::PARAMETER_MAX_PER_SORT_KEY = "max_per_sort_key";
		const string ForEachExpression::PARAMETER_RESULTS_IN_A_SUBMAP = "results_in_a_submap";
		const string ForEachExpression::PARAMETER_TEMPLATE = "template";
		const string ForEachExpression::PARAMETER_RECURSIVE = "recursive";
		const string ForEachExpression::DATA_RECURSIVE_CONTENT = "recursive_content";

		/// @FIXME Expressions does not appear in ServiceAPIListService.
		FunctionAPI ForEachExpression::getAPI() const
		{
			FunctionAPI api(
				"CMS Language",
				"Parameters map iteration",
				"You provide the name of a sub parameters map as the first parameter "
				"and a template that will be evaluated for each item in this sub map.\n"
				"Basic example:\n"
				"  <?a_service_that_creates_a_submap&\n"
				"    template=<{a_sub_map_entry&template=<@@>}>\n"
				"  ?>"
			);

			api.addParams(PARAMETER_EMPTY, "Specify a template to use when there is no data", false);
			api.addParams(PARAMETER_SORT_DOWN,
						  "The name of a field in the sub map used as the key for a down sort", false);
			api.addParams(PARAMETER_SORT_UP,
						  "The name of a field in the sub map used as the key for an up sort", false);
			api.addParams(PARAMETER_SORT_ALGO, "The sort algorithm, can be 'default' (the faster), "
						  "'alphanumeric' or 'alphanumeric_text_first'",
						  false);
			api.addParams(PARAMETER_MAX_PER_SORT_KEY, "Maximum items per sort key. "
						  "If the sort key contains the character '|', only the left part "
						  "of the key is concerned by this field.", false);
			api.addParams(PARAMETER_RESULTS_IN_A_SUBMAP,
						  "Returns the results in a new submap named 'foreach_results'", false);
			api.addParams(PARAMETER_TEMPLATE, "", false);
			api.addParams(PARAMETER_RECURSIVE, "Apply the template recursively", false);
			return api;
		}


		ForEachExpression::ForEachExpression(
			std::string::const_iterator& it,
			std::string::const_iterator end,
			bool ignoreWhiteChars
		):	_resultsInASubmap(false),
			_recursive(false)
		{
			// function name
			string arrayCode;
			for(;it != end && *it != '&' && *it != '}'; ++it)
			{
				arrayCode.push_back(*it);
			}
			arrayCode = ParametersMap::Trim(arrayCode);
			Item item;
			_variable.push_back(item);

			// Variable name parsing
			string::const_iterator it2(arrayCode.begin());
			while(it2 != arrayCode.end())
			{
				// Alphanum chars
				if( (*it2 >= 'a' && *it2 <= 'z') ||
					(*it2 >= 'A' && *it2 <= 'Z') ||
					(*it2 >= '0' && *it2 <= '9') ||
					*it2 == '_'
				){
					_variable.rbegin()->key.push_back(*it2);
					++it2;
				}
				else if(*it2 == '[')
				{	// Index
					_variable.rbegin()->index = Expression::Parse(it2, arrayCode.end(), "]");
				}
				else if(*it2 == '.')
				{	// Sub map
					Item item;
					_variable.push_back(item);
					++it2;
				}
				else
				{
					Log::GetInstance().error("Syntax error in script '" + arrayCode + "'");
					return;
				}
			}

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
						CMSScript parameterNodes(it, end, functionTermination, ignoreWhiteChars);
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
						else if(parameterNameStr == PARAMETER_SORT_ALGO)
						{
							// One of:
							// default (the faster)
							// alphanumeric
							// alphanumeric_text_first
							_sortAlgoNode = parameterNodes;
						}
						else if(parameterNameStr == PARAMETER_MAX_PER_SORT_KEY)
						{
							_maxPerSortKey = parameterNodes;
						}
						else if(parameterNameStr == PARAMETER_RESULTS_IN_A_SUBMAP)
						{
							_resultsInASubmap = true;
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
			util::ParametersMap& variables,
			size_t depth
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


			// Loop on items
			const ParametersMap* pm(NULL);
			for(size_t rank(0); (rank+1) < _variable.size(); ++rank)
			{
				const Item& item(_variable[rank]);

				// If first rank, select the source map
				if(rank == 0)
				{
					// Variable first
					if(variables.hasSubMaps(item.key))
					{
						pm = &variables;
					} // Else parameters
					else if(additionalParametersMap.hasSubMaps(item.key))
					{
						pm = &additionalParametersMap;
					}
					else // Sub map does not exist
					{
						break;
					}
				}

				// Submap check
				if(!pm->hasSubMaps(item.key))
				{
					break;
				}

				// If no index specified, take the first submap item
				if(!item.index.get())
				{
					pm = pm->getSubMaps(item.key).begin()->get();
				}
				else // Else search for the specified item
				{
					string idx(
						item.index->eval(request,additionalParametersMap,page,variables)
					);
					const ParametersMap::SubParametersMap::mapped_type& subMaps(
						pm->getSubMaps(item.key)
					);
					pm = NULL;
					BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& subMapItem, subMaps)
					{
						if(subMapItem->getDefault<string>(VariableExpression::FIELD_ID) == idx)
						{
							pm = subMapItem.get();
							break;
						}
					}

					// Index was not found
					if(!pm)
					{
						break;
					}
				}
			}
			const string& arrayCode(_variable.rbegin()->key);

			// No items to display
			if(	(	pm &&
					!pm->hasSubMaps(arrayCode)
				) || (
					!pm &&
					!additionalParametersMap.hasSubMaps(arrayCode) &&
					!variables.hasSubMaps(arrayCode)
				)
			){
				_emptyTemplate.display(stream, request, baseParametersMap, page, variables);
				return;
			}

			// Items read
			const ParametersMap::SubParametersMap::mapped_type& items(
				pm ?
				pm->getSubMaps(arrayCode) :
				(
					additionalParametersMap.hasSubMaps(arrayCode) ?
					additionalParametersMap.getSubMaps(arrayCode) :
					variables.getSubMaps(arrayCode)
				)
			);
			size_t itemsCount(items.size());

			// Sorting items : building the sorting key
			typedef multimap<string, boost::shared_ptr<ParametersMap>, 
					boost::function<bool(const string &, const string &)> > SortedItems;

			string sortAlgoStr(
				_sortAlgoNode.eval(request, additionalParametersMap, page, variables)
			);

			string maxPerSortKeyString(_maxPerSortKey.eval(request, additionalParametersMap, page, variables));
			if(maxPerSortKeyString.empty())
			{
				maxPerSortKeyString = "0";
			}
			size_t maxPerSortKeyValue(lexical_cast<size_t>(maxPerSortKeyString));

			util::alphanum_less<string> comparatorAlphanum;
			util::alphanum_text_first_less<string> comparatorAlphanumTextFirst;
			std::less<string> comparatorLess;
			boost::function<bool(const std::string &, const std::string &)> comparator;
			if(sortAlgoStr == "alphanumeric")
			{
				comparator = comparatorAlphanum;
			}
			else if(sortAlgoStr == "alphanumeric_text_first")
			{
				comparator = comparatorAlphanumTextFirst;
			}
			else
			{
				comparator = comparatorLess;
			}
			SortedItems sortedItems(boost::bind(comparator, _1, _2));
			if(!_sortUpTemplate.empty() || !_sortDownTemplate.empty())
			{
				// Loop on items
				size_t rank(0);
				string noRecursive;
				BOOST_FOREACH(const ParametersMap::SubParametersMap::mapped_type::value_type& item, items)
				{
					stringstream key;

					if(!item.get())
					{
						// A Null was added in a ParametersMap, escape it silently
						continue;
					}

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
						noRecursive,
						depth
					);

					// Insertion in the map
					sortedItems.insert(make_pair(key.str(), item));
				}

				// Remove multiple entries
				if(maxPerSortKeyValue)
				{
					size_t count(0);
					string previousKey;
					for(SortedItems::iterator it = sortedItems.begin();
						it != sortedItems.end(); /* no increment */)
					{
						string key(it->first);
						size_t found = key.find_first_of("|");
						if(found != std::string::npos)
						{
							key = key.substr(0, found);
						}

						if(key == previousKey)
						{
							count++;
							if(count >= maxPerSortKeyValue)
							{
								sortedItems.erase(it++);
							}
							else
							{
								it++;
							}
						}
						else
						{
							previousKey = key;
							count = 0;
							it++;
						}
					}

				}
			}

			size_t rank(0);

			// Return the result in a new submap in order to give a chance to the caller
			if(_resultsInASubmap)
			{
				boost::shared_ptr<ParametersMap> pm(new ParametersMap);
				if(!_sortUpTemplate.empty())
				{
					BOOST_FOREACH(const SortedItems::value_type item, sortedItems)
					{
						pm->insert("foreach_results", item.second);
					}
				}
				else if(!_sortDownTemplate.empty())
				{
					BOOST_REVERSE_FOREACH(const SortedItems::value_type item, sortedItems)

					{
						pm->insert("foreach_results", item.second);
					}
				}
				else
				{
					BOOST_FOREACH(const ParametersMap::SubParametersMap::mapped_type::value_type& item, items)
					{
						if(!item.get())
						{
							// A Null was added in a ParametersMap, escape it silently
							continue;
						}

						boost::shared_ptr<ParametersMap> subPm(item);
						pm->insert("foreach_results", subPm);
					}
				}

				stringstream recursiveContent;

				// Recursion
				if(_recursive)
				{
					display(
						recursiveContent,
						request,
						*pm,
						page,
						variables,
						depth + 1
					);
				}

				_displayItem(
					stream,
					request,
					page,
					baseParametersMap,
					*pm,
					variables,
					optional<const CMSScript&>(),
					templatePage,
					rank,
					itemsCount,
					recursiveContent.str(),
					depth
				);
			}
			else
			{
				// Case items are sorted ascending
				if(!_sortUpTemplate.empty())
				{
					BOOST_FOREACH(const SortedItems::value_type item, sortedItems)
					{
						stringstream recursiveContent;

						// Recursion
						if(_recursive && item.second->hasSubMaps(arrayCode))
						{
							display(
								recursiveContent,
								request,
								*item.second,
								page,
								variables,
								depth + 1
							);
						}

						_displayItem(
							stream,
							request,
							page,
							baseParametersMap,
							*item.second,
							variables,
							optional<const CMSScript&>(),
							templatePage,
							rank,
							itemsCount,
							recursiveContent.str(),
							depth
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
						if(_recursive && item.second->hasSubMaps(arrayCode))
						{
							display(
								recursiveContent,
								request,
								*item.second,
								page,
								variables,
								depth + 1
							);
						}

						_displayItem(
							stream,
							request,
							page,
							baseParametersMap,
							*item.second,
							variables,
							optional<const CMSScript&>(),
							templatePage,
							rank,
							itemsCount,
							recursiveContent.str(),
							depth
						);
					}
				}
				// Case items are read in the same order as in the service result
				else
				{
					BOOST_FOREACH(const ParametersMap::SubParametersMap::mapped_type::value_type& item, items)
					{
						stringstream recursiveContent;

						if(!item.get())
						{
							// A Null was added in a ParametersMap, escape it silently
							continue;
						}

						// Recursion
						if(_recursive && item->hasSubMaps(arrayCode))
						{
							display(
								recursiveContent,
								request,
								*item,
								page,
								variables,
								depth + 1
							);
						}

						_displayItem(
							stream,
							request,
							page,
							baseParametersMap,
							*item,
							variables,
							optional<const CMSScript&>(),
							templatePage,
							rank,
							itemsCount,
							recursiveContent.str(),
							depth
						);
					}
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
			optional<const CMSScript&> templateContent,
			const Webpage* templatePage,
			size_t& rank,
			size_t itemsCount,
			const string& recursiveContent,
			size_t depth
		) const {
			ParametersMap pm(item);
			pm.merge(baseParametersMap);
			pm.insert(DATA_RANK, rank++);
			pm.insert(DATA_DEPTH, depth);
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
			display(s, request, additionalParametersMap, page, variables, 0);
			return s.str();
		}
}	}

