
/** ObitiFileFormat class implementation.
	@file ObitiFileFormat.cpp

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

#include "ObitiFileFormat.hpp"
#include "Importer.hpp"
#include "PTFileFormat.hpp"
#include "ImpExModule.h"
#include "IConv.hpp"
#include "HTMLForm.h"
#include "PropertiesHTMLTable.h"
#include "DataSource.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "City.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "RollingStock.hpp"
#include "ServiceCalendarLink.hpp"
#include "StopArea.hpp"
#include "TransportNetwork.h"
#include "CalendarTemplateTableSync.h"
#include "CityTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "PTUseRuleTableSync.h"
#include "RollingStockTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "ServiceCalendarLinkTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/filesystem/operations.hpp>
#include <geos/geom/Geometry.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace calendar;
	using namespace graph;
	using namespace html;
	using namespace admin;
	using namespace server;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,ObitiFileFormat>::FACTORY_KEY("Obiti");
	}

	namespace pt
	{
		const std::string ObitiFileFormat::Importer_::FILE_ARRET("arrets");
		const std::string ObitiFileFormat::Importer_::FILE_ITINERAIRES("itineraires");
		const std::string ObitiFileFormat::Importer_::FILE_LIGNE("ligne");

		const std::string ObitiFileFormat::Importer_::PATH_HORAIRES("horaires");

		const std::string ObitiFileFormat::Importer_::SEP(";");

		const std::string ObitiFileFormat::Importer_::PARAMETER_ROLLING_STOCK_ID("rolling_stock_id");
		const std::string ObitiFileFormat::Importer_::PARAMETER_IMPORT_STOP_AREA("isa");
		const std::string ObitiFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string ObitiFileFormat::Importer_::PARAMETER_USE_RULE_BLOCK_ID_MASK("use_rule_block_id_mask");
		const std::string ObitiFileFormat::Importer_::PARAMETER_STOPS_DATA_SOURCE_ID("stops_data_source_id");
		const std::string ObitiFileFormat::Importer_::PARAMETER_STOPS_FROM_DATA_SOURCE("stops_from_data_source");

		const std::string ObitiFileFormat::Importer_::PARAMETER_PERIOD_CALENDAR_FIELD("period_calendar_field");
		const std::string ObitiFileFormat::Importer_::PARAMETER_DAYS_CALENDAR_FIELD("days_calendar_field");
		const std::string ObitiFileFormat::Importer_::PARAMETER_NUMBER_OF_OTHER_PARAMETERS("nb_other_parameters");
		const std::string ObitiFileFormat::Importer_::PARAMETER_BACKWARD_IN_SAME_FILE("backward_in_same_file");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<ObitiFileFormat>::Files MultipleFileTypesImporter<ObitiFileFormat>::FILES(
			ObitiFileFormat::Importer_::FILE_LIGNE.c_str(),
			ObitiFileFormat::Importer_::FILE_ARRET.c_str(),
			ObitiFileFormat::Importer_::FILE_ITINERAIRES.c_str(),
			ObitiFileFormat::Importer_::PATH_HORAIRES.c_str(),
		"");
	}


	namespace pt
	{
		bool ObitiFileFormat::Importer_::_checkPathsMap() const
		{
			/*
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_LIGNE));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it= _pathsMap.find(FILE_ARRET);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_ITINERAIRES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			*/
			return true;
		}



		ObitiFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	MultipleFileTypesImporter<ObitiFileFormat>(env, dataSource),
			PTDataCleanerFileFormat(env, dataSource),
			Importer(env, dataSource),
			_interactive(true),
			_lines(_dataSource, env),
			_stopAreas(_dataSource, env),
			_stopPoints(_dataSource, env)
		{}



		bool ObitiFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> request
		) const {
			ifstream inFile;
			string line;

			if(key != PATH_HORAIRES)
			{
				inFile.open(filePath.file_string().c_str());
				if(!inFile)
				{
					throw Exception("Could no open the file " + filePath.file_string());
				}

				if(!getline(inFile, line))
				{
					return false;
				}
				_loadFieldsMap(line);
				stream << "INFO : Loading file " << filePath << " as " << key << "<br />";
			}
			else
				stream << "INFO : Loading path " << filePath << " as " << key << "<br />";


			// 1 : lines
			if(key == FILE_LIGNE)
			{
				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);

					LineObitiElement lineObiti;

					string idLigne = _getValue("IDLigne");
					lineObiti.name = _getValue("Nom");
					lineObiti.shortname = _getValue("Code Commerciale");
					lineObiti.backward = _getValue("Sens") == "Retour" ? true : false;
					_linesMap.insert(make_pair(idLigne, lineObiti));
				}
			}

			// 2 : Stop Areas
			if(key == FILE_ARRET)
			{
				PTFileFormat::ImportableStopAreas linkedStopAreas;
				PTFileFormat::ImportableStopAreas nonLinkedStopAreas;
				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);

					string id(_getValue("IDArret"));
					string name(_getValue("Appellation Commerciale1"));
					string cityName(_getValue("Commune"));

					// City
					City* city(NULL);
					CityTableSync::SearchResult cities(
						CityTableSync::Search(_env, cityName, optional<string>(), optional<string>(), 0, 1)
					);
					if(cities.empty())
					{
						stream << "WARN : City " << cityName << " not found<br />";
					}
					else
					{
						city = cities.begin()->get();
					}

					if(request)
					{
						PTFileFormat::ImportableStopArea isa;
						isa.operatorCode = id;
						isa.name = name;
						isa.linkedStopAreas = _stopAreas.get(id);
						isa.cityName = cityName;

						if(isa.linkedStopAreas.empty())
						{
							nonLinkedStopAreas.push_back(isa);
						}
					}

					PTFileFormat::CreateOrUpdateStopAreas(
						_stopAreas,
						id,
						name,
						city,
						false,
						_stopAreaDefaultTransferDuration,
						_dataSource,
						_env,
						stream
					);
				}
			}

			// 3 : Physical Stops
			if(key == FILE_ITINERAIRES)
			{
				// 2.2 : stops
				PTFileFormat::ImportableStopPoints linkedStopPoints;
				PTFileFormat::ImportableStopPoints nonLinkedStopPoints;

				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);

					// Line
					string idLigne(_getValue("idLigne"));
					LinesMap::const_iterator it(_linesMap.find(idLigne));
					if(it == _linesMap.end())
					{
						stream << "WARN : Obiti line ID "<< idLigne << " not found<br />";
						return false;
					}
					LineObitiElement lineObiti(it->second);

					string stopAreaId(_getValue("idarret"));
					string id(_getValue("nom_commune") + " " + _getValue("nom_arret") + " " + lineObiti.shortname + " " + (lineObiti.backward ? "R" : "A"));
					string name(_getValue("app_comm_arret"));

					// Stop area
					const StopArea* stopArea(NULL);
					if(_stopAreas.contains(stopAreaId))
					{
						stopArea = *_stopAreas.get(stopAreaId).begin();
					}
					else
					{
						stream << "WARN : inconsistent stop area id "<< stopAreaId <<" in the stop point "<< id <<"<br />";
						continue;
					}

					// Point
					shared_ptr<geos::geom::Point> point;

					PTFileFormat::ImportableStopPoint isp;
					isp.operatorCode = id;
					isp.name = name;
					isp.cityName = _getValue("nom_commune");
					isp.linkedStopPoints = _stopPoints.get(id);
					isp.stopArea = stopArea;
					isp.coords = point;

					if(request && isp.linkedStopPoints.empty())
					{
						nonLinkedStopPoints.push_back(isp);
					}

					// Creation or update
					PTFileFormat::CreateOrUpdateStop(
						_stopPoints,
						id,
						name,
						NULL,
						stopArea,
						point.get(),
						_dataSource,
						_env,
						stream
					);
				}
			}

			// 4 : schedules path
			if(key == PATH_HORAIRES)
			{
				boost::filesystem::path path(filePath.file_string().c_str());
				std::set<std::string> schedulesFiles;

				if(boost::filesystem::is_directory(path)) {
					for(boost::filesystem::directory_iterator it(path), end; it != end; ++it) {
						if(boost::filesystem::is_regular_file(it->status())) {
							schedulesFiles.insert(it->path().filename());
						}
					}
				}
				else
				{
					stream << "Invalid path!" << "<br />";
					return false;
				}

				// RollingStock
				RollingStock* rollingStock;
				if(_rollingStock.get())
				{
					rollingStock = _rollingStock.get();
				}
				else
				{
					stream << "WARN : rollingStock not defined";
					return false;
				}

				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);
				shared_ptr<ImportableTableSync::ObjectBySource<StopPointTableSync> > stopPointsFromOtherImport;
				if(_stopsFromDataSource && _stopsDataSource)
					stopPointsFromOtherImport.reset(new ImportableTableSync::ObjectBySource<StopPointTableSync>(*_stopsDataSource, _env));

				int numSens = 1;
				if(_backwardInSameFile)
					numSens = 2;

				BOOST_FOREACH(const string& file, schedulesFiles)
				{
					ifstream inFile;
					string fileWithPath = filePath.file_string() + file;
					stream << fileWithPath << "<br />";
					inFile.open(fileWithPath.c_str());
					if(!inFile)
					{
						stream << "Could no open the file " << fileWithPath << "<br />";
						return false;
					}

					for(int i=1; i<=numSens; i++)
					{
						bool backward=false;
						if((numSens == 1 && file[file.size()-5] == 'R') || (numSens == 2 && i == 2))
							backward = true;

						streampos posSchedulesTable = inFile.tellg();
						if(i == 2)
						{
							// move to second Schedules table
							while(_line[0] != "")
							{
								getline(inFile, line);
								_loadLine(line);
							}
							while(_line[0] == "")
							{
								posSchedulesTable = inFile.tellg();
								getline(inFile, line);
								_loadLine(line);
							}
						}

						stream << "INFO : Loading file " << filePath.file_string().c_str()+file << "<br />";

						CommercialLine* commercialLine(PTFileFormat::GetLine(lines, file, _dataSource, _env, stream));
						if(!commercialLine)
						{
							stream << "No such line " << file << "<br />";
							return false;
						}

						// Routes
						string serviceID;
						string daysCalendarName;
						string periodCalendarName;

						ScheduledService::Schedules departureSchedules;
						ScheduledService::Schedules arrivalSchedules;
						time_duration lastTd(minutes(0));

						// Number of services
						_firstLine(inFile, line, posSchedulesTable);

						stream << "num of services : " << _line.size() << "<br />";

						for(int numService = 1; numService < _line.size(); numService++)
						{
							_firstLine(inFile, line, posSchedulesTable);

							// Get Period Calendar name
							if(!_moveToField(inFile,_periodCalendarField))
							{
								stream << "No such period field " << _periodCalendarField << "<br />";
								return false;
							}
							periodCalendarName = _line[numService] + " " + commercialLine->getNetwork()->getName();

							// Get Days Calendar name
							if(!_moveToField(inFile,_daysCalendarField))
							{
								stream << "No such period field " << _daysCalendarField << "<br />";
								return false;
							}
							daysCalendarName = _line[numService] + " " + commercialLine->getNetwork()->getName();

							// Ignore other parameters
							for(int i = 0; i < _numberOfOtherParameters; i++)
							{
								if(getline(inFile, line))
									_loadLine(line);
								else
								{
									stream << "Error with the number of other parameters : " << _numberOfOtherParameters << "<br />";
									return false;
								}
							}

							JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
							departureSchedules.clear();
							arrivalSchedules.clear();
							lastTd = minutes(0);

							// Schedule
							while(getline(inFile, line))
							{
								_loadLine(line);
								if(_line[0] == "")
									break;
								string stopPointId(to_lower_copy(trim_copy(_line[0])) + " " + commercialLine->getShortName() + " " + (backward ? "R" : "A"));
								string timeStr(_line[numService]);

								JourneyPattern::StopWithDepartureArrivalAuthorization::StopsSet stopPoints;
								if(_stopPoints.contains(stopPointId))
									stopPoints = _stopPoints.get(stopPointId);
								else if(stopPointsFromOtherImport)
								{
									// Todo : Add a generic function ?
									string stopPointName = to_lower_copy(trim_copy(_line[0]));
									replace_all(stopPointName,"é","e");
									replace_all(stopPointName,"è","e");
									replace_all(stopPointName,"ê","e");
									replace_all(stopPointName,"ë","e");
									replace_all(stopPointName,"â","a");
									replace_all(stopPointName,"à","a");
									replace_all(stopPointName,"ô","o");
									replace_all(stopPointName,"'"," ");
									replace_all(stopPointName,"-"," ");
									replace_all(stopPointName,"*","");

									string stopPointId(stopPointName + " " + (backward ? "R" : "A"));
									string stopPointId2(stopPointName);
									string stopPointId3(stopPointName + " " + (backward ? "A" : "R"));
									if(stopPointsFromOtherImport->contains(stopPointId))
									{
										stopPoints = stopPointsFromOtherImport->get(stopPointId);
										stream << "stop id "<< stopPointId <<" found in the service "<< numService <<"<br />";
									}
									else if(stopPointsFromOtherImport->contains(stopPointId2))
									{
										stopPoints = stopPointsFromOtherImport->get(stopPointId2);
										stream << "stop id "<< stopPointId2 <<" found in the service "<< numService <<"<br />";
									}
									else if(stopPointsFromOtherImport->contains(stopPointId3))
									{
										stopPoints = stopPointsFromOtherImport->get(stopPointId3);
										stream << "stop id "<< stopPointId3 <<" found in the service "<< numService <<"<br />";
									}
									else
									{
										stream << "stop id "<< stopPointId <<"/" << stopPointId2 << " not found in the service "<< numService <<"<br />";
										return false;
									}
								}
								else
								{
									stream << "WARN : inconsistent stop id "<< stopPointId <<" in the service "<< numService <<"<br />";
									return false;
								}

								if(timeStr.size() > 4)
								{
									time_duration td(
										lexical_cast<int>(timeStr.substr(0,2)),
										lexical_cast<int>(timeStr.substr(3,2)),
										0
									);
									if(td < lastTd)
									{
										td += hours(24);
									}
									departureSchedules.push_back(td - seconds(td.seconds()));
									arrivalSchedules.push_back(td.seconds() ? td + seconds(60 - td.seconds()) : td);
									lastTd = td;

									JourneyPattern::StopWithDepartureArrivalAuthorization stop(
										stopPoints,
										optional<double>(),
										true,
										true,
										true
									);
									stops.push_back(stop);
								}
							}

							if(stops.size() > 0)
							{
								stream << "service number " << numService << "<br />";

								CalendarTemplate* periodCalendar(NULL);
								CalendarTemplateTableSync::SearchResult calendars(
									CalendarTemplateTableSync::Search(
										_env,
										periodCalendarName,
										optional<RegistryKeyType>(),
										false,
										true,
										0,
										1,
										UP_LINKS_LOAD_LEVEL,
										optional<RegistryKeyType>())
								);
								if(calendars.empty())
									stream << "WARN : Calendar <pre>\"" << periodCalendarName << "\"</pre> not found<br />";
								else
									periodCalendar = calendars.begin()->get();


								CalendarTemplate* daysCalendar(NULL);
								CalendarTemplateTableSync::SearchResult calendars2(
									CalendarTemplateTableSync::Search(
										_env,
										daysCalendarName,
										optional<RegistryKeyType>(),
										false,
										true,
										0,
										1,
										UP_LINKS_LOAD_LEVEL,
										optional<RegistryKeyType>())
								);
								if(calendars2.empty())
									stream << "WARN : Calendar <pre>\"" << daysCalendarName << "\"</pre> not found<br />";
								else
									daysCalendar = calendars2.begin()->get();

								// Route
								JourneyPattern* route(
									PTFileFormat::CreateOrUpdateRoute(
										*commercialLine,
										commercialLine->getShortName(),
										commercialLine->getShortName(),
										optional<const string&>(),
										optional<Destination*>(),
										optional<const RuleUser::Rules&>(),
										backward,
										rollingStock,
										stops,
										_dataSource,
										_env,
										stream,
										true,
										true
								)	);
								if(route == NULL)
								{
									stream << "WARN : failure at route creation ("<< commercialLine->getShortName() <<")<br />";
									return false;
								}

								// Service
								ScheduledService* service(
									PTFileFormat::CreateOrUpdateService(
										*route,
										departureSchedules,
										arrivalSchedules,
										serviceID,
										_dataSource,
										_env,
										stream
								)	);

								// Calendars
								if(service)
								{
									boost::shared_ptr<ServiceCalendarLink> serviceCalendarLink;

									// Search for existing ServiceCalendarLink
									ServiceCalendarLinkTableSync::SearchResult serviceCalendarLinks(
										ServiceCalendarLinkTableSync::Search(
											_env,
											service->getKey(),
											0)
									);
									if(!serviceCalendarLinks.empty())
									{
										BOOST_FOREACH(shared_ptr<ServiceCalendarLink> scl, serviceCalendarLinks)
										{
											if((scl->getCalendarTemplate2() == periodCalendar) && (scl->getCalendarTemplate() == daysCalendar))
											{
												serviceCalendarLink = scl;
											}
										}
									}

									if(!serviceCalendarLink)
									{
										serviceCalendarLink = boost::shared_ptr<ServiceCalendarLink>(new ServiceCalendarLink(ServiceCalendarLinkTableSync::getId()));

										if(periodCalendar)
											serviceCalendarLink->setCalendarTemplate2(periodCalendar);
										else
											stream << "WARN : Calendar <pre>\"" << periodCalendarName << "\"</pre> not found<br />";
										if(daysCalendar)
											serviceCalendarLink->setCalendarTemplate(daysCalendar);
										else
											stream << "WARN : Calendar <pre>\"" << daysCalendarName << "\"</pre> not found<br />";

										serviceCalendarLink->setService(service);

										service->addCalendarLink(*serviceCalendarLink,true);

										_env.getEditableRegistry<ServiceCalendarLink>().add(shared_ptr<ServiceCalendarLink>(serviceCalendarLink));
									}
									service->setCalendarFromLinks();
								}
								else
								{
									stream << "WARN : failure at service creation ("<< serviceID <<")<br />";
									return false;
								}

							}
						}
					}
				}
			}
			return true;
		}



		void ObitiFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Fichiers</h1>";

			std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > choices;
			choices.push_back(make_pair(RegistryKeyType(0), string("(aucune source sélectionnée)")));
			BOOST_FOREACH(const Registry<DataSource>::value_type& source, Env::GetOfficialEnv().getRegistry<DataSource>())
				choices.push_back(make_pair(source.first, source.second->getName()));

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			t.getForm().addHiddenField(PTDataCleanerFileFormat::PARAMETER_FROM_TODAY, string("1"));
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Effacer données existantes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_OLD_DATA, false));
			stream << t.title("Import arrêts");
			stream << t.cell("Fichier ligne", t.getForm().getTextInput(_getFileParameterName(FILE_LIGNE), _pathsMap[FILE_LIGNE].file_string()));
			stream << t.cell("Fichier arrêt", t.getForm().getTextInput(_getFileParameterName(FILE_ARRET), _pathsMap[FILE_ARRET].file_string()));
			stream << t.cell("Fichier itinéraires", t.getForm().getTextInput(_getFileParameterName(FILE_ITINERAIRES), _pathsMap[FILE_ITINERAIRES].file_string()));
			stream << t.cell("Arrêts provenant d'une autre source de données", t.getForm().getOuiNonRadioInput(PARAMETER_STOPS_FROM_DATA_SOURCE, false));
			stream << t.cell("Source de l'import des arrêts",t.getForm().getSelectInput(PARAMETER_STOPS_DATA_SOURCE_ID, choices, optional<RegistryKeyType>(RegistryKeyType(0))));
			stream << t.title("Import horaires");
			stream << t.cell("Repertoire horaires", t.getForm().getTextInput(_getFileParameterName(PATH_HORAIRES), _pathsMap[PATH_HORAIRES].file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Mode de transport (ID)", t.getForm().getTextInput(PARAMETER_ROLLING_STOCK_ID, _rollingStock.get() ? lexical_cast<string>(_rollingStock->getKey()) : string()));
			stream << t.cell("Champs période de circulation", t.getForm().getTextInput(PARAMETER_PERIOD_CALENDAR_FIELD, _periodCalendarField));
			stream << t.cell("Champs jours de circulation", t.getForm().getTextInput(PARAMETER_DAYS_CALENDAR_FIELD, _daysCalendarField));
			stream << t.cell("Nombre de lignes de paramètres à ignorer", t.getForm().getTextInput(PARAMETER_NUMBER_OF_OTHER_PARAMETERS, lexical_cast<string>(_numberOfOtherParameters)));
			stream << t.cell("Horaires de retour dans le même fichier", t.getForm().getOuiNonRadioInput(PARAMETER_BACKWARD_IN_SAME_FILE, false));
			stream << t.cell("Temps de transfert par défaut (min)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_stopAreaDefaultTransferDuration.total_seconds() / 60)));
			stream << t.cell("Masque règles d'utilisation", t.getForm().getTextInput(PARAMETER_USE_RULE_BLOCK_ID_MASK, _serializePTUseRuleBlockMasks(_ptUseRuleBlockMasks)));
			stream << t.close();
		}



		db::DBTransaction ObitiFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			PTDataCleanerFileFormat::_addRemoveQueries(transaction);

			// Saving of each created or altered objects
			BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
			{
				StopAreaTableSync::Save(cstop.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
			{
				StopPointTableSync::Save(stop.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<CommercialLine>::value_type cline, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(cline.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<DesignatedLinePhysicalStop>::value_type lineStop, _env.getRegistry<DesignatedLinePhysicalStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<ServiceCalendarLink>::value_type link, _env.getRegistry<ServiceCalendarLink>())
			{
				ServiceCalendarLinkTableSync::Save(link.second.get(), transaction);
			}

			return transaction;
		}



		void ObitiFileFormat::Importer_::_loadFieldsMap( const std::string& line ) const
		{
			vector<string> cols;
			split(
				cols,
				line[line.size() - 1] == '\r' ? line.substr(0, line.size() - 1) : line,
				is_any_of(SEP)
			);
			_fieldsMap.clear();
			size_t rank(0);
			BOOST_FOREACH(const string& col, cols)
			{
				_fieldsMap[col] = rank;
				++rank;
			}
		}



		std::string ObitiFileFormat::Importer_::_getValue( const std::string& field ) const
		{
			return trim_copy(_line[_fieldsMap[field]]);
		}



		void ObitiFileFormat::Importer_::_loadLine( const std::string& line ) const
		{
			_line.clear();
			if(!line.empty())
			{
				string utfline(
					line[line.size() - 1] == '\r' ?
					line.substr(0, line.size() - 1) :
					line
				);
				utfline = IConv::IConv(_dataSource.getCharset(), "UTF-8").convert(line);
				split(_line, utfline, is_any_of(SEP));
			}
		}



		void ObitiFileFormat::Importer_::_firstLine(ifstream& inFile, std::string& line, streampos pos) const
		{
			inFile.clear();
			inFile.seekg(pos);
			getline(inFile, line);
			_loadLine(line);
		}



		bool ObitiFileFormat::Importer_::_moveToField(ifstream& inFile, const std::string& field) const
		{
			if(_line[0] == field)
				return true;
			else
			{
				std::string line;
				bool notTheEnd;
				do {
					notTheEnd = getline(inFile, line);
					if(notTheEnd) _loadLine(line);
				} while(_line[0] != field && notTheEnd);
				if((_line[0] != field) || !notTheEnd)
					return false;
				else return true;
			}
		}



		util::ParametersMap ObitiFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());

			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
			if(_rollingStock.get())
			{
				map.insert(PARAMETER_ROLLING_STOCK_ID, _rollingStock->getKey());
			}
			if(_stopsDataSource.get())
			{
				map.insert(PARAMETER_STOPS_DATA_SOURCE_ID, _stopsDataSource->getKey());
			}
			map.insert(PARAMETER_DAYS_CALENDAR_FIELD, _daysCalendarField);
			map.insert(PARAMETER_PERIOD_CALENDAR_FIELD, _periodCalendarField);
			map.insert(PARAMETER_NUMBER_OF_OTHER_PARAMETERS, _numberOfOtherParameters);
			map.insert(PARAMETER_BACKWARD_IN_SAME_FILE, _backwardInSameFile);
			map.insert(PARAMETER_USE_RULE_BLOCK_ID_MASK, _serializePTUseRuleBlockMasks(_ptUseRuleBlockMasks));
			map.insert(PARAMETER_STOPS_FROM_DATA_SOURCE, _stopsFromDataSource);
			return map;
		}



		void ObitiFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);

			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_daysCalendarField = map.getDefault<string>(PARAMETER_DAYS_CALENDAR_FIELD);
			_periodCalendarField = map.getDefault<string>(PARAMETER_PERIOD_CALENDAR_FIELD);
			_numberOfOtherParameters = map.getDefault<size_t>(PARAMETER_NUMBER_OF_OTHER_PARAMETERS,0);
			_backwardInSameFile = map.getDefault<bool>(PARAMETER_BACKWARD_IN_SAME_FILE,0);
			_stopsFromDataSource = map.getDefault<bool>(PARAMETER_STOPS_FROM_DATA_SOURCE,0);

			if(_stopsFromDataSource)
				if(map.getDefault<RegistryKeyType>(PARAMETER_STOPS_DATA_SOURCE_ID, 0))
				{
					_stopsDataSource = DataSourceTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_STOPS_DATA_SOURCE_ID), _env);
				}

			if(map.getDefault<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID, 0))
			{
				_rollingStock = RollingStockTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID), _env);
			}

			string ptUseRuleBlockMasksStr(map.getDefault<string>(PARAMETER_USE_RULE_BLOCK_ID_MASK));
			if(!ptUseRuleBlockMasksStr.empty())
			{
				vector<string> rules;
				split(rules, ptUseRuleBlockMasksStr, is_any_of(","));
				BOOST_FOREACH(const string& rule, rules)
				{
					vector<string> parts;
					split(parts, rule, is_any_of("="));

					if(parts.size() < 2)
					{
						continue;
					}

					try
					{
						shared_ptr<const PTUseRule> ptUseRule(
							PTUseRuleTableSync::Get(
								lexical_cast<RegistryKeyType>(parts[1]),
								_env
						)	);
						_ptUseRuleBlockMasks.insert(make_pair(parts[0], ptUseRule.get()));
					}
					catch (...)
					{
					}
				}
			}

		}



		std::string ObitiFileFormat::Importer_::_serializePTUseRuleBlockMasks( const PTUseRuleBlockMasks& object )
		{
			bool first(true);
			stringstream serializedPTUseRuleBlockMasks;
			BOOST_FOREACH(const PTUseRuleBlockMasks::value_type& rule, object)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					serializedPTUseRuleBlockMasks << ",";
				}
				serializedPTUseRuleBlockMasks << rule.first << "=" << rule.second->getKey();
			}
			return serializedPTUseRuleBlockMasks.str();
		}
	}
}
