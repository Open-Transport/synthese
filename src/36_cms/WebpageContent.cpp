
/** WebpageContent class implementation.
	@file WebpageContent.cpp

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

#include "WebpageContent.hpp"

#include "HTMLTable.h"
#include "Request.h"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace html;
	using namespace server;
	using namespace util;
	using namespace cms;

	template<> const Field ComplexObjectFieldDefinition<WebpageContent>::FIELDS[] = { Field("content1", SQL_TEXT), Field("ignore_white_chars", SQL_BOOLEAN), Field() };
	template<> const bool ComplexObjectFieldDefinition<WebpageContent>::EXPORT_CONTENT_AS_FILE = false;
	FIELD_COMPLEX_NO_LINKED_OBJECT_ID(WebpageContent)

	template<>
	void ComplexObjectField<WebpageContent, WebpageContent>::LoadFromRecord(
		WebpageContent& fieldObject,
		ObjectBase& object,
		const Record& record,
		const util::Env& env
	){
		if(record.isDefined(FIELDS[1].name))
		{
			fieldObject._ignoreWhiteChars = record.getDefault<bool>(FIELDS[1].name, false);
		}

		if(record.isDefined(FIELDS[0].name))
		{
			// At end because nodes generation needs the value of the other parameters to be updated
			fieldObject.setCode(record.getDefault<string>(FIELDS[0].name));
		}
	}



	template<>
	void ComplexObjectField<WebpageContent, WebpageContent>::SaveToParametersMap(
		const WebpageContent& fieldObject,
		const ObjectBase& object,
		util::ParametersMap& map,
		const std::string& prefix
	){
		// Content
		map.insert(
			prefix + FIELDS[0].name,
			ObjectField<void, string>::Serialize(
				fieldObject._code,
				map.getFormat()
		)	);

		// Ignore white chars
		map.insert(
			prefix + FIELDS[1].name,
			fieldObject._ignoreWhiteChars
		);
	}


	namespace cms
	{
		const string WebpageContent::PARAMETER_VAR = "VAR";
		const string WebpageContent::PARAMETER_TEMPLATE = "template";
		const string WebpageContent::PARAMETER_EMPTY = "empty";
		const string WebpageContent::ForeachNode::DATA_RANK = "rank";
		const string WebpageContent::ForeachNode::DATA_ITEMS_COUNT = "items_count";
		const string WebpageContent::PARAMETER_SORT_UP = "sort_up";
		const string WebpageContent::PARAMETER_SORT_DOWN = "sort_down";

		shared_recursive_mutex WebpageContent::_SharedMutex;



		WebpageContent::WebpageContent(
		):	_ignoreWhiteChars(false)
		{
		}



		WebpageContent::WebpageContent(
			const std::string& code,
			bool ignoreWhiteChars
		):	_code(code),
			_ignoreWhiteChars(ignoreWhiteChars)
		{
			_updateNodes();
		}



		void WebpageContent::_updateNodes()
		{
			boost::unique_lock<shared_recursive_mutex> lock(_SharedMutex);
			_nodes.clear();
			_parse(_nodes, _code.begin(), _code.end(), set<string>());
		}



		void WebpageContent::setCode( const std::string& value )
		{
			bool toUpdate(value != _code);
			_code = value;
			if(toUpdate)
			{
				_updateNodes();
			}
		}



		std::string::const_iterator WebpageContent::_parseText(
			std::ostream& stream,
			std::string::const_iterator it,
			std::string::const_iterator end,
			std::string termination
		) const	{
			std::string::const_iterator newEnd(search(it, end, termination.begin(), termination.end()));
			for(; it != newEnd; ++it)
			{
				stream.put(*it);
			}
			return it == end ? end : newEnd + termination.size();
		}





		std::string::const_iterator WebpageContent::_parse(
			WebpageContent::Nodes& nodes,
			std::string::const_iterator it,
			std::string::const_iterator end,
			std::set<std::string> termination
		) const {

			shared_ptr<TextNode> currentTextNode;

			while(it != end)
			{
				// Ignore white chars
				if(	_ignoreWhiteChars &&
					(*it == ' ' || *it == '\r' || *it == '\n')
				){
					++it;
					continue;
				}

				// Special characters
				if(*it == '\\' && it+1 != end)
				{
					if(!currentTextNode.get())
					{
						currentTextNode.reset(new TextNode);
					}
					++it;
					currentTextNode->text.push_back(*it);
					++it;
				} // Call to a public function
				else if(*it == '<' && it+1 != end && *(it+1)=='?' && it+2 != end)
				{
					++it;
					++it;

					// Handle current text node
					if(currentTextNode.get())
					{
						nodes.push_back(currentTextNode);
						currentTextNode.reset();
					}

					// Function node
					shared_ptr<ServiceNode> node(new ServiceNode);

					// function name
					string functionName;
					for(;it != end && *it != '&' && *it != '?'; ++it)
					{
						functionName.push_back(*it);
					}
					try
					{
						node->functionCreator = Factory<Function>::GetCreator(ParametersMap::Trim(functionName));

						// parameters
						if(it != end && *it == '?')
						{
							it += 2;
						}
						else
						{
							set<string> functionTermination;
							functionTermination.insert("&");
							functionTermination.insert("?>");
							while(it != end && *it == '&')
							{
								stringstream parameterName;
								it = _parseText(parameterName, it+1, end, "=");

								if(it != end)
								{
									// Parsing of the nodes
									Nodes parameterNodes;
									it = _parse(parameterNodes, it, end, functionTermination);

									// Special template parameter
									if(parameterName.str() == PARAMETER_TEMPLATE)
									{
										node->inlineTemplate = parameterNodes;
									}
									else
									{
										// Storage in template parameters if begins with VAR else in service parameters
										string parameterNameStr(ParametersMap::Trim(parameterName.str()));
										if(parameterNameStr.size() < PARAMETER_VAR.size() || parameterNameStr.substr(0, PARAMETER_VAR.size()) != PARAMETER_VAR)
										{
											node->serviceParameters.push_back(make_pair(parameterNameStr, parameterNodes));
										}
										else
										{
											node->templateParameters.push_back(make_pair(parameterNameStr.substr(PARAMETER_VAR.size()), parameterNodes));
										}
									}

									if(*(it-1) != '&')
									{
										break;
									}
									--it;
								}
						}	}

						nodes.push_back(static_pointer_cast<Node, ServiceNode>(node));
					}
					catch(FactoryException<Function>&)
					{
						for(; it != end && it+1 != end && *it != '?' && *(it+1) != '>' ; ++it) ;
						if(it != end && *it == '?')
						{
							++it;
							if(it != end && *it == '>')
							{
								++it;
						}	}
					}
				} // Variable getter or setter
				else if(*it == '<' && it+1 != end && *(it+1)=='@' && it+2 != end)
				{
					if(currentTextNode.get())
					{
						nodes.push_back(currentTextNode);
						currentTextNode.reset();
					}

					// variable name
					string parameter;
					for(it = it+2; it != end && *it != '=' && *it != '@'; ++it)
					{
						parameter.push_back(*it);
					}

					// Is it a get or a set ?
					if(it != end && *it == '=')
					{
						// Node creation
						shared_ptr<VariableUpdateNode> node(new VariableUpdateNode);
						node->variable = parameter;

						// Termination definition
						set<string> termination;
						termination.insert("@>");

						// Load of the value definition
						it = _parse(node->value, it+1, end, termination);

						// Storage
						nodes.push_back(static_pointer_cast<Node, VariableUpdateNode>(node));
					}
					else
					{
						// Node creation
						shared_ptr<ValueNode> valueNode(new ValueNode);
						valueNode->name = parameter;

						// Storage
						nodes.push_back(static_pointer_cast<Node, ValueNode>(valueNode));
					}

					// Jump over termination sequence
					if(it != end && *it == '@')
					{
						it += 2;
					}
				} // Foreach
				else if(*it == '<' && it+1 != end && *(it+1)=='{' && it+2 != end)
				{
					++it;
					++it;

					// Handle current text node
					if(currentTextNode.get())
					{
						nodes.push_back(currentTextNode);
						currentTextNode.reset();
					}

					// Function node
					shared_ptr<ForeachNode> node(new ForeachNode);

					// function name
					for(;it != end && *it != '&' && *it != '}'; ++it)
					{
						node->arrayCode.push_back(*it);
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
							it = _parseText(parameterName, it+1, end, "=");

							if(it != end)
							{
								Nodes parameterNodes;
								string parameterNameStr(ParametersMap::Trim(parameterName.str()));
								it = _parse(parameterNodes, it, end, functionTermination);

								if(parameterNameStr == WebPageDisplayFunction::PARAMETER_PAGE_ID)
								{
									node->pageCode = parameterNodes;
								}
								else if(parameterNameStr == PARAMETER_TEMPLATE)
								{
									node->inlineTemplate = parameterNodes;
								}
								else if(parameterNameStr == PARAMETER_EMPTY)
								{
									node->emptyTemplate = parameterNodes;
								}
								else if(parameterNameStr == PARAMETER_SORT_DOWN)
								{
									node->sortDownTemplate = parameterNodes;
								}
								else if(parameterNameStr == PARAMETER_SORT_UP)
								{
									node->sortUpTemplate = parameterNodes;
								}
								else
								{
									node->parameters.push_back(
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

					nodes.push_back(static_pointer_cast<Node, ForeachNode>(node));

				} // Shortcut to WebpageDisplayFunction
				else if(*it == '<' && it+1 != end && *(it+1)=='#' && it+2 != end)
				{
					++it;
					++it;

					// Handle current text node
					if(currentTextNode.get())
					{
						nodes.push_back(currentTextNode);
						currentTextNode.reset();
					}

					// Page node
					shared_ptr<IncludeNode> node(new IncludeNode);

					// page name
					node->pageName = "!";
					for(;it != end && *it != '&' && *it != '#'; ++it)
					{
						node->pageName.push_back(*it);
					}
					// parameters
					if(it != end && *it == '#')
					{
						it += 2;
					}
					else
					{
						set<string> functionTermination;
						functionTermination.insert("&");
						functionTermination.insert("#>");
						while(it != end && *it == '&')
						{
							stringstream parameterName;
							it = _parseText(parameterName, it+1, end, "=");

							if(it != end)
							{
								Nodes parameterNodes;
								it = _parse(parameterNodes, it, end, functionTermination);
								node->parameters.push_back(make_pair(ParametersMap::Trim(parameterName.str()), parameterNodes));
								if(*(it-1) != '&')
								{
									break;
								}
								--it;
							}
						}
					}

					nodes.push_back(static_pointer_cast<Node,IncludeNode>(node));
				} // Goto
				else if(*it == '<' && it+1 != end && *(it+1)=='%' && it+2 != end)
				{
					if(currentTextNode.get())
					{
						nodes.push_back(currentTextNode);
						currentTextNode.reset();
					}
					shared_ptr<GotoNode> node(new GotoNode);
					set<string> gotoEnding;
					gotoEnding.insert("%>");
					it = _parse(node->direction, it+2, end, gotoEnding);
					nodes.push_back(static_pointer_cast<Node,GotoNode>(node));

				} // Label
				else if(*it == '<' && it+1 != end && *(it+1)=='<' && it+2 != end)
				{
					if(currentTextNode.get())
					{
						nodes.push_back(currentTextNode);
						currentTextNode.reset();
					}
					shared_ptr<LabelNode> node(new LabelNode);
					stringstream s;
					it = _parseText(s, it+2, end, ">>");
					node->label = s.str();

					nodes.push_back(static_pointer_cast<Node,LabelNode>(node));
				} // Reached the end of a recursion level
				else
				{
					BOOST_FOREACH(const string& test, termination)
					{
						if(size_t(end - it) >= test.size())
						{
							string testIt;
							for(size_t i(0); i< test.size(); ++i)
							{
								testIt.push_back(*(it+i));
							}
							if(testIt == test)
							{
								if(currentTextNode.get())
								{
									nodes.push_back(static_pointer_cast<Node,TextNode>(currentTextNode));
								}
								return it+test.size();
							}
						}
					}
					if(!currentTextNode.get())
					{
						currentTextNode.reset(new TextNode);
					}
					currentTextNode->text.push_back(*it);
					++it;
				}
			}

			if(currentTextNode.get())
			{
				nodes.push_back(static_pointer_cast<Node,TextNode>(currentTextNode));
			}
			return it;
		}




		void WebpageContent::ServiceNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			// Service parameters evaluation
			ParametersMap serviceParametersMap;
			BOOST_FOREACH(const Parameters::value_type& param, serviceParameters)
			{
				stringstream s;
				BOOST_FOREACH(const Parameters::value_type::second_type::value_type& node, param.second)
				{
					node->display(s, request, additionalParametersMap, page, variables);
				}
				serviceParametersMap.insert(param.first, s.str());
			}
			if(!inlineTemplate.empty())
			{
				serviceParametersMap.insert(Function::PARAMETER_OUTPUT_FORMAT, string());
			}

			// Template parameters evaluation
			ParametersMap templateParametersMap(request.getFunction()->getTemplateParameters());
			BOOST_FOREACH(const Parameters::value_type& param, templateParameters)
			{
				stringstream s;
				BOOST_FOREACH(const Parameters::value_type::second_type::value_type& node, param.second)
				{
					node->display(s, request, additionalParametersMap, page, variables);
				}
				templateParametersMap.insert(param.first, s.str());
			}

			// Function
			shared_ptr<Function> function(functionCreator->create());
			try
			{
				if(dynamic_cast<FunctionWithSiteBase*>(function.get()))
				{
					static_cast<FunctionWithSiteBase*>(function.get())->setSite(page.getRoot());
				}
				function->setTemplateParameters(templateParametersMap);
				function->_setFromParametersMap(serviceParametersMap);
				if (function->isAuthorized(request.getSession()))
				{
					// Run of the service
					ParametersMap result(
						function->run(
							stream,
							request
					)	);

					// Display of the result if inline template defined
					if(!inlineTemplate.empty())
					{
						// Merge page parameters in result map
						result.merge(additionalParametersMap);

						// Display of each inline defined node
						BOOST_FOREACH(const WebpageContent::Nodes::value_type& node, inlineTemplate)
						{
							node->display(stream, request, result, page, variables);
						}
					}
				}
			}
			catch(RequestException&)
			{

			}
			catch(Request::RedirectException& e)
			{
				throw e;
			}
			catch(...)
			{

			}
		}



		void WebpageContent::LabelNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const {
		}



		void WebpageContent::GotoNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const {
		}



		void WebpageContent::ValueNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			// Variables list (debugging purpose)
			if(name.empty())
			{
				HTMLTable::ColsVector c;
				c.push_back("name");
				c.push_back("value");
				HTMLTable t(c, "table table-striped table-condensed sortable");
				stream << t.open();
				t.body(stream);
				BOOST_FOREACH(const ParametersMap::Map::value_type& item, variables.getMap())
				{
					stream << t.row();
					stream << t.col() << item.first;
					stream << t.col() << item.second;
				}
				BOOST_FOREACH(const ParametersMap::Map::value_type& item, additionalParametersMap.getMap())
				{
					// Variables first
					if(variables.getMap().find(item.first) != variables.getMap().end())
					{
						continue;
					}
					stream << t.row();
					stream << t.col() << item.first;
					stream << t.col() << item.second;
				}
				ParametersMap requestMap(request.getParametersMap());
				BOOST_FOREACH(
					const ParametersMap::Map::value_type& item,
					requestMap.getMap()
				){
					// Variables first
					if(variables.getMap().find(item.first) != variables.getMap().end())
					{
						continue;
					}
					// Parameters first
					if(additionalParametersMap.getMap().find(item.first) != additionalParametersMap.getMap().end())
					{
						continue;
					}
					stream << t.row();
					stream << t.col() << item.first;
					stream << t.col() << item.second;
				}
				stream << t.row();
				stream << t.col() << "host_name";
				stream << t.col() << request.getHostName();
				stream << t.row();
				stream << t.col() << "client_url";
				stream << t.col() << request.getClientURL();
				const Website* site(page.getRoot());
				if(site)
				{
					stream << t.row();
					stream << t.col() << "site";
					stream << t.col() << site->getKey();
				}
				BOOST_FOREACH(const ParametersMap::SubMapsKeys::value_type& item, additionalParametersMap.getSubMapsKeys())
				{
					stream << t.row();
					stream << t.col() << item;
					stream << t.col() << "(submap)";
				}
				stream << t.close();
			}
			else if(name == "client_url")
			{
				stream << request.getClientURL();
			}
			else if(name == "host_name")
			{
				stream << request.getHostName();
			}
			else if(name == "site")
			{
				const Website* site(page.getRoot());
				if(site)
				{
					stream << site->getKey();
				}
			}
			else
			{
				string value(variables.getDefault<string>(name));
				if(value.empty())
				{
					value = additionalParametersMap.getDefault<string>(name);
				}
				if(value.empty())
				{
					value = request.getParametersMap().getDefault<string>(name);
				}
				stream << value;
			}
		}



		void WebpageContent::IncludeNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			// Parameters
			ParametersMap pm;
			BOOST_FOREACH(const Parameters::value_type& param, parameters)
			{
				stringstream s;
				BOOST_FOREACH(const Parameters::value_type::second_type::value_type& node, param.second)
				{
					node->display(s, request, additionalParametersMap, page, variables);
				}
				pm.insert(param.first, s.str());
			}
			pm.merge(additionalParametersMap);

			Webpage* includedPage(page.getRoot()->getPageBySmartURL(pageName));
			if(includedPage)
			{
				includedPage->display(stream, request, pm, variables);
			}
		}



		void WebpageContent::TextNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const {
			stream << text;
		}



		void WebpageContent::ForeachNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			// Page load
			const Webpage* templatePage(NULL);
			if(inlineTemplate.empty())
			{
				stringstream pageCodeStream;
				BOOST_FOREACH(const Parameters::value_type::second_type::value_type& node, pageCode)
				{
					node->display(pageCodeStream, request, additionalParametersMap, page, variables);
				}

				string pageCodeStr(pageCodeStream.str());

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
			BOOST_FOREACH(const Parameters::value_type& param, parameters)
			{
				stringstream s;
				BOOST_FOREACH(const Parameters::value_type::second_type::value_type& node, param.second)
				{
					node->display(s, request, additionalParametersMap, page, variables);
				}
				baseParametersMap.insert(param.first, s.str());
			}

			// No items to display
			if(	!additionalParametersMap.hasSubMaps(arrayCode)
			){
				BOOST_FOREACH(const WebpageContent::Nodes::value_type& node, emptyTemplate)
				{
					node->display(stream, request, baseParametersMap, page, variables);
				}
				return;
			}

			// Items read
			const ParametersMap::SubParametersMap::mapped_type& items(
				additionalParametersMap.getSubMaps(arrayCode)
			);
			size_t itemsCount(items.size());

			// Sorting items : building the sorting key
			typedef map<string, shared_ptr<ParametersMap> > SortedItems;
			SortedItems sortedItems;
			if(!sortUpTemplate.empty() || !sortDownTemplate.empty())
			{
				// Loop on items
				size_t rank(0);
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
						templatePage,
						rank,
						itemsCount
					);

					// Insertion in the map
					sortedItems.insert(make_pair(key.str(), item));
				}
			}

			size_t rank(0);
			
			// Case items are sorted ascending
			if(!sortUpTemplate.empty())
			{
				BOOST_FOREACH(const SortedItems::value_type item, sortedItems)
				{
					_displayItem(
						stream,
						request,
						page,
						baseParametersMap,
						*item.second,
						variables,
						templatePage,
						rank,
						itemsCount
					);
				}
			}
			// Case items are sorted descending
			else if(!sortDownTemplate.empty())
			{
				BOOST_REVERSE_FOREACH(const SortedItems::value_type item, sortedItems)
				{
					_displayItem(
						stream,
						request,
						page,
						baseParametersMap,
						*item.second,
						variables,
						templatePage,
						rank,
						itemsCount
					);
				}
			}
			// Case items are read in the same order as in the service result
			else
			{
				BOOST_FOREACH(const ParametersMap::SubParametersMap::mapped_type::value_type& item, items)
				{
					_displayItem(
						stream,
						request,
						page,
						baseParametersMap,
						*item,
						variables,
						templatePage,
						rank,
						itemsCount
					);
				}
			}
		}



		void WebpageContent::ForeachNode::_displayItem(
			std::ostream& stream,
			const server::Request& request,
			const Webpage& page,
			const util::ParametersMap& baseParametersMap,
			const util::ParametersMap& item,
			util::ParametersMap& variables,
			const Webpage* templatePage,
			size_t& rank,
			size_t itemsCount
		) const {
			ParametersMap pm(item);
			pm.merge(baseParametersMap);
			pm.insert(DATA_RANK, rank++);
			pm.insert(DATA_ITEMS_COUNT, itemsCount);

			// Display by a template page
			if(templatePage)
			{
				templatePage->display(stream, request, pm, variables);
			}
			else // Display by an inline defined template
			{
				// Display of each inline defined node
				BOOST_FOREACH(const WebpageContent::Nodes::value_type& node, inlineTemplate)
				{
					node->display(stream, request, pm, page, variables);
				}
			}
		}


		void WebpageContent::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			boost::shared_lock<shared_recursive_mutex> lock(_SharedMutex);

			Nodes::const_iterator itNode(_nodes.begin());
			while(itNode != _nodes.end())
			{
				// Goto
				if(dynamic_cast<GotoNode*>(itNode->get()))
				{
					// Search of label
					stringstream label;
					BOOST_FOREACH(const shared_ptr<Node>& node, static_cast<GotoNode*>(itNode->get())->direction)
					{
						node->display(label, request, additionalParametersMap, page, variables);
					}
					if(!label.str().empty())
					{
						Nodes::const_iterator itGotoNode(itNode+1);
						for(; itGotoNode != _nodes.end(); ++itGotoNode)
						{
							if(dynamic_cast<LabelNode*>(itGotoNode->get()) && static_cast<LabelNode*>(itGotoNode->get())->label == label.str())
							{
								itNode = itGotoNode;
								break;
							}
						}
						if(itGotoNode == _nodes.end())
						{
							for(itGotoNode = _nodes.begin(); itGotoNode != itNode; ++itGotoNode)
							{
								if(dynamic_cast<LabelNode*>(itGotoNode->get()) && static_cast<LabelNode*>(itGotoNode->get())->label == label.str())
								{
									itNode = itGotoNode;
									break;
								}
							}
						}
					}
				}
				else
				{
					(*itNode)->display(stream, request, additionalParametersMap, page, variables);
				}
				++itNode;
			}
		}



		void WebpageContent::VariableUpdateNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const {

			// Evaluation of the value
			stringstream valueStr;
			BOOST_FOREACH(const Nodes::value_type& node, value)
			{
				node->display(valueStr, request, additionalParametersMap, page, variables);
			}

			// Storage in the variables map
			variables.insert(
				variable,
				valueStr.str()
			);
		}
}	}
