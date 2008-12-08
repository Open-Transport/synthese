
/** SentAlarm class header.
	@file SentAlarm.h

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

#ifndef SYNTHESE_SentAlarm_h__
#define SYNTHESE_SentAlarm_h__

#include "Alarm.h"
#include "17_messages/Types.h"

#include "DateTime.h"

#include "Registrable.h"

namespace synthese
{
	namespace messages
	{
		/** Sent Alarm Interface.
			@ingroup m17
		*/
		class SentAlarm
			: public Alarm
		{
		public:
			struct Complements
			{
				AlarmConflict	conflictStatus;
				int				recipientsNumber;
				Complements() : conflictStatus(ALARM_CONFLICT_UNKNOWN), recipientsNumber(UNKNOWN_VALUE) {}
			};

		private:
			Complements					_complements;

		public:
			SentAlarm(util::RegistryKeyType key = UNKNOWN_VALUE);
			~SentAlarm();
			void setComplements(const Complements& complements);

			virtual bool					getIsEnabled()		const = 0;
			virtual const time::DateTime&	getPeriodStart()	const = 0;
			virtual const time::DateTime&	getPeriodEnd()		const = 0;
			Complements						getComplements()	const;

			/** Applicability test.
				@param start Start of applicability period
				@param end End of applicability period
				@return true if the message is not empty and is valid for the whole period given as argument.
			*/
			bool isApplicable ( const time::DateTime& start, const time::DateTime& end ) const;

			bool isApplicable(const time::DateTime& date) const;


			
			/** Conflict between two alarms detector.
				@param other Other alarm to compare with
				@return synthese::messages::AlarmConflict the conflict situation between the two alarms if they where displayed on the same recipient
				@author Hugues Romain
				@date 2007

				Conflict :
				@code
<-------------------|
   				|------------------>
				@endcode

				Not conflict :
				@code
<-------------------|
          				|------------------>
				@endcode

				The other's start date is supposed to be after the current one. If not the method is relaunched with the parameters inverted.

				@todo Test each case in an unit test
			*/
			AlarmConflict wereInConflictWith(const SentAlarm& other) const;

			
			/** Gets the "worse" conflict status of the alarm in each recipient type..
				@return synthese::messages::AlarmConflict The conflict status of the alarm
				@author Hugues Romain
				@date 2007				
			*/
			AlarmConflict getConflictStatus() const;
		};
	}
}

#endif // SYNTHESE_SentAlarm_h__
