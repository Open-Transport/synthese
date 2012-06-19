
/** SelectQuery class header.
	@file SelectQuery.hpp

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

#ifndef SYNTHESE_db_SelectQuery_hpp__
#define SYNTHESE_db_SelectQuery_hpp__

#include "ComposedExpression.hpp"
#include "DBResult.hpp"
#include "DBModule.h"
#include "DB.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <utility>
#include <sstream>
#include <string>

namespace synthese
{
	namespace db
	{
		/** SelectQuery class.
			@ingroup m10
		*/
		template<class Table>
		class SelectQuery
		{

		private:
			typedef std::vector<std::pair<boost::shared_ptr<SQLExpression>, std::string> > FieldsType;
			struct JoinedTable { std::string table; std::string alias; boost::shared_ptr<SQLExpression> on; };
			typedef std::vector<JoinedTable> TablesType;
			typedef std::vector<boost::shared_ptr<SQLExpression> > WheresType;
			typedef std::vector<std::pair<boost::shared_ptr<SQLExpression>, bool> > OrdersType;
			typedef std::vector<boost::shared_ptr<SQLExpression> > GroupsType;

			FieldsType _fields;
			TablesType _tables;
			WheresType _wheres;
			GroupsType _groups;
			OrdersType _orders;
			std::size_t _first;
			std::size_t _number;

		public:
			SelectQuery() : _first(0), _number(0) {}

			//! @name Setters
			//@{
				void setFirst(std::size_t value) { _first = value; }
				void setNumber(std::size_t value) { _number = value; }
			//@}

			//! @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Basic field generator.
				/// @param expression expression of the field
				/// @param alias alias of the field
				void addField(
					boost::shared_ptr<SQLExpression> expression,
					std::string alias = std::string()
				);

				//////////////////////////////////////////////////////////////////////////
				/// Shortcut to add a simple field of the queried table.
				/// @param field the name of the field in the table
				/// @param alias alias of the field
				void addTableField(
					const std::string& field,
					std::string alias = std::string()
				);



				//////////////////////////////////////////////////////////////////////////
				/// Shortcut to add a static value as a field.
				/// @param value the value
				/// @param alias alias of the field
				template<class T>
				void addValueField(
					const T& value,
					std::string alias = std::string()
				);


				template<class Table2>
				void addTableAndEqualJoin(std::string field2 = TABLE_COL_ID, std::string field1 = TABLE_COL_ID, std::string alias = std::string());

				void addTableAndEqualJoin(std::string field2, std::string field1, std::string table2Name, std::string alias = std::string());

				template<class Table1, class Table2>
				void addTableAndEqualOtherJoin(std::string field2 = TABLE_COL_ID, std::string field1 = TABLE_COL_ID, std::string alias2 = std::string());

				template<class Table2>
				void addTableAndEqualOtherJoinAlias(std::string field2, std::string table1, std::string field1 = TABLE_COL_ID, std::string alias2 = std::string());

				template<class Table2>
				void addTableJoin(boost::shared_ptr<SQLExpression> onClause, std::string alias2 = std::string());


				void addOrderField(const std::string& field, bool raisingOrder);

				template<class Table1>
				void addOrderFieldOther(const std::string& field, bool raisingOrder);

				void addOrderFieldOtherAlias(const std::string& table, const std::string& field, bool raisingOrder);

				void addOrder(boost::shared_ptr<SQLExpression> expr, bool raisingOrder);

				template<class T>
				void addWhereField(const std::string field, const T& value, std::string op = ComposedExpression::OP_EQ);

				template<class Table1, class T>
				void addWhereFieldOther(const std::string field, const T& value, std::string op = ComposedExpression::OP_EQ);

				template<class T>
				void addWhereFieldOtherAlias(const std::string& table, const std::string field, const T& value, std::string op = ComposedExpression::OP_EQ);

				void addWhere(boost::shared_ptr<SQLExpression> expression) { _wheres.push_back(expression); }

				void addGroupByField(const std::string field = TABLE_COL_ID);

				template<class Table1>
				void addGroupByFieldOther(const std::string field = TABLE_COL_ID);

				void addGroupByFieldOtherAlias(const std::string& table, const std::string field = TABLE_COL_ID);
			//@}

			//! @name Services
			//@{
				std::string toString() const;
				DBResultSPtr execute() const;
			//@}
		};



		template<class Table>
		void synthese::db::SelectQuery<Table>::addField( boost::shared_ptr<SQLExpression> expression, std::string alias /*= std::string() */ )
		{
			_fields.push_back(
				std::make_pair(
					expression,
					alias
			)	);
		}



		template<class Table>
		void SelectQuery<Table>::addTableField(
			const std::string& field,
			std::string alias /*= std::string()*/
		){
			addField(
				FieldExpression::Get(Table::TABLE.NAME, field),
				alias
			);
		}




		template<class Table> template <class T>
		void SelectQuery<Table>::addValueField(
			const T& value,
			std::string alias /*= std::string()*/
		){
			addField(
				ValueExpression<T>::Get(value),
				alias
			);
		}



		template<class Table1> template<class Table2>
		void synthese::db::SelectQuery<Table1>::addTableJoin(
			boost::shared_ptr<SQLExpression> onClause,
			std::string alias2 /*= std::string()*/
		){
			JoinedTable t;
			t.alias = alias2;
			t.table = Table2::TABLE.NAME;
			t.on = onClause;
			_tables.push_back(t);
		}



		template<class Table1> template <class Table2>
		void SelectQuery<Table1>::addTableAndEqualJoin( std::string field2, std::string field1, std::string alias )
		{
			JoinedTable t;
			t.alias = alias;
			t.table = Table2::TABLE.NAME;
			t.on = ComposedExpression::Get(
				FieldExpression::Get(Table1::TABLE.NAME, field1),
				ComposedExpression::OP_EQ,
				FieldExpression::Get(alias.empty() ? Table2::TABLE.NAME : alias, field2)
			);
			_tables.push_back(t);
		}

		template<class Table1>
		void SelectQuery<Table1>::addTableAndEqualJoin( std::string field2, std::string field1, std::string table2Name, std::string alias )
		{
			JoinedTable t;
			t.alias = alias;
			t.table = table2Name;
			t.on = ComposedExpression::Get(
				FieldExpression::Get(Table1::TABLE.NAME, field1),
				ComposedExpression::OP_EQ,
				FieldExpression::Get(alias.empty() ? table2Name : alias, field2)
			);
			_tables.push_back(t);
		}


		template<class Table> template<class Table2, class Table1>
		void SelectQuery<Table>::addTableAndEqualOtherJoin(std::string field2, std::string field1, std::string alias2 /*= std::string()*/ )
		{
			JoinedTable t;
			t.alias = alias2;
			t.table = Table2::TABLE.NAME;
			t.on = ComposedExpression::Get(
				FieldExpression::Get(Table1::TABLE.NAME, field1),
				ComposedExpression::OP_EQ,
				FieldExpression::Get(alias2.empty() ? Table2::TABLE.NAME : alias2, field2)
			);
			_tables.push_back(t);
		}


		template<class Table> template<class Table2>
		void SelectQuery<Table>::addTableAndEqualOtherJoinAlias( std::string table1, std::string field1, std::string field2, std::string alias2 /*= std::string()*/ )
		{
			JoinedTable t;
			t.alias = alias2;
			t.table = Table2::TABLE.NAME;
			t.on = ComposedExpression::Get(
				FieldExpression::Get(table1, field1),
				ComposedExpression::OP_EQ,
				FieldExpression::Get(alias2.empty() ? Table2::TABLE.NAME : alias2, field2)
			);
			_tables.push_back(t);
		}



		template<class Table>
		void SelectQuery<Table>::addOrderField( const std::string& field, bool raisingOrder )
		{
			_orders.push_back(std::make_pair(FieldExpression::Get(Table::TABLE.NAME, field), raisingOrder));
		}


		template<class Table> template<class Table1>
		void SelectQuery<Table>::addOrderFieldOther( const std::string& field, bool raisingOrder )
		{
			_orders.push_back(std::make_pair(FieldExpression::Get(Table1::TABLE.NAME, field), raisingOrder));
		}



		template<class Table>
		void SelectQuery<Table>::addOrderFieldOtherAlias(const std::string& table, const std::string& field, bool raisingOrder )
		{
			_orders.push_back(std::make_pair(FieldExpression::Get(table, field), raisingOrder));
		}



		template<class Table> template<class T>
		void SelectQuery<Table>::addWhereField( const std::string field, const T& value, std::string op)
		{
			_wheres.push_back(
				ComposedExpression::Get(
					FieldExpression::Get(Table::TABLE.NAME, field),
					op,
					ValueExpression<T>::Get(value)
			)	);
		}



		template<class Table> template<class Table1, class T>
		void SelectQuery<Table>::addWhereFieldOther(const std::string field, const T& value, std::string op)
		{
			_wheres.push_back(
				ComposedExpression::Get(
					FieldExpression::Get(Table1::TABLE.NAME, field),
					op,
					ValueExpression<T>::Get(value)
			)	);
		}



		template<class Table> template<class T>
		void SelectQuery<Table>::addWhereFieldOtherAlias(const std::string& table, const std::string field, const T& value, std::string op)
		{
			_wheres.push_back(
				ComposedExpression::Get(
					FieldExpression::Get(table, field),
					op,
					ValueExpression<T>::Get(value)
			)	);
		}



		template<class Table>
		void SelectQuery<Table>::addGroupByField( const std::string field)
		{
			_groups.push_back(
				FieldExpression::Get(
					Table::TABLE.NAME,
					field
			)	);
		}



		template<class Table> template<class Table1>
		void SelectQuery<Table>::addGroupByFieldOther( const std::string field)
		{
			_groups.push_back(
				FieldExpression::Get(
					Table1::TABLE.NAME,
					field
			)	);
		}



		template<class Table>
		void SelectQuery<Table>::addGroupByFieldOtherAlias(const std::string& alias, const std::string field)
		{
			_groups.push_back(
				FieldExpression::Get(
					alias,
					field
			)	);
		}



		template<class Table>
		void SelectQuery<Table>::addOrder(
			boost::shared_ptr<SQLExpression> expr,
			bool raisingOrder
		){
			_orders.push_back(
				std::make_pair(expr, raisingOrder)
			);
		}



		template<class Table>
		std::string synthese::db::SelectQuery<Table>::toString() const
		{
			std::stringstream s;
			s << "SELECT ";

			// Fields
			if(_fields.empty())
			{
				s << Table::GetFieldsGetter();
			}
			else
			{
				bool first(true);
				BOOST_FOREACH(const FieldsType::value_type& field, _fields)
				{
					if(!first)
					{
						s << ",";
					}
					s << field.first->toString();
					if(!field.second.empty())
					{
						s << " AS " << field.second;
					}
					first = false;
				}
			}

			// Tables
			s << " FROM " << Table::TABLE.NAME;
			BOOST_FOREACH(const typename TablesType::value_type& table, _tables)
			{
				s << " INNER JOIN " << table.table;
				if(!table.alias.empty())
				{
					s << " AS " << table.alias;
				}
				s << " ON " << table.on->toString();
			}

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

			// Group by
			if(!_groups.empty())
			{
				s << " GROUP BY ";
				bool first(true);
				BOOST_FOREACH(const GroupsType::value_type& group, _groups)
				{
					if(!first)
					{
						s << ",";
					}
					s << group->toString();
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
			if(_first > 0)
			{
				s << " OFFSET " << _first;
			}

			return s.str();
		}



		template<class Table>
		DBResultSPtr synthese::db::SelectQuery<Table>::execute() const
		{
			return DBModule::GetDB()->execQuery(toString());
		}

	}
}

#endif // SYNTHESE_db_SelectQuery_hpp__
