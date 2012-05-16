
/** ComposedExpression class header.
	@file ComposedExpression.hpp

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

#ifndef SYNTHESE_db_ComposedExpression_hpp__
#define SYNTHESE_db_ComposedExpression_hpp__

#include "SQLExpression.hpp"

namespace synthese
{
	namespace db
	{
		//////////////////////////////////////////////////////////////////////////
		/// SQL Expression composed by two operands and one operator.
		/// Use ComposedExpression::Get to generate a shared_ptr instance.
		///	@ingroup m10
		class ComposedExpression:
			public SQLExpression
		{
		public:
			static const std::string OP_AND;
			static const std::string OP_OR;
			static const std::string OP_EQ;
			static const std::string OP_LIKE;
			static const std::string OP_SUP;
			static const std::string OP_SUPEQ;
			static const std::string OP_INF;
			static const std::string OP_INFEQ;
			static const std::string OP_BITAND;
			static const std::string OP_BITOR;
			static const std::string OP_IN;
			static const std::string OP_NOTIN;
			static const std::string OP_EXISTS;
			static const std::string OP_DIFF;
			static const std::string OP_ADD;
			static const std::string OP_SUB;
			static const std::string OP_MUL;
			static const std::string OP_DIV;

		private:
			boost::shared_ptr<SQLExpression> _expr1; //!< the first operand
			std::string _op;						 //!< the operator
			boost::shared_ptr<SQLExpression> _expr2; //!< the second operand

			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param expr1 the first operand
			/// @param op the operator (use OP_ constants defined in this class)
			/// @param expr2 the second operand
			ComposedExpression(
				boost::shared_ptr<SQLExpression> expr1,
				const std::string& op,
				boost::shared_ptr<SQLExpression> expr2
			);

		public:
			//////////////////////////////////////////////////////////////////////////
			/// SQL expression generator.
			/// @return the expression in SQL language
			virtual std::string toString() const;



			//////////////////////////////////////////////////////////////////////////
			/// Shared pointer construction helper.
			/// @param expr1 the first operand
			/// @param op the operator (use OP_ constants defined in this class)
			/// @param expr2 the second operand
			static boost::shared_ptr<SQLExpression> Get(
				boost::shared_ptr<SQLExpression> expr1,
				const std::string& op,
				boost::shared_ptr<SQLExpression> expr2
			);
		};
	}
}

#endif // SYNTHESE_db_ComposedExpression_hpp__

