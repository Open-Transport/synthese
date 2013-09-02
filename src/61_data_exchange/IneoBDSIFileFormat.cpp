
//////////////////////////////////////////////////////////////////////////
/// IneoBDSIFileFormat class implementation.
/// @file IneoBDSIFileFormat.cpp
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

#include "IneoBDSIFileFormat.hpp"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmRecipientTemplate.h"
#include "BroadcastPointAlarmRecipient.hpp"
#include "CommercialLine.h"
#include "DataSourceTableSync.h"
#include "Depot.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "DisplayScreen.h"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "Import.hpp"
#include "JourneyPatternCopy.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ParametersMap.h"
#include "Request.h"
#include "RequestException.h"
#include "ScenarioTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "SentScenario.h"
#include "ServerModule.h"
#include "StopPoint.hpp"

#include <boost/filesystem.hpp>

using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::filesystem;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace departure_boards;
	using namespace graph;
	using namespace impex;
	using namespace messages;
	using namespace pt;
	using namespace pt_operation;
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<FileFormat, data_exchange::IneoBDSIFileFormat>::FACTORY_KEY = "ineo_bdsi";

	namespace data_exchange
	{
		const string IneoBDSIFileFormat::Importer_::PARAMETER_MESSAGES_RECIPIENTS_DATASOURCE_ID = "mr_ds";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_PLANNED_DATASOURCE_ID = "th_ds";
		
		
		
		ParametersMap IneoBDSIFileFormat::Importer_::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void IneoBDSIFileFormat::Importer_::_setFromParametersMap(const ParametersMap& map)
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
				throw RequestException("No such planned data source");
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
				throw RequestException("No such messages recipients data source");
			}
		}
		
		
		
		bool IneoBDSIFileFormat::Importer_::_read(
		) const {
			date today(day_clock::local_day());
			date tomorrowday(today + days(1));
			ptime timenow(second_clock::local_time());

			boost::unique_lock<shared_mutex> lock(ServerModule::baseWriterMutex, boost::try_to_lock);
			if(!lock.owns_lock())
			{
				throw RequestException("IneoBDSIFileFormat: Already running");
			}

			//////////////////////////////////////////////////////////////////////////
			// Preparation of list of objects to remove

			// Scenarios
			DataSource::Links::mapped_type existingScenarios(
				_import.get<DataSource>()->getLinkedObjects<Scenario>()
			);
			BOOST_FOREACH(const DataSource::Links::mapped_type::value_type& existingScenario, existingScenarios)
			{
				_scenariosToRemove.insert(existingScenario.second->getKey());
			}


			//////////////////////////////////////////////////////////////////////////
			// Pre-loading objects from BDSI

			Courses courses;
			Lignes lignes;
			Arrets arrets;
			Chainages chainages;
			Programmations programmations;
			DB& db(*DBModule::GetDB());
			string todayStr("'"+ to_iso_extended_string(today) +"'");
			string tomorrowStr("'"+ to_iso_extended_string(tomorrowday) +"'");
			string timenowStr("'"+ to_simple_string(timenow) +"'");

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
					string name(result->getText("nom"));

					StopPoint* stopPoint(
						_plannedDataSource->getObjectByCode<StopPoint>(mnemol)
					);
					Depot* depot(NULL);
					if(stopPoint)
					{
						_logLoadDetail(
							"STOP",mnemol,name,stopPoint->getKey(),stopPoint->getConnectionPlace()->getFullName() + "/" + stopPoint->getName(), string(), string(), "OK"
						);
					}
					else
					{
						depot = _plannedDataSource->getObjectByCode<Depot>(mnemol);
						if(depot)
						{
							_logLoadDetail(
								"DEPOT",mnemol,name,depot->getKey(),depot->getName(),string(), string(), "OK"
							);
						}
						else
						{
							_logWarningDetail(
								"STOP/DEPOT",mnemol,name,0,string(), string(),string(), "NOT FOUND"
							);
							continue;
					}	}

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
					arret.nom = name;
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
					if(line)
					{
						_logLoadDetail(
							"LINE",mnemo,mnemo,line->getKey(),line->getName(),string(), string(), "OK"
						);
					}
					else
					{
						Log::GetInstance().warn("No such line : " + mnemo);
						_logWarningDetail(
							"LINE",mnemo,mnemo,0,string(), string(),string(), "NOT FOUND"
						);
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
					"SELECT * FROM "+ _database +".CHAINAGE WHERE jour="+ todayStr + "ORDER BY ref"
				);
			
				DBResultSPtr chainageResult(db.execQuery(chainageQuery));
				DBResultSPtr result(db.execQuery(query));
				chainageResult->next();
				while(result->next())
				{
					int ref(result->getInt("ref"));
					string name(result->getText("nom"));

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
						_logWarningDetail(
							"JOURNEYPATTERN",lexical_cast<string>(ref),name,0,string(),string(), string(),"LINE NOT FOUND"
						);
						chainages.erase(ref);
						do
						{
							int chainage_ref(chainageResult->getInt("chainage"));
							if(chainage_ref != ref)
							{
								break;
							}
						} while(chainageResult->next());
						continue;
					}
					chainage.ligne = &it->second;
					chainage.nom = name;
					chainage.sens = (result->getText("sens") == "R");
					_logLoadDetail(
						"JOURNEYPATTERN",lexical_cast<string>(ref),name,0,string(),string(), string(),"OK"
					);
					
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
							_logWarningDetail(
								"STOPPOINT",lexical_cast<string>(arretChn.ref),lexical_cast<string>(arretChn.ref),0,string(),string(), string(),"Bad arret field in arretchn"
							);
							chainage.arretChns.pop_back();
							continue;
						}
						arretChn.arret = &it->second;
						arretChn.pos = chainageResult->getInt("pos");
						arretChn.type = chainageResult->getText("type");
						_logLoadDetail(
							"STOPPOINT",lexical_cast<string>(arretChn.ref),arretChn.arret->nom,0,string(),string(), string(),"OK"
						);

					} while(chainageResult->next());
				}
			}


			// Courses
			{
				
				// no need to query to tomorrow.
				// as we place to import every day very early in the morning.

				string horaireQuery(
					"SELECT "+ _database +".HORAIRE.* FROM "+ _database +".HORAIRE "+
					"INNER JOIN "+ _database +".ARRETCHN ON "+ _database +".HORAIRE.arretchn="+ _database +".ARRETCHN.ref AND "+
					_database +".HORAIRE.jour="+ _database +".ARRETCHN.jour "+
					"WHERE ("+ _database +".HORAIRE.jour="+ todayStr + "AND "+ _database +".HORAIRE.hra> "+ timenowStr +
					") AND ("+ _database +".HORAIRE.jour="+ tomorrowStr + "AND "+ _database +".HORAIRE.hra< "+ timenowStr +
					") ORDER BY "+ _database +".HORAIRE.course, "+ _database +".ARRETCHN.pos"
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
						_logWarningDetail(
							"SERVICE",lexical_cast<string>(ref),lexical_cast<string>(ref),0,string(),string(), string(),"Bad chainage field in course "
						);
						courses.erase(ref);
					}
					else
					{
						_logLoadDetail(
							"SERVICE",lexical_cast<string>(ref),lexical_cast<string>(ref),0,string(),string(), string(),"OK"
						);
						course.chainage = &it->second;
						course.syntheseService = NULL;
					}

					// Horaires loop
					time_duration now(second_clock::local_time().time_of_day());
					const time_duration dayBreakTime(hours(3));

					// Patch for mistake in real time departure of the vehicle from the stop
					time_duration now_plus_35(second_clock::local_time().time_of_day());
					now_plus_35 += seconds(35);

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
							// Patch for schedules after midnight
							if(horaire.had < dayBreakTime)
							{
								horaire.had += hours(24);
							}
							horaire.haa = duration_from_string(horaireResult->getText("haa"));
							if(horaire.haa < dayBreakTime)
							{
								horaire.haa += hours(24);
							}
							horaire.hrd = duration_from_string(horaireResult->getText("hrd"));
							if(horaire.hrd < dayBreakTime)
							{
								horaire.hrd += hours(24);
							}
							horaire.hra = duration_from_string(horaireResult->getText("hra"));
							if(horaire.hra < dayBreakTime)
							{
								horaire.hra += hours(24);
							}
							horaire.htd = duration_from_string(horaireResult->getText("htd"));
							if(horaire.htd < dayBreakTime)
							{
								horaire.htd += hours(24);
							}
							horaire.hta = duration_from_string(horaireResult->getText("hta"));
							if(horaire.hta < dayBreakTime)
							{
								horaire.hta += hours(24);
							}

							// Patch for bad schedules when the bus is at stop
							if(	(	(	horaireResult->getText("etat_harr") == "R" &&
										horaireResult->getText("etat_hdep") == "E"
									) || (
										horaire.hrd > now
									)
								) &&
								horaire.hrd <= now_plus_35
							){
								horaire.hrd = now_plus_35;
							}

							_logDebugDetail(
								"SCHEDULE_HTD",lexical_cast<string>(ref),lexical_cast<string>(ref),0,string(),horaireResult->getText("htd"),to_simple_string(horaire.htd),"OK"
							);
							_logDebugDetail(
								"SCHEDULE_HTA",lexical_cast<string>(ref),lexical_cast<string>(ref),0,string(),horaireResult->getText("hta"),to_simple_string(horaire.hta),"OK"
							);
							_logDebugDetail(
								"SCHEDULE_HRD",lexical_cast<string>(ref),lexical_cast<string>(ref),0,string(),horaireResult->getText("hrd"),to_simple_string(horaire.hrd),"OK"
							);
							_logDebugDetail(
								"SCHEDULE_HRA",lexical_cast<string>(ref),lexical_cast<string>(ref),0,string(),horaireResult->getText("hra"),to_simple_string(horaire.hra),"OK"
							);
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
					replace_all(programmation.content, "\r", " ");
					replace_all(programmation.content, "\n", " ");
					replace_all(programmation.content, "  ", " ");
					programmation.messageTitle = result->getText("nature_dst");
					programmation.title = result->getText("titre");
					programmation.active = (result->getText("actif") == "O");
					programmation.priority = (result->getText("alternance") == "N");
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
							_logWarning("No such display screen : " + dest.destinataire);
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

			{ // Scenarios and messages

				// Loop on objects present in the database (search for creations and updates)
				BOOST_FOREACH(const Programmations::value_type& itProg, programmations)
				{
					const Programmation& programmation(itProg.second);

					boost::shared_ptr<SentScenario> updatedScenario;
					boost::shared_ptr<Alarm> updatedMessage;
					SentScenario* scenario(
						static_cast<SentScenario*>(
							_import.get<DataSource>()->getObjectByCode<Scenario>(
								lexical_cast<string>(programmation.ref)
					)	)	);
					Alarm* message(NULL);
					if(!scenario)
					{
						// Creation of the scenario
						updatedScenario.reset(
							new SentScenario(
								ScenarioTableSync::getId()
						)	);
						updatedScenario->addCodeBySource(
							*_import.get<DataSource>(),
							lexical_cast<string>(programmation.ref)
						);
						updatedScenario->setIsEnabled(true);
						_env.getEditableRegistry<Scenario>().add(updatedScenario);

						// Creation of the message
						updatedMessage.reset(
							new SentAlarm(
								AlarmTableSync::getId()
						)	);
						updatedMessage->setScenario(updatedScenario.get());
						updatedScenario->addMessage(*updatedMessage);
						scenario = updatedScenario.get();
						message = updatedMessage.get();
						_env.getEditableRegistry<Alarm>().add(updatedMessage);
					}
					else
					{
						_scenariosToRemove.erase(scenario->getKey());

						// Message content
						const Scenario::Messages& messages(scenario->getMessages());
						if(messages.size() != 1)
						{
							_logWarning(
								"Corrupted message : scenario should contain one message : " + lexical_cast<string>(scenario->getKey())
							);
							
							SentScenario::Messages::const_iterator it(messages.begin());
							for(++it; it != messages.end(); ++it)
							{
								_messagesToRemove.insert((*it)->getKey());
							}
						}
						message = const_cast<Alarm*>(*messages.begin());
						if(	message->getLongMessage() != programmation.content ||
							message->getShortMessage() != programmation.messageTitle ||
							(message->getLevel() == ALARM_LEVEL_WARNING) != programmation.priority
						){
							updatedMessage = AlarmTableSync::GetCastEditable<SentAlarm>(
								message->getKey(),
								_env
							);
						}

						// Scenario updates
						if(	scenario->getName() != programmation.title ||
							scenario->getPeriodStart() != programmation.startTime ||
							scenario->getPeriodEnd() != programmation.endTime ||
							scenario->getIsEnabled() != programmation.active
						){
							updatedScenario = ScenarioTableSync::GetCastEditable<SentScenario>(
								scenario->getKey(),
								_env
							);
						}
					}

					if(updatedMessage.get())
					{
						updatedMessage->setLongMessage(programmation.content);
						updatedMessage->setShortMessage(programmation.messageTitle);
						updatedMessage->setLevel(programmation.priority ? ALARM_LEVEL_WARNING : ALARM_LEVEL_INFO);
					}
					if(updatedScenario.get())
					{
						updatedScenario->setName(programmation.title);
						updatedScenario->setPeriodStart(programmation.startTime);
						updatedScenario->setPeriodEnd(programmation.endTime);
						updatedScenario->setIsEnabled(programmation.active);
					}


					// Adding of existing object links to the removal list
					Alarm::LinkedObjects::mapped_type existingRecipients(
						(*scenario->getMessages().begin())->getLinkedObjects(
							BroadcastPointAlarmRecipient::FACTORY_KEY
					)	);
					BOOST_FOREACH(const Alarm::LinkedObjects::mapped_type::value_type& aol, existingRecipients)
					{
						_alarmObjectLinksToRemove.insert(aol->getKey());
					}

					// Loop on destinataires)
					BOOST_FOREACH(const Programmation::Destinataires::value_type& itDest, programmation.destinataires)
					{
						// Search for existing link
						const AlarmObjectLink* toNotRemove(NULL);
						BOOST_FOREACH(const Alarm::LinkedObjects::mapped_type::value_type& aol, existingRecipients)
						{
							if(aol->getObjectId() == itDest.syntheseDisplayBoard->getKey())
							{
								toNotRemove = aol;
								break;
							}
						}
						if(toNotRemove)
						{
							_alarmObjectLinksToRemove.erase(toNotRemove->getKey());
							continue;
						}

						// Link creation
						boost::shared_ptr<AlarmObjectLink> link(new AlarmObjectLink);
						link->setKey(AlarmObjectLinkTableSync::getId());
						link->setAlarm(message);
						link->setObjectId(itDest.syntheseDisplayBoard->getKey());
						link->setRecipient(BroadcastPointAlarmRecipient::FACTORY_KEY);
						_env.getEditableRegistry<AlarmObjectLink>().add(link);
					}
				}
			}

			// Services
			{
				// Management of the ref field
				typedef vector<const Course*> ServicesToUpdate;
				ServicesToUpdate servicesToUpdate; // Old = New
				typedef map<string, const ScheduledService*> ServicesToRemove;
				ServicesToRemove servicesToRemove; // Old

				ServicesToUpdate servicesToLinkAndUpdate; // New
				ServicesToUpdate servicesToMoveAndUpdate; // Old -> New

				// Services
				DataSource::Links::mapped_type existingServices(
					_plannedDataSource->getLinkedObjects<ScheduledService>()
				);
				BOOST_FOREACH(const DataSource::Links::mapped_type::value_type& existingService, existingServices)
				{
					servicesToRemove.insert(
						make_pair(
							existingService.first,
							static_cast<ScheduledService*>(existingService.second)
					)	);
				}

				// Search for existing service with same key
				// If found, take the service into servicesToUpdate and remove it from servicesToRemove
				BOOST_FOREACH(Courses::value_type& itCourse, courses)
				{
					Course& course(itCourse.second);

					if(!course.mustBeImported())
					{
						continue;
					}

					// Known ref ?
					ScheduledService* service(
						_import.get<DataSource>()->getObjectByCode<ScheduledService>(lexical_cast<string>(course.ref))
					);
					if(!service)
					{
						continue;
					}
						
					// Checks if the service and the course are matching
					if(course != *service)
					{
						continue;
					}

					// OK the service is sent to simple update
					course.syntheseService = service;
					servicesToUpdate.push_back(&course);
					servicesToRemove.erase(lexical_cast<string>(course.ref));
				}

				// Search for existing services with other key
				// If found, take the service into servicesToMoveAndUpdate and remove it from servicesToRemove
				BOOST_FOREACH(Courses::value_type& itCourse, courses)
				{
					Course& course(itCourse.second);

					// Jump over already joined services
					if( course.syntheseService ||
						!course.mustBeImported()
					){
						continue;
					}

					// Loop on non linked services
					BOOST_FOREACH(const ServicesToRemove::value_type& itService, servicesToRemove)
					{
						// Check if the service matches
						if(	course != *itService.second)
						{
							continue;
						}

						// OK
						course.syntheseService = const_cast<ScheduledService*>(itService.second);
						servicesToMoveAndUpdate.push_back(&course);
						servicesToRemove.erase(itService.first);
						break;
					}
					if(course.syntheseService)
					{
						continue;
					}

					// If not found, attempt to find a non linked service
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
								!jp.hasLinkWithSource(*_import.get<DataSource>())
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
							boost::shared_ptr<JourneyPattern> jp(new JourneyPattern(JourneyPatternTableSync::getId()));
							jp->setCommercialLine(course.chainage->ligne->syntheseLine);
							jp->setWayBack(course.chainage->sens);
							jp->setName(course.chainage->nom);
							jp->addCodeBySource(
								*_import.get<DataSource>(),
								lexical_cast<string>(course.chainage->ref)
							);
							_env.getEditableRegistry<JourneyPattern>().add(jp);
							course.chainage->syntheseJourneyPatterns.push_back(jp.get());

							size_t rank(0);
							BOOST_FOREACH(const Chainage::ArretChns::value_type& arretChn, course.chainage->arretChns)
							{
								boost::shared_ptr<DesignatedLinePhysicalStop> ls(
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
								_env.getEditableRegistry<LineStop>().add(ls);
								++rank;
							}
						}
					}

					// Search for a service with the same planned schedules
					BOOST_FOREACH(const JourneyPattern* route, course.chainage->syntheseJourneyPatterns)
					{
						boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
									*route->sharedServicesMutex
						);
						BOOST_FOREACH(Service* sservice, route->getServices())
						{
							ScheduledService* service(
								dynamic_cast<ScheduledService*>(sservice)
							);
							if(!service)
							{
								continue;
							}
							if(	service->isActive(today) &&
								course == *service
							){
								course.syntheseService = service;
								servicesToLinkAndUpdate.push_back(&course);
								servicesToRemove.erase(lexical_cast<string>(course.ref));
								break;
							}
						}
						if(course.syntheseService)
						{
							break;
						}
					}
					if(course.syntheseService)
					{
						continue;
					}

					// Creation
					const JourneyPattern* route(
						*course.chainage->syntheseJourneyPatterns.begin()
						);
					course.syntheseService = new ScheduledService(
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
					course.syntheseService->setSchedules(departureSchedules, arrivalSchedules, true);
					course.syntheseService->setPath(const_cast<JourneyPattern*>(route));
					course.syntheseService->addCodeBySource(*_import.get<DataSource>(), lexical_cast<string>(course.ref));
					course.syntheseService->setActive(today);
					_env.getEditableRegistry<ScheduledService>().add(boost::shared_ptr<ScheduledService>(course.syntheseService));

					servicesToLinkAndUpdate.push_back(&course);
				}

				DataSource* dataSourceOnUpdateEnv(
					DataSourceTableSync::GetEditable(_import.get<DataSource>()->getKey(), _env).get()
				);

				// Loop on services to update
				BOOST_FOREACH(const ServicesToUpdate::value_type& it, servicesToUpdate)
				{
					it->updateService(*it->syntheseService);
				}

				// Loop on services to move and update
				BOOST_FOREACH(const ServicesToUpdate::value_type& it, servicesToMoveAndUpdate)
				{
					boost::shared_ptr<ScheduledService> oldService(
						ScheduledServiceTableSync::GetEditable(
							it->syntheseService->getKey(),
							_env
					)	);
					Importable::DataSourceLinks links(oldService->getDataSourceLinks());
					links.erase(dataSourceOnUpdateEnv);
					links.insert(make_pair(dataSourceOnUpdateEnv, lexical_cast<string>(it->ref)));
					oldService->setDataSourceLinksWithoutRegistration(links);
					it->updateService(*it->syntheseService);
				}

				// Loop on services to link and update
				BOOST_FOREACH(const ServicesToUpdate::value_type& it, servicesToLinkAndUpdate)
				{
					boost::shared_ptr<ScheduledService> oldService(
						ScheduledServiceTableSync::GetEditable(
							it->syntheseService->getKey(),
							_env
					)	);
					Importable::DataSourceLinks links(oldService->getDataSourceLinks());
					links.erase(dataSourceOnUpdateEnv);
					links.insert(make_pair(dataSourceOnUpdateEnv, lexical_cast<string>(it->ref)));
					oldService->setDataSourceLinksWithoutRegistration(links);
					it->updateService(*it->syntheseService);
				}

				// Remove services from today
				BOOST_FOREACH(const ServicesToRemove::value_type& it, servicesToRemove)
				{
					boost::shared_ptr<ScheduledService> oldService(
						ScheduledServiceTableSync::GetEditable(
							it.second->getKey(),
							_env
					)	);
					Importable::DataSourceLinks links(oldService->getDataSourceLinks());
					links.erase(dataSourceOnUpdateEnv);
					oldService->setDataSourceLinksWithoutRegistration(links);
					oldService->setInactive(today);
				}
			}

			return true;
		}
		


		IneoBDSIFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			DatabaseReadImporter<IneoBDSIFileFormat>(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		void IneoBDSIFileFormat::Importer_::_logLoadDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logLoad(content.str());
		}



		void IneoBDSIFileFormat::Importer_::_logWarningDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logWarning(content.str());
		}



		void IneoBDSIFileFormat::Importer_::_logDebugDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logDebug(content.str());
		}



		DBTransaction IneoBDSIFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Saving
			DBModule::SaveEntireEnv(_env, transaction);

			// Removals
			BOOST_FOREACH(RegistryKeyType id, _alarmObjectLinksToRemove)
			{
				DBTableSyncTemplate<AlarmObjectLinkTableSync>::Remove(NULL, id, transaction, false);
			}
			BOOST_FOREACH(RegistryKeyType id, _scenariosToRemove)
			{
				DBTableSyncTemplate<ScenarioTableSync>::Remove(NULL, id, transaction, false);
			}
			BOOST_FOREACH(RegistryKeyType id, _messagesToRemove)
			{
				DBTableSyncTemplate<AlarmTableSync>::Remove(NULL, id, transaction, false);
			}

			return transaction;
		}



		bool IneoBDSIFileFormat::Importer_::Course::mustBeImported() const
		{
			// Jump over courses with incomplete chainages
			if(horaires.size() != chainage->arretChns.size())
			{
				return false;
			}

			// Jump over dead runs
			BOOST_FOREACH(const Chainage::ArretChns::value_type& it, chainage->arretChns)
			{
				if(!it.arret->syntheseStop)
				{
					return false;
				}
			}

			return true;
		}



		bool IneoBDSIFileFormat::Importer_::Course::operator==( const pt::ScheduledService& service ) const
		{
			// Basic route check
			const JourneyPattern& jp(*static_cast<const JourneyPattern*>(service.getPath()));
			if(	jp.getCommercialLine()->getKey() != chainage->ligne->syntheseLine->getKey() ||
				jp.getEdges().size() != chainage->arretChns.size()
			){
				return false;
			}

			// Check of each stop
			for(size_t i(0); i<chainage->arretChns.size(); ++i)
			{
				const graph::Edge* edge(jp.getEdge(i));
				if(	!dynamic_cast<const DesignatedLinePhysicalStop*>(edge) ||
					chainage->arretChns[i].arret->syntheseStop->getKey() != jp.getEdge(i)->getFromVertex()->getKey())
				{
					return false;
				}
			}

			// Check of the planned schedules
			for(size_t i(0); i<horaires.size(); ++i)
			{
				const DesignatedLinePhysicalStop& edge(*static_cast<const DesignatedLinePhysicalStop*>(service.getPath()->getEdge(i)));
				if(!edge.getScheduleInput())
				{
					continue;
				}

				if(	(	edge.isArrivalAllowed() &&
					(	horaires[i].hta.hours() != service.getArrivalSchedule(false, i).hours() ||
						horaires[i].hta.minutes() != service.getArrivalSchedule(false, i).minutes()
				)	) || (
					edge.isDepartureAllowed() &&
					(	horaires[i].htd.hours() != service.getDepartureSchedule(false, i).hours() ||
						horaires[i].htd.minutes() != service.getDepartureSchedule(false, i).minutes()
				)	)	){
						return false;
				}
			}

			// OK the service matches
			return true;
		}





		void IneoBDSIFileFormat::Importer_::Course::updateService( pt::ScheduledService& service ) const
		{
			// Update of the real time schedules
			SchedulesBasedService::Schedules departureSchedules;
			SchedulesBasedService::Schedules arrivalSchedules;
			for(size_t i(0); i<horaires.size(); ++i)
			{
				departureSchedules.push_back(horaires[i].hrd);
				arrivalSchedules.push_back(horaires[i].hra);
			}
			service.setRealTimeSchedules(departureSchedules, arrivalSchedules);
		}



		bool IneoBDSIFileFormat::Importer_::Course::operator!=( const pt::ScheduledService& op ) const
		{
			return !operator==(op);
		}
}	}

