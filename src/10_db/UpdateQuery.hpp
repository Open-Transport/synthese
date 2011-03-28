
/** UpdateQuery class header.
	@file UpdateQuery.hpp
	@author Sylvain Pasche
	@date 2011

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

#ifndef SYNTHESE_db_UpdateQuery_hpp__
#define SYNTHESE_db_UpdateQuery_hpp__

#include "SQLExpression.hpp"
#include "DBTransaction.hpp"
#include "DB.hpp"
#include "DBModule.h"

#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace db
	{
		/** UpdateQuery class.
			@ingroup m10
		*/
		template<class Table>
		class UpdateQuery
		{
		private:
			typedef std::pair<std::string, boost::shared_ptr<SQLExpression> > UpdateType;
			typedef std::vector<UpdateType> UpdatesType;
			typedef std::vector<boost::shared_ptr<SQLExpression> > WheresType;

			std::string _table;
			UpdatesType _updates;
			WheresType _wheres;

		public:
			UpdateQuery() : _table(Table::TABLE.NAME) {}
			UpdateQuery(const std::string table) : _table(table) {}

			//! @name Modifiers
			//@{
				template<class T>
				void addUpdateField(const std::string field, const T& value);

				template<class T>
				void addWhereField(const std::string field, const T& value, std::string op = ComposedExpression::OP_EQ);
			//@}

			//! @name Services
			//@{
				void execute(boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()) const;
			//@}

		private:
			std::string _whereQueryPart() const;

		};



		template<class Table> template<class T>
		void UpdateQuery<Table>::addUpdateField( const std::string field, const T& value)
		{
			_updates.push_back(
				std::make_pair(
					field,
					ValueExpression<T>::Get(value)
			)	);
		}



		template<class Table> template<class T>
		void UpdateQuery<Table>::addWhereField( const std::string field, const T& value, std::string op)
		{
			_wheres.push_back(
				ComposedExpression::Get(
					FieldExpression::Get(Table::TABLE.NAME, field),
					op,
					ValueExpression<T>::Get(value)
			)	);
		}



		template<class Table>
		std::string UpdateQuery<Table>::_whereQueryPart() const
		{
			std::stringstream s;

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

			return s.str();
		}



		template<class Table>
		void UpdateQuery<Table>::execute(boost::optional<DBTransaction&> transaction) const
		{
			// TODO: this should run inside of a transaction, an id could appear or disappear between the SELECT and the UPDATE.

			DB* db = DBModule::GetDB();

			std::string selectQuery("SELECT " + TABLE_COL_ID + " FROM " + Table::TABLE.NAME + _whereQueryPart());
			DBResultSPtr rows = db->execQuery(selectQuery);

			// TODO: call rowUpdated directly from this loop once checkModificationEvents() infrastructure is removed.
			std::vector<util::RegistryKeyType> updatedIds;
			while (rows->next())
			{
				updatedIds.push_back(rows->getLongLong(TABLE_COL_ID));
			}

			std::stringstream updateQuery;
			updateQuery << "UPDATE " << Table::TABLE.NAME << " SET ";

			bool first(true);
			BOOST_FOREACH(const UpdateType& update, _updates)
			{
				if(!first)
				{
					updateQuery << ",";
				}
				updateQuery << update.first << "=" << update.second->toString();
				first = false;
			}

			updateQuery << _whereQueryPart();

			db->execUpdate(updateQuery.str(), transaction);

			BOOST_FOREACH(util::RegistryKeyType updatedId, updatedIds)
			{
				db->addDBModifEvent(
					DB::DBModifEvent(
						Table::TABLE.NAME,
						DB::MODIF_UPDATE,
						updatedId
					),
					transaction
				);
			}
#ifdef DO_VERIFY_TRIGGER_EVENTS
			db->checkModificationEvents();
#endif
		}



		struct DummyTableSync
		{
			static const DBTableSync::Format TABLE;
		};



		// Same as UpdateQuery, but the table name is given to the constructor.
		// Useful to run an update query without a table sync class.
		class DynamicUpdateQuery : public UpdateQuery<DummyTableSync>
		{
		public:
			DynamicUpdateQuery(const std::string& table) : UpdateQuery<DummyTableSync>(table) {}
		};
	}
}

#endif // SYNTHESE_db_UpdateQuery_hpp__
