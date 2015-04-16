
/** TripleOperatorExpression class implementation.
	@file TripleOperatorExpression.cpp

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

#include "TripleOperatorExpression.hpp"

#include "ParametersMap.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace cms
	{
		TripleOperatorExpression::TripleOperatorExpression(
			const boost::shared_ptr<Expression>& expr1,
			const boost::shared_ptr<Expression>& expr2,
			const boost::shared_ptr<Expression>& expr3,
			Operator op
		):	_expr1(expr1),
			_expr2(expr2),
			_expr3(expr3),
			_operator(op)
		{}



		std::string TripleOperatorExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			// Check if all operands are not null
			if( !_expr1.get() ||
				!_expr2.get() ||
				!_expr3.get()
			){
				return string();
			}

			// Operands evaluation
			string expr1(_expr1->eval(request, additionalParametersMap, page, variables));

			// Applying operator
			switch(_operator)
			{
			case IF_THEN_ELSE:
				expr1 = ParametersMap::Trim(expr1);
				if(expr1.empty() || expr1 == "0")
				{
					return _expr3->eval(request, additionalParametersMap, page, variables);
				}
				else
				{
					return _expr2->eval(request, additionalParametersMap, page, variables);
				}
			}

			// Should never happen
			return string();
		}



		boost::optional<TripleOperatorExpression::Operator> TripleOperatorExpression::ParseOperator1(
			std::string::const_iterator& it,
			const std::string::const_iterator end
		){
			if(	CompareText(it, end, "?"))
			{
				return IF_THEN_ELSE;
			}

			return optional<Operator>();
		}



		boost::optional<TripleOperatorExpression::Operator> TripleOperatorExpression::ParseOperator2(
			std::string::const_iterator& it,
			const std::string::const_iterator end
		){
			if(	CompareText(it, end, ":"))
			{
				return IF_THEN_ELSE;
			}

			return optional<Operator>();
		}
}	}

