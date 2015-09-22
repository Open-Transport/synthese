
/** ScenarioStopAction class implementation.
	@file ScenarioStopAction.cpp

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

#include "ScenarioStopAction.h"

#include "AlarmObjectLink.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRecipient.h"
#include "DataSource.h"
#include "DBTransaction.hpp"
#include "Profile.h"
#include "SentScenarioTableSync.h"
#include "SentScenario.h"
#include "Session.h"
#include "User.h"
#include "AlarmTableSync.h"
#include "MessagesLog.h"
#include "MessagesRight.h"
#include "MessagesSection.hpp"
#include "MessageApplicationPeriod.hpp"
#include "Profile.h"
#include "Session.h"
#include "User.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "CustomBroadcastPointTableSync.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::property_tree;

namespace synthese
{
	using namespace db;	
	using namespace impex;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace messages;

	template<> const string util::FactorableTemplate<Action, messages::ScenarioStopAction>::FACTORY_KEY("scenariostop");

	namespace messages
	{
		const string ScenarioStopAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "s";
		const string ScenarioStopAction::PARAMETER_DATASOURCE_ID = Action_PARAMETER_PREFIX + "_datasource_id";
		const string ScenarioStopAction::PARAMETER_REMOVE = Action_PARAMETER_PREFIX + "_remove";
		const string ScenarioStopAction::PARAMETER_ARCHIVE = Action_PARAMETER_PREFIX + "_archive";



		ParametersMap ScenarioStopAction::getParametersMap() const
		{
			ParametersMap m;
			if(_scenario)
			{
				m.insert(PARAMETER_SCENARIO_ID, _scenario->getKey());
			}
			return m;
		}



		void ScenarioStopAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Datasource
			RegistryKeyType dataSourceId(map.getDefault<RegistryKeyType>(PARAMETER_DATASOURCE_ID, 0));
			if(dataSourceId) try
			{
				_dataSource = Env::GetOfficialEnv().get<DataSource>(dataSourceId).get();
			}
			catch(...)
			{
				throw ActionException("No such data source");
			}

			// Scenario
			if(_dataSource)
			{
				_scenario = dynamic_cast<SentScenario*>(
					_dataSource->getObjectByCode<SentScenario>(map.get<string>(PARAMETER_SCENARIO_ID))
				);
				if(!_scenario)
				{
					throw ActionException("No such scenario");
				}
			}
			else
			{
				try
				{
					_scenario = SentScenarioTableSync::GetCastEditable<SentScenario>(
							map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID),
							*_env
						).get();
				}
				catch (...) {
					throw ActionException("Invalid scenario");
				}
			}

			// True removal ?
			_remove = map.getDefault<bool>(PARAMETER_REMOVE, false);

			_archive = map.getDefault<bool>(PARAMETER_ARCHIVE, false);
		}



		void ScenarioStopAction::run(Request& request)
		{
			if(_remove)
			{
				DBTransaction transaction;
				SentScenarioTableSync::Remove(NULL, _scenario->getKey(), transaction, false);
				transaction.run();
			}
			else
			{
				// Action
				_scenario->setPeriodEnd(_stopDateTime);
				_scenario->setIsEnabled(false);

				if (_archive)
				{
					_scenario->setArchived(true);
				}

				SentScenarioTableSync::Save(_scenario);

				// Log
				MessagesLog::addUpdateEntry(_scenario, "Diffusion arrêtée le " + to_simple_string(_stopDateTime), request.getUser().get());
			}
		}



		ScenarioStopAction::ScenarioStopAction(
		): FactorableTemplate<Action, ScenarioStopAction>(),
			_scenario(NULL),
			_dataSource(NULL),
			_stopDateTime(second_clock::local_time()),
			_remove(false),
			_archive(false)
		{}



		bool ScenarioStopAction::isAuthorized(const Session* session
		) const {
/*			bool result = session && session->hasProfile();
			if (_scenario)
			{
				if (!_scenario->getSections().empty())
				{
					BOOST_FOREACH(const Scenario::Sections::value_type& section, _scenario->getSections())
					{
						result = result && session->getUser()->getProfile()->isAuthorized<MessagesRight>(
							DELETE_RIGHT,
							UNKNOWN_RIGHT_LEVEL,
							MessagesRight::MESSAGES_SECTION_FACTORY_KEY + "/" + lexical_cast<string>(section->getKey())
						);
					}
				}
			}
			return result;
			*/
			return true;
		}


		SentScenario* ScenarioStopAction::findScenarioByMessagesAndCalendars(
			boost::optional<boost::property_tree::ptree> messagesAndCalendars,
			bool compareRecipients
		) {
			if (messagesAndCalendars)
			{
				// Loop on events
				SentScenarioTableSync::SearchResult scenarios;
				scenarios = SentScenarioTableSync::Search(
					Env::GetOfficialEnv(),
					boost::optional<std::string>()
				);

				BOOST_FOREACH(const boost::shared_ptr<Scenario>& scenario, scenarios)
				{
					if(!dynamic_cast<const SentScenario*>(scenario.get()))
					{
						continue;
					}
					SentScenario* sscenario = static_cast<SentScenario*>(scenario.get());
					if (!sscenario->getIsEnabled())
					{
						// Match only enabled scenarios because "delete" scenarios are actually archived
						continue;
					}

					// Loop on messages
					bool allAlarmsFound(true);
					Scenario::Messages alarms = sscenario->getMessages();
					BOOST_FOREACH(const Alarm* alarm, alarms)
					{
						bool allPeriodsFound(true);
						if (!alarm->getCalendar())
						{
							continue;
						}

						BOOST_FOREACH(MessageApplicationPeriod* period, alarm->getCalendar()->getApplicationPeriods())
						{
							bool periodFound(false);
							BOOST_FOREACH(const ptree::value_type& calendarNode, messagesAndCalendars->get_child("calendar"))
							{
								BOOST_FOREACH(const ptree::value_type& periodNode, calendarNode.second.get_child("period"))
								{
									string startDateStr(periodNode.second.get("start_date", string()));
									ptime startDate = startDateStr.empty() ? ptime(not_a_date_time) : time_from_string(startDateStr);
									string endDateStr(periodNode.second.get("end_date", string()));
									ptime endDate = endDateStr.empty() ? ptime(not_a_date_time) : time_from_string(endDateStr);

									if (period->get<StartTime>() == startDate &&
										period->get<EndTime>() == endDate)
									{
										periodFound = true;
										break;
									}
								}
								if (periodFound)
								{
									break; // to avoid loop on other calendar
								}
							}
							if (!periodFound)
							{
								allPeriodsFound = false;
								break;
							}
						}
						if (!allPeriodsFound)
						{
							allAlarmsFound = false;
							break;
						}

						bool alarmFound(false);
						BOOST_FOREACH(const ptree::value_type& calendarNode, messagesAndCalendars->get_child("calendar"))
						{
							BOOST_FOREACH(const ptree::value_type& messageNode, calendarNode.second.get_child("message"))
							{
								const std::string alarmShortMsg = alarm->getShortMessage();
								const std::string alarmLongMsg = alarm->getLongMessage();
								int alarmRepeatInterval = alarm->getRepeatInterval();

								const std::string nodeShortMsg = messageNode.second.get("title", string());
								const std::string nodeLongMsg = messageNode.second.get("content", string());
								int nodeRepeatInterval = messageNode.second.get("repeat_interval", 0);

								if ((alarmShortMsg == nodeShortMsg) && (alarmLongMsg == nodeLongMsg) &&	(alarmRepeatInterval == nodeRepeatInterval))
								{
									bool identicalRecipients(true);
									// Content of message is equal, verify recipients if needed
									if (compareRecipients)
									{
										// Loop on all recipient factories
										BOOST_FOREACH(boost::shared_ptr<AlarmRecipient> linkType, Factory<AlarmRecipient>::GetNewCollection())
										{
											// Existing links of this factory in the existent message
											Alarm::LinkedObjects::mapped_type existingLinks(alarm->getLinkedObjects(linkType->getFactoryKey()));
											boost::optional<const ptree&> recipientNode = messageNode.second.get_child_optional(linkType->getFactoryKey() + "_recipient");
											size_t nbRecipientLinks = (recipientNode ? recipientNode.get().size() : 0);

											// If the number of links differs between the existing alarm and the request then they do not match
											if(nbRecipientLinks != existingLinks.size())
											{
												identicalRecipients = false;
												break;
											}

											BOOST_FOREACH(const AlarmObjectLink* link, existingLinks)
											{
												bool linkFound(false);

												// Loop on recipients
												BOOST_FOREACH(const ptree::value_type& linkNode, recipientNode.get())
												{
													if (link->getObjectId() == linkNode.second.get("recipient_id", RegistryKeyType(0)) &&
														link->getParameter() == linkNode.second.get("parameter", string()))
													{
														linkFound = true;
														break;
													}
												}

												if (!linkFound)
												{
													// Recipients are different
													identicalRecipients = false;
													break;
												}
											}

											if (!identicalRecipients)
											{
												break; // to avoid loop on other recipient factories
											}
										} // Loop on recipient factories
									}

									if (identicalRecipients)
									{
										alarmFound = true;
										break;
									}
								} // Comparison on content of message
							} // Loop on messages of ptree calendar
						} // Loop on calendars of ptree

						if (!alarmFound)
						{
							// This is not the searched scenario
							allAlarmsFound = false;
							break;
						}
					} // Loop on messages of the scenario
					if (!allAlarmsFound)
					{
						// Jump to following scenario
						continue;
					}
					else
					{
						// It is possible to eliminate the scenario if ptree has more messages than scenario
						size_t nbMessagesInPtree(0);
						BOOST_FOREACH(const ptree::value_type& calendarNode, messagesAndCalendars->get_child("calendar"))
						{
							nbMessagesInPtree += calendarNode.second.count("message");
						}
						if (nbMessagesInPtree != alarms.size())
						{
							// Jump to following scenario
							continue;
						}
						else
						{
							// This is the searched scenario !
							return sscenario;
						}
					}
				}// Loop on all scenarios

				return NULL;
			}
			else
			{
				return NULL;
			}
		}

		void ScenarioStopAction::deleteAlarmObjectLinks(
			boost::optional<boost::property_tree::ptree> messagesAndCalendars,
			Request& request
		) {
			if (messagesAndCalendars)
			{
				// Loop on messages
				Scenario::Messages alarms = _scenario->getMessages();
				BOOST_FOREACH(const Alarm* alarm, alarms)
				{
					bool allPeriodsFound(true);
					if (!alarm->getCalendar())
					{
						continue;
					}
					BOOST_FOREACH(MessageApplicationPeriod* period, alarm->getCalendar()->getApplicationPeriods())
					{
						bool periodFound(false);
						BOOST_FOREACH(const ptree::value_type& calendarNode, messagesAndCalendars->get_child("calendar"))
						{
							BOOST_FOREACH(const ptree::value_type& periodNode, calendarNode.second.get_child("period"))
							{
								string startDateStr(periodNode.second.get("start_date", string()));
								ptime startDate = startDateStr.empty() ? ptime(not_a_date_time) : time_from_string(startDateStr);
								string endDateStr(periodNode.second.get("end_date", string()));
								ptime endDate = endDateStr.empty() ? ptime(not_a_date_time) : time_from_string(endDateStr);

								if (period->getStart() == startDate &&
									period->getEnd() == endDate)
								{
									periodFound = true;
									break;
								}
							}
							if (periodFound)
							{
								break; // to avoid loop on other calendar
							}
						}
						if (!periodFound)
						{
							allPeriodsFound = false;
							break;
						}
					}
					if (!allPeriodsFound)
					{
						break;
					}

					BOOST_FOREACH(const ptree::value_type& calendarNode, messagesAndCalendars->get_child("calendar"))
					{
						BOOST_FOREACH(const ptree::value_type& messageNode, calendarNode.second.get_child("message"))
						{
							if (alarm->getShortMessage() == messageNode.second.get("title", string()) &&
								alarm->getLongMessage() == messageNode.second.get("content", string()) &&
								alarm->getRepeatInterval() == messageNode.second.get("repeat_interval", 0))
							{
								// This is the message to work on
								// Loop on all recipient factories
								BOOST_FOREACH(boost::shared_ptr<AlarmRecipient> linkType, Factory<AlarmRecipient>::GetNewCollection())
								{
									std::string linkTypeKey = linkType->getFactoryKey();

									// Existing links of this factory in the existent message
									Alarm::LinkedObjects::mapped_type existingLinks(alarm->getLinkedObjects(linkTypeKey));
									boost::optional<const ptree&> recipientNode = messageNode.second.get_child_optional(linkTypeKey + "_recipient");

									if(recipientNode)
									{
										BOOST_FOREACH(const AlarmObjectLink* link, existingLinks)
										{
											RegistryKeyType linkId = link->getObjectId();
											RegistryTableType linkTableId = decodeTableId(linkId);

											if(("displayscreen" == linkTypeKey) && (CustomBroadcastPointTableSync::TABLE.ID == linkTableId))
											{
												// Ineo messages have custom broadcast point recipients that must not be deleted
												// because they indicate the type of the message (Passenger, Driver,...)
												continue;
											}

											// Loop on recipients of the ptree
											BOOST_FOREACH(const ptree::value_type& linkNode, recipientNode.get())
											{
												if (linkId == linkNode.second.get("recipient_id", RegistryKeyType(0)) &&
													link->getParameter() == linkNode.second.get("parameter", string()))
												{
													// Delete this link
													DBTransaction transaction;
													AlarmObjectLinkTableSync::Remove(request.getSession().get(), link->getKey(), transaction, false);
													transaction.run();
												}
											}
										}
									}
								} // Loop on recipient factories
							} // Comparison on content of message
						} // Loop on messages of ptree calendar
					} // Loop on calendars of ptree
				} // Loop on messages of the scenario
			}
		}
}	}
