
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

#include "BroadcastPoint.hpp"
#include "Env.h"
#include "SentScenario.h"
#include "ServerModule.h"
#include "ScenarioTemplate.h"
#include "ScenarioTableSync.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "TextTemplateTableSync.h"
#include "TextTemplate.h"

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
				"Messages activations"
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
			ScenarioTableSync::SearchResult templates(
				ScenarioTableSync::SearchTemplates(env, folderId)
			);
			BOOST_FOREACH(const boost::shared_ptr<Scenario>& st, templates)
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
			, std::string prefix /*= std::string()  */
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

					// Change the thread status
					ServerModule::SetCurrentThreadWaiting();
				}

				// Wait 500 ms
				this_thread::sleep(milliseconds(500));
			}

		}



		//////////////////////////////////////////////////////////////////////////
		/// Updates the activated messages cache
		void MessagesModule::UpdateActivatedMessages()
		{
			// Now
			ptime now(second_clock::local_time());

			// Wait for the availability of the cache
			mutex::scoped_lock(_activatedMessagesMutex);

			// Duplicate the cache to run deactivation triggers
			ActivatedMessages decativatedMessages(_activatedMessages);

			// Loop on all messages
			BOOST_FOREACH(
				const Registry<Alarm>::value_type& message,
				Env::GetOfficialEnv().getRegistry<Alarm>()
			){
				// Avoid library messages
				SentAlarm* sentMessage(
					dynamic_cast<SentAlarm*>(message.second.get())
				);
				if(sentMessage == NULL)
				{
					continue;
				}

				// Record active message
				if(sentMessage->isApplicable(now))
				{
					// Remove the message as deactivated one
					decativatedMessages.erase(sentMessage);

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
			}

			// Erase deactivated messages
			BOOST_FOREACH(const ActivatedMessages::value_type& sentMessage, decativatedMessages)
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
			BOOST_FOREACH(SentAlarm* message, _activatedMessages)
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



		bool MessagesModule::SentAlarmLess::operator()(
			SentAlarm* left,
			SentAlarm* right
		) const {
			assert(left && right);

			if(left->getLevel() != right->getLevel())
			{
				return left->getLevel() > right->getLevel();
			}

			if(!left->getScenario()->getPeriodStart().is_not_a_date_time())
			{
				if(right->getScenario()->getPeriodStart().is_not_a_date_time())
				{
					return true;
				}
				if(left->getScenario()->getPeriodStart() != right->getScenario()->getPeriodStart())
				{
					return left->getScenario()->getPeriodStart() > right->getScenario()->getPeriodStart();
				}
			}
			else
			{
				if(!right->getScenario()->getPeriodStart().is_not_a_date_time())
				{
					return false;
				}
			}

			return left < right;
		}
	}
}
