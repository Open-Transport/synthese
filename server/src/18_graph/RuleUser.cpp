
/** RuleUser class implementation.
	@file RuleUser.cpp

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

#include "RuleUser.h"
#include "AllowedUseRule.h"
#include "ForbiddenUseRule.h"
#include "Exception.h"
#include "AccessParameters.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace graph
	{
		const UseRule& RuleUser::getUseRule(
			const size_t rank
		) const	{
			const UseRule* rule(_rules[rank]);
			if(rule)
			{
				return *rule;
			}
			const RuleUser* parentRuleUser(_getParentRuleUser());
			if (parentRuleUser)
			{
				return parentRuleUser->getUseRule(rank);
			}
			return (rank == USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET) ?
				static_cast<const UseRule&>(*AllowedUseRule::INSTANCE) :
				static_cast<const UseRule&>(*ForbiddenUseRule::INSTANCE);
		}



		bool RuleUser::isCompatibleWith(
			const AccessParameters& accessParameters
		) const	{
			const UseRule& rule(getUseRule(accessParameters.getUserClassRank()));
			return rule.isCompatibleWith(accessParameters);
		}



		const UseRule* RuleUser::getRule( UserClassCode classCode ) const
		{
			return _rules[classCode - USER_CLASS_CODE_OFFSET];
		}



		RuleUser::Rules RuleUser::GetEmptyRules()
		{
			return Rules(USER_CLASSES_VECTOR_SIZE, NULL);
		}
	}
}
