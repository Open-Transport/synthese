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

#include "Object.hpp"

#include "MessageType.hpp"
#include "NumericField.hpp"
#include "SchemaMacros.hpp"
#include "ScenarioCalendar.hpp"
#include "StringField.hpp"
#include "TimeField.hpp"
#include "PtimeField.hpp"

namespace synthese
{
	namespace messages
	{
		class Scenario;
	}

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(messages::ScenarioCalendar),
		FIELD(StartHour),
		FIELD(EndHour),
		FIELD(StartTime),
		FIELD(EndTime)
	> MessageApplicationPeriodRecord;

	namespace messages
	{
		/** MessageApplicationPeriod class.
			@ingroup m17
		*/
		class MessageApplicationPeriod:
			public Object<MessageApplicationPeriod, MessageApplicationPeriodRecord>
		{
		public:
			static const std::string TAG_DATE;

			/// Chosen registry class.
			typedef util::Registry<MessageApplicationPeriod>	Registry;

			MessageApplicationPeriod(
				util::RegistryKeyType id = 0
			);

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			/// @name Services
			//@{
				/**
					Checks if the specified time is in the defined period.
					@param time the time to check
					@return true if the time is in the defined period
				 */
				bool isInside(const boost::posix_time::ptime& time) const;

				/**
					Return start timestamp for the specified date
					or posix_time::not_a_date_time if none.

					getValue must be invoked first to known if the period is
					active for that date+time.

					@param date date of interest or today if not set
					@result start timestamp (date and time) or posix_time::not_a_date_time if none
				 */
				boost::posix_time::ptime getStart(
					const boost::posix_time::ptime& date = boost::posix_time::not_a_date_time
				) const;

				/**
					Return end timestamp for the specified date
					or posix_time::not_a_date_time if none.

					getValue must be invoked first to known if the period is
					active for that date+time.

					@param date date of interest or today if not set
					@result end timestamp (date and time) or posix_time::not_a_date_time if none
				 */
				boost::posix_time::ptime getEnd(
					const boost::posix_time::ptime& date = boost::posix_time::not_a_date_time
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Checks if the specified time is in the defined period.
				/// @param time the time to check
				/// @return true if the time is in the defined period
				bool isAfter(const boost::posix_time::ptime& time) const;
			//@}
		};
}	}

#endif // SYNTHESE_messages_MessageApplicationPeriod_hpp__
