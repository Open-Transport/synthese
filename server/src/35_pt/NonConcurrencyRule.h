
/** NonConcurrencyRule class header.
	@file NonConcurrencyRule.h

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

#ifndef SYNTHESE_env_NonConcurrencyRule_h__
#define SYNTHESE_env_NonConcurrencyRule_h__

#include "Object.hpp"

#include "CommercialLine.h"
#include "MinutesField.hpp"
#include "PointerField.hpp"

namespace synthese
{
	FIELD_POINTER(PriorityLine, pt::CommercialLine)
	FIELD_POINTER(HiddenLine, pt::CommercialLine)
	FIELD_MINUTES(Delay)

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(PriorityLine),
		FIELD(HiddenLine),
		FIELD(Delay)
	> NonConcurrencyRuleSchema;

	namespace pt
	{
		/** Non-concurrency rule class.
			@ingroup m35
		*/
		class NonConcurrencyRule:
			public Object<NonConcurrencyRule, NonConcurrencyRuleSchema>,
			public virtual util::Registrable
		{
		public:
			NonConcurrencyRule(
				util::RegistryKeyType key = 0
			);

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			virtual void unlink();
		};
	}
}

#endif // SYNTHESE_env_NonConcurrencyRule_h__
