
/** MessagesModule class implementation.
	@file MessagesModule.cpp

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

#include "MessagesModule.h"

#include "AlarmObjectLink.h"
#include "BroadcastPoint.hpp"
#include "CommercialLine.h"
#include "Env.h"
#include "MessagesSection.hpp"
#include "MessageApplicationPeriod.hpp"
#include "NotificationEvent.hpp"
#include "NotificationEventTableSync.hpp"
#include "NotificationLog.hpp"
#include "SentScenario.h"
#include "ServerModule.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateTableSync.h"
#include "SentScenarioTableSync.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "TextTemplateTableSync.h"
#include "TextTemplate.h"
#include "ScenarioAutopilot.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace messages;

	namespace util
	{
		template<> const std::string util::FactorableTemplate<ModuleClass, messages::MessagesModule>::FACTORY_KEY("17_messages");
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<MessagesModule>::NAME("Gestion d'actualités");

		template<> void ModuleClassTemplate<MessagesModule>::PreInit()
		{
		}

		template<> void ModuleClassTemplate<MessagesModule>::Init()
		{
			ServerModule::AddThread(
				&MessagesModule::MessagesActivationThread,
				"MessageActivation"
			);
			ServerModule::AddThread(
				&MessagesModule::ScenariiActivationThread,
				"ScenariiActivation"
			);
			ServerModule::AddThread(
				&MessagesModule::NotificationThread,
				"Notification"
				);
		}

		template<> void ModuleClassTemplate<MessagesModule>::Start()
		{
		}

		template<> void ModuleClassTemplate<MessagesModule>::End()
		{
		}



		template<> void ModuleClassTemplate<MessagesModule>::InitThread(
		){
		}



		template<> void ModuleClassTemplate<MessagesModule>::CloseThread(
		){
		}
	}

	namespace messages
	{
		MessagesModule::ActivatedMessages MessagesModule::_activatedMessages;
		boost::mutex MessagesModule::_activatedMessagesMutex;
		long MessagesModule::_lastMinute(60);
		long MessagesModule::_lastMinuteScenario(60);
		bool MessagesModule::_messagesActivationRanOnce = false;
		bool MessagesModule::_scenariosActivationRanOnce = false;

		MessagesModule::Labels MessagesModule::GetScenarioTemplatesLabels(
			string withAllLabel,
			string withNoLabel
			, optional<RegistryKeyType> folderId
			, string prefix
		){
			Labels m;
			if (!withAllLabel.empty())
			{
				m.push_back(make_pair(optional<RegistryKeyType>(), withAllLabel));
			}
			if (!withNoLabel.empty())
			{
				m.push_back(make_pair(0, withNoLabel));
			}

			Env env;
			ScenarioTemplateTableSync::SearchResult templates(
				ScenarioTemplateTableSync::Search(env, folderId)
			);
			BOOST_FOREACH(const boost::shared_ptr<ScenarioTemplate>& st, templates)
			{
				m.push_back(make_pair(st->getKey(), prefix + st->getName()));
			}

			if (folderId)
			{
				ScenarioFolderTableSync::SearchResult folders(
					ScenarioFolderTableSync::Search(env, *folderId)
				);
				BOOST_FOREACH(const boost::shared_ptr<ScenarioFolder>& folder, folders)
				{
					Labels r(GetScenarioTemplatesLabels(string(), string(), folder->getKey(), prefix + folder->getName() +"/"));
					m.insert(m.end(),r.begin(), r.end());
				}
			}
			return m;
		}



		MessagesModule::Labels MessagesModule::GetScenarioFoldersLabels(
			RegistryKeyType folderId /*= 0 */
			, std::string prefix /*= std::string()	*/
			, optional<RegistryKeyType> forbiddenFolderId
		){
			Labels m;
			if (folderId == 0)
				m.push_back(make_pair(0, "(racine)"));

			Env env;
			ScenarioFolderTableSync::SearchResult folders(
				ScenarioFolderTableSync::Search(env, folderId)
			);
			BOOST_FOREACH(const boost::shared_ptr<ScenarioFolder>& folder, folders)
			{
				if (forbiddenFolderId && folder->getKey() == *forbiddenFolderId)
					continue;

				m.push_back(make_pair(folder->getKey(), prefix + folder->getName()));

				Labels r(GetScenarioFoldersLabels(folder->getKey(), prefix + folder->getName() +"/", forbiddenFolderId));
				m.insert(m.end(),r.begin(), r.end());
			}
			return m;
		}



		MessagesModule::LevelLabels MessagesModule::getLevelLabels(
			bool withAll /*= false*/
		){
			LevelLabels m;
			if (withAll)
			{
				m.push_back(make_pair(optional<AlarmLevel>(), "(tous)"));
			}
			m.push_back(make_pair(ALARM_LEVEL_INFO, getLevelLabel(ALARM_LEVEL_INFO)));
			m.push_back(make_pair(ALARM_LEVEL_WARNING, getLevelLabel(ALARM_LEVEL_WARNING)));
			return m;
		}



		MessagesModule::Labels MessagesModule::GetLevelLabelsWithScenarios(
			bool withAll /*= false*/
		){
			Labels m;
			if (withAll)
				m.push_back(make_pair(optional<RegistryKeyType>(), "(tous)"));

			Labels s(MessagesModule::GetScenarioTemplatesLabels());
			for(Labels::const_iterator it(s.begin()); it != s.end(); ++it)
			{
				m.push_back(make_pair(it->first, "Scénario " + it->second));
			}
			return m;
		}



		MessagesModule::Labels MessagesModule::getTextTemplateLabels(const AlarmLevel& level)
		{
			Env env;
			Labels m;
			TextTemplateTableSync::SearchResult templates(
				TextTemplateTableSync::Search(env, level)
			);
			BOOST_FOREACH(const boost::shared_ptr<TextTemplate>& text, templates)
			{
				m.push_back(make_pair(text->getKey(), text->getName()));
			}
			return m;
		}

		std::string MessagesModule::getLevelLabel( const AlarmLevel& level )
		{
			switch (level)
			{
			case ALARM_LEVEL_INFO : return "Complémentaire";
			case ALARM_LEVEL_WARNING : return "Prioritaire";
			case ALARM_LEVEL_SCENARIO: return "Scénario";
			default: return "Inconnu";
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// This thread updates every minute the list of activated messages.
		void MessagesModule::MessagesActivationThread()
		{
			while(true)
			{
				// Check if a new minute has begun
				ptime now(second_clock::local_time());
				if(now.time_of_day().minutes() != _lastMinute)
				{
					// Change the thread status
					ServerModule::SetCurrentThreadRunningAction();

					// Update the activated messages cache
					UpdateActivatedMessages();

					// Update the last seconds cache
					_lastMinute = now.time_of_day().minutes();

					_messagesActivationRanOnce = true;

					// Change the thread status
					ServerModule::SetCurrentThreadWaiting();
				}

				// Wait 500 ms
				this_thread::sleep(milliseconds(500));
			}

		}
		
		//////////////////////////////////////////////////////////////////////////
		/// This thread updates every minute the list of enabled scenarii.
		void MessagesModule::ScenariiActivationThread()
		{
			boost::shared_ptr<SentScenarioDao> sentScenarioDao(new ScenarioTableSync());
			boost::shared_ptr<ScenarioAutopilot> scenarioAutopilot(new ScenarioAutopilot(*sentScenarioDao));

			while(true)
			{
				// Check if a new minute has begun
				ptime now(second_clock::local_time());
				if(now.time_of_day().minutes() != _lastMinuteScenario)
				{
					// Change the thread status
					ServerModule::SetCurrentThreadRunningAction();

					scenarioAutopilot->runOnce();

					// Update the last seconds cache
					_lastMinuteScenario = now.time_of_day().minutes();

					_scenariosActivationRanOnce = true;

					// Change the thread status
					ServerModule::SetCurrentThreadWaiting();
				}

				// Wait 500 ms
				this_thread::sleep(milliseconds(500));
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// This thread handles every minute the notification events.
		void MessagesModule::NotificationThread()
		{
			while (true)
			{
				if((true == _messagesActivationRanOnce) && (true == _scenariosActivationRanOnce))
				{
					// Change the thread status
					ServerModule::SetCurrentThreadRunningAction();

					// Handle notification events.
					// Loop over events until nothing to process.
					while (HandleNotificationEvents() > 0);
				}

				// Change the thread status
				ServerModule::SetCurrentThreadWaiting();

				// Wait 500 ms
				this_thread::sleep(milliseconds(500));
			}

		}
		
		//////////////////////////////////////////////////////////////////////////
		/// Management of the notification events
		/// @return number of attempts for processed events
		int MessagesModule::HandleNotificationEvents()
		{
			int processedEvents = 0;

			// Loop on notification events in READY or IN_PROGRESS status
			Env env = Env::GetOfficialEnv();
			NotificationEventTableSync::SearchResult pendingEvents(
				NotificationEventTableSync::GetPendingEvents(env)
			);

			Log::GetInstance().trace("NotificationThread pending events: "
					+ boost::lexical_cast<string>(pendingEvents.size()));

			BOOST_FOREACH(const boost::shared_ptr<NotificationEvent>& event, pendingEvents)
			{
				NotificationProvider& eventProvider = *(event->get<NotificationProvider>());
				bool to_process = false;

				// Provider evaluates next attempt timestamp or expiry
				posix_time::ptime nextAttempt = eventProvider.nextAttemptTime(event);

				Log::GetInstance().trace("NotificationThread event "
						+ boost::lexical_cast<string>(event->get<Key>())
						+ " next attempt "
						+ posix_time::to_iso_string(nextAttempt));

				if (nextAttempt.is_not_a_date_time())
				{
					event->set<Status>(FAILED);
					// expired or maximum attempts reach
					NotificationLog::AddNotificationEventEntry(event);
				}
				else
				{
					posix_time::ptime now = posix_time::second_clock::local_time();
					if (nextAttempt <= now)
					{
						event->set<Status>(IN_PROGRESS);
						event->set<LastAttempt>(now);
						event->set<Attempts>(event->get<Attempts>() + 1);
						to_process = true;
					}
				}
				// Check point before attempt
				NotificationEventTableSync::Save(event.get());

				if (!to_process) continue;	// attempt in future, check next event

				processedEvents++;
				try
				{
					// Notify NotificationProvider
					if (eventProvider.notify(event)) {
						// Succeeded
						event->markSuccessful();
						NotificationEventTableSync::Save(event.get());

						NotificationLog::AddNotificationEventEntry(event);
					}
				}
				catch(const std::exception& e)
				{
					NotificationLog::AddNotificationProviderFailure(
						const_cast<NotificationProvider*>(&eventProvider),
						std::string("Exception ") + e.what(),
						const_cast<Alarm*>(&(event->get<Alarm>().get()))
					);
				}
				catch(...)
				{
					NotificationLog::AddNotificationProviderFailure(
						const_cast<NotificationProvider*>(&eventProvider),
						"Unknown exception",
						const_cast<Alarm*>(&(event->get<Alarm>().get()))
					);
				}
			}
			return processedEvents;
		}
		
		bool MessagesModule::_selectMessagesToActivate( const Alarm& object )
		{
			// Now
			ptime now(second_clock::local_time());

			// Record active message
			if(!object.isApplicable(now))
			{
				return false;
			}

			return true;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Updates the activated messages cache
		void MessagesModule::UpdateActivatedMessages()
		{
			// Wait for the availability of the cache
			mutex::scoped_lock(_activatedMessagesMutex);

			// Duplicate the cache to run unactivation triggers
			ActivatedMessages unactivatedMessages(_activatedMessages);

			// Loop on all messages
			Alarm::Registry::Vector messagesToUpdate(
				Env::GetOfficialEnv().getRegistry<Alarm>().getVector(&_selectMessagesToActivate)
			);
			recursive_mutex::scoped_lock registryLock(Env::GetOfficialEnv().getRegistry<Alarm>().getMutex());

			BOOST_FOREACH(
				const Alarm::Registry::Vector::value_type& message,
				messagesToUpdate
			){
				boost::shared_ptr<Alarm> sentMessage(
					dynamic_pointer_cast<Alarm, Alarm>(message)
				);

				// Remove the message as unactivated one
				unactivatedMessages.erase(sentMessage);

				// Check if the message was already activated
				if(_activatedMessages.find(sentMessage) == _activatedMessages.end())
				{
					// Record the message as activated
					_activatedMessages.insert(sentMessage);

					// Run the display start trigger on each broadcast point
					BOOST_FOREACH(
						const BroadcastPoint::BroadcastPoints::value_type& bp,
						BroadcastPoint::GetBroadcastPoints()
					){
						bp->onDisplayStart(*sentMessage);
					}
				}
			}

			// Erase deactivated messages
			BOOST_FOREACH(const ActivatedMessages::value_type& sentMessage, unactivatedMessages)
			{
				// Remove from cache
				_activatedMessages.erase(sentMessage);

				// Run the display end trigger
				BOOST_FOREACH(
					const BroadcastPoint::BroadcastPoints::value_type& bp,
					BroadcastPoint::GetBroadcastPoints()
				){
					bp->onDisplayEnd(*sentMessage);
				}
			}
		}
		
		//////////////////////////////////////////////////////////////////////////
		/// Updates the enabled scenarii
		void MessagesModule::UpdateEnabledScenarii()
		{
			// Loop on all sent scenarii
			SentScenarioTableSync::SearchResult scenarios;

			scenarios = SentScenarioTableSync::Search(
				Env::GetOfficialEnv(),
				boost::optional<std::string>()
			);
			BOOST_FOREACH(const boost::shared_ptr<SentScenario>& scenario, scenarios)
			{
				if(!dynamic_cast<const SentScenario*>(scenario.get()))
				{
					continue;
				}
				SentScenario* sscenario = static_cast<SentScenario*>(scenario.get());
				if (_enableScenarioIfAutoActivation(sscenario))
				{
					SentScenarioTableSync::Save(sscenario);
				}
			}
		}
		
		bool MessagesModule::_enableScenarioIfAutoActivation( SentScenario* sscenario )
		{
			// Is the scenario associated to an auto_activation section ?
			bool autoChange(false);
			BOOST_FOREACH(const MessagesSection* section, sscenario->get<Sections>())
			{
				if (section->get<AutoActivation>())
				{
					autoChange = true;
					break;
				}
			}

			if (autoChange)
			{
				ptime now(second_clock::local_time());
				bool shouldBeEnabled(false);
				BOOST_FOREACH(const ScenarioCalendar* calendar, sscenario->getCalendars())
				{
					BOOST_FOREACH( const ScenarioCalendar::ApplicationPeriods::value_type& period, calendar->getApplicationPeriods() )
					{
						if(period->getValue(now))
						{
							shouldBeEnabled = true;
							break;
						}
					}
				}

				if (shouldBeEnabled && !sscenario->getIsEnabled())
				{
					sscenario->setIsEnabled(true);
					return true;
				}
				if (!shouldBeEnabled && sscenario->getIsEnabled())
				{
					sscenario->setIsEnabled(false);
					return true;
				}
			}

			return false;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Lists the message to display on a broadcast point according to the
		/// specified parameters.
		/// @param broadcastPoint the broadcast point to check
		/// @param parameters the broadcast parameters
		/// @return the list of the messages to display
		MessagesModule::ActivatedMessages MessagesModule::GetActivatedMessages(
			const BroadcastPoint& broadcastPoint,
			const util::ParametersMap& parameters
		){
			// Wait for the availability of the cache
			mutex::scoped_lock(_activatedMessagesMutex);

			// Initialisation of the result
			ActivatedMessages result;

			// Check each message
			BOOST_FOREACH(boost::shared_ptr<Alarm> message, _activatedMessages)
			{
				// Check the message
				if(message->isOnBroadcastPoint(
					broadcastPoint,
					parameters
				)	){
					result.insert(message);
				}
			}

			return result;
		}



		bool MessagesModule::_selectAlarm( const Alarm& object )
		{
			if(!dynamic_cast<const Alarm*>(&object))
			{
				return false;
			}

			return true;
		}



		void MessagesModule::ClearAllBroadcastCaches()
		{
			recursive_mutex::scoped_lock registryLock(Env::GetOfficialEnv().getRegistry<Alarm>().getMutex());
			Alarm::Registry::Vector sentAlarms(
				Env::GetOfficialEnv().getRegistry<Alarm>().getVector(&_selectAlarm)
			);
			BOOST_FOREACH(const Alarm::Registry::Vector::value_type& alarm, sentAlarms)
			{
				// Jump over scenarios
				Alarm& sentAlarm(static_cast<Alarm&>(*alarm));
				
				// Clear the cache
				sentAlarm.clearBroadcastPointsCache();
			}
		}



		bool MessagesModule::AlarmLess::operator()(
			boost::shared_ptr<Alarm> left,
			boost::shared_ptr<Alarm> right
		) const {
			assert(left.get() && right.get());

			if(left->getLevel() != right->getLevel())
			{
				return left->getLevel() > right->getLevel();
			}
			const SentScenario* leftScenario =
				dynamic_cast<const SentScenario*>(left->getScenario());
			const SentScenario* rightScenario = 
				dynamic_cast<const SentScenario*>(right->getScenario());
			if (!leftScenario) return 0;
			if (!rightScenario) return 0;
			
			if(!leftScenario->getPeriodStart().is_not_a_date_time())
			{
				if(rightScenario->getPeriodStart().is_not_a_date_time())
				{
					return true;
				}
				if(leftScenario->getPeriodStart() != rightScenario->getPeriodStart())
				{
					return leftScenario->getPeriodStart() > rightScenario->getPeriodStart();
				}
			}
			else
			{
				if(!rightScenario->getPeriodStart().is_not_a_date_time())
				{
					return false;
				}
			}

			// Sort by commercial line if both alarm are linked to
			{
				boost::shared_ptr<const pt::CommercialLine> firstLineLeft, firstLineRight;
				BOOST_FOREACH(Alarm::LinkedObjects::value_type& leftId, left->getLinkedObjects())
				{
					if(leftId.first != "line")continue;
					BOOST_FOREACH(const AlarmObjectLink* link, leftId.second)
					{
						if(Env::GetOfficialEnv().contains<pt::CommercialLine>(link->getObjectId()))
						{
							firstLineLeft = Env::GetOfficialEnv().get<pt::CommercialLine>(link->getObjectId());
							break;
						}
					}
					break;
				}
				BOOST_FOREACH(Alarm::LinkedObjects::value_type& rightId, right->getLinkedObjects())
				{
					if(rightId.first != "line")continue;
					BOOST_FOREACH(const AlarmObjectLink* link, rightId.second)
					{
						if(Env::GetOfficialEnv().contains<pt::CommercialLine>(link->getObjectId()))
						{
							firstLineRight = Env::GetOfficialEnv().get<pt::CommercialLine>(link->getObjectId());
							break;
						}
					}
					break;
				}
				if(firstLineLeft.get() && firstLineRight.get())
				{
					return *firstLineLeft.get() < *firstLineRight.get();
				}
				else
				{
					if(firstLineLeft.get())return true; // Only left is associated to a line
					if(firstLineRight.get())return false; // Only right is associated to a line
				}
			}

			return left.get() < right.get();
		}
	}
}
