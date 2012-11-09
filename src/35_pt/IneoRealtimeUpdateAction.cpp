
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
#include "CommercialLine.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "DisplayScreen.h"
#include "DisplayScreenAlarmRecipient.h"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "JourneyPatternCopy.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ParametersMap.h"
#include "Request.h"
#include "ScenarioSentAlarmInheritedTableSync.h"
#include "ScenarioTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "SentScenario.h"
#include "SentScenarioInheritedTableSync.h"
#include "StopPoint.hpp"

using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace departure_boards;
	using namespace graph;
	using namespace impex;
	using namespace messages;
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, pt::IneoRealtimeUpdateAction>::FACTORY_KEY = "ineo_realtime_update";

	namespace pt
	{
		const string IneoRealtimeUpdateAction::PARAMETER_MESSAGES_RECIPIENTS_DATASOURCE_ID = Action_PARAMETER_PREFIX + "_mr_ds";
		const string IneoRealtimeUpdateAction::PARAMETER_PLANNED_DATASOURCE_ID = Action_PARAMETER_PREFIX + "_th_ds";
		const string IneoRealtimeUpdateAction::PARAMETER_REAL_TIME_DATASOURCE_ID = Action_PARAMETER_PREFIX + "_rt_ds";
		const string IneoRealtimeUpdateAction::PARAMETER_DATABASE = Action_PARAMETER_PREFIX + "db";
		
		
		
		ParametersMap IneoRealtimeUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void IneoRealtimeUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Planned datasource
			try
			{
				_plannedDataSource = Env::GetOfficialEnv().get<DataSource>(
					map.get<RegistryKeyType>(PARAMETER_PLANNED_DATASOURCE_ID)
				);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw ActionException("No such real time data source");
			}

			// Messages recipients datasource
			try
			{
				_messagesRecipientsDataSource = Env::GetOfficialEnv().get<DataSource>(
					map.get<RegistryKeyType>(PARAMETER_MESSAGES_RECIPIENTS_DATASOURCE_ID)
				);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw ActionException("No such messages recipients data source");
			}

			// Real time datasource
			try
			{
				_realTimeDataSource = Env::GetOfficialEnv().get<DataSource>(
					map.get<RegistryKeyType>(PARAMETER_REAL_TIME_DATASOURCE_ID)
				);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw ActionException("No such real time data source");
			}

			// Database
			_database = map.get<string>(PARAMETER_DATABASE);
		}
		
		
		
		void IneoRealtimeUpdateAction::run(
			Request& request
		){
			date today(day_clock::local_day());

			//////////////////////////////////////////////////////////////////////////
			// Preparation of list of objects to remove

			// Scenarios
			set<RegistryKeyType> scenariosToRemove;
			DataSource::Links::mapped_type existingScenarios(
				_realTimeDataSource->getLinkedObjects<SentScenario>()
			);
			BOOST_FOREACH(const DataSource::Links::mapped_type::value_type& existingScenario, existingScenarios)
			{
				scenariosToRemove.insert(existingScenario.second->getKey());
			}

			// Services
			set<RegistryKeyType> servicesToRemove;
			DataSource::Links::mapped_type existingServices(
				_realTimeDataSource->getLinkedObjects<ScheduledService>()
			);
			BOOST_FOREACH(const DataSource::Links::mapped_type::value_type& existingService, existingServices)
			{
				if(!static_cast<ScheduledService*>(existingService.second)->isActive(today))
				{
					continue;
				}
				servicesToRemove.insert(existingService.second->getKey());
			}

			// Alarm object links
			set<RegistryKeyType> alarmObjectLinksToRemove;


			//////////////////////////////////////////////////////////////////////////
			// Pre-loading objects from BDSI

			Courses courses;
			Lignes lignes;
			Arrets arrets;
			Chainages chainages;
			Programmations programmations;
			DB& db(*DBModule::GetDB());
			string todayStr("'"+ to_iso_extended_string(today) +"'");

			// Arrets
			{
				string query(
					"SELECT * FROM "+ _database +".ARRET WHERE jour="+ todayStr
				);
				DBResultSPtr result(db.execQuery(query));
				while(result->next())
				{
					// SYNTHESE stop point
					string mnemol(result->getText("mnemol"));
					StopPoint* stopPoint(
						_plannedDataSource->getObjectByCode<StopPoint>(mnemol)
					);
					if(!stopPoint)
					{
						Log::GetInstance().warn("No such stop point : " + mnemol);
						continue;
					}

					// Ref
					int ref(result->getInt("ref"));

					// Registration
					Arret& arret(
						arrets.insert(
							make_pair(
								ref,
								Arret()
						)	).first->second
					);

					// Copy of values
					arret.nom = result->getText("nom");
					arret.ref = ref;
					arret.syntheseStop = stopPoint;
				}
			}

			// Lignes
			{
				string query(
					"SELECT * FROM "+ _database +".LIGNE WHERE jour="+ todayStr
				);
				DBResultSPtr result(db.execQuery(query));
				while(result->next())
				{
					// SYNTHESE line
					string mnemo(result->getText("mnemo"));
					CommercialLine* line(
						_plannedDataSource->getObjectByCode<CommercialLine>(mnemo)
					);
					if(!line)
					{
						Log::GetInstance().warn("No such line : " + mnemo);
						continue;
					}

					// Ref
					int ref(result->getInt("ref"));

					// Registration
					Ligne& ligne(
						lignes.insert(
							make_pair(
								ref,
								Ligne()
						)	).first->second
					);

					// Copy of values
					ligne.ref = ref;
					ligne.syntheseLine = line;
				}
			}

			// Chainages
			{
				string chainageQuery(
					"SELECT * FROM "+ _database +".ARRETCHN "+
					"WHERE jour="+ todayStr +" ORDER BY "+ _database +".ARRETCHN.chainage, "+ _database +".ARRETCHN.pos"
				);
				string query(
					"SELECT * FROM "+ _database +".CHAINAGE WHERE jour="+ todayStr
				);
			
				DBResultSPtr chainageResult(db.execQuery(chainageQuery));
				DBResultSPtr result(db.execQuery(query));
				chainageResult->next();
				while(result->next())
				{
					int ref(result->getInt("ref"));
					Chainage& chainage(
						chainages.insert(
							make_pair(
								ref,
								Chainage()
						)	).first->second
					);
					chainage.ref = ref;
					
					Lignes::iterator it(lignes.find(result->getInt("ligne")));
					if(it == lignes.end())
					{
						Log::GetInstance().warn("Bad ligne field in chainage "+ lexical_cast<string>(ref));
						chainages.erase(ref);
						continue;
					}
					chainage.ligne = &it->second;
					chainage.nom = result->getText("nom");
					chainage.sens = (result->getText("sens") == "R");
					
					// Arretchn loop
					do
					{
						int chainage_ref(chainageResult->getInt("chainage"));
						if(chainage_ref != ref)
						{
							break;
						}

						ArretChn& arretChn(
							*chainage.arretChns.insert(
								chainage.arretChns.end(),
								ArretChn()
						)	);
						arretChn.ref = chainageResult->getInt("ref");

						Arrets::iterator it(arrets.find(chainageResult->getInt("arret")));
						if(it == arrets.end())
						{
							Log::GetInstance().warn("Bad arret field in arretchn "+ lexical_cast<string>(arretChn.ref));
							chainage.arretChns.pop_back();
							continue;
						}
						arretChn.arret = &it->second;
						arretChn.pos = chainageResult->getInt("pos");
						arretChn.type = chainageResult->getText("type");

					} while(chainageResult->next());
				}
			}


			// Courses
			{
				string horaireQuery(
					"SELECT "+ _database +".HORAIRE.* FROM "+ _database +".HORAIRE "+
					"INNER JOIN "+ _database +".ARRETCHN ON "+ _database +".HORAIRE.arretchn="+ _database +".ARRETCHN.ref AND "+
					_database +".HORAIRE.jour="+ _database +".ARRETCHN.jour "+
					"WHERE "+ _database +".HORAIRE.jour="+ todayStr +" ORDER BY "+ _database +".HORAIRE.course, "+ _database +".ARRETCHN.pos"
				);
				string query(
					"SELECT * FROM "+ _database +".COURSE WHERE jour="+ todayStr
				);
				DBResultSPtr horaireResult(db.execQuery(horaireQuery));
				DBResultSPtr result(db.execQuery(query));
				horaireResult->next();
				while(result->next())
				{
					int ref(result->getInt("ref"));
					Course& course(
						courses.insert(
							make_pair(
								ref,
								Course()
						)	).first->second
					);
					course.ref = ref;
					Chainages::iterator it(chainages.find(result->getInt("chainage")));
					if(it == chainages.end())
					{
						Log::GetInstance().warn("Bad chainage field in course "+ lexical_cast<string>(ref));
						courses.erase(ref);
					}
					else
					{
						course.chainage = &it->second;
						course.syntheseService = NULL;
					}

					// Horaires loop
					do
					{
						int course_ref(horaireResult->getInt("course"));
						if(course_ref != ref)
						{
							break;
						}

						if(it != chainages.end())
						{
							Horaire& horaire(
								*course.horaires.insert(
									course.horaires.end(),
									Horaire()
							)	);
							horaire.ref = horaireResult->getInt("ref");
							horaire.had = duration_from_string(horaireResult->getText("had"));
							horaire.haa = duration_from_string(horaireResult->getText("haa"));
							horaire.hrd = duration_from_string(horaireResult->getText("hrd"));
							horaire.hra = duration_from_string(horaireResult->getText("hra"));
							horaire.htd = duration_from_string(horaireResult->getText("htd"));
							horaire.hta = duration_from_string(horaireResult->getText("hta"));
						}

					} while(horaireResult->next());
			}	}
			

			// Programmations
			{
				string query(
					"SELECT * FROM "+ _database +".PROGRAMMATION WHERE nature_dst LIKE 'BORNE%'"
				);
				string destQuery(
					"SELECT * FROM "+ _database +".DESTINATAIRE WHERE ref_prog IN (SELECT "+
					_database +".PROGRAMMATION.ref FROM "+ _database +".PROGRAMMATION WHERE nature_dst LIKE 'BORNE%') ORDER BY ref_prog"
				);
				DBResultSPtr result(db.execQuery(query));
				DBResultSPtr destResult(db.execQuery(destQuery));
				destResult->next();
				while(result->next())
				{
					int ref(result->getInt("ref"));
					Programmation& programmation(
						programmations.insert(
							make_pair(
								ref,
								Programmation()
						)	).first->second
					);
					programmation.ref = ref;

					programmation.content = result->getText("diffusion_msg");
					programmation.messageTitle = result->getText("nature_dst");
					programmation.title = result->getText("titre");
					programmation.startTime = ptime(
						from_string(result->getText("date_deb")),
						duration_from_string(result->getText("heure_deb"))
					);
					programmation.endTime = ptime(
						from_string(result->getText("date_fin")),
						duration_from_string(result->getText("heure_fin"))
					);
					if(programmation.endTime.time_of_day().hours() <= 4)
					{
						programmation.endTime += days(1);
					}

					do 
					{
						int prog_ref(destResult->getInt("ref_prog"));
						if(prog_ref != ref)
						{
							break;
						}

						Destinataire dest;
						dest.destinataire = destResult->getText("destinataire");

						// SYNTHESE departure board
						dest.syntheseDisplayBoard = _messagesRecipientsDataSource->getObjectByCode<DisplayScreen>(
							dest.destinataire
						); // Boards comes from BDSI too !!
						if(!dest.syntheseDisplayBoard)
						{
							Log::GetInstance().warn("No such display screen : " + dest.destinataire);
							continue;
						}

						programmation.destinataires.insert(
							programmation.destinataires.end(),
							dest
						);

					} while(destResult->next());
				}
			}


			//////////////////////////////////////////////////////////////////////////
			// Import content analyzing

			// Objects to update in database
			Env updatesEnv;

			{ // Scenarios and messages

				// Loop on objects present in the database (search for creations and updates)
				BOOST_FOREACH(const Programmations::value_type& itProg, programmations)
				{
					const Programmation& programmation(itProg.second);

					shared_ptr<SentScenario> updatedScenario;
					shared_ptr<SentAlarm> updatedMessage;
					SentScenario* scenario(_realTimeDataSource->getObjectByCode<SentScenario>(lexical_cast<string>(programmation.ref)));
					if(!scenario)
					{
						// Creation of the scenario
						updatedScenario.reset(
							new SentScenario(
								ScenarioTableSync::getId()
						)	);
						updatedScenario->addCodeBySource(
							*_realTimeDataSource,
							lexical_cast<string>(programmation.ref)
						);
						updatedScenario->setIsEnabled(true);
						updatesEnv.getEditableRegistry<SentScenario>().add(updatedScenario);

						// Creation of the message
						updatedMessage.reset(
							new SentAlarm(
								AlarmTableSync::getId()
						)	);
						updatedMessage->setScenario(updatedScenario.get());
						updatedScenario->addMessage(*updatedMessage);
						scenario = updatedScenario.get();
						updatesEnv.getEditableRegistry<SentAlarm>().add(updatedMessage);
					}
					else
					{
						scenariosToRemove.erase(scenario->getKey());

						// Message content
						if(scenario->getMessages().size() != 1)
						{
							Log::GetInstance().warn(
								"Corrupted message : scenario should contain one message : " + lexical_cast<string>(scenario->getKey())
							);
							continue;
						}
						const SentAlarm& message(**scenario->getMessages().begin());
						if(	message.getLongMessage() != programmation.content ||
							message.getShortMessage() != programmation.messageTitle
						){
							updatedMessage = ScenarioSentAlarmInheritedTableSync::GetEditable(
								message.getKey(),
								updatesEnv
							);
						}

						// Scenario updates
						if(	scenario->getName() != programmation.title ||
							scenario->getPeriodStart() != programmation.startTime ||
							scenario->getPeriodEnd() != programmation.endTime
						){
							updatedScenario = SentScenarioInheritedTableSync::GetEditable(
								scenario->getKey(),
								updatesEnv
							);
						}
					}

					if(updatedMessage.get())
					{
						updatedMessage->setLongMessage(programmation.content);
						updatedMessage->setShortMessage(programmation.messageTitle);
					}
					if(updatedScenario.get())
					{
						updatedScenario->setName(programmation.title);
						updatedScenario->setPeriodStart(programmation.startTime);
						updatedScenario->setPeriodEnd(programmation.endTime);
					}


					// Adding of existing object links to the removal list
					DisplayScreenAlarmRecipient::LinkedObjectsSet existingRecipients(
						DisplayScreenAlarmRecipient::getLinkedObjects(**scenario->getMessages().begin())
					);
					BOOST_FOREACH(const DisplayScreenAlarmRecipient::LinkedObjectsSet::value_type& dsit, existingRecipients)
					{
						alarmObjectLinksToRemove.insert(dsit.second->getKey());
					}

					// Loop on destinataires)
					BOOST_FOREACH(const Programmation::Destinataires::value_type& itDest, programmation.destinataires)
					{
						// Search for existing link
						DisplayScreenAlarmRecipient::LinkedObjectsSet::const_iterator itDS(
							existingRecipients.find(itDest.syntheseDisplayBoard)
						);
						if(itDS != existingRecipients.end())
						{
							alarmObjectLinksToRemove.erase(itDS->second->getKey());
							continue;
						}

						// Link creation
						shared_ptr<AlarmObjectLink> link(new AlarmObjectLink);
						link->setKey(AlarmObjectLinkTableSync::getId());
						link->setAlarm(const_cast<SentAlarm*>(updatedMessage.get()));
						link->setObjectId(itDest.syntheseDisplayBoard->getKey());
						updatesEnv.getEditableRegistry<AlarmObjectLink>().add(link);
					}
				}
			}

			// Services
			{
				// Loop on the services present in the database and link to existing or new services
				BOOST_FOREACH(const Courses::value_type& itCourse, courses)
				{
					const Course& course(itCourse.second);

					// Jump over courses with incomplete chainages
					if(course.horaires.size() != course.chainage->arretChns.size())
					{
						continue;
					}

					// Known ref ?
					ScheduledService* service(
						_realTimeDataSource->getObjectByCode<ScheduledService>(lexical_cast<string>(course.ref))
					);

					// If found, check if the service corresponds to the data
					if(service)
					{
						// Check of the chainage
						JourneyPattern& jp(*static_cast<JourneyPattern*>(service->getPath()));
						const Chainage& chainage(*course.chainage);
						if(jp.getCommercialLine()->getKey() != chainage.ligne->syntheseLine->getKey() ||
							jp.getEdges().size() != chainage.arretChns.size()
						){
							service = NULL;
						}
						else
						{
							for(size_t i(0); i<chainage.arretChns.size(); ++i)
							{
								const graph::Edge* edge(jp.getEdge(i));
								if(	!dynamic_cast<const DesignatedLinePhysicalStop*>(edge) ||
									chainage.arretChns[i].arret->syntheseStop->getKey() != jp.getEdge(i)->getFromVertex()->getKey())
								{
									service = NULL;
									break;
								}
							}
						}

						// Check of the planned schedules
						if(service)
						{
							if(course != *service)
							{
								service = NULL;
							}
						}
					}

					// If service was not found, search for existing service
					if(!service)
					{
						// Get SYNTHESE Journey pattern(s)
						if(course.chainage->syntheseJourneyPatterns.empty())
						{
							BOOST_FOREACH(Path* path, course.chainage->ligne->syntheseLine->getPaths())
							{
								if(!dynamic_cast<JourneyPattern*>(path)
								){
									continue;
								}

								const JourneyPattern& jp(*static_cast<JourneyPattern*>(path));

								if(	!jp.hasLinkWithSource(*_plannedDataSource) &&
									!jp.hasLinkWithSource(*_realTimeDataSource)
								){
									continue;
								}

								if(	jp.getWayBack() != course.chainage->sens ||
									jp.getEdges().size() != course.chainage->arretChns.size()
								){
									continue;
								}

								// Stops comparison
								bool ok(true);
								for(size_t i(0); i<course.chainage->arretChns.size(); ++i)
								{
									if(jp.getEdge(i)->getFromVertex()->getKey() != course.chainage->arretChns[i].arret->syntheseStop->getKey())
									{
										ok = false;
										break;
									}
								}
								if(!ok)
								{
									continue;
								}

								// Register the journey pattern
								course.chainage->syntheseJourneyPatterns.push_back(&jp);
								BOOST_FOREACH(const JourneyPattern::SubLines::value_type& subline, jp.getSubLines())
								{
									course.chainage->syntheseJourneyPatterns.push_back(subline);
								}
							}

							// If no journey pattern was found, creation of a new journey pattern
							if(course.chainage->syntheseJourneyPatterns.empty())
							{
								shared_ptr<JourneyPattern> jp(new JourneyPattern(JourneyPatternTableSync::getId()));
								jp->setCommercialLine(course.chainage->ligne->syntheseLine);
								jp->setWayBack(course.chainage->sens);
								jp->setName(course.chainage->nom);
								jp->addCodeBySource(
									*_realTimeDataSource,
									lexical_cast<string>(course.chainage->ref)
								);
								updatesEnv.getEditableRegistry<JourneyPattern>().add(jp);
								course.chainage->syntheseJourneyPatterns.push_back(jp.get());

								size_t rank(0);
								BOOST_FOREACH(const Chainage::ArretChns::value_type& arretChn, course.chainage->arretChns)
								{
									shared_ptr<DesignatedLinePhysicalStop> ls(
										new DesignatedLinePhysicalStop(
											LineStopTableSync::getId(),
											jp.get(),
											rank,
											rank+1 < course.chainage->arretChns.size(),
											rank > 0,
											0,
											arretChn.arret->syntheseStop,
											arretChn.type != "N"
									)	);
									jp->addEdge(*ls);
									updatesEnv.getEditableRegistry<DesignatedLinePhysicalStop>().add(ls);
									++rank;
								}
							}
						}

						// Search for a service with the same planned schedules
						BOOST_FOREACH(const JourneyPattern* route, course.chainage->syntheseJourneyPatterns)
						{
							BOOST_FOREACH(Service* sservice, route->getServices())
							{
								service = dynamic_cast<ScheduledService*>(sservice);
								if(!service)
								{
									continue;
								}
								if(	service->isActive(today) &&
									course == *service
								){
									service->addCodeBySource(*_realTimeDataSource, lexical_cast<string>(course.ref));
									break;
								}
								service = NULL;
							}
							if(service)
							{
								break;
							}
						}

						// If no service was found creation of a new service
						if(!service)
						{
							const JourneyPattern* route(
								*course.chainage->syntheseJourneyPatterns.begin()
							);
							service = new ScheduledService(
								ScheduledServiceTableSync::getId(),
								string(),
								const_cast<JourneyPattern*>(route)
							);
							SchedulesBasedService::Schedules departureSchedules;
							SchedulesBasedService::Schedules arrivalSchedules;
							for(size_t i(0); i<course.horaires.size(); ++i)
							{
								if(!static_cast<const DesignatedLinePhysicalStop*>(route->getEdge(i))->getScheduleInput())
								{
									continue;
								}

								departureSchedules.push_back(course.horaires[i].htd);
								arrivalSchedules.push_back(course.horaires[i].hta);
							}
							service->setSchedules(departureSchedules, arrivalSchedules, true);
							service->setPath(const_cast<JourneyPattern*>(route));
							service->addCodeBySource(*_realTimeDataSource, lexical_cast<string>(course.ref));
							service->setActive(today);
							//route->addService(*service, false);
							updatesEnv.getEditableRegistry<ScheduledService>().add(shared_ptr<ScheduledService>(service));
						}
						else
						{
							servicesToRemove.erase(service->getKey());
						}
					}

					// Update of the real time schedules
					SchedulesBasedService::Schedules departureSchedules;
					SchedulesBasedService::Schedules arrivalSchedules;
					for(size_t i(0); i<course.horaires.size(); ++i)
					{
						departureSchedules.push_back(course.horaires[i].hrd);
						arrivalSchedules.push_back(course.horaires[i].hra);
					}
					service->setRealTimeSchedules(departureSchedules, arrivalSchedules);

				}
			}


			//////////////////////////////////////////////////////////////////////////
			// Storage

			DBTransaction transaction;

			// Saving
			DBModule::SaveEntireEnv(updatesEnv, transaction);

			// Removals
			BOOST_FOREACH(RegistryKeyType id, alarmObjectLinksToRemove)
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



		IneoRealtimeUpdateAction::IneoRealtimeUpdateAction()
		{

		}



		bool IneoRealtimeUpdateAction::Course::operator==( const pt::ScheduledService& op ) const
		{
			for(size_t i(0); i<horaires.size(); ++i)
			{
				const DesignatedLinePhysicalStop& edge(*static_cast<const DesignatedLinePhysicalStop*>(op.getPath()->getEdge(i)));
				if(!edge.getScheduleInput())
				{
					continue;
				}

				if(	(	edge.isArrivalAllowed() &&
						horaires[i].hta != op.getArrivalSchedule(false, i)
				) || (
						edge.isDepartureAllowed() &&
						horaires[i].htd != op.getDepartureSchedule(false, i)
				)	){
					return false;
				}
			}
			return true;
		}



		bool IneoRealtimeUpdateAction::Course::operator!=( const pt::ScheduledService& op ) const
		{
			return !(operator==(op));
		}
}	}

