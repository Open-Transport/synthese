
/** ScenarioCalendar class header.
	@file ScenarioCalendar.hpp

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

#ifndef SYNTHESE_messages_ScenarioCalendar_hpp__
#define SYNTHESE_messages_ScenarioCalendar_hpp__

#include "Object.hpp"

#include "Scenario.h"
#include "SchemaMacros.hpp"
#include "StringField.hpp"

namespace synthese
{
	FIELD_POINTER(ScenarioPointer, messages::Scenario)

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(Name),
		FIELD(ScenarioPointer)
	> ScenarioCalendarRecord;

	namespace messages
	{
		class MessageApplicationPeriod;

		/** ScenarioCalendar class.
			@ingroup m17
		*/
		class ScenarioCalendar:
			public Object<ScenarioCalendar, ScenarioCalendarRecord>
		{
		public:
			static const std::string TAG_APPLICATION_PERIOD;

			ScenarioCalendar(
				util::RegistryKeyType id=0
			);

			typedef std::set<
				MessageApplicationPeriod*
			> ApplicationPeriods;

		private:
			mutable ApplicationPeriods _applicationPeriods;

		public:

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
				virtual void beforeDelete(boost::optional<db::DBTransaction&> transaction) const;
			//@}

			const ApplicationPeriods& getApplicationPeriods() const { return _applicationPeriods; }
			void setApplicationPeriods(const ApplicationPeriods& value) const { _applicationPeriods = value; }

			/// @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Checks if the specified time is in the defined calendar.
				/// @param time the time to check
				/// @return true if the time is in any of calendar application periods
				bool isInside(const boost::posix_time::ptime& time) const;

				//////////////////////////////////////////////////////////////////////////
				/// Checks if the specified time is after the defined calendar.
				/// @param time the time to check
				/// @return true if the time is after all calendar application periods
				bool isAfter(const boost::posix_time::ptime& time) const;
			//@}


			virtual void addAdditionalParameters(
				util::ParametersMap& map,
				std::string prefix = std::string()
			) const;
		};
}	}

#endif // SYNTHESE_messages_ScenarioCalendar_hpp__

