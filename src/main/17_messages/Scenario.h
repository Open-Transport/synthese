
/** Scenario class header.
	@file Scenario.h

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

#ifndef SYNTHESE_Scenario_h__
#define SYNTHESE_Scenario_h__

#include <string>
#include <set>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "04_time/DateTime.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;

		/** Scenario of alarms diffusion.
			@ingroup m17

			A scenario is a collection of alarm templates.
		*/
		class Scenario : public util::Registrable<uid, Scenario>
		{
		public:
			typedef std::set<Alarm*>	AlarmsSet;

		private:
			bool						_isATemplate;
			bool						_isEnabled;
			std::string					_name;
			synthese::time::DateTime	_periodStart; //!< Alarm applicability period start
			synthese::time::DateTime	_periodEnd;   //!< Alarm applicability period end
			AlarmsSet					_alarms;

		public:
			Scenario();

			const std::string&		getName()			const;
			bool					getIsATemplate()	const;
			const time::DateTime&	getPeriodStart()	const;
			const time::DateTime&	getPeriodEnd()		const;
			bool					getIsEnabled()		const;
			const AlarmsSet&		getAlarms()			const;

			void setName(const std::string& name);

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
			void setIsATemplate(bool isATemplate);
			void setIsEnabled(bool value);
			void addAlarm(Alarm* alarm);
			void removeAlarm(Alarm* alarm);

			boost::shared_ptr<Scenario>	createCopy()	const;

			void	stop(const time::DateTime& now);
		};
	}
}

#endif // SYNTHESE_Scenario_h__
