
/** ReplaceQuery class header.
	@file ReplaceQuery.h

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

#ifndef SYNTHESE_db_ReplaceQuery_h__
#define SYNTHESE_db_ReplaceQuery_h__

#include "DB.hpp"
#include "DBModule.h"
#include "DBInterSYNTHESE.hpp"
#include "DBRecord.hpp"
#include "InterSYNTHESEModule.hpp"
#include "ParametersMap.h"
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
			util::RegistryKeyType _id;
			DBContent _content;

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
				_content.push_back(Cell(_id));
			}



			//////////////////////////////////////////////////////////////////////////
			/// Adds a value field to the query
			/// @param value value to add, handled by ValueExpression
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			template<class T>
			void addField(const T& value);


			void addFieldNull();


			template<template<class> class T>
			void addFrameworkField(
				const typename T<void>::Type& value
			){
				T<void>::SaveToDBContent(value, _content);
			}



			//////////////////////////////////////////////////////////////////////////
			/// Runs the query.
			/// @param transaction (optional) transaction within the query must be run
			/// @author Hugues Romain
			/// @date 2010
			void execute(
				boost::optional<DBTransaction&> transaction
			) const;
		};



		template<class TableSync>
		void synthese::db::ReplaceQuery<TableSync>::addFieldNull()
		{
			_content.push_back(
				Cell(boost::optional<std::string>())
			);
		}



		template<class TableSync>
		void ReplaceQuery<TableSync>::execute(boost::optional<DBTransaction&> transaction) const
		{
			DBRecord record(
				*DBModule::GetTableSync(TableSync::TABLE.ID)
			);
			record.setContent(_content);
			DBModule::GetDB()->replaceStmt(
				_id,
				record,
				transaction
			);
		}


		template<class Table> template<class T>
		void ReplaceQuery<Table>::addField(const T& value)
		{
			_content.push_back(
				Cell(value)
			);
		}
	}
}

#endif // SYNTHESE_db_ReplaceQuery_h__
