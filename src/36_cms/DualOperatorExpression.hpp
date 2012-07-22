
/** DualOperatorExpression class header.
	@file DualOperatorExpression.hpp

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

#ifndef SYNTHESE_cms_DualOperatorExpression_hpp__
#define SYNTHESE_cms_DualOperatorExpression_hpp__

#include "Expression.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace cms
	{
		/** DualOperatorExpression class.
			@ingroup m36
		*/
		class DualOperatorExpression:
			public Expression
		{
		public:
			enum Operator
			{
				ADD,
				SUB,
				MUL,
				DIV,
				MOD,
				OR,
				AND,
				BIT_AND,
				BIT_OR,
				POW
			};

		private:

			boost::shared_ptr<Expression> _left;
			Operator _operator;
			boost::shared_ptr<Expression> _right;

		public:
			DualOperatorExpression(
				boost::shared_ptr<Expression> left,
				Operator op,
				boost::shared_ptr<Expression> right
			);

			virtual std::string eval(
				const server::Request& request,
				const util::ParametersMap& additionalParametersMap,
				const Webpage& page,
				util::ParametersMap& variables
			) const;


			static boost::optional<Operator> ParseOperator(
				std::string::const_iterator& it,
				const std::string::const_iterator end
			);
		};
	}
}

#endif // SYNTHESE_cms_DualOperatorExpression_hpp__

