////////////////////////////////////////////////////////////////////////////////
/// ScenarioSubclassTemplate class header.
///	@file ScenarioSubclassTemplate.h
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

#ifndef SYNTHESE_ScenarioSCTPL_h__
#define SYNTHESE_ScenarioSCTPL_h__

#include "Scenario.h"

#include <string>
#include <set>

namespace synthese
{
	namespace messages
	{
		/** Scenario of alarms diffusion.
		@ingroup m17

		A scenario is a collection of alarms.
		The type of the contained alarms defines two categories of scenario :
		- the template scenario
		- the sent scenario
		*/
		template<class T>
		class ScenarioSubclassTemplate : public Scenario
		{
		public:
			typedef std::set<T*>	AlarmsSet;
			typedef T				AlarmType;

		private:
			AlarmsSet					_alarms;

		protected:
			ScenarioSubclassTemplate(const std::string name = std::string())
				: Scenario(name) {}
			virtual ~ScenarioSubclassTemplate() {}

		public:
			/** Constant Alarms getter.
				@return const AlarmsSet& The set of alarms
				@author Hugues Romain
				@date 2007				
			*/
			const AlarmsSet& getAlarms() const { return _alarms; }
			AlarmsSet& getAlarms() { return _alarms; }

			void addAlarm(T* alarm) { _alarms.insert(alarm); }

			void removeAlarm(T* alarm);
		};

		template<class T>
		void ScenarioSubclassTemplate<T>::removeAlarm(T* alarm )
		{
			typename AlarmsSet::iterator it = _alarms.find(alarm);
			if (it != _alarms.end())
				_alarms.erase(it);
		}
	}
}

#endif // SYNTHESE_Scenario_h__
