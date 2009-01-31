////////////////////////////////////////////////////////////////////////////////
/// ScenarioSentAlarm class header.
///	@file ScenarioSentAlarm.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_ScenarioSentAlarm_h__
#define SYNTHESE_ScenarioSentAlarm_h__

#include "Registry.h"
#include "SentAlarm.h"

namespace synthese
{
	namespace messages
	{
		class SentScenario;
		class AlarmTemplate;

		/** ScenarioSentAlarm class.
			@ingroup m17
		*/
		class ScenarioSentAlarm
		:	public SentAlarm
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<ScenarioSentAlarm>	Registry;

		private:
			const SentScenario* 	_scenario;
			const AlarmTemplate*	_template;

		public:
			/** Copy constructor.
				@param source Alarm template to copy
				@author Hugues Romain
				@date 2007				
				@warning the recipients are not copied. Do it at the table synchronization.
			*/
			ScenarioSentAlarm(
				const SentScenario& scenario,
				const ScenarioSentAlarm& source
			);
			
			/** Alarm template instanciation constructor.
				@param scenario Scenario which belongs the new alarm
				@param source Alarm template to copy
				@author Hugues Romain
				@date 2007				
				@warning the recipients are not copied. Do it at the table synchronization.
			*/
			ScenarioSentAlarm(
				const SentScenario& scenario,
				const AlarmTemplate& source
			);
			
			/** Basic constructor.
				@param scenario Scenario which belongs the new alarm
				@author Hugues Romain
				@date 2007				
			*/
			ScenarioSentAlarm(
				util::RegistryKeyType key = UNKNOWN_VALUE,
				const SentScenario* scenario = NULL
			);
			
			~ScenarioSentAlarm();

			bool					getIsEnabled()		const;
			const time::DateTime&	getPeriodStart()	const;
			const time::DateTime&	getPeriodEnd()		const;
			
			const SentScenario*		getScenario()		const;
			const AlarmTemplate*	getTemplate()		const;
			
			
			void					setScenario(const SentScenario* scenario);
			void					setTemplate(const AlarmTemplate* value);
		};
	}
}

#endif // SYNTHESE_ScenarioSentAlarm_h__
