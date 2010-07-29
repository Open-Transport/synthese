
/** DeleteQuery class header.
	@file DeleteQuery.hpp

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

#ifndef SYNTHESE_db_DeleteQuery_hpp__
#define SYNTHESE_db_DeleteQuery_hpp__

#include "SQLExpression.hpp"
#include "SQLiteTransaction.h"
#include "SQLite.h"
#include "DBModule.h"

#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace db
	{
		/** DeleteQuery class.
			@ingroup m10
		*/
		template<class Table>
		class DeleteQuery
		{

		private:
			typedef std::vector<boost::shared_ptr<SQLExpression> > WheresType;
			typedef std::vector<std::pair<boost::shared_ptr<SQLExpression>, bool> > OrdersType;
			
			WheresType _wheres;
			OrdersType _orders;
			std::size_t _number;

		public:
			DeleteQuery() : _number(0) {}

			//! @name Setters
			//@{
				void setNumber(std::size_t value) { _number = value; }
			//@}

			//! @name Modifiers
			//@{
				void addOrderField(const std::string field, bool raisingOrder);

				template<class T>
				void addWhereField(const std::string field, const T& value, std::string op = ComposedExpression::OP_EQ);
			//@}

			//! @name Services
			//@{
				std::string toString() const;
				void execute(boost::optional<SQLiteTransaction&> transaction = boost::optional<SQLiteTransaction&>()) const;
			//@}
		};



		template<class Table>
		void DeleteQuery<Table>::addOrderField( const std::string field, bool raisingOrder )
		{
			_orders.push_back(std::make_pair(FieldExpression::Get(Table::TABLE.NAME, field), raisingOrder));
		}



		template<class Table> template<class T>
		void DeleteQuery<Table>::addWhereField( const std::string field, const T& value, std::string op)
		{
			_wheres.push_back(
				ComposedExpression::Get(
					FieldExpression::Get(Table::TABLE.NAME, field),
					op,
					ValueExpression<T>::Get(value)
			)	);
		}



		template<class Table>
		std::string DeleteQuery<Table>::toString() const
		{
			std::stringstream s;
			s << "DELETE";

			// Tables
			s << " FROM " << Table::TABLE.NAME;

			// Where
			if(!_wheres.empty())
			{
				s << " WHERE ";
				bool first(true);
				BOOST_FOREACH(const WheresType::value_type& where, _wheres)
				{
					if(!first)
					{
						s << " AND ";
					}
					s << where->toString();
					first = false;
				}
			}

			// Order
			if(!_orders.empty())
			{
				s << " ORDER BY ";
				bool first(true);
				BOOST_FOREACH(const OrdersType::value_type& order, _orders)
				{
					if(!first)
					{
						s << ",";
					}
					s << order.first->toString() << " " << (order.second ? "ASC" : "DESC");
					first = false;
				}
			}

			// Numbers
			if(_number > 0)
			{
				s << " LIMIT " << _number;
			}

			return s.str();
		}



		template<class Table>
		void DeleteQuery<Table>::execute(boost::optional<SQLiteTransaction&> transaction) const
		{
			return DBModule::GetSQLite()->execUpdate(toString(), transaction);
		}
	}
}

#endif // SYNTHESE_db_DeleteQuery_hpp__
