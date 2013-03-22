
/** MessageApplicationPeriod class header.
	@file MessageApplicationPeriod.hpp

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

#ifndef SYNTHESE_messages_MessageApplicationPeriod_hpp__
#define SYNTHESE_messages_MessageApplicationPeriod_hpp__

#include "Calendar.h"
#include "Object.hpp"

#include "MessageType.hpp"
#include "NumericField.hpp"
#include "SchemaMacros.hpp"
#include "StringField.hpp"
#include "TimeField.hpp"
#include "PtimeField.hpp"

namespace synthese
{
	namespace messages
	{
		class Alarm;
		class Scenario;
	}

	FIELD_POINTER(AlarmPointer, messages::Alarm)
	FIELD_POINTER(ScenarioPointer, messages::Scenario)
	FIELD_STRING(Dates)

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(AlarmPointer),
		FIELD(ScenarioPointer),
		FIELD(StartHour),
		FIELD(EndHour),
		FIELD(StartTime),
		FIELD(EndTime),
		FIELD(Dates)
	> MessageApplicationPeriodRecord;

	namespace messages
	{
		class SentAlarm;
		class SentScenario;

		/** MessageApplicationPeriod class.
			@ingroup m17
		*/
		class MessageApplicationPeriod:
			public Object<MessageApplicationPeriod, MessageApplicationPeriodRecord>,
			public calendar::Calendar
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<MessageApplicationPeriod>	Registry;

			MessageApplicationPeriod(
				util::RegistryKeyType id = 0
			);

			typedef std::set<
				MessageApplicationPeriod*
			> ApplicationPeriods;

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			/// @name Services
			//@{
				bool getValue(const boost::posix_time::ptime& time) const;
			//@}
		};
	}
}

#endif // SYNTHESE_messages_MessageApplicationPeriod_hpp__

