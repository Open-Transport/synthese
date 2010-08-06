
/** ReplaceQuery class header.
	@file ReplaceQuery.h

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

#ifndef SYNTHESE_db_ReplaceQuery_h__
#define SYNTHESE_db_ReplaceQuery_h__

#include "DBModule.h"
#include "SQLite.h"
#include "SQLExpression.hpp"

#include <vector>

namespace synthese
{
	namespace db
	{
		//////////////////////////////////////////////////////////////////////////
		/// SQL REPLACE query generator.
		/// @ingroup m10
		/// @author Hugues Romain
		/// @date 2010
		//////////////////////////////////////////////////////////////////////////
		template<class TableSync>
		class ReplaceQuery
		{
		private:
			typedef std::vector<boost::shared_ptr<SQLExpression> > Fields;

			Fields _fields;
			util::RegistryKeyType _id;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor with auto-increment.
			/// @param object object corresponding to the record in the database
			/// @author Hugues Romain
			/// @date 2010
			//////////////////////////////////////////////////////////////////////////
			/// If the id of the object is not defined, then the object is updated by
			/// applying auto increment on it.
			ReplaceQuery(
				typename TableSync::ObjectType& object
			){
				if(object.getKey() == 0)
				{
					object.setKey(TableSync::getId());
				}
				_id = object.getKey();
			}


			//////////////////////////////////////////////////////////////////////////
			/// Adds a value field to the query
			/// @param value value to add, handled by ValueExpression
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			template<class T>
			void addField(const T& value);



			//////////////////////////////////////////////////////////////////////////
			/// Adds an expression field to the query
			/// @param value expression to add (pointer to SQLExpression object)
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			void addFieldExpression(boost::shared_ptr<SQLExpression> value);



			//////////////////////////////////////////////////////////////////////////
			/// Runs the query.
			/// @param transaction (optional) transaction within the query must be run
			/// @author Hugues Romain
			/// @date 2010
			void execute(
				boost::optional<SQLiteTransaction&> transaction
			) const;
		};



		template<class TableSync>
		void synthese::db::ReplaceQuery<TableSync>::addFieldExpression( boost::shared_ptr<SQLExpression> value )
		{
			_fields.push_back(value);
		}





		template<class TableSync>
		void ReplaceQuery<TableSync>::execute( boost::optional<SQLiteTransaction&> transaction ) const
		{
			std::stringstream query;
			query
				<< " REPLACE INTO " << TableSync::TABLE.NAME << " VALUES("
				<< _id;
			BOOST_FOREACH(const Fields::value_type& field, _fields)
			{
				query << std::fixed << "," << field->toString();
			}
			query << ");";
			DBModule::GetSQLite()->execUpdate(query.str(), transaction);
		}


		template<class Table> template<class T>
		void ReplaceQuery<Table>::addField(const T& value)
		{
			_fields.push_back(
				ValueExpression<T>::Get(value)
			);
		}
	}
}

#endif // SYNTHESE_db_ReplaceQuery_h__
