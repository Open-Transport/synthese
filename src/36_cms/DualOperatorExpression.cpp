
/** DualOperatorExpression class implementation.
	@file DualOperatorExpression.cpp

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

#include "DualOperatorExpression.hpp"

#include <complex>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	namespace cms
	{
		DualOperatorExpression::DualOperatorExpression(
			boost::shared_ptr<Expression> left,
			Operator op,
			boost::shared_ptr<Expression> right
		):	_left(left),
			_operator(op),
			_right(right)
		{}



		std::string DualOperatorExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			// Check if all operands are not null
			if(	!_left.get() ||
				!_right.get()
			){
				return string();
			}

			// Operands evaluation
			string left(_left->eval(request, additionalParametersMap, page, variables));
			trim(left);
			string right(_right->eval(request, additionalParametersMap, page, variables));
			trim(right);

			// Non numeric operator
			switch(_operator)
			{
			case OR:
				if ((!left.empty() && left != "0") || (!right.empty() && right != "0"))
				{
					return "1";
				}
				else
				{
					return "0";
				}

			case AND:
				if((!left.empty() && left != "0") && (!right.empty() && right != "0"))
				{
					return "1";
				}
				else
				{
					return "0";
				}

			// Decimal operator
			case ADD:
			case SUB:
			case MUL:
			case DIV:
			case POW:
				try
				{
					double leftDbl(lexical_cast<double>(left));
					double rightDbl(lexical_cast<double>(right));
				
					switch(_operator)
					{
					case ADD:
						return lexical_cast<string>(leftDbl + rightDbl);

					case SUB:
						return lexical_cast<string>(leftDbl - rightDbl);

					case MUL:
						return lexical_cast<string>(leftDbl * rightDbl);

					case DIV:
						return lexical_cast<string>(leftDbl / rightDbl);

					case POW:
						return lexical_cast<string>(pow(leftDbl, rightDbl));
					}
				}
				catch(bad_lexical_cast&)
				{
					break;
				}

			// Integer operator
			case MOD:
			case BIT_AND:
			case BIT_OR:
				try
				{
					int leftInt(lexical_cast<int>(left));
					int rightInt(lexical_cast<int>(right));

					switch(_operator)
					{
					case MOD:
						return lexical_cast<string>(leftInt - (rightInt * (leftInt / rightInt)));

					case BIT_AND:
						return lexical_cast<string>(leftInt & rightInt);

					case BIT_OR:
						return lexical_cast<string>(leftInt | rightInt);
					}
				}
				catch(bad_lexical_cast&)
				{
					break;
				}
			}

			// Admit + as concatenation operator
			if(_operator == ADD)
			{
				return left + right;
			}

			return string();
		}



		boost::optional<DualOperatorExpression::Operator> DualOperatorExpression::ParseOperator(
			std::string::const_iterator& it,
			const std::string::const_iterator end
		){
			if(	CompareText(it, end, "+")
			){
				return ADD;
			}
			if(	CompareText(it, end, "-")
			){
				return SUB;
			}
			if(	CompareText(it, end, "*")
			){
				return MUL;
			}
			if(	CompareText(it, end, "/")
			){
				return DIV;
			}
			if(	CompareText(it, end, "%")
			){
				return MOD;
			}
			if(	CompareText(it, end, "||")
			){
				return OR;
			}
			if(	CompareText(it, end, "&&")
			){
				return AND;
			}
			if(	CompareText(it, end, "|")
			){
				return BIT_OR;
			}
			if(	CompareText(it, end, "&")
			){
				return BIT_AND;
			}
			if(	CompareText(it, end, "^")
			){
				return POW;
			}
			return optional<Operator>();
		}
}	}
