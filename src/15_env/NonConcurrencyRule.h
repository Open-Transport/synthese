
/** NonConcurrencyRule class header.
	@file NonConcurrencyRule.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "Registrable.h"


namespace synthese
{
	namespace env
	{
		/** Non-concurrency rule class.
			@ingroup m35
		*/
		class NonConcurrencyRule
		:	public virtual util::Registrable
		{
			util::RegistryKeyType	_prorityLine;
			util::RegistryKeyType	_hiddenLine;
			int	_delay;

		public:
			NonConcurrencyRule(
				util::RegistryKeyType key,
				util::RegistryKeyType priorityLine = UNKNOWN_VALUE
				, util::RegistryKeyType hiddenLine = UNKNOWN_VALUE
				, int delay = UNKNOWN_VALUE
			);

			uid getPriorityLine()	const;
			uid	getHiddenLine()		const;
			int	getDelay()			const;

			void setPriorityLine(util::RegistryKeyType value);
			void setHiddenLine(util::RegistryKeyType value);
			void setDelay(int value);

		};
	}
}

#endif // SYNTHESE_env_NonConcurrencyRule_h__
