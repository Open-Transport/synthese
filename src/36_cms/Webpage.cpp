
/** Webpage class implementation.
	@file Webpage.cpp

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

#include "Webpage.h"
#include "ParametersMap.h"
#include "DynamicRequest.h"
#include "FunctionWithSite.h"
#include "ServerModule.h"
#include "WebPageDisplayFunction.h"
#include "CMSModule.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	namespace util
	{
		template<> const string Registry<cms::Webpage>::KEY("Webpage");
	}

	namespace cms
	{
		const string Webpage::Node::PARAMETER_TRANSMIT_PARAMETERS("tp");

		Webpage::Webpage( util::RegistryKeyType id  ):
			Registrable(id),
			_startDate(posix_time::not_a_date_time),
			_endDate(posix_time::not_a_date_time),
			_template(NULL),
			_doNotUseTemplate(false),
			_hasForum(false),
			_ignoreWhiteChars(false),
			_rawEditor(false)
		{
		}



		bool Webpage::mustBeDisplayed( boost::posix_time::ptime now /*= boost::posix_time::second_clock::local_time()*/ ) const
		{
			return
				(_startDate.is_not_a_date_time() || _startDate <= now) &&
				(_endDate.is_not_a_date_time() || _endDate >= now)
			;
		}



		std::string::const_iterator Webpage::_parse(
			Webpage::Nodes& nodes,
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
					shared_ptr<FunctionNode> node(new FunctionNode);

					// function name
					string functionName;
					for(;it != end && *it != '&' && *it != '?'; ++it)
					{
						functionName.push_back(*it);
					}
					try
					{
						node->functionCreator = Factory<Function>::GetCreator(functionName);

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
									Nodes parameterNodes;
									it = _parse(parameterNodes, it, end, functionTermination);
									node->parameters.push_back(make_pair(parameterName.str(), parameterNodes));
									if(*(it-1) != '&')
									{
										break;
									}
									--it;
								}
						}	}

						nodes.push_back(static_pointer_cast<Node,FunctionNode>(node));
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
				} // Shortcut to GetValueFunction
				else if(*it == '<' && it+1 != end && *(it+1)=='@' && it+2 != end)
				{
					if(currentTextNode.get())
					{
						nodes.push_back(currentTextNode);
						currentTextNode.reset();
					}

					stringstream parameter;
					it = _parseText(parameter, it+2, end, "@>");
					shared_ptr<ValueNode> valueNode(new ValueNode);
					valueNode->name = parameter.str();

					nodes.push_back(static_pointer_cast<Node,ValueNode>(valueNode));

				} // Shortcut to WebPageDisplayFunction
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
								node->parameters.push_back(make_pair(parameterName.str(), parameterNodes));
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
						if(end - it >= test.size())
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



		void Webpage::display(
			std::ostream& stream,
			const server::Request& request,
			const server::ParametersMap& aditionalParametersMap
		) const	{

			Nodes::const_iterator itNode(_nodes.begin());
			while(itNode != _nodes.end())
			{
				// Goto
				if(dynamic_cast<GotoNode*>(itNode->get()))
				{
					// Search of label
					stringstream label;
					BOOST_FOREACH(boost::shared_ptr<Node> node, static_cast<GotoNode*>(itNode->get())->direction)
					{
						node->display(label, request, aditionalParametersMap, *this);
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
					(*itNode)->display(stream, request, aditionalParametersMap,*this);
				}
				++itNode;
			}
		}



		std::string Webpage::getMimeType() const
		{
			return _mimeType.empty() ? "text/html" : _mimeType;
		}



		Webpage* Webpage::getTemplate() const
		{
			if(_doNotUseTemplate)
			{
				return NULL;
			}
			if(_template && _template != this)
			{
				return _template;
			}
			if(getRoot()->getDefaultTemplate() != this)
			{
				return getRoot()->getDefaultTemplate();
			}
			return NULL;
		}



		std::size_t Webpage::getDepth() const
		{
			size_t depth(0);
			for(Webpage* page(getParent()); page; page = page->getParent())
			{
				++depth;
			}
			return depth;
		}



		std::string Webpage::getFullName() const
		{
			string result;
			for(const Webpage* page(this); page; page = page->getParent())
			{
				result = page->getName() + (result.empty() ? "/" : string()) + result;
			}
			if(getRoot())
			{
				result = getRoot()->getName() + "/" + result;
			}
			return result;
		}



		void Webpage::setContent( const std::string& value )
		{
			if(value != _content)
			{
				_nodes.clear();
				_parse(_nodes, value.begin(), value.end(), set<string>());
			}
			_content = value;
		}



		std::string::const_iterator Webpage::_parseText(
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



		void Webpage::FunctionNode::display(
			std::ostream& stream,
			const server::Request& request,
			const server::ParametersMap& aditionalParametersMap,
			const Webpage& page
		) const	{

			// Parameters
			ParametersMap pm(request.getFunction()->getSavedParameters());
			ParametersMap callParametersMap;
			BOOST_FOREACH(const Parameters::value_type& param, parameters)
			{
				stringstream s;
				BOOST_FOREACH(const Parameters::value_type::second_type::value_type& node, param.second)
				{
					node->display(s, request, aditionalParametersMap, page);
				}
				pm.insert(param.first, s.str());
				callParametersMap.insert(param.first, s.str());
			}
			if(callParametersMap.isDefined(PARAMETER_TRANSMIT_PARAMETERS))
			{
				callParametersMap.merge(pm);
			}

			// Function
			shared_ptr<Function> function(functionCreator->create());
			try
			{
				function->setSavedParameters(callParametersMap);
				function->_setFromParametersMap(callParametersMap);
				if (function->isAuthorized(request.getSession()))
				{
					function->run(stream, request);
				}
			}
			catch(RequestException& e)
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



		void Webpage::LabelNode::display(
			std::ostream& stream,
			const server::Request& request,
			const server::ParametersMap& aditionalParametersMap,
			const Webpage& page
		) const {
		}



		void Webpage::GotoNode::display(
			std::ostream& stream,
			const server::Request& request,
			const server::ParametersMap& aditionalParametersMap,
			const Webpage& page
		) const {
		}



		void Webpage::ValueNode::display(
			std::ostream& stream,
			const server::Request& request,
			const server::ParametersMap& aditionalParametersMap,
			const Webpage& page
		) const	{

			if(name == "client_url")
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
				string value(aditionalParametersMap.getDefault<string>(name));
				if(value.empty())
				{
					value = request.getParametersMap().getDefault<string>(name);
				}
				else
				{
					stream << value;
				}
			}

		}



		void Webpage::IncludeNode::display(
			std::ostream& stream,
			const server::Request& request,
			const server::ParametersMap& aditionalParametersMap,
			const Webpage& page
		) const	{

			// Parameters
			ParametersMap pm;
			BOOST_FOREACH(const Parameters::value_type& param, parameters)
			{
				stringstream s;
				BOOST_FOREACH(const Parameters::value_type::second_type::value_type& node, param.second)
				{
					node->display(s, request, aditionalParametersMap, page);
				}
				pm.insert(param.first, s.str());
			}
			pm.merge(aditionalParametersMap);

			Webpage* includedPage(page.getRoot()->getPageBySmartURL(pageName));
			if(includedPage)
			{
				includedPage->display(stream, request, pm);
			}

		}



		void Webpage::TextNode::display(
			std::ostream& stream,
			const server::Request& request,
			const server::ParametersMap& aditionalParametersMap,
			const Webpage& page
		) const {
			stream << text;
		}
}	}
