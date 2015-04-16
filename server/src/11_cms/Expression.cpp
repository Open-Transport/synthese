
/** Expression class implementation.
	@file Expression.cpp

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

#include "Expression.hpp"

#include "ConstantExpression.hpp"
#include "DualOperatorExpression.hpp"
#include "ForEachExpression.hpp"
#include "IncludeExpression.hpp"
#include "ServiceExpression.hpp"
#include "SingleOperatorExpression.hpp"
#include "TripleOperatorExpression.hpp"
#include "VariableExpression.hpp"
#include "VariablesDebugExpression.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	namespace cms
	{
		boost::shared_ptr<Expression> Expression::Parse(
			string::const_iterator& it,
			string::const_iterator end,
			const string& termination
		){
			enum ParserStatus
			{
				UNDEFINED,
				DOUBLE_QUOTE_STRING,
				SINGLE_QUOTE_STRING,
				NUMBER,
				VARIABLE,
				TERMINATED
			};

			optional<string> number;
			optional<SingleOperatorExpression::Operator> singleOperator;
			optional<DualOperatorExpression::Operator> dualOperator;
			optional<TripleOperatorExpression::Operator> tripleOperator;
			optional<string> str;
			boost::shared_ptr<Expression> expr1;
			boost::shared_ptr<Expression> expr2;
			boost::shared_ptr<Expression> expr;
			VariableExpression::Items variable;

			if(	CompareText(it, end, termination))
			{
				return boost::shared_ptr<Expression>(new VariablesDebugExpression);
			}

			ParserStatus status(UNDEFINED);
			while(it != end)
			{
				// Statuses without delimiter
				switch(status)
				{
				case NUMBER:

					// Number chars
					if( (*it >= '0' && *it <= '9') ||
						*it == '.'
					){
						number->push_back(*it);
						++it;
						break;
					}

					// Other char
					expr.reset(new ConstantExpression(*number));
					number.reset();
					status = UNDEFINED;
					break;

				case VARIABLE:
					// Alphanum chars
					if( (*it >= 'a' && *it <= 'z') ||
						(*it >= 'A' && *it <= 'Z') ||
						(*it >= '0' && *it <= '9') ||
						*it == '_'
					){
						variable.rbegin()->key.push_back(*it);
						++it;
						break;
					}

					// Index
					if(	*it == '[')
					{
						++it;
						variable.rbegin()->index = Parse(it, end, "]");
						break;
					}

					// Sub map
					if(	*it == '.')
					{
						++it;
						VariableExpression::Item item;
						variable.push_back(item);
						break;
					}

					// Other char
					expr.reset(new VariableExpression(variable));
					variable.clear();
					status = UNDEFINED;
					break;

				case UNDEFINED:

					// Characters to trim
					if(*it == ' ' || *it == '\r' ||*it == '\n')
					{
						++it;
						continue;
					}

					// Termination
					if(	CompareText(it, end, termination))
					{
						status = TERMINATED;
						break;
					}

					// Number
					if((*it >= '0' && *it <= '9'))
					{
						status = NUMBER;
						number = string();
						number->push_back(*it);
						++it;
						break;
					}

					// Double quote string
					if(*it == '"')
					{
						status = DOUBLE_QUOTE_STRING;
						++it;
						str = string();
						break;
					}

					// Single quote string
					if(*it == '\'')
					{
						status = SINGLE_QUOTE_STRING;
						++it;
						str = string();
						break;
					}

					// Variable
					if(	(*it >= 'a' && *it <= 'z') ||
						(*it >= 'A' && *it <= 'Z') ||
						(*it == '_')
					){
						status = VARIABLE;
						VariableExpression::Item item;
						item.key.push_back(*it);
						variable.push_back(item);
						++it;
						break;
					}

					// Parenthesis
					if(	*it == '(')
					{
						++it;
						expr = Parse(it, end, ")");
						break;
					}

					// Service call
					if(	*it == '<' && it+1 != end && it+2 != end && *(it+1)=='?'
					){
						++it;
						++it;

						expr.reset(new ServiceExpression(it, end));
						break;
					}

					// Expression call
					if(	*it == '<' && it+1 != end && it+2 != end && *(it+1)=='@'
					){
						++it;
						++it;

						expr = Parse(it, end, "@>");
						break;
					}

					// Inclusion call
					if(	*it == '<' && it+1 != end && it+2 != end && *(it+1)=='#'
					){
						++it;
						++it;

						expr.reset(new IncludeExpression(it, end));
						break;
					}

					// For each call
					if(	*it == '<' && it+1 != end && it+2 != end && *(it+1)=='{'
					){
						++it;
						++it;

						expr.reset(new ForEachExpression(it, end, false));
						break;
					}

					// Single operator
					if(!expr1.get())
					{
						singleOperator = SingleOperatorExpression::ParseOperator(it, end);
						if(singleOperator)
						{
							break;
						}
					}
					
					// Dual operator
					if(expr1.get())
					{
						dualOperator = DualOperatorExpression::ParseOperator(it, end);
						if(dualOperator)
						{
							break;
						}
					}

					// Triple operator
					if(expr1.get())
					{
						if(expr2.get())
						{
							optional<TripleOperatorExpression::Operator> op2(
								TripleOperatorExpression::ParseOperator2(it, end)
							);
							if(!op2 || *op2 != *tripleOperator)
							{
								status = TERMINATED;
								expr1.reset();
								expr2.reset();
							}
							break;
						}
						else
						{
							tripleOperator = TripleOperatorExpression::ParseOperator1(it, end);
							if(tripleOperator)
							{
								break;
							}
						}
					}

					// Other ignored chars
					++it;
					break;

				case DOUBLE_QUOTE_STRING:
				case SINGLE_QUOTE_STRING:

					// Escaped char
					if(	*it == '\\' && it+1 != end
					){
						++it;
						str->push_back(*it);
						++it;
						break;
					}

					// End
					if( (status == DOUBLE_QUOTE_STRING && *it == '"') ||
						(status == SINGLE_QUOTE_STRING && *it == '\'')
					){
						status = UNDEFINED;
						++it;
						expr.reset(new ConstantExpression(*str));
						str.reset();
						break;
					}

					// Add all other char
					str->push_back(*it);
					++it;
					break;
					
				case TERMINATED:
					break;
				}

				// Handling last terminated item
				if(	status == UNDEFINED ||
					status == TERMINATED
				){
					// Handling a valid expression ofter an operator
					if(expr.get())
					{
						if(singleOperator)
						{
							expr1.reset(
								new SingleOperatorExpression(
									expr,
									*singleOperator
							)	);
							singleOperator.reset();
						}
						else if(dualOperator && expr1.get())
						{
							expr1.reset(
								new DualOperatorExpression(
								expr1,
								*dualOperator,
								expr
							)	);
						}
						else if(tripleOperator)
						{
							if(expr2.get())
							{
								expr1.reset(
									new TripleOperatorExpression(
										expr1,
										expr2,
										expr,
										*tripleOperator
								)	);
							}
							else
							{
								expr2 = expr;
							}
						}
						else
						{
							expr1 = expr;
						}
						expr.reset();
					}
				}

				// Termination
				if(status == TERMINATED)
				{
					break;
				}
			}

			return expr1;
		}



		void Expression::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			stream << eval(request, additionalParametersMap, page, variables);
		}



		bool Expression::CompareText(
			std::string::const_iterator& it,
			std::string::const_iterator end,
			const std::string& text
		){
			string::const_iterator it2(it);
			for(size_t i(0); i<text.size(); ++i)
			{
				if(	it2 == end ||
					*it2 != text.at(i)
				){
					return false;
				}
				++it2;
			}
			it = it2;
			return true;
		}
}	}
