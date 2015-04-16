
/** NonConcurrencyRule class implementation.
	@file NonConcurrencyRule.cpp

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

#include "NonConcurrencyRule.h"

using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace pt;

	CLASS_DEFINITION(NonConcurrencyRule, "t056_non_concurrency_rules", 56)
	FIELD_DEFINITION_OF_OBJECT(NonConcurrencyRule, "non_concurrency_rule_id", "non_concurrency_rule_ids")

	FIELD_DEFINITION_OF_TYPE(PriorityLine, "priority_line_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(HiddenLine, "hidden_line_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Delay, "delay", SQL_INTEGER)


	namespace pt
	{
		NonConcurrencyRule::NonConcurrencyRule(
			RegistryKeyType key
		):	Registrable(key),
			Object<NonConcurrencyRule, NonConcurrencyRuleSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(PriorityLine),
					FIELD_DEFAULT_CONSTRUCTOR(HiddenLine),
					FIELD_VALUE_CONSTRUCTOR(Delay, minutes(0))
			)	)
		{}



		void NonConcurrencyRule::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<HiddenLine>())
			{
				get<HiddenLine>()->addConcurrencyRule(this);
			}
		}



		void NonConcurrencyRule::unlink()
		{
			if(get<HiddenLine>())
			{
				get<HiddenLine>()->removeConcurrencyRule(this);
			}
		}
	}
}
