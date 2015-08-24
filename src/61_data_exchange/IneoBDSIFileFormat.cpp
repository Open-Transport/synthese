
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
#include "CityTableSync.h"
#include "CommercialLine.h"
#include "DataSourceTableSync.h"
#include "DeactivationPTDataInterSYNTHESE.hpp"
#include "Depot.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "DisplayScreen.h"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "Import.hpp"
#include "InterSYNTHESEModule.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ParametersMap.h"
#include "Request.h"
#include "RequestException.h"
#include "ScenarioTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "SentScenario.h"
#include "ServerModule.h"
#include "StopAreaTableSync.hpp"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "MessagesSection.hpp"

#include <boost/filesystem.hpp>

using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::filesystem;
using namespace geos::geom;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace departure_boards;
	using namespace geography;
	using namespace graph;
	using namespace impex;
	using namespace inter_synthese;
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
		const string IneoBDSIFileFormat::Importer_::PARAMETER_DB_CONN_STRING("conn_string");
		const string IneoBDSIFileFormat::Importer_::PARAMETER_MESSAGES_RECIPIENTS_DATASOURCE_ID = "mr_ds";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_PLANNED_DATASOURCE_ID = "th_ds";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_HYSTERESIS = "hysteresis";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_DELAY_BUS_STOP = "delay_bus_stop";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_DAY_BREAK_TIME = "day_break_time";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_CONVERT_STOP_CODE_TO_LOWER = "convert_stop_code_to_lower";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_MESSAGES_SECTION = "ms";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_SEND_DEACTIVATIONS_BY_INTERSYNTHESE = "send_deactivations_by_inter_synthese";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_AUTO_CREATE_STOPS = "auto_create_stops";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_DEFAULT_CITY_ID = "default_city_id";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_DEFAULT_TRANSFER_DURATION = "stop_area_default_transfer_duration";
		const string IneoBDSIFileFormat::Importer_::PARAMETER_NON_COMMERCIAL = "non_commercial";
		
		recursive_mutex IneoBDSIFileFormat::Importer_::_tabRunningBdsiMutex;
		set<RegistryKeyType> IneoBDSIFileFormat::Importer_::_runningBdsi;
		
		
		
		ParametersMap IneoBDSIFileFormat::Importer_::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}
		
		
		
		void IneoBDSIFileFormat::Importer_::_setFromParametersMap(const ParametersMap& map)
		{
			_dbConnString = map.getOptional<string>(PARAMETER_DB_CONN_STRING);
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

			// Messages section
			RegistryKeyType sectionId(map.getDefault<RegistryKeyType>(PARAMETER_MESSAGES_SECTION, 0));
			if(sectionId) try
			{
				_messagesSection = Env::GetOfficialEnv().get<MessagesSection>(sectionId);
			}
			catch(ObjectNotFoundException<MessagesSection>&)
			{
				throw RequestException("No such messages section");
			}

			// Hysteresis
			_hysteresis = seconds(
				map.getDefault<long>(PARAMETER_HYSTERESIS, 0)
			);

			// Delay for bus at stop
			_delay_bus_stop = seconds(
				map.getDefault<long>(PARAMETER_DELAY_BUS_STOP, 35)
			);

			// Day break time
			_dayBreakTime = duration_from_string(
				map.getDefault<string>(PARAMETER_DAY_BREAK_TIME, "03:00:00")
			);

			// Convert stop code to lower ?
			_stopCodeToLower = map.getDefault<bool>(PARAMETER_CONVERT_STOP_CODE_TO_LOWER, false);
			
			// Send service deactivations by inter_synthese
			_sendDeactivationsByInterSYNTHESE = map.getDefault<bool>(PARAMETER_SEND_DEACTIVATIONS_BY_INTERSYNTHESE, false);

			// Auto Create stops ?
			_autoCreateStops = map.getDefault<bool>(PARAMETER_AUTO_CREATE_STOPS, false);
			if (_autoCreateStops)
			{
				if (!map.isDefined(PARAMETER_DEFAULT_CITY_ID))
				{
					throw RequestException(PARAMETER_DEFAULT_CITY_ID + " is mandatory for stop auto creation.");
				}

				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_DEFAULT_CITY_ID), _env);
				_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_DEFAULT_TRANSFER_DURATION, 8));
			}

			// Eliminate non-commercial services
			_nonCommercial = map.getDefault<bool>(PARAMETER_NON_COMMERCIAL, false);
		}



		void IneoBDSIFileFormat::Importer_::_selectAndLoadCourse(
			Courses& courses,
			const Course::Horaires& horaires,
			const Chainage& chainage,
			const std::string& courseRef,
			const time_duration& nowDuration,
			Chainages& chainages
		) const {

			// Jump over courses with incomplete chainages
			if(horaires.size() > chainage.arretChns.size())
			{
				_logWarningDetail(
					"SERVICE",courseRef,courseRef,0,string(),string(), string(),"Bad horaire number compared to chainage arretchn number "
				);
				return;
			}
			else if (horaires.size() < chainage.arretChns.size())
			{
				// There is less horaire than arretchn so we create a new Chainage
				Chainage* newChainage;
				Chainage::ArretChns arretChns;
				// Loop on horaires to create arretChns
				std::size_t horaireCount(0);
				bool firstStopFound(false);
				BOOST_FOREACH(const Chainage::ArretChns::value_type& it, chainage.arretChns)
				{
					if (!firstStopFound && it.ref != horaires.at(0).arretchn)
					{
						continue;
					}
					else if (it.ref == horaires.at(0).arretchn)
					{
						firstStopFound = true;
					}
					ArretChn& arretChn(
						*arretChns.insert(
							arretChns.end(),
							ArretChn()
					)	);
					arretChn.ref = it.ref;
					arretChn.arret = it.arret;
					arretChn.pos = it.pos;
					arretChn.type = it.type;
					horaireCount++;
					if (horaireCount >= horaires.size())
					{
						break;
					}
				}
				
				newChainage = _createAndReturnChainage(chainages,arretChns,*(chainage.ligne),chainage.nom,chainage.sens,chainage.ref + "-" + lexical_cast<string>(horaires.size()));
				
				// Jump over dead runs
				BOOST_FOREACH(const Chainage::ArretChns::value_type& it, arretChns)
				{
					if(!it.arret->syntheseStop)
					{
						return;
					}
				}
				
				// OK let's store the service
				Course& course(
					courses.insert(
						make_pair(
							courseRef,
							Course()
					)	).first->second
				);
				course.ref = courseRef;
				course.horaires = horaires;
				course.chainage = newChainage;
				course.syntheseService = NULL;
				
				// Trace
				_logLoadDetail(
					"SERVICE",courseRef,courseRef,0,string(),string(), string(),"OK"
				);
			}
			else
			{
				// Jump over dead runs
				BOOST_FOREACH(const Chainage::ArretChns::value_type& it, chainage.arretChns)
				{
					if(!it.arret->syntheseStop)
					{
						return;
					}
				}
		
				// OK let's store the service
				Course& course(
					courses.insert(
						make_pair(
							courseRef,
							Course()
					)	).first->second
				);
				course.ref = courseRef;
				course.horaires = horaires;
				course.chainage = &chainage;
				course.syntheseService = NULL;

				// Trace
				_logLoadDetail(
					"SERVICE",courseRef,courseRef,0,string(),string(), string(),"OK"
				);
			}
		}



		void IneoBDSIFileFormat::Importer_::_selectAndLoadChainage(
			Chainages& chainages,
			const Chainage::ArretChns& arretchns,
			const Ligne& ligne,
			const std::string& nom,
			bool sens,
			const std::string& ref
		) const	{

			// Check if arretchns is long enough
			if(arretchns.size() < 2)
			{
				_logWarningDetail(
					"JOURNEYPATTERN",ref,nom,0,string(),string(), string(),arretchns.empty() ? "JOURNEYPATTERN HAS NO STOPS" : "JOURNEYPATTERN HAS ONLY ONE STOP"
				);
				return;
			}

			Chainage& chainage(
				chainages.insert(
					make_pair(
						ref,
						Chainage()
				)	).first->second
			);
			chainage.ref = ref;
			chainage.ligne = &ligne;
			chainage.nom = nom;
			chainage.sens = sens;
			chainage.arretChns = arretchns;
			_logLoadDetail(
				"JOURNEYPATTERN",ref,nom,0,string(),string(), string(),"OK"
			);

		}
		
		IneoBDSIFileFormat::Importer_::Chainage* IneoBDSIFileFormat::Importer_::_createAndReturnChainage(
			Chainages& chainages,
			const Chainage::ArretChns& arretchns,
			const Ligne& ligne,
			const std::string& nom,
			bool sens,
			const std::string& ref
		) const	{
		
			// Check if arretchns is long enough
			if(arretchns.size() < 2)
			{
				_logWarningDetail(
					"JOURNEYPATTERN",ref,nom,0,string(),string(), string(),arretchns.empty() ? "JOURNEYPATTERN HAS NO STOPS" : "JOURNEYPATTERN HAS ONLY ONE STOP"
				);
				return NULL;
			}
			
			Chainage& chainage(
				chainages.insert(
					make_pair(
						ref,
						Chainage()
				)	).first->second
			);
			chainage.ref = ref;
			chainage.ligne = &ligne;
			chainage.nom = nom;
			chainage.sens = sens;
			chainage.arretChns = arretchns;
			_logLoadDetail(
				"JOURNEYPATTERN",ref,nom,0,string(),string(), string(),"OK"
			);
			
			return &chainage;
		
		}
		
		
		
		bool IneoBDSIFileFormat::Importer_::_read(
		) const {

			boost::shared_ptr<DB> db;
			if(_dbConnString)
			{
				db = DBModule::GetDBForStandaloneUse(*_dbConnString);
			}
			else
			{
				db = DBModule::GetDBSPtr();
			}

			const time_duration dayBreakTime(hours(3));
			date today(day_clock::local_day());
			ptime now(second_clock::local_time());
			if(now.time_of_day() < dayBreakTime)
			{
				today -= days(1);
			}
			ptime nextDayBreak(now.date(), dayBreakTime);
			if(now.time_of_day() >= dayBreakTime)
			{
				nextDayBreak += hours(24);
			}

			boost::unique_lock<shared_mutex> lock(ServerModule::baseWriterMutex, boost::try_to_lock);
			if(!lock.owns_lock())
			{
				// If another BDSI import running, we can do this one if it is really another
				recursive_mutex::scoped_lock scoped_lock(_tabRunningBdsiMutex);
				if (_runningBdsi.empty())
				{
					// No BDSI import is running, another thread owns the baseWriterMutex so
					// we don't make import
					throw RequestException("IneoBDSIFileFormat: Another action forbids to run IneoBDSIFileFormat");
				}
				
				if (_runningBdsi.find(getImport().getKey()) != _runningBdsi.end())
				{
					// Another BDSI import is running and it is the same
					throw RequestException("IneoBDSIFileFormat: Already running for import " + getImport().get<Name>());
				}
				
				// Another BDSI import is running but it is not the same
				_runningBdsi.insert(getImport().getKey());
			}
			else
			{
				// We check that import is not running (it could be in the save method)
				recursive_mutex::scoped_lock scoped_lock(_tabRunningBdsiMutex);
				if (!_runningBdsi.empty())
				{
					if (_runningBdsi.find(getImport().getKey()) != _runningBdsi.end())
					{
						// Another BDSI import is running and it is the same
						throw RequestException("IneoBDSIFileFormat: Already running for import " + getImport().get<Name>());
					}
				}
				
				// Another BDSI import may be running but it is not the same
				_runningBdsi.insert(getImport().getKey());
			}
			

			//////////////////////////////////////////////////////////////////////////
			// Pre-loading objects from BDSI

			Courses courses;
			Lignes lignes;
			Arrets arrets;
			Chainages chainages;
			Programmations programmations;
			string todayStr("'"+ to_iso_extended_string(today) +"'");

			DataSource* dataSourceOnSharedEnv(
				Env::GetOfficialEnv().getEditable<DataSource>(_import.get<DataSource>()->getKey()).get()
			);
			bool saveStops = false;
			ImportableTableSync::ObjectBySource<StopPointTableSync> alreadyAutoGeneratedStopPoints(*dataSourceOnSharedEnv, Env::GetOfficialEnv());

			// Arrets
			{
				string query(
					"SELECT ref, mnemol, nom FROM "+ _database +".ARRET GROUP BY ref ORDER BY ref"
				);
				DBResultSPtr result(db->execQuery(query));
				while(result->next())
				{
					// Fields load
					string mnemol(result->get<string>("mnemol"));
					if (_stopCodeToLower)
					{
						boost::algorithm::to_lower(mnemol);
					}
					string name(result->get<string>("nom"));
					string ref(result->get<string>("ref"));

					// SYNTHESE stop point
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
							if (!_autoCreateStops)
							{
								_logWarningDetail(
									"STOP/DEPOT",mnemol,name,0,string(), string(),string(), "NOT FOUND"
								);
								continue;
							}
							else
							{
								// The stop may have already been autocreated
								if(alreadyAutoGeneratedStopPoints.contains(mnemol))
								{
									stopPoint = *(alreadyAutoGeneratedStopPoints.get(mnemol)).begin();
									_logLoadDetail(
										"STOP",mnemol,name,stopPoint->getKey(),stopPoint->getConnectionPlace()->getFullName() + "/" + stopPoint->getName(), string(), string(), "OK"
									);
								}
								else
								{
									// Auto creation of the stop
									bool tryToRecognizeStopPoint(false);
									if (mnemol.length() == 6)
									{
										int stopNumber = 0;
										tryToRecognizeStopPoint = true;
										try {
											stopNumber = lexical_cast<int>(mnemol.substr(4,2));
										}
										catch (...) {
											tryToRecognizeStopPoint = false;
										}
									}
									if (tryToRecognizeStopPoint)
									{
										// We look for a physical stop with the same 4 first characters
										const StopArea* stopAreaForCreation = NULL;
										DataSource::LinkedObjects existingStopPoints(
											_plannedDataSource->getLinkedObjects<StopPoint>()
										);
										BOOST_FOREACH(const DataSource::LinkedObjects::value_type& existingStopPoint, existingStopPoints)
										{
											StopPoint& stopPoint(static_cast<StopPoint&>(*existingStopPoint.second));
											BOOST_FOREACH(const std::string& code, stopPoint.getCodesBySource(*_plannedDataSource))
											{
												if (code.length() == 6 &&
													code.substr(0,4) == mnemol.substr(0,4))
												{
													// We use the stop area of this stop
													stopAreaForCreation = stopPoint.getConnectionPlace();
													break;
												}
											}
											if (stopAreaForCreation)
											{
												break;
											}
										}

										if (!stopAreaForCreation)
										{
											// We create a new stop area for this stop point
											StopArea* stopArea(
												new StopArea(
													StopAreaTableSync::getId(),
													true,
													_stopAreaDefaultTransferDuration
											)	);
											Importable::DataSourceLinks linksStopArea;
											linksStopArea.insert(make_pair(dataSourceOnSharedEnv, mnemol));
											stopArea->setDataSourceLinksWithoutRegistration(linksStopArea);
											stopArea->setCity(const_cast<City*>(_defaultCity.get()));
											stopArea->setName(name);
											_env.getEditableRegistry<StopArea>().add(boost::shared_ptr<StopArea>(stopArea));
											_logCreation(
												"Creation of the stop area with key "+ mnemol +" ("+ _defaultCity->getName() +" "+ name + ")"
											);

											stopPoint = new StopPoint(
												StopPointTableSync::getId(),
												name,
												stopArea,
												boost::shared_ptr<Point>(),
												false
											);
											Importable::DataSourceLinks linksStopPoint;
											linksStopPoint.insert(make_pair(dataSourceOnSharedEnv, mnemol));
											stopPoint->setDataSourceLinksWithoutRegistration(linksStopPoint);
											_env.getEditableRegistry<StopPoint>().add(boost::shared_ptr<StopPoint>(stopPoint));
											alreadyAutoGeneratedStopPoints.add(*stopPoint);

											// Log
											stringstream logStream;
											logStream << "Creation of the physical stop with key " << mnemol << " (" << name <<  ")";
											_logCreation(logStream.str());
										}
										else
										{
											stopPoint = new StopPoint(
												StopPointTableSync::getId(),
												name,
												stopAreaForCreation,
												boost::shared_ptr<Point>(),
												false
											);
											Importable::DataSourceLinks links;
											links.insert(make_pair(dataSourceOnSharedEnv, mnemol));
											stopPoint->setDataSourceLinksWithoutRegistration(links);
											_env.getEditableRegistry<StopPoint>().add(boost::shared_ptr<StopPoint>(stopPoint));
											alreadyAutoGeneratedStopPoints.add(*stopPoint);

											// Log
											stringstream logStream;
											logStream << "Creation of the physical stop with key " << mnemol << " (" << name <<  ")";
											_logCreation(logStream.str());
										}

										saveStops = true;
									}
									else
									{
										// We create a new stop area for this stop point
										StopArea* stopArea(
											new StopArea(
												StopAreaTableSync::getId(),
												true,
												_stopAreaDefaultTransferDuration
										)	);
										Importable::DataSourceLinks linksStopArea;
										linksStopArea.insert(make_pair(dataSourceOnSharedEnv, mnemol));
										stopArea->setDataSourceLinksWithoutRegistration(linksStopArea);
										stopArea->setCity(const_cast<City*>(_defaultCity.get()));
										stopArea->setName(name);
										_env.getEditableRegistry<StopArea>().add(boost::shared_ptr<StopArea>(stopArea));
										_logCreation(
											"Creation of the stop area with key "+ mnemol +" ("+ _defaultCity->getName() +" "+ name + ")"
										);

										stopPoint = new StopPoint(
											StopPointTableSync::getId(),
											name,
											stopArea,
											boost::shared_ptr<Point>(),
											false
										);
										Importable::DataSourceLinks linksStopPoint;
										linksStopPoint.insert(make_pair(dataSourceOnSharedEnv, mnemol));
										stopPoint->setDataSourceLinksWithoutRegistration(linksStopPoint);
										_env.getEditableRegistry<StopPoint>().add(boost::shared_ptr<StopPoint>(stopPoint));
										alreadyAutoGeneratedStopPoints.add(*stopPoint);

										// Log
										stringstream logStream;
										logStream << "Creation of the physical stop with key " << mnemol << " (" << name <<  ")";
										_logCreation(logStream.str());

										saveStops = true;
									}
								}
							}
					}	}

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
					"SELECT ref, mnemo FROM "+ _database +".LIGNE WHERE jour="+ todayStr
				);
				DBResultSPtr result(db->execQuery(query));
				while(result->next())
				{
					// Fields load
					string mnemo(result->get<string>("mnemo"));
					string ref(result->get<string>("ref"));

					// SYNTHESE line
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
			
			if(_dbConnString)
			{
				Log::GetInstance().debug("IneoBDSIFileFormat : on en a lu " + lexical_cast<string>(lignes.size()));
				Log::GetInstance().debug("IneoBDSIFileFormat : On lit les chainages dans la BDSI");
			}

			// Chainages
			{
				string chainageQuery(
					"SELECT "+
						_database +".ARRETCHN.ref,"+
						_database +".ARRETCHN.arret,"+
						_database +".ARRETCHN.pos,"+
						_database +".ARRETCHN.type,"+
						_database +".ARRETCHN.chainage,"+
						_database +".CHAINAGE.nom,"+
						_database +".CHAINAGE.sens,"+
						_database +".CHAINAGE.ligne "+
					" FROM "+
						_database +".ARRETCHN "+
						" INNER JOIN "+ _database +".CHAINAGE ON "+
							_database +".CHAINAGE.ref="+ _database +".ARRETCHN.chainage AND "+ _database +".CHAINAGE.jour="+ _database +".ARRETCHN.jour "+
					"WHERE "+
						_database +".CHAINAGE.jour="+ todayStr +
					" ORDER BY "+
						_database +".ARRETCHN.chainage, "+
						_database +".ARRETCHN.pos"
				);			
				DBResultSPtr chainageResult(db->execQuery(chainageQuery));
				string lastRef;
				const Ligne* ligne(NULL);
				Chainage::ArretChns arretChns;
				bool sens(false);
				string nom;
				while(chainageResult->next())
				{
					// Fields load
					string ref(chainageResult->getText("chainage"));

					// The chainage ref has changed : transform last collected data into a chainage if selected
					if(	ligne &&
						lastRef != ref
					){
						_selectAndLoadChainage(
							chainages,
							arretChns,
							*ligne,
							nom,
							sens,
							lastRef
						);
					}

					// Entering new chainage
					if(ref != lastRef)
					{
						string ligneRef(chainageResult->get<string>("ligne"));
						nom = chainageResult->getText("nom");

						// Check of the ligne
						Lignes::const_iterator it(
							lignes.find(ligneRef)
						);
						if(it == lignes.end())
						{
							ligne = NULL;
							_logWarningDetail(
								"JOURNEYPATTERN",ref,nom,0,string(),string(), string(),"LINE NOT FOUND"
							);
						}
						else
						{
							ligne = &it->second;
							sens = (chainageResult->getText("sens") == "R");
						}

						// Beginning load of the next ref
						lastRef = ref;
						arretChns.clear();
					}

					// Avoid useless work
					if(!ligne)
					{
						continue;
					}

					// Fields load
					string arretChnRef(chainageResult->get<string>("ref"));
					string arret(chainageResult->get<string>("arret"));

					// Check of arret
					Arrets::iterator itArret(arrets.find(arret));
					if(itArret == arrets.end())
					{
						_logWarningDetail(
							"STOPPOINT",arretChnRef,arretChnRef,0,string(),string(), string(),"Bad arret field in arretchn"
						);
						continue;
					}
					
					ArretChn& arretChn(
						*arretChns.insert(
							arretChns.end(),
							ArretChn()
					)	);
					arretChn.ref = arretChnRef;
					arretChn.arret = &itArret->second;
					arretChn.pos = chainageResult->getInt("pos");
					arretChn.type = chainageResult->getText("type");
					_logLoadDetail(
						"STOPPOINT",arretChnRef,arretChn.arret->nom,0,string(),string(), string(),"OK"
					);
				}
				// Load the last chainage
				if(	ligne
				){
					_selectAndLoadChainage(
						chainages,
						arretChns,
						*ligne,
						nom,
						sens,
						lastRef
					);
				}
			}

			// Courses
			{
				string horaireQuery(
					"SELECT "+
						_database +".HORAIRE.hra,"+
						_database +".HORAIRE.hrd,"+
						_database +".HORAIRE.hta,"+
						_database +".HORAIRE.htd,"+
						_database +".HORAIRE.etat_harr,"+
						_database +".HORAIRE.etat_hdep,"+
						_database +".HORAIRE.course,"+
						_database +".HORAIRE.arretchn,"+
						_database +".ARRETCHN.chainage "+
					"FROM "+
						_database +".HORAIRE "+
						"INNER JOIN "+ _database +".ARRETCHN ON "+
							_database +".HORAIRE.arretchn="+ _database +".ARRETCHN.ref AND "+ _database +".HORAIRE.jour="+ _database +".ARRETCHN.jour "+
						"LEFT JOIN "+ _database +".COURSE ON "+
							_database +".COURSE.ref="+ _database +".HORAIRE.course AND "+ _database +".COURSE.jour="+ _database +".ARRETCHN.jour "+
					"WHERE "+
						_database +".HORAIRE.jour="+ todayStr +
					( _nonCommercial ? " AND "+ _database +".COURSE.type != 'H'" : "" ) +
					" ORDER BY "+
						_database +".HORAIRE.course, "+
						_database +".ARRETCHN.pos"
				);
				DBResultSPtr horaireResult(db->execQuery(horaireQuery));
				time_duration nowDuration(now.time_of_day() < _dayBreakTime ? now.time_of_day() + hours(24) : now.time_of_day());
				time_duration nowPlusDelay(nowDuration + _delay_bus_stop);
				string lastCourseRef;
				Course::Horaires horaires;
				const Chainage* chainage(NULL);
				while(horaireResult->next())
				{
					string courseRef(horaireResult->get<string>("course"));

					// The course ref has changed : transform last collected data into a course if selected
					if(	chainage &&
						courseRef != lastCourseRef
					){
						_selectAndLoadCourse(
							courses,
							horaires,
							*chainage,
							lastCourseRef,
							nowDuration,
							chainages
						);
					}

					// Entering new course : check if the chainage exists
					if(courseRef != lastCourseRef)
					{
						// Check of the chainage
						Chainages::const_iterator itChainage(
							chainages.find(
								horaireResult->get<string>("chainage")
						)	);
						if(itChainage == chainages.end())
						{
							chainage = NULL;
							_logWarningDetail(
								"SERVICE",courseRef,courseRef,0,string(),string(), string(),"Bad chainage field in course "
							);
						}
						else
						{
							chainage = &itChainage->second;
						}

						// Now entering in the new course
						lastCourseRef = courseRef;
						horaires.clear();
					}

					// Avoid useless work
					if(!chainage)
					{
						continue;
					}

					// New horaire
					Horaire& horaire(
						*horaires.insert(
							horaires.end(),
							Horaire()
					)	);

					// Fields load
					horaire.hrd = duration_from_string(horaireResult->getText("hrd"));
					horaire.hra = duration_from_string(horaireResult->getText("hra"));
					horaire.htd = duration_from_string(horaireResult->getText("htd"));
					horaire.hta = duration_from_string(horaireResult->getText("hta"));
					horaire.arretchn = horaireResult->getText("arretchn");

					// Patch for schedules after midnight
					if(horaire.hrd < dayBreakTime)
					{
						horaire.hrd += hours(24);
					}
					if(horaire.hra < dayBreakTime)
					{
						horaire.hra += hours(24);
					}
					if(horaire.htd < dayBreakTime)
					{
						horaire.htd += hours(24);
					}
					if(horaire.hta < dayBreakTime)
					{
						horaire.hta += hours(24);
					}

					// Patch for bad schedules when the bus is at stop
					if(	(	(	horaireResult->getText("etat_harr") == "R" &&
								horaireResult->getText("etat_hdep") == "E"
							) || (
								horaire.hrd > nowDuration
							)
						) &&
						horaire.hrd <= nowPlusDelay
					){
						horaire.hrd = nowPlusDelay;
					}

					// Trace
					_logTraceDetail(
						"SCHEDULE_HTD",courseRef,courseRef,0,string(),horaireResult->getText("htd"),to_simple_string(horaire.htd),"OK"
					);
					_logTraceDetail(
						"SCHEDULE_HTA",courseRef,courseRef,0,string(),horaireResult->getText("hta"),to_simple_string(horaire.hta),"OK"
					);
					_logTraceDetail(
						"SCHEDULE_HRD",courseRef,courseRef,0,string(),horaireResult->getText("hrd"),to_simple_string(horaire.hrd),"OK"
					);
					_logTraceDetail(
						"SCHEDULE_HRA",courseRef,courseRef,0,string(),horaireResult->getText("hra"),to_simple_string(horaire.hra),"OK"
					);
				}

				// Load lastly collected data
				if(chainage)
				{
					_selectAndLoadCourse(
						courses,
						horaires,
						*chainage,
						lastCourseRef,
						nowDuration,
						chainages
					);
				}
			}

			// Programmations
			{
				string query(
					"SELECT * FROM "+ _database +".PROGRAMMATION WHERE nature_dst LIKE 'BORNE%'"
				);
				string destQuery(
					"SELECT * FROM "+ _database +".DESTINATAIRE WHERE ref_prog IN (SELECT "+
					_database +".PROGRAMMATION.ref FROM "+ _database +".PROGRAMMATION WHERE nature_dst LIKE 'BORNE%') ORDER BY ref_prog"
				);
				DBResultSPtr result(db->execQuery(query));
				DBResultSPtr destResult(db->execQuery(destQuery));
				bool readDestResult(false);
				if (destResult->next())
				{
					readDestResult = true;
				}
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

							if (!destResult->next())
							{
								readDestResult = false;
							}
							continue;
						}

						programmation.destinataires.insert(
							programmation.destinataires.end(),
							dest
						);

						if (!destResult->next())
						{
							readDestResult = false;
						}

					} while(readDestResult);
				}
			}


			//////////////////////////////////////////////////////////////////////////
			// Import content analyzing

			{ // Scenarios and messages
				// Scenarios
				DataSource::LinkedObjects existingScenarios(
					dataSourceOnSharedEnv->getLinkedObjects<Scenario>()
				);
				BOOST_FOREACH(const DataSource::LinkedObjects::value_type& existingScenario, existingScenarios)
				{
					_scenariosToRemove.insert(existingScenario.second->getKey());
				}

				// Loop on objects present in the database (search for creations and updates)
				BOOST_FOREACH(const Programmations::value_type& itProg, programmations)
				{
					const Programmation& programmation(itProg.second);

					boost::shared_ptr<SentScenario> updatedScenario;
					boost::shared_ptr<Alarm> updatedMessage;
					SentScenario* scenario(
						static_cast<SentScenario*>(
							dataSourceOnSharedEnv->getObjectByCode<Scenario>(
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

						if (_messagesSection.get())
						{
							updatedScenario->addSection(*_messagesSection.get());
						}

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
						if (_messagesSection)
						{
							updatedMessage->setSection(_messagesSection.get());
						}
					}
					if(updatedScenario.get())
					{
						updatedScenario->setName(programmation.title);
						updatedScenario->setPeriodStart(programmation.startTime);
						updatedScenario->setPeriodEnd(programmation.endTime);
						updatedScenario->setIsEnabled(programmation.active);
						if (_messagesSection.get())
						{
							updatedScenario->addSection(*_messagesSection.get());
						}
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
				typedef set<ScheduledService*> ServicesSet;
				typedef vector<const Course*> CoursesVector;
				ServicesSet servicesToRemove; // Services running this day and canceled
				ServicesSet servicesToUnlink; // Services linked to this datasource to unlink
				ServicesSet servicesToMove;
				CoursesVector servicesToLink;
				CoursesVector servicesToUpdate; // Update of the schedules must be done on these services
			
				

				// Existing services coming from theoretical data
				DataSource::LinkedObjects existingJourneyPatterns(
					_plannedDataSource->getLinkedObjects<JourneyPattern>()
				);
				BOOST_FOREACH(const DataSource::LinkedObjects::value_type& existingJourneyPattern, existingJourneyPatterns)
				{
					JourneyPattern& journeyPattern(static_cast<JourneyPattern&>(*existingJourneyPattern.second));
					
					boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
						*(journeyPattern.sharedServicesMutex)
					);

					BOOST_FOREACH(const ServiceSet::value_type& existingService, journeyPattern.getAllServices())
					{
						// Jump over continuous services
						ScheduledService* service(dynamic_cast<ScheduledService*>(existingService));
						if(!service)
						{
							continue;
						}

						// Jump over non active services
						if(!service->isActive(today))
						{
							continue;
						}

						// Jump over non imported services
						if(service->getNextRTUpdate() > nextDayBreak)
						{
							continue;
						}

						servicesToRemove.insert(service);
					}
				}

				// Existing services coming from last real time imports
				DataSource::LinkedObjects existingServices(
					dataSourceOnSharedEnv->getLinkedObjects<ScheduledService>()
				);
				BOOST_FOREACH(const DataSource::LinkedObjects::value_type& existingService, existingServices)
				{
					ScheduledService* service(dynamic_cast<ScheduledService*>(existingService.second));
					if(!service)
					{
						continue;
					}

					servicesToUnlink.insert(service);

					// Jump over non imported services
					if(service->getNextRTUpdate() > nextDayBreak)
					{
						continue;
					}

					// Jump over non active services
					if(!service->isActive(today))
					{
						continue;
					}

					servicesToRemove.insert(service); // Should already be present if not created by this import
				}


				// Search for existing service with same key
				BOOST_FOREACH(const Courses::value_type& itCourse, courses)
				{
					const Course& course(itCourse.second);

					// Known ref ?
					ScheduledService* service(
						dataSourceOnSharedEnv->getObjectByCode<ScheduledService>(course.ref)
					);
					if(!service)
					{
						if(_dbConnString)
						{
							Log::GetInstance().debug("IneoBDSIFileFormat : on ne trouve pas la course : " + course.ref);
						}
						continue;
					}
						
					// Checks if the service and the course are matching
					if(course != *service)
					{
						if(_dbConnString)
						{
							Log::GetInstance().debug("IneoBDSIFileFormat : on a trouvé une course qui ne match pas : " + course.ref);
						}
						continue;
					}

					// OK the service is sent to simple update
					course.syntheseService = service;

					// This service must not be unlinked
					servicesToUnlink.erase(service);

					// This service must be updated
					servicesToUpdate.push_back(&course);

					// This service must not be removed
					servicesToRemove.erase(service);

					// The service can be deactivated
					if (!service->isActive(today))
					{
						service->setActive(today);
						servicesToLink.push_back(&course);
					}
				}

				// Search for existing services
				size_t createdServices(0);
				BOOST_FOREACH(const Courses::value_type& itCourse, courses)
				{
					const Course& course(itCourse.second);

					// Jump over already joined services
					if( course.syntheseService
					){
						continue;
					}

					// Search for a service with the same planned schedules
					ScheduledService* inactiveService(NULL); // to remember an inactive service available for this course (if no other)
					BOOST_FOREACH(
						const JourneyPattern* route,
						course.chainage->getSYNTHESEJourneyPatterns(
							*_plannedDataSource,
							*dataSourceOnSharedEnv,
							_env
						)
					){
						boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
							*route->sharedServicesMutex
						);
						BOOST_FOREACH(Service* sservice, route->getAllServices())
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

								// The service must be updated
								servicesToUpdate.push_back(&course);

								// The service must not be removed
								servicesToRemove.erase(service);

								// The service must be linked
								if(!service->hasCodeBySource(*dataSourceOnSharedEnv, course.ref))
								{
									servicesToLink.push_back(&course);
								}
								break;
							}
							else if ( course == *service )
							{
								inactiveService = service;
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
					
					// Maybe an inactive service has been found and can be activated
					if (inactiveService)
					{
						inactiveService->setActive(today);
						course.syntheseService = inactiveService;
						
						// The service must be updated
						servicesToUpdate.push_back(&course);
						
						// The service must not be removed
						servicesToRemove.erase(inactiveService);
						
						// The service must be linked
						servicesToLink.push_back(&course);
						continue;
					}

					// No existing service has been found : creation of a new service
					course.createService(today, _env);

					// Log
					++createdServices;
					_logDebugDetail(
						"SERVICE CREATION",
						course.ref,
						string(),
						course.syntheseService->getKey(),
						course.syntheseService->getServiceNumber(),
						string(),
						to_simple_string(course.horaires.at(0).htd),
						string()
					);

					// This service must be linked
					servicesToLink.push_back(&course);

					// This service must be updated
					servicesToUpdate.push_back(&course);
				}

				// Loop on services to unlink
				BOOST_FOREACH(ScheduledService* service, servicesToUnlink)
				{
					string oldCode;
					try
					{
						if(service->hasLinkWithSource(*dataSourceOnSharedEnv))
						{
							oldCode = service->getACodeBySource(*dataSourceOnSharedEnv);
						}
						Importable::DataSourceLinks links(service->getDataSourceLinks());
						service->removeSourceLinks(*dataSourceOnSharedEnv, true);
						links.erase(dataSourceOnSharedEnv);
						service->setDataSourceLinksWithoutRegistration(links);
						_servicesToSave.insert(service);

						_logDebugDetail(
							"SERVICE UNLINK",
							oldCode,
							string(),
							service->getKey(),
							service->getServiceNumber(),
							string(),
							string(),
							string()
						);
					}
					catch(...)
					{
						assert(false);
					}
				}

				// Loop on services to update
				size_t updated(0);
				BOOST_FOREACH(const Course* course, servicesToUpdate)
				{
					try
					{
						Course::UpdateDeltas result(
							course->updateService(getHysteresis())
						);
						if(result)
						{
							++updated;
							_logDebugDetail(
								"REAL-TIME SCHEDULES UPDATE",
								course->ref,
								course->chainage->ligne->ref+"/"+course->chainage->ref,
								course->syntheseService->getKey(),
								course->syntheseService->getServiceNumber(),
								string(),
								lexical_cast<string>(result->first)+"s/"+lexical_cast<string>(result->second)+"s",
								string()
							);
						}
					}
					catch (...)
					{
						assert(false);
					}
				}


				// Loop on services to link
				BOOST_FOREACH(const Course* course, servicesToLink)
				{
					try
					{
						Importable::DataSourceLinks links(course->syntheseService->getDataSourceLinks());
						links.insert(make_pair(dataSourceOnSharedEnv, course->ref));
						course->syntheseService->setDataSourceLinksWithRegistration(links);
						_servicesToSave.insert(course->syntheseService);

						_logDebugDetail(
							"SERVICE LINK",
							course->ref,
							string(),
							course->syntheseService->getKey(),
							course->syntheseService->getServiceNumber(),
							string(),
							string(),
							string()
						);
					}
					catch (...)
					{
						assert(false);
					}
				}

				// Remove services from today
				BOOST_FOREACH(ScheduledService* service, servicesToRemove)
				{
					try
					{
						service->setInactive(today);
						_servicesToSave.insert(service);

						_logDebugDetail(
							"SERVICE DEACTIVATION",
							string(),
							string(),
							service->getKey(),
							service->getServiceNumber(),
							string(),
							string(),
							string()
						);
						
						// Deactivation Inter-SYNTHESE sync
						if(_sendDeactivationsByInterSYNTHESE)
						{
							util::Log::GetInstance().debug("Désactivation de la course " + lexical_cast<string>(service->getKey()));
							DeactivationPTDataInterSYNTHESE::Content content(*service);
							inter_synthese::InterSYNTHESEModule::Enqueue(
								content,
								boost::optional<db::DBTransaction&>(),
								service
							);
						}
					}
					catch (...)
					{
						assert(false);
					}
				}

				// Log
				_logInfo("Courses attachées : "+ lexical_cast<string>(servicesToLink.size()));
				_logInfo("Courses détachées : "+ lexical_cast<string>(servicesToUnlink.size()));
				_logInfo("Courses avec mise à jour des horaires temps réel : "+ lexical_cast<string>(updated));
				_logInfo("Courses sans mise à jour des horaires temps réel : "+ lexical_cast<string>(servicesToUpdate.size() - updated));
				_logInfo("Courses créées : "+ lexical_cast<string>(createdServices));
				_logInfo("Courses supprimées : "+ lexical_cast<string>(servicesToRemove.size()));
			}

			// Release lock
			/*{
				recursive_mutex::scoped_lock scoped_lock(_tabRunningBdsiMutex);
				
				// Release lock
				_runningBdsi.erase(getImport().getKey());
			}*/
			// We do not release lock, we keep it for the _saveNow method

			_saveNow(saveStops).run();

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
			DatabaseReadImporter<IneoBDSIFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			_hysteresis(seconds(0)),
			_stopCodeToLower(false),
			_autoCreateStops(false),
			_nonCommercial(false)
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



		void IneoBDSIFileFormat::Importer_::_logTraceDetail(
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
			_logTrace(content.str());
		}



		DBTransaction IneoBDSIFileFormat::Importer_::_save() const
		{
			return DBTransaction();
		}

		DBTransaction IneoBDSIFileFormat::Importer_::_saveNow(bool saveStops) const
		{
			DBTransaction transaction;
			// We check that import is not running
			/*{
				recursive_mutex::scoped_lock scoped_lock(_tabRunningBdsiMutex);
				if (!_runningBdsi.empty())
				{
					if (_runningBdsi.find(getImport().getKey()) != _runningBdsi.end())
					{
						// Another BDSI import is running and it is the same
						throw RequestException("IneoBDSIFileFormat: Already running");
					}
				}
				
				// Another BDSI import may be running but it is not the same
				_runningBdsi.insert(getImport().getKey());
			}*/
			// We do not check for the lock, we know we have it as the only caller for _saveNow is read method
			
			boost::shared_lock<boost::shared_mutex> lockVDV(ServerModule::IneoBDSIAgainstVDVMutex);

			// Created stops
			if (saveStops)
			{
				BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
				{
					StopAreaTableSync::Save(cstop.second.get(), transaction);
				}
				BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
				{
					StopPointTableSync::Save(stop.second.get(), transaction);
				}
			}

			// Created journey patterns
			BOOST_FOREACH(const JourneyPattern::Registry::value_type& journeyPattern, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(journeyPattern.second.get(), transaction);
			}

			// Created line stops
			BOOST_FOREACH(const LineStop::Registry::value_type& lineStop, _env.getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}

			// Created services
			BOOST_FOREACH(const ScheduledService::Registry::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
				_servicesToSave.erase(service.second.get()); // Avoid double save
			}

			// Updated services
			BOOST_FOREACH(ScheduledService* service, _servicesToSave)
			{
				ScheduledServiceTableSync::Save(service, transaction);
			}

			// Scenarios
			BOOST_FOREACH(const Scenario::Registry::value_type& scenario, _env.getRegistry<Scenario>())
			{
				ScenarioTableSync::Save(scenario.second.get(), transaction);
			}

			// Messages
			BOOST_FOREACH(const Alarm::Registry::value_type& alarm, _env.getRegistry<Alarm>())
			{
				AlarmTableSync::Save(alarm.second.get(), transaction);
			}

			// Message links
			BOOST_FOREACH(const AlarmObjectLink::Registry::value_type& aol, _env.getRegistry<AlarmObjectLink>())
			{
				AlarmObjectLinkTableSync::Save(aol.second.get(), transaction);
			}

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

			// Output of SQL queries in trace mode
			if (_minLogLevel < IMPORT_LOG_DEBG)
			{
				BOOST_FOREACH(const DBTransaction::ModifiedRows::value_type& query, transaction.getUpdatedRows())
				{
					_logTrace(query.first +" "+ lexical_cast<string>(query.second));
				}
			}
			
			// Release lock
			{
				recursive_mutex::scoped_lock scoped_lock(_tabRunningBdsiMutex);
				
				// Another BDSI import is running but it is not the same
				_runningBdsi.erase(getImport().getKey());
			}

			return transaction;
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
					dynamic_cast<const StopArea*>(chainage->arretChns[i].arret->syntheseStop->getHub())->getKey() != dynamic_cast<const StopArea*>(jp.getEdge(i)->getFromVertex()->getHub())->getKey())
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
				
				boost::posix_time::time_duration htaToCompare = horaires[i].hta;
				if (htaToCompare.seconds())
				{
					htaToCompare += seconds(60 - htaToCompare.seconds());
				}
				boost::posix_time::time_duration htdToCompare = horaires[i].htd;
				if (htdToCompare.seconds())
				{
					htdToCompare -= seconds(htdToCompare.seconds());
				}

				if(	(	edge.isArrivalAllowed() &&
					(	htaToCompare.hours() != service.getArrivalSchedule(false, i).hours() ||
						htaToCompare.minutes() != service.getArrivalSchedule(false, i).minutes()
				)	) || (
					edge.isDepartureAllowed() &&
					(	htdToCompare.hours() != service.getDepartureSchedule(false, i).hours() ||
						htdToCompare.minutes() != service.getDepartureSchedule(false, i).minutes()
				)	)	){
					return false;
				}
			}

			// OK the service matches
			return true;
		}




		/// @return true if the update has been done
		IneoBDSIFileFormat::Importer_::Course::UpdateDeltas IneoBDSIFileFormat::Importer_::Course::updateService(
			const boost::posix_time::time_duration& hysteresis
		) const	{

			// Update of the real time schedules
			SchedulesBasedService::Schedules departureSchedules(
				syntheseService->getDepartureSchedules(true, true)
			);
			SchedulesBasedService::Schedules arrivalSchedules(
				syntheseService->getArrivalSchedules(true, true)
			);
			bool updated(false);
			time_duration maxDelta(seconds(0));
			time_duration minDelta(seconds(0));
			for(size_t i(0); i<horaires.size(); ++i)
			{
				time_duration delta(departureSchedules[i] - horaires[i].hrd);
				if(delta<minDelta)
				{
					minDelta = delta;
				}
				if(delta>maxDelta)
				{
					maxDelta = delta;
				}
				if(delta < -hysteresis || delta > hysteresis)
				{
					updated = true;
					break;
				}
				delta = arrivalSchedules[i] - horaires[i].hra;
				if(delta<minDelta)
				{
					minDelta = delta;
				}
				if(delta>maxDelta)
				{
					maxDelta = delta;
				}
				if(delta < -hysteresis || delta > hysteresis)
				{
					updated = true;
					break;
				}
			}
			if(!updated && !syntheseService->hasRealTimeData())
			{
				// No update is needed but we do it anyway so that RT schedule will be set
				syntheseService->setRealTimeSchedules(departureSchedules, arrivalSchedules);
			}
			else if(!updated)
			{
				return UpdateDeltas();
			}
			for(size_t i(0); i<chainage->arretChns.size(); ++i)
			{
				if(chainage->arretChns[i].arret->syntheseStop->getKey() != 
					(syntheseService->getRealTimeVertex(i) ? syntheseService->getRealTimeVertex(i)->getKey() : syntheseService->getVertex(i)->getKey())
				) {
					syntheseService->setRealTimeVertex(i, chainage->arretChns[i].arret->syntheseStop);
				}
			}
			
			// if course is ended, don't update it because it may have been cleaned by RTDataCleaner
			// and we do not want update the course for tomorrow
			boost::posix_time::ptime now(boost::posix_time::second_clock::local_time());
			if(now.time_of_day() > syntheseService->GetTimeOfDay(horaires[horaires.size()-1].hrd) + minutes(1))
			{
				return UpdateDeltas();
			}

			for(size_t i(0); i<horaires.size(); ++i)
			{
				departureSchedules[i] = horaires[i].hrd;
				arrivalSchedules[i] = horaires[i].hra;
			}
			syntheseService->setRealTimeSchedules(departureSchedules, arrivalSchedules);
			return make_pair(minDelta, maxDelta);
		}



		bool IneoBDSIFileFormat::Importer_::Course::operator!=( const pt::ScheduledService& op ) const
		{
			return !operator==(op);
		}



		void IneoBDSIFileFormat::Importer_::Course::createService(
			const date& today,
			util::Env& temporaryEnvironment
		) const	{
			const JourneyPattern* route(
				*chainage->syntheseJourneyPatterns.begin()
			);

			// Service creation
			boost::shared_ptr<ScheduledService> service(
				new ScheduledService(
					ScheduledServiceTableSync::getId(),
					string(),
					const_cast<JourneyPattern*>(route)
			)	);

			// Properties
			service->setPath(const_cast<JourneyPattern*>(route));
			service->setActive(today);

			// Theoretical schedules
			SchedulesBasedService::Schedules departureSchedules;
			SchedulesBasedService::Schedules arrivalSchedules;
			for(size_t i(0); i<horaires.size(); ++i)
			{
				if(!static_cast<const DesignatedLinePhysicalStop*>(route->getEdge(i))->getScheduleInput())
				{
					continue;
				}

				if (horaires[i].htd.seconds()) {
					departureSchedules.push_back(horaires[i].htd - seconds(horaires[i].htd.seconds()));
				}
				else {
					departureSchedules.push_back(horaires[i].htd);
				}
				if (horaires[i].hta.seconds()) {
					arrivalSchedules.push_back(horaires[i].hta + seconds(60 - horaires[i].hta.seconds()));
				}
				else {
					arrivalSchedules.push_back(horaires[i].hta);
				}
			}
			service->setDataSchedules(departureSchedules, arrivalSchedules);

			const_cast<JourneyPattern*>(route)->addService(*service, false);

			// Registration of the service in the temporary environment
			temporaryEnvironment.getEditableRegistry<ScheduledService>().add(service);
			// ...and in the official because there is link with dataSource in the official env
			// and service should not be deleted with temporaryEnvironment
			Env::GetOfficialEnv().getEditableRegistry<ScheduledService>().add(service);

			// Registration of the service in the course
			syntheseService = service.get();
			
			const_cast<JourneyPattern*>(route)->addService(*service.get(), false);
		}



		//////////////////////////////////////////////////////////////////////////
		/// Get corresponding journey patterns from SYNTHESE, compared by value.
		/// If no existing journey pattern was found, a new one is created in the temporary environment
		const IneoBDSIFileFormat::Importer_::Chainage::SYNTHESEJourneyPatterns& IneoBDSIFileFormat::Importer_::Chainage::getSYNTHESEJourneyPatterns(
			const DataSource& theoreticalDataSource,
			const DataSource& realTimeDataSource,
			Env& temporaryEnvironment
		) const	{
			// If the cache is empty, search or create journey patterns
			if(syntheseJourneyPatterns.empty())
			{
				// Search existing journey patterns on the line of the chainage
				BOOST_FOREACH(Path* path, ligne->syntheseLine->getPaths())
				{
					// Jump over Free DRT areas
					if(!dynamic_cast<JourneyPattern*>(path)
					){
						continue;
					}

					const JourneyPattern& jp(*static_cast<JourneyPattern*>(path));

					// Jump over non imported journey patterns
					if(	!jp.hasLinkWithSource(theoreticalDataSource) &&
						!jp.hasLinkWithSource(realTimeDataSource)
					){
						continue;
					}

					// Check the direction and the stops size
					if(	jp.getWayBack() != sens ||
						jp.getEdges().size() != arretChns.size()
					){
						continue;
					}

					// Stops comparison
					bool ok(true);
					for(size_t i(0); i<arretChns.size(); ++i)
					{
						if(dynamic_cast<const StopArea*>(jp.getEdge(i)->getFromVertex()->getHub())->getKey() != dynamic_cast<const StopArea*>(arretChns[i].arret->syntheseStop->getHub())->getKey())
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
					syntheseJourneyPatterns.push_back(&jp);
				}
			}

			// If no journey pattern was found, creation of a new journey pattern
			if(syntheseJourneyPatterns.empty())
			{
				// Object creation
				boost::shared_ptr<JourneyPattern> jp(
					new JourneyPattern(JourneyPatternTableSync::getId())
				);

				// Properties
				jp->setCommercialLine(ligne->syntheseLine);
				jp->setWayBack(sens);
				jp->setName(nom);
				jp->addCodeBySource(realTimeDataSource, ref);
				ligne->syntheseLine->addPath(jp.get());
				
				Log::GetInstance().debug("IneoBDSIFileFormat : Creation of JourneyPattern " + ref);

				// Stops
				size_t rank(0);
				BOOST_FOREACH(const ArretChns::value_type& arretChn, arretChns)
				{
					// Line stop creation
					boost::shared_ptr<LineStop> ls(
						new LineStop(
							LineStopTableSync::getId(),
							jp.get(),
							rank,
							rank+1 < arretChns.size(),
							rank > 0,
							0,
							*arretChn.arret->syntheseStop
					)	);
					ls->set<ScheduleInput>(arretChn.type != "N" || rank == 0); // Rank 0 should always be a scheduled stop

					// registration of the line stop into the journey pattern
					ls->link(temporaryEnvironment, true);

					// Add the edge to the vertex
/*						if (rank+1 < course.chainage->arretChns.size())
					{
						(arretChn.arret->syntheseStop)->addDepartureEdge(ls.get());
					}
					if (rank > 0)
					{
						(arretChn.arret->syntheseStop)->addArrivalEdge(ls.get());
					}
*/
					// Registration of the line stop into the temporary environment, to be saved after the import
					temporaryEnvironment.getEditableRegistry<LineStop>().add(ls);
					// ...and in the official because there is link with dataSource in the official env
					// and ls should not be deleted with temporaryEnvironment
					Env::GetOfficialEnv().getEditableRegistry<LineStop>().add(ls);
					++rank;
				}

				// Registration of the journey pattern in the temporary environment, to be saved after the import
				temporaryEnvironment.getEditableRegistry<JourneyPattern>().add(jp);
				// ...and in the official because there is link with dataSource in the official env
				// and jp should not be deleted with temporaryEnvironment
				Env::GetOfficialEnv().getEditableRegistry<JourneyPattern>().add(jp);

				// Registration of the created journey pattern in the cache
				syntheseJourneyPatterns.push_back(jp.get());
			}

			return syntheseJourneyPatterns;
			
		}
}	}

