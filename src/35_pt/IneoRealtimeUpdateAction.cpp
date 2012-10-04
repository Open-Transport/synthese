
//////////////////////////////////////////////////////////////////////////
/// IneoRealtimeUpdateAction class implementation.
/// @file IneoRealtimeUpdateAction.cpp
/// @author Hugues Romain
/// @date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "IneoRealtimeUpdateAction.hpp"

#include "ActionException.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRecipientTemplate.h"
#include "DisplayScreen.h"
#include "DisplayScreenAlarmRecipient.h"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "ScenarioTableSync.h"
#include "SentScenario.h"
#include "SentScenarioInheritedTableSync.h"

using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace departure_boards;
	using namespace impex;
	using namespace messages;
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, pt::IneoRealtimeUpdateAction>::FACTORY_KEY = "ineo_realtime_update";

	namespace pt
	{
		const string IneoRealtimeUpdateAction::PARAMETER_DATASOURCE_ID = Action_PARAMETER_PREFIX + "ds";
		const string IneoRealtimeUpdateAction::PARAMETER_DATABASE = Action_PARAMETER_PREFIX + "db";
		
		
		
		ParametersMap IneoRealtimeUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void IneoRealtimeUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Datasource
			try
			{
				_dataSource = Env::GetOfficialEnv().get<DataSource>(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID));
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw ActionException("No such data source");
			}

			// Database
			_database = map.get<string>(PARAMETER_DATABASE);
		}
		
		
		
		void IneoRealtimeUpdateAction::run(
			Request& request
		){
			shared_ptr<DB> db = DBModule::GetDBSPtr();

			// Objects to update in database
			Env updatesEnv;
			set<RegistryKeyType> scenariosToRemove;
			map<string, SentScenario*> addedScenarios;

			{ // Scenarios and messages

				set<string> refs;
				stringstream query;

				// Loop on objects present in the database (search for creations and updates)
				query <<
					"SELECT *" <<
					" FROM " << _database << ".PROGRAMMATION " <<
					" WHERE nature_dst LIKE 'BORNE%'"
				;
				DBResultSPtr result(db->execQuery(query.str()));
				while(result->next())
				{
					// Fields reading
					string ref(result->getText("ref"));
					string content(result->getText("diffusion_msg"));
					string messageTitle(result->getText("nature_dst"));
					string title(result->getText("titre"));
					ptime startTime(
						from_string(result->getText("date_deb")),
						duration_from_string(result->getText("heure_deb"))
					);
					ptime endTime(
						from_string(result->getText("date_fin")),
						duration_from_string(result->getText("heure_fin"))
					);
					if(endTime.time_of_day().hours() <= 4)
					{
						endTime += days(1);
					}
					

					refs.insert(ref);

					shared_ptr<SentScenario> updatedScenario;
					shared_ptr<SentAlarm> updatedMessage;
					SentScenario* scenario(_dataSource->getObjectByCode<SentScenario>(ref));
					if(!scenario)
					{
						// Creation of the scenario
						updatedScenario.reset(
							new SentScenario(
								ScenarioTableSync::getId()
						)	);
						updatedScenario->addCodeBySource(
							*_dataSource,
							ref
						);
						updatedScenario->setIsEnabled(true);
						addedScenarios.insert(make_pair(ref, updatedScenario.get()));
						updatesEnv.getEditableRegistry<SentScenario>().add(updatedScenario);

						// Creation of the message
						updatedMessage.reset(
							new SentAlarm(
								AlarmTableSync::getId()
						)	);
						updatedMessage->setScenario(updatedScenario.get());
						updatesEnv.getEditableRegistry<SentAlarm>().add(updatedMessage);
					}
					else
					{
						// Message content
						if(scenario->getMessages().size() != 1)
						{
							Log::GetInstance().warn(
								"Corrupted message : it should contain one message : " + lexical_cast<string>(scenario->getKey())
							);
							continue;
						}
						const SentAlarm& message(**scenario->getMessages().begin());
						if(	message.getLongMessage() != content ||
							message.getShortMessage() != messageTitle
						){
							updatedMessage = ScenarioSentAlarmInheritedTableSync::GetEditable(
								message.getKey(),
								updatesEnv
							);
						}

						// Scenario updates
						if(	scenario->getName() != title ||
							scenario->getPeriodStart() != startTime ||
							scenario->getPeriodEnd() != endTime
						){
							updatedScenario = SentScenarioInheritedTableSync::GetEditable(
								scenario->getKey(),
								updatesEnv
							);
						}
					}

					if(updatedMessage.get())
					{
						updatedMessage->setLongMessage(content);
						updatedMessage->setShortMessage(messageTitle);
					}
					if(updatedScenario.get())
					{
						updatedScenario->setName(title);
						updatedScenario->setPeriodStart(startTime);
						updatedScenario->setPeriodEnd(endTime);
					}
				}

				// Loop on objects present in the environment (search for deletions)
				DataSource::Links::mapped_type existingScenarios(
					_dataSource->getLinkedObjects<SentScenario>()
				);
				BOOST_FOREACH(const DataSource::Links::mapped_type::value_type& existingScenario, existingScenarios)
				{
					if(refs.find(existingScenario.second->getACodeBySource(*_dataSource)) == refs.end())
					{
						scenariosToRemove.insert(existingScenario.second->getKey());
					}
				}
			}

			set<RegistryKeyType> linksToRemove;
			{ // Messages recipients

				set<pair<string,string> > refs;
				stringstream query;

				// Loop on objects present in the database (search for creations and updates)
				query <<
					"SELECT *" <<
					" FROM " << _database << ".DESTINATAIRE" <<
					" WHERE " <<
					" EXISTS(SELECT ref FROM " << _database << ".PROGRAMMATION WHERE nature_dst LIKE 'BORNE%')" <<
					" ORDER BY ref_prog"
				;
				DBResultSPtr result(db->execQuery(query.str()));
				string ref_prog;
				typedef map<string, map<string, bool> > RecipientsMap;
				RecipientsMap recipients;
				while(result->next())
				{
					RecipientsMap::iterator it(
						recipients.find(result->getText("ref_prog"))
					);
					if(it == recipients.end())
					{
						it = recipients.insert(
							make_pair(
								result->getText("ref_prog"),
								RecipientsMap::mapped_type()
						)	).first;
					}
					it->second.insert(
						make_pair(
							result->getText("destinataire"),
							false
					)	);
				}
				
				BOOST_FOREACH(RecipientsMap::value_type& it, recipients)
				{
					SentScenario* scenario(_dataSource->getObjectByCode<SentScenario>(it.first));
					if(!scenario)
					{
						map<string, SentScenario*>::const_iterator it(addedScenarios.find(it.first));
						if(it != addedScenarios.end())
						{
							scenario = it->second;
						}
					}
					if(!scenario)
					{
						Log::GetInstance().warn("Corrupted message recipient : it should link to a valid scenario : " + it.first);
					}
					else if(scenario->getMessages().size() != 1)
					{
						Log::GetInstance().warn("Corrupted message : it should contain one message : " + lexical_cast<string>(scenario->getKey()));
					}
					else
					{
						const SentAlarm& message(**scenario->getMessages().begin());

						// Recipients to add
						DisplayScreenAlarmRecipient::LinkedObjectsSet existingRecipients(
							DisplayScreenAlarmRecipient::getLinkedObjects(message)
						);
						BOOST_FOREACH(const DisplayScreenAlarmRecipient::LinkedObjectsSet::value_type& dsit, existingRecipients)
						{
							const DisplayScreen& ds(*dsit.first);
							string code(ds.getACodeBySource(*_dataSource));
							RecipientsMap::mapped_type::iterator it2(it.second.find(code));
							if(it2 == it.second.end())
							{
								linksToRemove.insert(dsit.second->getKey());
							}
							else
							{
								it2->second = true;
							}
						}
					
						BOOST_FOREACH(const RecipientsMap::mapped_type::value_type& it2, it.second)
						{
							if(it2.second)
							{
								continue;
							}
							
							DisplayScreen* ds(
								_dataSource->getObjectByCode<DisplayScreen>(
									it2.first
							)	);
							if(!ds)
							{
								Log::GetInstance().warn("No such display screen : " + it2.first);
								continue;
							}

							shared_ptr<AlarmObjectLink> link;
							link->setAlarm(const_cast<SentAlarm*>(&message));
							link->setObjectId(ds->getKey());
							updatesEnv.getEditableRegistry<AlarmObjectLink>().add(link);
						}
					}
				}
			}
			
			// Savings
			DBTransaction transaction;
			DBModule::SaveEntireEnv(updatesEnv, transaction);
			BOOST_FOREACH(RegistryKeyType id, linksToRemove)
			{
				AlarmObjectLinkTableSync::RemoveRow(id, transaction);
			}
			BOOST_FOREACH(RegistryKeyType id, scenariosToRemove)
			{
				ScenarioTableSync::RemoveRow(id, transaction);
			}
			transaction.run();
		}
		
		
		
		bool IneoRealtimeUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
}	}

