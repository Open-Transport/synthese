
/** ComposedExpression class implementation.
	@file ComposedExpression.cpp

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

#include "SQLExpression.hpp"

using namespace std;
using namespace boost;

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


		string FieldExpression::toString() const
		{
			stringstream s;
			if(!_table.empty())
			{
				s << _table << ".";
			}
			s << (_field.empty() ? std::string("*") : _field);
			return s.str();
		}



		boost::shared_ptr<SQLExpression> FieldExpression::Get( const std::string& table, const std::string& field )
		{
			return shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new FieldExpression(table,field))
			);
		}

		std::string SubQueryExpression::toString() const
		{
			stringstream s;
			s << "(" << _subquery << ")";
			return s.str();
		}



		boost::shared_ptr<SQLExpression> SubQueryExpression::Get( const std::string& subQuery )
		{
			return shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new SubQueryExpression(subQuery))
			);
		}


		std::string NotExpression::toString() const
		{
			stringstream s;
			s << "NOT " << _expression->toString();
			return s.str();
		}



		boost::shared_ptr<SQLExpression> NotExpression::Get(
			shared_ptr<SQLExpression> expression
		){
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new NotExpression(expression))
			);
		}


		std::string ComposedExpression::toString() const
		{
			stringstream s;
			s << "(" << _expr1->toString() << " " << _op << " " << _expr2->toString() << ")";
			return s.str();
		}



		boost::shared_ptr<SQLExpression> ComposedExpression::Get(
			shared_ptr<SQLExpression> expr1,
			std::string op,
			boost::shared_ptr<SQLExpression> expr2
		){
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new ComposedExpression(expr1, op, expr2))
			);
		}


		std::string IsNullExpression::toString() const
		{
			stringstream s;
			s << _expression->toString() << " IS NULL";
			return s.str();
		}



		shared_ptr<SQLExpression> IsNullExpression::Get(
			shared_ptr<SQLExpression> expression
		){
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new IsNullExpression(expression))
			);
		}



		boost::shared_ptr<SQLExpression> ValueExpression<std::string>::Get(const std::string& value)
		{
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new ValueExpression<std::string>(value))
			);
		}


		boost::shared_ptr<SQLExpression> ValueExpression<boost::posix_time::ptime>::Get(const boost::posix_time::ptime& value)
		{
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new ValueExpression<boost::posix_time::ptime>(value))
			);
		}


		boost::shared_ptr<SQLExpression> ValueExpression<boost::gregorian::date>::Get(const boost::gregorian::date& value)
		{
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new ValueExpression<boost::gregorian::date>(value))
			);
		}


		boost::shared_ptr<SQLExpression> ValueExpression<boost::logic::tribool>::Get(const boost::logic::tribool& value)
		{
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new ValueExpression<boost::logic::tribool>(value))
			);
		}


		boost::shared_ptr<SQLExpression> ValueExpression<RawSQL>::Get(const RawSQL& value)
		{
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new ValueExpression<RawSQL>(value))
			);
		}
	}
}
