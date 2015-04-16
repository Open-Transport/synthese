
/** SQLExpression class implementation.
	@file SQLExpression.cpp

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

#include "SQLExpression.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	namespace db
	{
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
			return boost::shared_ptr<SQLExpression>(
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
			return boost::shared_ptr<SQLExpression>(
				static_cast<SQLExpression*>(new SubQueryExpression(subQuery))
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
