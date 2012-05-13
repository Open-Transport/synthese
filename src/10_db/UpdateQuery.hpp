
/** UpdateQuery class header.
	@file UpdateQuery.hpp
	@author Sylvain Pasche
	@date 2011

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

#ifndef SYNTHESE_db_UpdateQuery_hpp__
#define SYNTHESE_db_UpdateQuery_hpp__

#include "ComposedExpression.hpp"

#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace db
	{
		class DBTransaction;

		/** UpdateQuery class.
			@ingroup m10
		*/
		class DynamicUpdateQuery
		{
		private:
			typedef std::pair<std::string, boost::shared_ptr<SQLExpression> > UpdateType;
			typedef std::vector<UpdateType> UpdatesType;
			typedef std::vector<boost::shared_ptr<SQLExpression> > WheresType;

			std::string _table;
			UpdatesType _updates;
			WheresType _wheres;

		public:
			DynamicUpdateQuery(const std::string table) : _table(table) {}

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



		template<class Table>
		class UpdateQuery:
			public DynamicUpdateQuery
		{
		public:
			UpdateQuery():
			DynamicUpdateQuery(Table::TABLE.NAME)
			{}
		};



		template<class T>
		void DynamicUpdateQuery::addUpdateField(
			const std::string field,
			const T& value
		){
			_updates.push_back(
				std::make_pair(
					field,
					ValueExpression<T>::Get(value)
			)	);
		}



		template<class T>
		void DynamicUpdateQuery::addWhereField(
			const std::string field,
			const T& value, std::string op
		){
			_wheres.push_back(
				ComposedExpression::Get(
					FieldExpression::Get(_table, field),
					op,
					ValueExpression<T>::Get(value)
			)	);
		}
}	}

#endif // SYNTHESE_db_UpdateQuery_hpp__
