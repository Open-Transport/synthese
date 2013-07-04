
//////////////////////////////////////////////////////////////////////////
/// IneoRealTimeUpdateAction class implementation.
/// @file IneoRealTimeUpdateAction.cpp
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

#include "IneoRealTimeUpdateAction.hpp"

#include "ActionException.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmRecipientTemplate.h"
#include "CommercialLine.h"
#include "DataSourceTableSync.h"
#include "Depot.hpp"
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
#include "ScenarioTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "SentScenario.h"
#include "ServerModule.h"
#include "StopPoint.hpp"
#include "Vehicle.hpp"
#include "VehicleTableSync.hpp"

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
	using namespace pt;
	using namespace pt_operation;
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace vehicle;
	
	template<>
	const string FactorableTemplate<Action, data_exchange::IneoRealTimeUpdateAction>::FACTORY_KEY = "ineo_realtime_update";

	namespace data_exchange
	{
		const string IneoRealTimeUpdateAction::PARAMETER_MESSAGES_RECIPIENTS_DATASOURCE_ID = Action_PARAMETER_PREFIX + "_mr_ds";
		const string IneoRealTimeUpdateAction::PARAMETER_PLANNED_DATASOURCE_ID = Action_PARAMETER_PREFIX + "_th_ds";
		const string IneoRealTimeUpdateAction::PARAMETER_REAL_TIME_DATASOURCE_ID = Action_PARAMETER_PREFIX + "_rt_ds";
		const string IneoRealTimeUpdateAction::PARAMETER_DATABASE = Action_PARAMETER_PREFIX + "db";
		const string IneoRealTimeUpdateAction::PARAMETER_TIME_FLOOR = "time_floor"; // In seconds

		
		ParametersMap IneoRealTimeUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void IneoRealTimeUpdateAction::_setFromParametersMap(const ParametersMap& map)
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

			// Time Floor
			_timeFloor = map.getDefault<int>(PARAMETER_TIME_FLOOR, 1);

			// Database
			_database = map.get<string>(PARAMETER_DATABASE);
		}
		
		
		
		void IneoRealTimeUpdateAction::run(
			Request& request
		){
			boost::mutex::scoped_lock lock(ServerModule::baseWriterMutex, boost::try_to_lock);
			if(!lock.owns_lock())
			{
				throw ActionException("IneoRealTimeUpdateAction: Already running a base update");
			}

			date today(day_clock::local_day());

			//////////////////////////////////////////////////////////////////////////
			// Preparation of list of objects to remove

			// Scenarios
			set<RegistryKeyType> scenariosToRemove;
			DataSource::Links::mapped_type existingScenarios(
				_realTimeDataSource->getLinkedObjects<Scenario>()
			);
			BOOST_FOREACH(const DataSource::Links::mapped_type::value_type& existingScenario, existingScenarios)
			{
				scenariosToRemove.insert(existingScenario.second->getKey());
			}

			// Alarm object links
			set<RegistryKeyType> alarmObjectLinksToRemove;

			// Alarms
			set<RegistryKeyType> messagesToRemove;

			// Vehicles
			set<RegistryKeyType> vehiclesToRemove;
			DataSource::Links::mapped_type existingVehicles(
				_plannedDataSource->getLinkedObjects<Vehicle>()
			);
			BOOST_FOREACH(const DataSource::Links::mapped_type::value_type& existingVehicle, existingVehicles)
			{
				vehiclesToRemove.insert(existingVehicle.second->getKey());
			}

			//////////////////////////////////////////////////////////////////////////
			// Pre-loading objects from BDSI

			Courses courses;
			Lignes lignes;
			Arrets arrets;
			Chainages chainages;
			Programmations programmations;
			IneoVehicles ineoVehicles;
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
					Depot* depot(NULL);
					if(!stopPoint)
					{
						depot = _plannedDataSource->getObjectByCode<Depot>(mnemol);
						if(!depot)
						{
							Log::GetInstance().warn("No such stop point or depot : " + mnemol);
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
					"SELECT * FROM "+ _database +".CHAINAGE WHERE jour="+ todayStr + "ORDER BY ref"
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

			// Vehicle
			{
				string query("SELECT VEHICULE.* FROM "+ _database +".VEHICULE");
				DBResultSPtr result(db.execQuery(query));
				while(result->next())
				{
					int ref(result->getInt("ref"));
					IneoVehicle& vehicle(
						ineoVehicles.insert(
							make_pair(
								ref,
								IneoVehicle()
						)	).first->second
					);
					vehicle.ref = lexical_cast<string>(ref);
					vehicle.available = (result->getText("neutralise") == "N" ? true : false);
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
					shared_ptr<Alarm> updatedMessage;
					SentScenario* scenario(
						static_cast<SentScenario*>(
								_realTimeDataSource->getObjectByCode<Scenario>(
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
							*_realTimeDataSource,
							lexical_cast<string>(programmation.ref)
						);
						updatedScenario->setIsEnabled(true);
						updatesEnv.getEditableRegistry<Scenario>().add(updatedScenario);

						// Creation of the message
						updatedMessage.reset(
							new SentAlarm(
								AlarmTableSync::getId()
						)	);
						updatedMessage->setScenario(updatedScenario.get());
						updatedScenario->addMessage(*updatedMessage);
						scenario = updatedScenario.get();
						message = updatedMessage.get();
						updatesEnv.getEditableRegistry<Alarm>().add(updatedMessage);
					}
					else
					{
						scenariosToRemove.erase(scenario->getKey());

						// Message content
						const Scenario::Messages& messages(scenario->getMessages());
						if(messages.size() != 1)
						{
							Log::GetInstance().warn(
								"Corrupted message : scenario should contain one message : " + lexical_cast<string>(scenario->getKey())
							);
							
							SentScenario::Messages::const_iterator it(messages.begin());
							for(++it; it != messages.end(); ++it)
							{
								messagesToRemove.insert((*it)->getKey());
							}
						}
						message = const_cast<Alarm*>(*messages.begin());
						if(	message->getLongMessage() != programmation.content ||
							message->getShortMessage() != programmation.messageTitle ||
							(message->getLevel() == ALARM_LEVEL_WARNING) != programmation.priority
						){
							updatedMessage = AlarmTableSync::GetCastEditable<SentAlarm>(
								message->getKey(),
								updatesEnv
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
								updatesEnv
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
					Alarm::LinkedObjects::mapped_type existingRecipients;
					const Alarm::LinkedObjects& linkedObjects(
						(*scenario->getMessages().begin())->getLinkedObjects()
					);
					Alarm::LinkedObjects::const_iterator it(
						linkedObjects.find(
							DisplayScreenAlarmRecipient::FACTORY_KEY
					)	);
					if(it != linkedObjects.end())
					{
						existingRecipients = it->second;
					}
					BOOST_FOREACH(const Alarm::LinkedObjects::mapped_type::value_type& aol, existingRecipients)
					{
						alarmObjectLinksToRemove.insert(aol->getKey());
					}

					// Loop on destinataires)
					BOOST_FOREACH(const Programmation::Destinataires::value_type& itDest, programmation.destinataires)
					{
						// Search for existing link
						const AlarmObjectLink* toNotRemove(NULL);
						BOOST_FOREACH(const Alarm::LinkedObjects::mapped_type::value_type& aol, existingRecipients)
						{
							if(aol->getObject() == itDest.syntheseDisplayBoard)
							{
								toNotRemove = aol;
								break;
							}
						}
						if(toNotRemove)
						{
							alarmObjectLinksToRemove.erase(toNotRemove->getKey());
							continue;
						}

						// Link creation
						shared_ptr<AlarmObjectLink> link(new AlarmObjectLink);
						link->setKey(AlarmObjectLinkTableSync::getId());
						link->setAlarm(message);
						link->setObject(itDest.syntheseDisplayBoard);
						link->setRecipientKey("displayscreen");
						updatesEnv.getEditableRegistry<AlarmObjectLink>().add(link);
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
						_realTimeDataSource->getObjectByCode<ScheduledService>(lexical_cast<string>(course.ref))
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
								updatesEnv.getEditableRegistry<LineStop>().add(ls);
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
					course.syntheseService->addCodeBySource(*_realTimeDataSource, lexical_cast<string>(course.ref));
					course.syntheseService->setActive(today);
					updatesEnv.getEditableRegistry<ScheduledService>().add(shared_ptr<ScheduledService>(course.syntheseService));

					servicesToLinkAndUpdate.push_back(&course);
				}

				DataSource* dataSourceOnUpdateEnv(
					DataSourceTableSync::GetEditable(_realTimeDataSource->getKey(), updatesEnv).get()
				);

				// Loop on services to update
				BOOST_FOREACH(const ServicesToUpdate::value_type& it, servicesToUpdate)
				{
					it->updateService(*it->syntheseService, _timeFloor);
				}

				// Loop on services to move and update
				BOOST_FOREACH(const ServicesToUpdate::value_type& it, servicesToMoveAndUpdate)
				{
					shared_ptr<ScheduledService> oldService(
						ScheduledServiceTableSync::GetEditable(
							it->syntheseService->getKey(),
							updatesEnv,
							FIELDS_ONLY_LOAD_LEVEL
					)	);
					Importable::DataSourceLinks links(oldService->getDataSourceLinks());
					links.erase(dataSourceOnUpdateEnv);
					links.insert(make_pair(dataSourceOnUpdateEnv, lexical_cast<string>(it->ref)));
					oldService->setDataSourceLinksWithoutRegistration(links);
					it->updateService(*it->syntheseService, _timeFloor);
				}

				// Loop on services to link and update
				BOOST_FOREACH(const ServicesToUpdate::value_type& it, servicesToLinkAndUpdate)
				{
					shared_ptr<ScheduledService> oldService(
						ScheduledServiceTableSync::GetEditable(
							it->syntheseService->getKey(),
							updatesEnv,
							FIELDS_ONLY_LOAD_LEVEL
					)	);
					Importable::DataSourceLinks links(oldService->getDataSourceLinks());
					links.erase(dataSourceOnUpdateEnv);
					links.insert(make_pair(dataSourceOnUpdateEnv, lexical_cast<string>(it->ref)));
					oldService->setDataSourceLinksWithoutRegistration(links);
					it->updateService(*it->syntheseService, _timeFloor);
				}

				// Remove services from today
				BOOST_FOREACH(const ServicesToRemove::value_type& it, servicesToRemove)
				{
					shared_ptr<ScheduledService> oldService(
						ScheduledServiceTableSync::GetEditable(
							it.second->getKey(),
							updatesEnv,
							FIELDS_ONLY_LOAD_LEVEL
					)	);
					Importable::DataSourceLinks links(oldService->getDataSourceLinks());
					links.erase(dataSourceOnUpdateEnv);
					oldService->setDataSourceLinksWithoutRegistration(links);
					oldService->setInactive(today);
				}
			}

			{ // Vehicles

				// Loop on objects present in the database (search for creations and updates)
				BOOST_FOREACH(const IneoVehicles::value_type& itVehicle, ineoVehicles)
				{
					const IneoVehicle& ineoVehicle(itVehicle.second);

					shared_ptr<Vehicle> updatedVehicle;
					Vehicle* vehicle(
						static_cast<Vehicle*>(
								_plannedDataSource->getObjectByCode<Vehicle>(
									ineoVehicle.ref
					)	)	);
					if(!vehicle)
					{
						// Creation of the vehicle
						updatedVehicle.reset(
							new Vehicle(
								VehicleTableSync::getId()
						)	);
						updatedVehicle->addCodeBySource(
							*_plannedDataSource,
							ineoVehicle.ref
						);
						updatesEnv.getEditableRegistry<Vehicle>().add(updatedVehicle);
					}
					else
					{
						vehiclesToRemove.erase(vehicle->getKey());

						if(	vehicle->getNumber() != ineoVehicle.ref ||
							vehicle->getAvailable() != ineoVehicle.available
						){
							updatedVehicle = VehicleTableSync::GetCastEditable<Vehicle>(
								vehicle->getKey(),
								updatesEnv
							);
						}
					}

					if(updatedVehicle.get())
					{
						updatedVehicle->setNumber(ineoVehicle.ref);
						updatedVehicle->setAvailable(ineoVehicle.available);
					}

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
				DBTableSyncTemplate<AlarmObjectLinkTableSync>::Remove(NULL, id, transaction, false);
			}
			BOOST_FOREACH(RegistryKeyType id, scenariosToRemove)
			{
				DBTableSyncTemplate<ScenarioTableSync>::Remove(NULL, id, transaction, false);
			}
			BOOST_FOREACH(RegistryKeyType id, messagesToRemove)
			{
				DBTableSyncTemplate<AlarmTableSync>::Remove(NULL, id, transaction, false);
			}
			// Vehicle removals
			BOOST_FOREACH(RegistryKeyType id, vehiclesToRemove)
			{
				DBTableSyncTemplate<VehicleTableSync>::Remove(NULL, id, transaction, false);
			}

			transaction.run();
		}
		
		
		
		bool IneoRealTimeUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		IneoRealTimeUpdateAction::IneoRealTimeUpdateAction()
		{

		}



		bool IneoRealTimeUpdateAction::Course::mustBeImported() const
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



		bool IneoRealTimeUpdateAction::Course::operator==( const pt::ScheduledService& service ) const
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


		// Round the given time to the closest minute
		time_duration
		IneoRealTimeUpdateAction::Course::_applyTimeFloor(
			const time_duration &td,
			size_t timeFloor
		) const
		{
			time_duration tdResult( seconds(td.total_seconds() - td.total_seconds() % timeFloor) );
			return tdResult;
		}


		void IneoRealTimeUpdateAction::Course::updateService(
			pt::ScheduledService& service,
			size_t timeFloor
		) const
		{
			// Update of the real time schedules
			SchedulesBasedService::Schedules departureSchedules;
			SchedulesBasedService::Schedules arrivalSchedules;
			for(size_t i(0); i<horaires.size(); ++i)
			{
				departureSchedules.push_back(_applyTimeFloor(horaires[i].hrd, timeFloor));
				arrivalSchedules.push_back(_applyTimeFloor(horaires[i].hra, timeFloor));
			}
			service.setRealTimeSchedules(departureSchedules, arrivalSchedules);
		}



		bool IneoRealTimeUpdateAction::Course::operator!=( const pt::ScheduledService& op ) const
		{
			return !operator==(op);
		}
}	}

