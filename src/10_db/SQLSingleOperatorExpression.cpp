
/** SQLSingleOperatorExpression class implementation.
	@file SQLSingleOperatorExpression.hpp

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

#include "SQLSingleOperatorExpression.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	namespace db
	{
		const string SQLSingleOperatorExpression::OP_NOT = "NOT ";
		const string SQLSingleOperatorExpression::OP_MAX = "MAX";
		const string SQLSingleOperatorExpression::OP_MIN = "MIN";
		const string SQLSingleOperatorExpression::OP_ABS = "ABS";
		const string SQLSingleOperatorExpression::OP_IS_NULL = " IS NULL";



		SQLSingleOperatorExpression::SQLSingleOperatorExpression(
			const string& op,
			boost::shared_ptr<SQLExpression> expr
		):	_op(op),
			_expr(expr)
		{}



		string SQLSingleOperatorExpression::toString() const
		{
			stringstream s;

			// Special case for IS NULL operator
			if(_op == OP_IS_NULL)
			{
				s << _expr->toString() << _op;
			}
			else // General case
			{
				s << _op << "(" << _expr->toString() << ")";
			}

			return s.str();
		}



		boost::shared_ptr<SQLExpression> SQLSingleOperatorExpression::Get(
			const string& op,
			boost::shared_ptr<SQLExpression> expr
		){
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(
					new SQLSingleOperatorExpression(op, expr)
			)	);
		}
}	}
