
/** SQLSingleOperatorExpression class header.
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

#ifndef SYNTHESE_db_SQLSingleOperatorExpression_hpp__
#define SYNTHESE_db_SQLSingleOperatorExpression_hpp__

#include "SQLExpression.hpp"

namespace synthese
{
	namespace db
	{
		//////////////////////////////////////////////////////////////////////////
		/// SQL Expression composed by two operands and one operator.
		/// Use SQLSingleOperatorExpression::Get to generate a shared_ptr instance.
		///	@ingroup m10
		class SQLSingleOperatorExpression:
			public SQLExpression
		{
		public:
			static const std::string OP_NOT;
			static const std::string OP_MAX;
			static const std::string OP_MIN;
			static const std::string OP_ABS;
			static const std::string OP_IS_NULL;

		private:
			std::string _op;						//!< the operator
			boost::shared_ptr<SQLExpression> _expr; //!< the operand



			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param op the operator (use OP_ constants defined in this class)
			/// @param expr the operand
			SQLSingleOperatorExpression(
				const std::string& op,
				boost::shared_ptr<SQLExpression> expr
			);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// SQL expression generator.
			/// @return the expression in SQL language
			virtual std::string toString() const;



			//////////////////////////////////////////////////////////////////////////
			/// Shared pointer construction helper.
			/// @param op the operator (use OP_ constants defined in this class)
			/// @param expr the operand
			static boost::shared_ptr<SQLExpression> Get(
				const std::string& op,
				boost::shared_ptr<SQLExpression> expr
			);
		};
}	}

#endif // SYNTHESE_db_SQLSingleOperatorExpression_hpp__

