
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

#include "ConstantExpression.hpp"
#include "ForEachExpression.hpp"
#include "GotoNode.hpp"
#include "IncludeExpression.hpp"
#include "HTMLTable.h"
#include "LabelNode.hpp"
#include "Request.h"
#include "ServiceExpression.hpp"
#include "VariableUpdateNode.hpp"
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

	template<> const Field ComplexObjectFieldDefinition<WebpageContent>::FIELDS[] = {
		Field("content1", SQL_TEXT, true),
		Field("ignore_white_chars", SQL_BOOLEAN),
		Field("mime_type", SQL_TEXT),
	Field() };
	FIELD_COMPLEX_NO_LINKED_OBJECT_ID(WebpageContent)



	template<>
	void ComplexObjectField<WebpageContent, WebpageContent>::LoadFromRecord(
		WebpageContent& fieldObject,
		ObjectBase& object,
		const Record& record,
		const util::Env& env
	){
		if(record.isDefined(FIELDS[2].name))
		{
			string value(record.getDefault<string>(FIELDS[2].name));
			try
			{
				fieldObject._mimeType = MimeTypes::GetMimeTypeByString(value);
			}
			catch(Exception&)
			{
				vector<string> parts;
				split(parts, value, is_any_of("/"));
				if(parts.size() >= 2)
				{
					fieldObject._mimeType = MimeType(parts[0], parts[1], "");
				}
				else
				{
					fieldObject._mimeType = MimeTypes::HTML;
				}
			}
		}

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
		const std::string& prefix,
		boost::logic::tribool withFiles
	){
		// Content
		if(	boost::logic::indeterminate(withFiles) ||
			FIELDS[0].exportOnFile == withFiles
		){
			map.insert(
				prefix + FIELDS[0].name,
				ObjectField<void, string>::Serialize(
					fieldObject._code,
					map.getFormat()
			)	);
		}

		// Ignore white chars
		if(	boost::logic::indeterminate(withFiles) ||
			FIELDS[1].exportOnFile == withFiles
		){
			map.insert(
				prefix + FIELDS[1].name,
				fieldObject._ignoreWhiteChars
			);
		}

		// Mime type
		if(	boost::logic::indeterminate(withFiles) ||
			FIELDS[2].exportOnFile == withFiles
		){
			map.insert(
				prefix + FIELDS[2].name,
				ObjectField<void, string>::Serialize(
					string(fieldObject._mimeType),
					map.getFormat()
			)	);
		}
	}



	template<>
	void ComplexObjectField<WebpageContent, WebpageContent>::SaveToFilesMap(
		const WebpageContent& fieldObject,
		const ObjectBase& object,
		FilesMap& map
	){
		FilesMap::File item;
		item.content = fieldObject._code;
		item.mimeType = fieldObject._mimeType;
		map.insert(
			FIELDS[0].name,
			item
		);
	}


	namespace cms
	{
		shared_recursive_mutex WebpageContent::_SharedMutex;



		WebpageContent::WebpageContent(
		):	_ignoreWhiteChars(false),
			_mimeType(MimeTypes::HTML)
		{}



		WebpageContent::WebpageContent(
			const std::string& code,
			bool ignoreWhiteChars,
			MimeType mimeType
		):	_code(code),
			_ignoreWhiteChars(ignoreWhiteChars),
			_mimeType(mimeType)
		{
			_updateNodes();
		}



		WebpageContent::WebpageContent(
			std::string::const_iterator& it,
			std::string::const_iterator end,
			std::set<std::string> termination
		):	_ignoreWhiteChars(false),
			_mimeType(MimeTypes::HTML)
		{
			_parse(it, end, termination);
		}



		void WebpageContent::_updateNodes()
		{
			boost::unique_lock<shared_recursive_mutex> lock(_SharedMutex);
			string::const_iterator it(_code.begin());
			_parse(it, _code.end(), set<string>());
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




		void WebpageContent::_parse(
			std::string::const_iterator& it,
			std::string::const_iterator end,
			std::set<std::string> termination
		){
			_nodes.clear();

			string currentText;

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
							shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					// Parsing service node
					shared_ptr<ServiceExpression> node(
						new ServiceExpression(it, end)
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
							shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					// variable name
					string parameter;
					it += 2;
					string::const_iterator it2;
					for(it2 = it;
						it2 != end && *it2 != '=' && *it2 != '@' && *it2 != '<';
						++it2
					){
						parameter.push_back(*it2);
					}

					// Is a variable set ?
					if(	it2 != end &&
						*it2 == '=' &&
						(it2+1 == end || *(it2+1) != '=') &&
						(*(it2-1) != '!')
					){
						it = it2;
						++it;

						trim(parameter);

						VariableUpdateNode::Items items;
						items.push_back(VariableUpdateNode::Item());
						for(string::const_iterator it3(parameter.begin()); it3!=parameter.end(); )
						{
							// Alphanum chars
							if( (*it3 >= 'a' && *it <= 'z') ||
								(*it3 >= 'A' && *it <= 'Z') ||
								(*it3 >= '0' && *it <= '9') ||
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
							shared_ptr<WebpageContentNode>(
								new VariableUpdateNode(items, it, end)
						)	);
					}
					else // Is an expression
					{
						shared_ptr<Expression> expr(
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
							shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					_nodes.push_back(
						shared_ptr<WebpageContentNode>(
							new ForEachExpression(it, end)
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
							shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					_nodes.push_back(
						shared_ptr<WebpageContentNode>(
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
							shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					_nodes.push_back(
						shared_ptr<WebpageContentNode>(
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
							shared_ptr<WebpageContentNode>(
								new ConstantExpression(currentText)
						)	);
						currentText.clear();
					}

					_nodes.push_back(
						shared_ptr<WebpageContentNode>(
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
					shared_ptr<WebpageContentNode>(
						new ConstantExpression(currentText)
				)	);
				currentText.clear();
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



		std::string WebpageContent::eval(
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
