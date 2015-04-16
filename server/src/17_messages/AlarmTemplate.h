
/** AlarmTemplate class header.
	@file AlarmTemplate.h

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

#ifndef SYNTHESE_AlarmTemplate_h__
#define SYNTHESE_AlarmTemplate_h__

#include "Alarm.h"

namespace synthese
{
	namespace messages
	{
		class ScenarioTemplate;



		//////////////////////////////////////////////////////////////////////////
		/// Message template (part of a scenario template).
		/// Message and scenario templates are part of the messaging library.
		/// There are a copy of sent message, without information on sending dates.
		///	@ingroup m17
		/// @author Hugues Romain
		class AlarmTemplate:
			public Alarm
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor for reading the object from the database.
			///	@param key ID of the alarm
			///	@param scenario pointer to the scenario its belong
			AlarmTemplate(
				util::RegistryKeyType key = 0,
				const ScenarioTemplate* scenario = NULL
			);



			//////////////////////////////////////////////////////////////////////////
			/// Copy constructor for a message of an other scenario.
			/// @param scenario the scenario of the new message
			/// @param source the message to copy
			AlarmTemplate(
				const ScenarioTemplate& scenario,
				const Alarm& source
			);



			//////////////////////////////////////////////////////////////////////////
			/// Copy constructor.
			/// Is equivalent to AlarmTemplate(*source.getSceario(), source)
			/// @param source the message to copy
			AlarmTemplate(const AlarmTemplate& source);



			//////////////////////////////////////////////////////////////////////////
			/// Destructor.
			~AlarmTemplate();



			//////////////////////////////////////////////////////////////////////////
			/// Scenario getter.
			/// @return the scenario
			const ScenarioTemplate* getScenario() const;
		};
}	}

#endif // SYNTHESE_AlarmTemplate_h__
