
/** SingleSentAlarm class header.
	@file SingleSentAlarm.h

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

#ifndef SYNTHESE_SingleSentAlarm_h__
#define SYNTHESE_SingleSentAlarm_h__

#include "17_messages/SentAlarm.h"

namespace synthese
{
	namespace messages
	{
		/** SingleSentAlarm class.
			@ingroup m17
		*/
		class SingleSentAlarm
			: public SentAlarm
		{
		private:
			bool			_enabled;
			time::DateTime	_periodStart; //!< Alarm applicability period start
			time::DateTime	_periodEnd;   //!< Alarm applicability period end

		public:
			SingleSentAlarm();
			~SingleSentAlarm();

			void setIsEnabled(bool value);
			void setPeriodStart ( const time::DateTime& periodStart);
			void setPeriodEnd ( const time::DateTime& periodEnd);

			bool					getIsEnabled()		const;
			const time::DateTime&	getPeriodStart()	const;
			const time::DateTime&	getPeriodEnd()		const;

		};
	}
}

#endif // SYNTHESE_SingleSentAlarm_h__
