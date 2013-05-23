
/** RankUpdateQuery class header.
	@file RankUpdateQuery.hpp
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

#ifndef SYNTHESE_db_RankUpdateQuery_hpp__
#define SYNTHESE_db_RankUpdateQuery_hpp__

#include "SQLExpression.hpp"
#include "DBTransaction.hpp"
#include "DB.hpp"
#include "DBModule.h"
#include "UpdateQuery.hpp"

#include <vector>
#include <utility>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

using std::string;
using std::vector;

namespace synthese
{
	namespace db
	{
		/** RankUpdateQuery class.
			@ingroup m10
		*/
		template<class Table>
		class RankUpdateQuery
		{
		private:

			boost::shared_ptr<UpdateQuery<Table> > _updateQuery;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructs a query to update a rank column in a table.
			/// This is typically used when a table contains a rank column for ordering rows
			/// and a new row is inserted or removed. In this case, all the ranking numbers
			/// following the changed row have to be incremented to decremented.
			/// @param rankColumn Name of the rank column
			/// @param delta Amount to add to the rank value of the updated rows.
			/// @param minRank Only rows with a rank value greater or equal to this parameter are updated.
			/// @param maxRank Only rows with a rank value less or equal to this parameter are updated.
			/// @author Sylvain Pasche
			/// @date 2011
			RankUpdateQuery(
				const std::string& rankColumn,
				int delta,
				size_t minRank,
				boost::optional<size_t> maxRank = boost::optional<size_t>()
			){
				_updateQuery.reset(new UpdateQuery<Table>());

				_updateQuery->addUpdateField(rankColumn, RawSQL(rankColumn + " + " + boost::lexical_cast<string>(delta)));

				_updateQuery->addWhereField(
					rankColumn,
					minRank,
					ComposedExpression::OP_SUPEQ
				);

				if(maxRank)
				{
					_updateQuery->addWhereField(
						rankColumn,
						*maxRank,
						ComposedExpression::OP_INFEQ
					);
				}
			}

			//! @name Modifiers
			//@{
				template<class T>
				void addWhereField(const std::string field, const T& value, std::string op = ComposedExpression::OP_EQ);
			//@}

			//! @name Services
			//@{
				void execute(boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()) const;
			//@}
		};



		template<class Table> template<class T>
		void RankUpdateQuery<Table>::addWhereField( const std::string field, const T& value, std::string op)
		{
			_updateQuery->addWhereField(field, value, op);
		}



		template<class Table>
		void RankUpdateQuery<Table>::execute(boost::optional<DBTransaction&> transaction) const
		{

			_updateQuery->execute(transaction);
		}
	}
}

#endif // SYNTHESE_db_RankUpdateQuery_hpp__
