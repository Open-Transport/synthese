
/** CMSScript class implementation.
	@file CMSScript.cpp

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

#include "CMSScript.hpp"

#include "ConstantExpression.hpp"
#include "ForEachExpression.hpp"
#include "GotoNode.hpp"
#include "IncludeExpression.hpp"
#include "HTMLTable.h"
#include "LabelNode.hpp"
#include "MapUpdateNode.hpp"
#include "Request.h"
#include "ServiceExpression.hpp"
#include "StaticFunctionRequest.h"
#include "VariableUpdateNode.hpp"
#include "EmbeddedScriptNode.hpp"
#include "Webpage.h"
#include "WebPageDisplayFunction.h"
#include "Website.hpp"

#include <boost/algorithm/string.hpp>


using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;

	namespace cms
	{
		CMSScript::CMSScript(
		):	_code(),
			_ignoreWhiteChars(false),
			_doNotEvaluate(false),
			_sharedMutex(new shared_recursive_mutex)
		{}



		CMSScript::CMSScript(
			const std::string& code,
			bool ignoreWhiteChars /*= false*/,
			bool doNotEvaluate /*= false */
		):	_code(code),
			_ignoreWhiteChars(ignoreWhiteChars),
			_doNotEvaluate(doNotEvaluate),
			_sharedMutex(new shared_recursive_mutex)
		{
			_updateNodes();
		}


		//////////////////////////////////////////////////////////////////////////
		/// Constructor by string part parsing.
		CMSScript::CMSScript(
			string::const_iterator& it,
			string::const_iterator end,
			std::set<string> termination,
			bool ignoreWhiteChars
		):	_ignoreWhiteChars(ignoreWhiteChars),
			_doNotEvaluate(false),
			_sharedMutex(new synthese::util::shared_recursive_mutex)
		{
			_parse(it, end, termination);
		}


		bool CMSScript::operator==(const CMSScript& other) const
		{
			return _ignoreWhiteChars == other._ignoreWhiteChars &&
				_doNotEvaluate == other._doNotEvaluate &&
				_code == other._code;
		}



		void CMSScript::_updateNodes()
		{
			boost::unique_lock<shared_recursive_mutex> lock(*_sharedMutex);
			if(_doNotEvaluate)
			{
				_nodes.clear();
				_nodes.push_back(
					boost::shared_ptr<WebpageContentNode>(
						new ConstantExpression(_code)
				)	);
			}
			else
			{
				// Read the first line of the code : if it starts with '#!' this is an embedded script, else it is a CMS page
				std::istringstream codeStream(_code);
				std::string firstLine;

				std::getline(codeStream, firstLine);
				boost::algorithm::trim_copy_if(firstLine, is_any_of(" \t"));

				if(boost::algorithm::starts_with(firstLine, "#!"))
				{
					// The script interpreter declaration is the content of the first line (e.g. #!/bin/python)
					std::string scriptInterpreter = firstLine;
					std::string scriptCode("");
					size_t startIndex = _code.find(scriptInterpreter);

					// The script code is everything after the interpreter declaration
					startIndex += scriptInterpreter.size();
					scriptCode = _code.substr(startIndex);

					// Clear node list and push a single EmbeddedScriptNode
					_nodes.clear();
					_nodes.push_back(
						boost::shared_ptr<WebpageContentNode>(
							new EmbeddedScriptNode(scriptInterpreter, scriptCode)
					)	);
				}

				else
				{
					// Interpret the code as CMS language
					string::const_iterator it(_code.begin());
					_parse(it, _code.end(), set<string>());
				}
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Runs a nodes update if the value has changed.
		void cms::CMSScript::setCode( const std::string& value )
		{
			bool toUpdate(value != _code);
			_code = value;
			if(toUpdate)
			{
				_updateNodes();
			}
		}



		bool CMSScript::update(
			const std::string& code,
			bool ignoreWhiteChars,
			bool doNotEvaluate
		){
			bool toUpdate(
				_code != code ||
				_ignoreWhiteChars != ignoreWhiteChars ||
				_doNotEvaluate != doNotEvaluate
			);
			_code = code;
			_ignoreWhiteChars = ignoreWhiteChars;
			_doNotEvaluate = doNotEvaluate;
			if(toUpdate)
			{
				_updateNodes();
			}

			return toUpdate;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Parses the content and put it in the nodes cache.
		/// @retval nodes object to write the result on
		/// @param it iterator on the beginning of the string to parse
		/// @param end iterator on the end of the string to parse
		/// @param termination termination string to detect to interrupt the parsing
		/// @return iterator on the end of the parsing
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		/// The parsing stops when the iterator has reached the end of the string, or if the ?> sequence has been found, indicating that the following text belongs to
		///	a lower level of recursion.
		/// If the level of recursion is superior than 0, then the output is encoded
		/// as an url to avoid mistake when the result of parsing is considered as
		/// a single parameter of a function call.
		void CMSScript::_parse(
			std::string::const_iterator& it,
			std::string::const_iterator end,
			std::set<std::string> termination
		){
			_nodes.clear();

			std::string::const_iterator beginning(it);
			bool terminationFound(false);
			string currentText;

			while(it != end)
			{
				// Ignore white chars
				if(	_ignoreWhiteChars &&
					(*it == ' ' || *it == '\r' || *it == '\n' || *it == '\t')
				){
					++it;
					continue;
				}

				// Special characters
				if(*it == '\\' && it+1 != end)
				{
					++it;
					currentText.push_back(*it);
					++it;
				} // Call to a public function
				else if(*it == '<' && it+1 != end && *(it+1)=='?' && it+2 != end)
				{
					++it;
					++it;

					// Handle current text node
					if(!currentText.empty())
					{
						_nodes.push_back(
							boost::shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					// Parsing service node
					boost::shared_ptr<ServiceExpression> node(
						new ServiceExpression(it, end, _ignoreWhiteChars)
					);

					if(node->getFunctionCreator())
					{
						_nodes.push_back(
							static_pointer_cast<WebpageContentNode, ServiceExpression>(
								node
						)	);
					}

				} // Variable getter or setter
				else if(*it == '<' && it+1 != end && *(it+1)=='@' && it+2 != end)
				{
					if(!currentText.empty())
					{
						_nodes.push_back(
							boost::shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					// variable name
					string parameter;
					it += 2;
					string::const_iterator it2;
					bool inDoubleQuotes(false);
					size_t serviceRecursion(0);
					for(it2 = it;
						it2 != end;
						++it2
					){
						// Escape if = or @ or out of double quotes
						if(!inDoubleQuotes && !serviceRecursion)
						{
							if(*it2 == '=' || *it2 == ':' || *it2 == '@')
							{
								break;
							}
						}

						parameter.push_back(*it2);

						if(inDoubleQuotes)
						{
							if(*it2 == '"')
							{
								inDoubleQuotes = false;
							}
						}
						else
						{
							if(!serviceRecursion)
							{
								if(*it2 == '"')
								{
									inDoubleQuotes = true;
								}
							}
							else
							{
								if(	(	*it2 == '@' ||
										*it2 == '?' ||
										*it2 == '#' ||
										*it2 == '}' ||
										*it2 == '%' ||
										*it2 == '>'
									) &&
									it2+1 != end &&
									*(it2+1) == '>'
								){
									++it2;
									--serviceRecursion;
								}
							}
							if(*it2 == '<' &&
								it2+1 != end &&
								(	*(it2+1) == '@' ||
									*(it2+1) == '?' ||
									*(it2+1) == '#' ||
									*(it2+1) == '{' ||
									*(it2+1) == '%' ||
									*(it2+1) == '<'
								)
							){
								++serviceRecursion;
								++it2;
							}
						}
					}

					// Is a variable set ?
					if(	it2 != end &&
						*it2 == '=' &&
						(it2+1 == end || *(it2+1) != '=') &&
						(*(it2-1) != '!') &&
						(*(it2-1) != '<') &&
						(*(it2-1) != '>')
					){
						it = it2;
						++it;

						trim(parameter);

						VariableUpdateNode::Items items;
						items.push_back(VariableUpdateNode::Item());
						for(string::const_iterator it3(parameter.begin()); it3!=parameter.end(); )
						{
							// Alphanum chars
							if( (*it3 >= 'a' && *it3 <= 'z') ||
								(*it3 >= 'A' && *it3 <= 'Z') ||
								(*it3 >= '0' && *it3 <= '9') ||
								*it3 == '_'
							){
								items.rbegin()->key.push_back(*it3);
								++it3;
								continue;
							}

							// Index
							if(	*it3 == '[')
							{
								++it3;
								items.rbegin()->index = Expression::Parse(it3, parameter.end(), "]");
								continue;
							}

							// Sub map
							if(	*it3 == '.')
							{
								++it3;
								items.push_back(VariableUpdateNode::Item());
								continue;
							}

							// Ignored char
							++it3;
						}

						// Node creation
						_nodes.push_back(
							boost::shared_ptr<WebpageContentNode>(
								new VariableUpdateNode(items, it, end)
						)	);
					}
					else if(
						it2 != end &&
						*it2 == ':' &&
						it2+1 != end &&
						*(it2+1) == '='
					){	// Is a map set ?
						it = it2;
						++it;
						++it;

						trim(parameter);

						MapUpdateNode::Items items;
						items.push_back(MapUpdateNode::Item());
						for(string::const_iterator it3(parameter.begin()); it3!=parameter.end(); )
						{
							// Alphanum chars
							if( (*it3 >= 'a' && *it3 <= 'z') ||
								(*it3 >= 'A' && *it3 <= 'Z') ||
								(*it3 >= '0' && *it3 <= '9') ||
								*it3 == '_'
							){
								items.rbegin()->key.push_back(*it3);
								++it3;
								continue;
							}

							// Index
							if(	*it3 == '[')
							{
								++it3;
								items.rbegin()->index = Expression::Parse(it3, parameter.end(), "]");
								continue;
							}

							// Sub map
							if(	*it3 == '.')
							{
								++it3;
								items.push_back(MapUpdateNode::Item());
								continue;
							}

							// Ignored char
							++it3;
						}

						// Node creation
						_nodes.push_back(
							boost::shared_ptr<WebpageContentNode>(
								new MapUpdateNode(items, it, end)
						)	);

					}
					else // Is an expression
					{
						boost::shared_ptr<Expression> expr(
							Expression::Parse(it, end, "@>")
						);

						// Storage
						if(expr.get())
						{
							_nodes.push_back(expr);
						}
					}
				} // Foreach
				else if(*it == '<' && it+1 != end && *(it+1)=='{' && it+2 != end)
				{
					++it;
					++it;

					// Handle current text node
					if(!currentText.empty())
					{
						_nodes.push_back(
							boost::shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					_nodes.push_back(
						boost::shared_ptr<WebpageContentNode>(
							new ForEachExpression(it, end, _ignoreWhiteChars)
					)	);

				} // Shortcut to WebpageDisplayFunction
				else if(*it == '<' && it+1 != end && *(it+1)=='#' && it+2 != end)
				{
					++it;
					++it;

					// Handle current text node
					if(!currentText.empty())
					{
						_nodes.push_back(
							boost::shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					_nodes.push_back(
						boost::shared_ptr<WebpageContentNode>(
							new IncludeExpression(it, end)
					)	);

				} // Goto
				else if(*it == '<' && it+1 != end && *(it+1)=='%' && it+2 != end)
				{
					++it;
					++it;

					if(!currentText.empty())
					{
						_nodes.push_back(
							boost::shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					_nodes.push_back(
						boost::shared_ptr<WebpageContentNode>(
							new GotoNode(it, end)
					)	);

				} // Label
				else if(*it == '<' && it+1 != end && *(it+1)=='<' && it+2 != end)
				{
					++it;
					++it;

					if(!currentText.empty())
					{
						_nodes.push_back(
							boost::shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					_nodes.push_back(
						boost::shared_ptr<WebpageContentNode>(
							new LabelNode(it, end)
					)	);

				}
				else 
				{
					// Check if the termination was reached
					bool terminated(false);
					BOOST_FOREACH(const string& test, termination)
					{
						if(Expression::CompareText(it, end, test))
						{
							terminated = true;
							break;
						}
					}
					if(terminated)
					{
						terminationFound = true;
						break;
					}

					// The character is added to the current text
					currentText.push_back(*it);
					++it;
				}
			}

			if(!currentText.empty())
			{
				_nodes.push_back(
					boost::shared_ptr<WebpageContentNode>(
						new ConstantExpression(currentText)
				)	);
				currentText.clear();
			}

			if(!termination.empty() && !terminationFound)
			{
				string message("CMS parsing error : a block beginning by ");
				for(size_t i(0); i<20 && beginning != end; ++i, ++beginning)
				{
					message.push_back(*beginning);
				}
				message += " was never terminated by "+ *termination.begin();
				Log::GetInstance().warn(message);
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Evaluates the nodes on a stream.
		/// @param stream stream to write on
		/// @param request current request
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		void CMSScript::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			boost::shared_lock<shared_recursive_mutex> lock(*_sharedMutex);

			Nodes::const_iterator itNode(_nodes.begin());
			while(itNode != _nodes.end())
			{
				// Goto
				if(dynamic_cast<GotoNode*>(itNode->get()))
				{
					// Search of label
					string label(
						static_cast<GotoNode*>(itNode->get())->eval(
							request,
							additionalParametersMap,
							page,
							variables
					)	);
					if(!label.empty())
					{
						Nodes::const_iterator itGotoNode(itNode+1);
						for(; itGotoNode != _nodes.end(); ++itGotoNode)
						{
							if(dynamic_cast<LabelNode*>(itGotoNode->get()) && static_cast<LabelNode*>(itGotoNode->get())->getLabel() == label)
							{
								itNode = itGotoNode;
								break;
							}
						}
						if(itGotoNode == _nodes.end())
						{
							for(itGotoNode = _nodes.begin(); itGotoNode != itNode; ++itGotoNode)
							{
								if(dynamic_cast<LabelNode*>(itGotoNode->get()) && static_cast<LabelNode*>(itGotoNode->get())->getLabel() == label)
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



		void CMSScript::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap
		) const	{
			ParametersMap variables;
			Website fakeSite;
			Webpage fakePage;
			fakePage.setRoot(&fakeSite);
			display(stream, request, additionalParametersMap, fakePage, variables);
		}



		void CMSScript::display(
			std::ostream& stream,
			const util::ParametersMap& additionalParametersMap
		) const	{
			StaticFunctionRequest<WebPageDisplayFunction> fakeRequest;
			display(stream, fakeRequest, additionalParametersMap);
		}



		void CMSScript::display( std::ostream& stream ) const
		{
			ParametersMap additionalParametersMap;
			display(stream, additionalParametersMap);
		}


		//////////////////////////////////////////////////////////////////////////
		/// Evaluates the nodes on a new string.
		/// @param request current request
		/// @author Hugues Romain
		/// @date 2012
		/// @since 3.4.0
		std::string CMSScript::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			stringstream s;
			display(s, request, additionalParametersMap, page, variables);
			return s.str();
		}



		std::string CMSScript::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap
		) const	{
			stringstream s;
			display(s, request, additionalParametersMap);
			return s.str();
		}



		std::string CMSScript::eval( const util::ParametersMap& additionalParametersMap) const
		{
			stringstream s;
			display(s, additionalParametersMap);
			return s.str();
		}



		std::string CMSScript::eval() const
		{
			stringstream s;
			display(s);
			return s.str();
		}
}	}

