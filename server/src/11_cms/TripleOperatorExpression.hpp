
/** TripleOperatorExpression class header.
	@file TripleOperatorExpression.hpp

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

#ifndef SYNTHESE_cms_TripleOperatorExpression_hpp__
#define SYNTHESE_cms_TripleOperatorExpression_hpp__

#include "Expression.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace cms
	{
		/** TripleOperatorExpression class.
			@ingroup m36
		*/
		class TripleOperatorExpression:
			public Expression
		{
		public:
			enum Operator
			{
				IF_THEN_ELSE
			};

		private:
			boost::shared_ptr<Expression> _expr1;
			boost::shared_ptr<Expression> _expr2;
			boost::shared_ptr<Expression> _expr3;
			Operator _operator;

		public:
			TripleOperatorExpression(
				const boost::shared_ptr<Expression>& expr1,
				const boost::shared_ptr<Expression>& expr2,
				const boost::shared_ptr<Expression>& expr3,
				Operator op
			);

			virtual std::string eval(
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;


			static boost::optional<Operator> ParseOperator1(
				std::string::const_iterator& it,
				const std::string::const_iterator end
			);

			static boost::optional<Operator> ParseOperator2(
				std::string::const_iterator& it,
				const std::string::const_iterator end
			);
		};
}	}

#endif // SYNTHESE_cms_TripleOperatorExpression_hpp__

