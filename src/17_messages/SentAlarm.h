
/** SentAlarm class header.
	@file SentAlarm.h

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

#ifndef SYNTHESE_SentAlarm_h__
#define SYNTHESE_SentAlarm_h__

#include "Alarm.h"

#include "MessageApplicationPeriod.hpp"
#include "MessagesTypes.h"
#include "Registry.h"

namespace synthese
{
	namespace messages
	{
		class AlarmTemplate;
		class SentScenario;

		/** Sent Alarm Interface.
			@ingroup m17
		*/
		class SentAlarm:
			public Alarm
		{
		public:

		private:
			const AlarmTemplate*	_template;

			typedef std::map<
				std::pair<
					const BroadcastPoint*,
					util::ParametersMap
				>, bool
			> BroadcastPointsCache;

            mutable BroadcastPointsCache _broadcastPointsCache;
            mutable boost::mutex _broadcastPointsCacheMutex;


		public:
			/** Copy constructor.
				@param source Alarm template to copy (should be of the same scenario)
				@author Hugues Romain
				@date 2007
				@warning the recipients are not copied. Do it at the table synchronization.
			*/
			SentAlarm(
				const SentScenario& scenario,
				const SentAlarm& source
			);



			/** Alarm template instantiation constructor.
				@param scenario Scenario which belongs the new alarm
				@param source Alarm template to copy
				@author Hugues Romain
				@date 2007
				@warning the recipients are not copied. Do it at the table synchronization.
			*/
			SentAlarm(
				const SentScenario& scenario,
				const AlarmTemplate& source
			);



			/** Alarm generic dynamic copy constructor.
				@param scenario Scenario which belongs the new alarm
				@param source Alarm to copy
				@author Hugues Romain
				@date 2013
				@warning the recipients are not copied. Do it at the table synchronization.
			*/
			SentAlarm(
				const SentScenario& scenario,
				const Alarm& source
			);



			/** Basic constructor.
				@param scenario Scenario which belongs the new alarm
				@author Hugues Romain
				@date 2007
			*/
			SentAlarm(
				util::RegistryKeyType key = 0,
				const SentScenario* scenario = NULL
			);

			~SentAlarm();

			//! @name Getters
			//@{
				const AlarmTemplate*	getTemplate()		const;
				const SentScenario*		getScenario()		const;
			//@}

			//! @name Setters
			//@{
				void					setTemplate(const AlarmTemplate* value);
			//@}

			virtual void toParametersMap(
				util::ParametersMap& pm,
				bool withScenario,
				std::string prefix = std::string(),
				bool withRecipients = false
			) const;


			bool isApplicable(
				const boost::posix_time::ptime& when
			) const;

			virtual bool isOnBroadcastPoint(
				const BroadcastPoint& point,
				const util::ParametersMap& parameters
			) const;

			void clearBroadcastPointsCache() const;
		};
}	}

#endif // SYNTHESE_SentAlarm_h__
