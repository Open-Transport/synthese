
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

#include "01_util/Registrable.h"
#include "01_util/Constants.h"

namespace synthese
{
	namespace env
	{
		class CommercialLine;

		/** Non-concurrency rule class.
			@ingroup m35
		*/
		class NonConcurrencyRule : public util::Registrable<uid, NonConcurrencyRule>
		{
			const CommercialLine*	_prorityLine;
			const CommercialLine*	_hiddenLine;
			int						_delay;

		public:
			NonConcurrencyRule(
				const CommercialLine* priorityLine = NULL
				, const CommercialLine* hiddenLine = NULL
				, int delay = UNKNOWN_VALUE
			);

			const CommercialLine*	getPriorityLine()	const;
			const CommercialLine*	getHiddenLine()		const;
			int						getDelay()			const;

			void setPriorityLine(const CommercialLine* value);
			void setHiddenLine(const CommercialLine* value);
			void setDelay(int value);

		};
	}
}

#endif // SYNTHESE_env_NonConcurrencyRule_h__
