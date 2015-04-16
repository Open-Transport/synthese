
/** ComposedExpression class implementation.
	@file ComposedExpression.cpp

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

#include "ComposedExpression.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	namespace db
	{
		const string ComposedExpression::OP_AND("AND");
		const string ComposedExpression::OP_BITAND("&");
		const string ComposedExpression::OP_BITOR("|");
		const string ComposedExpression::OP_EQ("=");
		const string ComposedExpression::OP_INF("<");
		const string ComposedExpression::OP_INFEQ("<=");
		const string ComposedExpression::OP_LIKE("LIKE");
		const string ComposedExpression::OP_OR("OR");
		const string ComposedExpression::OP_SUP(">");
		const string ComposedExpression::OP_SUPEQ(">=");
		const string ComposedExpression::OP_IN("IN");
		const string ComposedExpression::OP_NOTIN("NOT IN");
		const string ComposedExpression::OP_DIFF("!=");
		const string ComposedExpression::OP_ADD("+");
		const string ComposedExpression::OP_SUB("-");
		const string ComposedExpression::OP_MUL("*");
		const string ComposedExpression::OP_DIV("/");



		ComposedExpression::ComposedExpression(
			boost::shared_ptr<SQLExpression> expr1,
			const string& op,
			boost::shared_ptr<SQLExpression> expr2
		):	_expr1(expr1),
			_op(op),
			_expr2(expr2)
		{}



		string ComposedExpression::toString() const
		{
			stringstream s;
			s << "(" << _expr1->toString() << " " << _op << " " << _expr2->toString() << ")";
			return s.str();
		}



		boost::shared_ptr<SQLExpression> ComposedExpression::Get(
			boost::shared_ptr<SQLExpression> expr1,
			const string& op,
			boost::shared_ptr<SQLExpression> expr2
		){
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(
					new ComposedExpression(expr1, op, expr2)
			)	);
		}
}	}
