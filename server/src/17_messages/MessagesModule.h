
/** MessagesModule class header.
	@file MessagesModule.h

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

#ifndef SYNTHESE_MessagesModule_H__
#define SYNTHESE_MessagesModule_H__

#include "UtilConstants.h"
#include "ModuleClassTemplate.hpp"
#include "Registry.h"
#include "MessagesTypes.h"

#include <vector>
#include <string>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
#include <boost/thread/mutex.hpp>

namespace synthese
{
	/** @defgroup m17Exceptions 17.01 Exceptions
		@ingroup m17

		@defgroup m17LS 17.10 Table synchronizers
		@ingroup m17

		@defgroup m17Pages 17.11 Pages
		@ingroup m17

		@defgroup m17Rights 17.12 Rights
		@ingroup m17

		@defgroup m17Logs 17.13 DB Logs
		@ingroup m17

		@defgroup m17Admin 17.14 Administration pages
		@ingroup m17

		@defgroup m17Functions 17.15 Functions
		@ingroup m17

		@defgroup m17Actions 17.15 Actions
		@ingroup m17

		@defgroup m17 17 Messages
		@ingroup m1
	@{	*/

	/** 17 Messages module namespace.
	*/
	namespace messages
	{
		class BroadcastPoint;
		class Alarm;
		class SentScenario;

		/** 17 Messages module class.
		*/
		class MessagesModule:
			public server::ModuleClassTemplate<MessagesModule>
		{
		public:
			struct AlarmLess : public std::binary_function<boost::shared_ptr<Alarm>, boost::shared_ptr<Alarm>, bool>
			{
				//////////////////////////////////////////////////////////////////////////
				/// Order by decreasing priority level, then by line number, then by start date, then by address
				bool operator()(boost::shared_ptr<Alarm> left, boost::shared_ptr<Alarm> right) const;
			};
			typedef std::set<boost::shared_ptr<Alarm>, AlarmLess> ActivatedMessages;

		private:
			static ActivatedMessages _activatedMessages;
			static boost::mutex _activatedMessagesMutex;
			static boost::posix_time::ptime _lastSecondActivation;
			static long _lastMinuteScenario;
			static bool _messagesActivationRanOnce;
			static bool _scenariosActivationRanOnce;

			static bool _selectAlarm(const Alarm& object);
			static bool _enableScenarioIfAutoActivation(SentScenario* sscenario);

		public:
			static void UpdateActivatedMessages();
			static void UpdateAutomaticallyManagedScenarii();
			static int HandleNotificationEvents();

			static ActivatedMessages GetActivatedMessages(
				const BroadcastPoint& broadcastPoint,
				const util::ParametersMap& parameters
			);

			static ActivatedMessages GetActivatedMessagesAt(
				const BroadcastPoint& broadcastPoint,
				const util::ParametersMap& parameters,
				const boost::posix_time::ptime& date
			);

			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > Labels;

			/** Labels list containing each scenario template ordered by folder, indicating the full path in the folder tree.
				@param withAllLabel if non empty, add an option "all scenarios" (value -1) with the specified label
				@param folderId id of the main parent folder (optional) :
					- 0/default value is the root folder
					- UNKWNOWN_VALUE = do not use this criteria : return all templates without their full path
				@param prefix text to add at the beginning of each item (optional)
				@return The list
				@author Hugues Romain
				@date 2008
			*/
			static Labels GetScenarioTemplatesLabels(
				std::string withAllLabel = std::string(),
				std::string withNoLabel = std::string(),
				boost::optional<util::RegistryKeyType> folderId = boost::optional<util::RegistryKeyType>(),
				std::string prefix = std::string()
			);



			/** Labels list containing each scenario template folder, indicating the full path in the folder tree.
				@param folderId id of the main parent folder (optional)
				@param prefix text to add at the beginning of each item (optional)
				@param forbiddenFolderId id of a folder which must not be present in the result
				@return The list
				@author Hugues Romain
				@date 2008
			*/
			static Labels GetScenarioFoldersLabels(
				util::RegistryKeyType folderId = 0
				, std::string prefix = std::string()
				, boost::optional<util::RegistryKeyType> forbiddenFolderId = boost::optional<util::RegistryKeyType>()
			);

			typedef std::vector<std::pair<boost::optional<AlarmLevel>, std::string> > LevelLabels;

			static LevelLabels getLevelLabels(bool withAll = false);

			static Labels GetLevelLabelsWithScenarios(
				bool withAll
			);

			static std::string							getLevelLabel(const AlarmLevel& level);

			static void MessagesActivationThread();

			static void ScenariiActivationThread();

			static void NotificationThread();

			static void ClearAllBroadcastCaches();
		};
	}
	/** @} */
}

#endif // SYNTHESE_MessagesModule_H__
