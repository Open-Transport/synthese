
/** SentScenario class header.
	@file SentScenario.h

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

#ifndef SYNTHESE_SentScenario_h__
#define SYNTHESE_SentScenario_h__

#include "17_messages/ScenarioSubclassTemplate.h"

#include "04_time/DateTime.h"

#include "01_util/Registrable.h"

namespace synthese
{
	namespace messages
	{
		class ScenarioSentAlarm;
		class ScenarioTemplate;

		/** Scenario instance class.
			@ingroup m17
		*/
		class SentScenario
			: public ScenarioSubclassTemplate<ScenarioSentAlarm>
			, public util::Registrable<uid, SentScenario>
		{
		private:
			bool			_isEnabled;
			time::DateTime	_periodStart; //!< Alarm applicability period start
			time::DateTime	_periodEnd;   //!< Alarm applicability period end

		public:
			SentScenario();
			SentScenario(
				const ScenarioTemplate& source
				, time::DateTime periodStart = time::DateTime(time::TIME_UNKNOWN)
				, time::DateTime periodEnd = time::DateTime(time::TIME_UNKNOWN)
				);
			~SentScenario();

			const time::DateTime&	getPeriodStart()	const;
			const time::DateTime&	getPeriodEnd()		const;
			bool					getIsEnabled()		const;

			uid getId() const;

			/** Start broadcast date setter.
				Updates the alarms too.
				@param periodStart Start broadcast date
			*/
			void setPeriodStart ( const synthese::time::DateTime& periodStart);
			
			/** End broadcast date setter.
				Updates the alarms too.
				@param periodEnd End broadcast date
			*/
			void setPeriodEnd ( const synthese::time::DateTime& periodEnd);
			void setIsEnabled(bool value);
		};
	}
}

#endif // SYNTHESE_SentScenario_h__
