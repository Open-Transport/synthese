
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
#include "AlarmRecipientTemplate.h"
#include "DisplayScreen.h"
#include "DisplayScreenAlarmRecipient.h"
#include "DBModule.h"
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
		
		
		
		ParametersMap IneoRealtimeUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void IneoRealtimeUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_dataSource = Env::GetOfficialEnv().get<DataSource>(map.get<RegistryKeyType>(PARAMETER_DATASOURCE_ID));
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw ActionException("No such data source");
			}
		}
		
		
		
		void IneoRealtimeUpdateAction::run(
			Request& request
		){
			shared_ptr<DB> db = DBModule::GetDBSPtr();

			// Objects to update in database
			Env updatesEnv;
			set<RegistryKeyType> scenariosToRemove;

			{ // Scenarios and messages

				set<string> refs;
				stringstream query;

				// Loop on objects present in the database (search for creations and updates)
				query <<
					"SELECT *" <<
					" FROM PROGRAMMATION " <<
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
							ref,
							true
						);
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
			{ // Messages recipients

				set<pair<string,string> > refs;
				stringstream query;

				// Loop on objects present in the database (search for creations and updates)
				query <<
					"SELECT *" <<
					" FROM DESTINATAIRE" <<
					" WHERE " <<
					" EXISTS(SELECT id FROM PROGRAMMATION WHERE nature_dst LIKE 'BORNE%')" <<
					" ORDER BY ref_prog"
				;
				DBResultSPtr result(db->execQuery(query.str()));
				string ref_prog;
				map<string, bool> recipients;
				while(result->next())
				{
					// Fields reading
					if(ref_prog.empty())
					{
						ref_prog = result->getText("ref_prog");
					}

					if(ref_prog == result->getText("ref_prog"))
					{
						recipients.insert(
							make_pair(
								result->getText("destinataire"),
								false
						)	);
					}
					else
					{
						SentScenario* scenario(_dataSource->getObjectByCode<SentScenario>(ref_prog));
						if(!scenario)
						{
							Log::GetInstance().warn("Corrupted message recipient : it should link to a valid scenario : " + ref_prog);
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
								if(recipients.find(code) == recipients.end())
								{
									// delete
								}



							}
						}
					}

					// Loop on objects present in the environment (search for deletions)
				}
			}
		}
		
		
		
		bool IneoRealtimeUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}
}	}

