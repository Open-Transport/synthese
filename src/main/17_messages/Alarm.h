
/** Alarm class header.
	@file Alarm.h

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

#ifndef SYNTHESE_MESSAGES_ALARM_H
#define SYNTHESE_MESSAGES_ALARM_H

#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "04_time/DateTime.h"

#include "17_messages/Types.h"

namespace synthese
{
	namespace messages
	{
		class Scenario;

		/** Alarm message.
			@ingroup m17

			An alarm message is intended to be broadcasted at a time period into several destinations :
				- display screens
				- route planner results
				- etc.

			An alarm can be sent individually (single alarm) or in a group builded from a scenario (grouped alarm)
			The _scenario attribute points to the group if applicable.
		*/
		class Alarm : public synthese::util::Registrable<uid,Alarm>
		{
		private:
			AlarmLevel					_level;
			bool						_enabled;
			bool						_isATemplate;
			std::string					_shortMessage;  //!< Alarm message
			std::string					_longMessage;  //!< Alarm message
			synthese::time::DateTime	_periodStart; //!< Alarm applicability period start
			synthese::time::DateTime	_periodEnd;   //!< Alarm applicability period end
			Scenario*					_scenario;	//!< Group of alarms which this one belongs. Can be null = single alarm.

		    
		public:

			Alarm();
			
			//! @name Getters/Setters
			//@{
				bool					getIsEnabled()		const;
				const std::string&		getShortMessage()	const;
				const std::string&		getLongMessage()	const;
				const AlarmLevel&		getLevel()			const;
				const time::DateTime&	getPeriodStart()	const;
				const time::DateTime&	getPeriodEnd()		const;
				bool					getIsATemplate()	const;
				const Scenario*			getScenario()		const;
				Scenario*				getScenario();

				void setLevel (const AlarmLevel& level);
				void setShortMessage( const std::string& message);
				void setLongMessage( const std::string& message);
				void setPeriodStart ( const time::DateTime& periodStart);
				void setPeriodEnd ( const time::DateTime& periodEnd);
				void setIsATemplate(bool value);

				/** Scenario setter.
					Updates the dates of the alarm with the ones of the scenario.
					@param scenario Scenario to link with
				*/
				void setScenario(Scenario* scenario);
				void setIsEnabled(bool value);

			//@}
		    
		    
			//! \name Calculateurs
			//@{

				/** Applicability test.
					@param start Start of applicability period
					@param end End of applicability period
					@return true if the message is not empty and is valid for the whole period given as argument.
				*/
				bool isApplicable ( const time::DateTime& start, const time::DateTime& end ) const;

				bool isApplicable(const time::DateTime& date) const;

				/** Copy of alarm template.
					@param scenario Scenario the new alarm belongs to.
					@return The new alarm created upon the one
				*/
				boost::shared_ptr<Alarm>	createCopy(Scenario* scenario)	const;
			//@}
		    
		};
	}
}

#endif
