
/** IneoFileFormat class implementation.
	@file IneoFileFormat.cpp

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

#include "IneoFileFormat.hpp"
#include "TransportNetwork.h"
#include "StopArea.hpp"
#include "PTFileFormat.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ImpExModule.h"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "PTPlaceAdmin.h"
#include "StopPointAdmin.hpp"
#include "StopAreaAddAction.h"
#include "StopArea.hpp"
#include "DataSource.h"
#include "IConv.hpp"
#include "Importer.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "IneoFileFormat.hpp"
#include "City.h"
#include "PTFileFormat.hpp"
#include "CityTableSync.h"
#include "Junction.hpp"
#include "JunctionTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "AllowedUseRule.h"
#include "ForbiddenUseRule.h"

#include <fstream>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/LineString.h>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::gregorian;
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
		template<> const string FactorableTemplate<FileFormat,IneoFileFormat>::FACTORY_KEY("Ineo");
	}

	namespace pt
	{
		const std::string IneoFileFormat::Importer_::FILE_PTF("ptf");
		const std::string IneoFileFormat::Importer_::FILE_PNT("pnt");
		const std::string IneoFileFormat::Importer_::FILE_DIS("dis");
		const std::string IneoFileFormat::Importer_::FILE_DST("dst");
		const std::string IneoFileFormat::Importer_::FILE_LIG("lig");
		const std::string IneoFileFormat::Importer_::FILE_CJV("cjv");
		const std::string IneoFileFormat::Importer_::FILE_HOR("hor");
		const std::string IneoFileFormat::Importer_::FILE_CAL("cal");
		const std::string IneoFileFormat::Importer_::SEP(";");

		const std::string IneoFileFormat::Importer_::PARAMETER_NETWORK_ID = "net";
		const std::string IneoFileFormat::Importer_::PARAMETER_TRANSPORT_MODE_TYPE_LG_MASK = "transport_mode_type_lg_mask";
		const std::string IneoFileFormat::Importer_::PARAMETER_AUTO_IMPORT_STOPS("isa");
		const std::string IneoFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string IneoFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string IneoFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const std::string IneoFileFormat::Importer_::PARAMETER_LINE_READ_METHOD("line_read_method");
		const std::string IneoFileFormat::Importer_::VALUE_CIDX("CIDX");
		const std::string IneoFileFormat::Importer_::VALUE_SV("SV");
		const std::string IneoFileFormat::Importer_::PARAMETER_LINE_SHORT_NAME_FIELD = "line_short_name_field";
		const std::string IneoFileFormat::Importer_::VALUE_NLGIV = "NLGIV";
		const std::string IneoFileFormat::Importer_::VALUE_MNLC = "MNLC";
		const std::string IneoFileFormat::Importer_::PARAMETER_ADD_WAYBACK_TO_JOURNEYPATTERN_CODE = "add_wayback_to_journeypattern_code";
		const std::string IneoFileFormat::Importer_::PARAMETER_STOP_AREA_ID_FIELD = "stop_area_id_field";
		const std::string IneoFileFormat::Importer_::VALUE_MNCP = "MNCP";
		const std::string IneoFileFormat::Importer_::PARAMETER_STOP_ID_FIELD = "stop_id_field";
		const std::string IneoFileFormat::Importer_::VALUE_MNLP = "MNLP";
		const std::string IneoFileFormat::Importer_::VALUE_IDENTSMS = "IdentSMS";
		const std::string IneoFileFormat::Importer_::PARAMETER_STOP_CITY_CODE_FIELD = "stop_city_code_field";
		const std::string IneoFileFormat::Importer_::VALUE_CODE_COMMUNE = "code_commune";
		const std::string IneoFileFormat::Importer_::PARAMETER_STOP_NAME_FIELD = "stop_name_field";
		const std::string IneoFileFormat::Importer_::VALUE_LIBP = "LIBP";
		const std::string IneoFileFormat::Importer_::VALUE_LIBCOM = "LIBCOM";
		const std::string IneoFileFormat::Importer_::PARAMETER_STOP_HANDICAPPED_ACCESSIBILITY_FIELD = "stop_handicapped_accessibility_field";
		const std::string IneoFileFormat::Importer_::VALUE_UFR = "UFR";
		const string IneoFileFormat::Importer_::PARAMETER_JOURNEY_PATTERN_LINE_OVERLOAD_FIELD = "journey_pattern_line_overload_field";
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<IneoFileFormat>::Files MultipleFileTypesImporter<IneoFileFormat>::FILES(
			IneoFileFormat::Importer_::FILE_PNT.c_str(),
			IneoFileFormat::Importer_::FILE_PTF.c_str(),
			IneoFileFormat::Importer_::FILE_DIS.c_str(),
			IneoFileFormat::Importer_::FILE_DST.c_str(),
			IneoFileFormat::Importer_::FILE_LIG.c_str(),
			IneoFileFormat::Importer_::FILE_CAL.c_str(),
			IneoFileFormat::Importer_::FILE_CJV.c_str(),
			IneoFileFormat::Importer_::FILE_HOR.c_str(),
		"");
	}


	namespace pt
	{
		bool IneoFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_PNT));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_DIS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_DST);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_LIG);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_CJV);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_HOR);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_CAL);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		IneoFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	MultipleFileTypesImporter<IneoFileFormat>(env, dataSource),
			PTDataCleanerFileFormat(env, dataSource),
			Importer(env, dataSource),
			_autoImportStops(false),
			_interactive(false),
			_displayLinkedStops(false),
			_stopPoints(_dataSource, _env),
			_lines(_dataSource, _env),
			_destinations(_dataSource, _env),
			_addWaybackToJourneyPatternCode(false)
		{}



		bool IneoFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> request
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}
			_clearFieldsMap();
			date now(day_clock::local_day());

			// 1 : Stops
			if(key == FILE_PNT)
			{
				ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(_dataSource, _env);

				// 1.1 Stop areas
				if(_importStopAreas)
				{
					// Loop
					while(_readLine(inFile))
					{
						if(_section != "P")
						{
							continue;
						}

						// Code field
						string stopAreaCode(_getValue(_stopAreaIdField));

						// Name field
						string name(_getValue(_stopNameField));

						// City
						shared_ptr<const City> city;
						if(!_stopCityCodeField.empty())
						{
							string cityCode(_getValue(_stopCityCodeField));
							CityTableSync::SearchResult cities(
								CityTableSync::Search(
									_env,
									optional<string>(),
									optional<string>(),
									cityCode
							)	);
							if(!cities.empty())
							{
								city = *cities.begin();
							}
						}
						else
						{
							city = _defaultCity;
						}

						// Stop area creation or update
						PTFileFormat::CreateOrUpdateStopAreas(
							stopAreas,
							stopAreaCode,
							name,
							city.get(),
							!_stopCityCodeField.empty(),
							_stopAreaDefaultTransferDuration,
							_dataSource,
							_env,
							stream
						);
				}	}

				
				// 1.2 : stops
				RuleUser::Rules handicappedRules;
				handicappedRules.push_back(NULL);
				handicappedRules.push_back(AllowedUseRule::INSTANCE.get());
				handicappedRules.push_back(NULL);

				RuleUser::Rules handicappedForbiddenRules;
				handicappedForbiddenRules.push_back(NULL);
				handicappedForbiddenRules.push_back(ForbiddenUseRule::INSTANCE.get());
				handicappedForbiddenRules.push_back(NULL);

				// Loop
				inFile.clear();
				inFile.seekg(0, ios::beg);
				string fakeLine;
				getline(inFile, fakeLine);
				while(_readLine(inFile))
				{
					if(_section != "P")
					{
						continue;
					}

					// Code field
					string id(_getValue(_stopIdField));

					// MNLP code
					string mnlp(_getValue(VALUE_MNLP));
					
					// Name field
					string name(_getValue(_stopNameField));

					// Point
					shared_ptr<geos::geom::Point> point;
					if(!_getValue("X").empty() && !_getValue("Y").empty())
					{
						try
						{
							point = _dataSource.getActualCoordinateSystem().createPoint(
								lexical_cast<double>(_getValue("X")),
								lexical_cast<double>(_getValue("Y"))
							);
							if(point->isEmpty())
							{
								point.reset();
							}
						}
						catch(boost::bad_lexical_cast&)
						{
						}
					}

					// Handicapped
					optional<const RuleUser::Rules&> handicapped;
					if(!_stopHandicappedAccessibilityField.empty())
					{
						handicapped = 
							(_getValue(_stopHandicappedAccessibilityField) == "O") ?
							handicappedRules :
							handicappedForbiddenRules;
					}

					// Stop area
					string stopAreaCode(_getValue(_stopAreaIdField));
					set<StopArea*> stopAreasSet(stopAreas.get(stopAreaCode));
					if(stopAreasSet.empty())
					{
						stream << "WARN : Stop area " << stopAreaCode << " was not found for stop " << id << ". Stop update or creation is ignored.<br />";
						continue;
					}

					// Create or update stop
					set<StopPoint*> matchingStops(
						PTFileFormat::CreateOrUpdateStop(
							_stopPoints,
							id,
							name,
							handicapped,
							*stopAreasSet.begin(),
							point.get(),
							_dataSource,
							_env,
							stream
					)	);

					// Adding of the code to the stop if not already exists
					BOOST_FOREACH(StopPoint* stop, matchingStops)
					{
						stop->addCodeBySource(_dataSource, mnlp);
						_stopPoints.add(*stop);
					}

					// Updating the code map to take into account of secondary codes
				}
			}
			else if(key == FILE_PTF)
			{
				Geometries::key_type key;
				CoordinateSequence* sequence(NULL);
				while(_readLine(inFile))
				{
					if(_section == "NLP")
					{
						if(sequence && sequence->getSize() > 1)
						{
							_geometries.insert(
								make_pair(
									key,
									shared_ptr<LineString>(
										_dataSource.getActualCoordinateSystem().getGeometryFactory().createLineString(sequence)
							)	)	);
						}
						key = make_pair(_getValue("NLP1"), _getValue("NLP2"));
						sequence = _dataSource.getActualCoordinateSystem().getGeometryFactory().getCoordinateSequenceFactory()->create(0, 2);
					}
					if(_section == "PF" && sequence)
					{
						sequence->add(
							Coordinate(
								lexical_cast<double>(_getValue("X")),
								lexical_cast<double>(_getValue("Y"))
						)	);
					}
				}
				if(sequence && sequence->getSize() > 1)
				{
					_geometries.insert(
						make_pair(
							key,
							shared_ptr<LineString>(
								_dataSource.getActualCoordinateSystem().getGeometryFactory().createLineString(sequence)
					)	)	);
				}
			}
			// 2 : Distances
			else if(key == FILE_DIS)
			{
				while(_readLine(inFile))
				{
					if(_section == "D")
					{
						_distances.insert(
							make_pair(
								make_pair(
									_getValue("MNEO"),
									_getValue("MNED")
								),
								lexical_cast<MetricOffset>(_getValue("DIST"))
						)	);
					}
				}
			}
			// 3 : Destinations
			else if(key == FILE_DST)
			{
				while(_readLine(inFile))
				{
					if(_section == "DST")
					{
						Destination* destination(
							PTFileFormat::CreateOrUpdateDestination(
								_destinations,
								_getValue("NDSTG"),
								_getValue("DSTBL"),
								_getValue("DSTTS"),
								_dataSource,
								_env,
								stream
						)	);

						// Destination overload
						if(!_journeyPatternLineOverloadField.empty())
						{
							string overloadedLine(_getValue(_journeyPatternLineOverloadField));
							if(!overloadedLine.empty())
							{
								_destinationLineOverloads[destination] = overloadedLine;
							}
						}
					}
				}
			}
			// 4 : Lines
			else if(key == FILE_LIG)
			{
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);
				CommercialLine* line(NULL);
				Destination* destination(NULL);
				JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
				string lineId;
				string jpName;
				bool jpWayback(false);
				string jpKey;
				string lastStopCode;
				MetricOffset dst(0);
				bool atLeastAnInexistantStop(false);
				CommercialLine* overloadedLine(NULL);
				while(true)
				{
					_readLine(inFile);

					if((_section == "L" || _section =="CH" || _section.empty()) && !stops.empty())
					{
						JourneyPattern* route(
							PTFileFormat::CreateOrUpdateRoute(
								overloadedLine ? *overloadedLine : *line,
								optional<const string&>(),
								jpName,
								optional<const string&>(), // destination
								destination,
								optional<const RuleUser::Rules&>(),
								jpWayback,
								NULL,
								stops,
								_dataSource,
								_env,
								stream,
								true,
								true
						)	);
						stops.clear();
						atLeastAnInexistantStop = false;
						_journeyPatterns[make_pair(lineId,jpKey)] = route;
					}
					if(_section.empty())
					{
						break;
					}
					if(_section == "L")
					{
						// ID
						lineId = _getValue("MNLG");

						line = PTFileFormat::CreateOrUpdateLine(
							lines,
							lineId,
							_getValue("LIBLG"),
							_getValue(_lineShortNameField),
							optional<RGBColor>(),
							*_network,
							_dataSource,
							_env,
							stream
						);
					}
					else if(_section == "CH")
					{
						jpName = _getValue("LIBCH");
						jpWayback = (_getValue("SENS") != "A");
						jpKey = 
							_addWaybackToJourneyPatternCode ?
							_getValue("SENS") + _getValue("NCH") :
							_getValue("NCH")
						;
						stops.clear();
						atLeastAnInexistantStop = false;
						lastStopCode.clear();
						dst = 0;

						// Destination
						destination = NULL;
						overloadedLine = NULL;
						string destinationCode(_getValue("NDST"));
						if(!destinationCode.empty())
						{
							set<Destination*> destinations(_destinations.get(destinationCode));
							if(destinations.empty())
							{
								stream << "WARN : Destination " << destinationCode << " not found in journey pattern " << jpKey << "<br />";
							}
							else
							{
								destination = *destinations.begin();
								map<Destination*,string>::const_iterator it(_destinationLineOverloads.find(destination));
								if(it != _destinationLineOverloads.end())
								{
									overloadedLine = PTFileFormat::GetLine(
										lines,
										it->second,
										_dataSource,
										_env,
										stream
									);
								}
							}
						}
					}
					else if(_section == "PC")
					{
						string stopCode(_getValue("MNL"));
						shared_ptr<LineString> lineString;
						if(!lastStopCode.empty())
						{
							// Distance
							std::map<std::pair<std::string, std::string>, graph::MetricOffset>::const_iterator it(_distances.find(
									make_pair(lastStopCode, stopCode)
							)	);
							if(it != _distances.end())
							{
								dst += it->second;
							}
							else
							{
								stream << "WARN : distance between " << lastStopCode << " and " << stopCode << " not found.<br />";
							}

							// Geometry
							Geometries::const_iterator itGeom(
								_geometries.find(
									make_pair(lastStopCode, stopCode)
							)	);
							if(itGeom != _geometries.end())
							{
								lineString = itGeom->second;
							}
						}
						ImportableTableSync::ObjectBySource<StopPointTableSync>::Set linkedStops(
							_stopPoints.get(
								stopCode
						)	);
						if(linkedStops.empty())
						{
							atLeastAnInexistantStop = true;
						}
						else
						{
							if(lineString.get())
							{
								stops.rbegin()->_geometry = lineString;
							}
							JourneyPattern::StopWithDepartureArrivalAuthorization stop(
								linkedStops,
								dst,
								true,
								true,
								_getValue("TYPC") == "R"
							);
							stops.push_back(stop);
						}
						lastStopCode = stopCode;
					}
				}
			}
			else if(key == FILE_CAL)
			{
				// Already read in _setFromParemetersMap
			}
			else if(key == FILE_CJV)
			{
				int day(0);
				while(_readLine(inFile))
				{
					if(_section == "JC")
					{
						day = lexical_cast<int>(_getValue("CODE_JC"));
					}
					else if(_section == "CJDV")
					{
						vector<string> cjdvs;
						for(int i=0; i<10; ++i)
						{
							cjdvs.push_back(lexical_cast<string>(i));
						}
						cjdvs.push_back("A");
						cjdvs.push_back("B");
						cjdvs.push_back("C");
						cjdvs.push_back("D");
						cjdvs.push_back("E");
						cjdvs.push_back("F");
						BOOST_FOREACH(const string& cjdv, cjdvs)
						{
							if(_getValue("C"+cjdv) == "O")
							{
								_calendars[cjdv].push_back(day);
							}
						}
					}
				}
			}
			else if(key == FILE_HOR)
			{
				bool active(true);
				JourneyPattern* route(NULL);
				ScheduledService::Schedules departureSchedules;
				ScheduledService::Schedules arrivalSchedules;
				int ph(0);
				vector<date> dates;
				time_duration lastTd(minutes(0));
				string lineNum;
				while(true)
				{
					_readLine(inFile);

					if((_section == "C" || _section.empty() || _section == "SV") && !departureSchedules.empty() && route)
					{
						ScheduledService* service(
							PTFileFormat::CreateOrUpdateService(
								*route,
								departureSchedules,
								arrivalSchedules,
								string(),
								_dataSource,
								_env,
								stream
						)	);
						if(service)
						{
							BOOST_FOREACH(const date& dat, dates)
							{
								service->setActive(dat);
							}
						}
					}
					if(_section.empty())
					{
						break;
					}

					if(_section == "PH")
					{
						ph = lexical_cast<int>(_getValue("NPH"));
					}
					else if(_section == "SV")
					{
						if(_lineReadMethod == VALUE_SV)
						{
							lineNum.clear();
							string mnesv(_getValue("MNESV"));
							if(mnesv.length() >= 3)
							{
								lineNum = mnesv.substr(0, mnesv.size() - 2);
							}
						}
					}
					else if(_section == "C")
					{
						if(_getValue("TCOU") != "0")
						{
							active = false;
							continue;
						}
						active = true;
						if(_lineReadMethod == VALUE_CIDX)
						{
							lineNum = lexical_cast<string>(lexical_cast<int>(_getValue("CIDX").substr(5,2)));
						}
						string jpNum(
							_addWaybackToJourneyPatternCode ?
							_getValue("SENS") + _getValue("ORD") :
							_getValue("ORD")
						);

						route = NULL;
						if(!lineNum.empty() && !jpNum.empty())
						{
							route = _journeyPatterns[make_pair(lineNum,jpNum)];
						}

						departureSchedules.clear();
						arrivalSchedules.clear();
						dates.clear();
						lastTd = minutes(0);

						BOOST_FOREACH(int day, _calendars[_getValue("CJDV")])
						{
							BOOST_FOREACH(const date& dat, _dates[make_pair(ph, day)])
							{
								dates.push_back(dat);
							}
						}
					}
					else if(active && _section == "H")
					{
						string timeStr(_getValue("HOR"));
						time_duration td(
							lexical_cast<int>(timeStr.substr(0,2)),
							lexical_cast<int>(timeStr.substr(2,2)),
							(timeStr.size() >= 6) ? lexical_cast<int>(timeStr.substr(4,2)) : 0
						);
						if(td < lastTd)
						{
							td += hours(24);
						}

						arrivalSchedules.push_back(td.seconds() ? td + seconds(60 - td.seconds()) : td);

						// TBAT
						string tbatStr(_getValue("TBAT"));
						if(!tbatStr.empty())
						{
							td += minutes(lexical_cast<long>(tbatStr));
						}
						departureSchedules.push_back(td - seconds(td.seconds()));

						lastTd = td;
					}
					if(_section.empty())
					{
						break;
					}
				}
			}
			return true;
		}



		void IneoFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Fichiers</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Fichiers");
			stream << t.cell("Fichier PNT (arrêts)", t.getForm().getTextInput(_getFileParameterName(FILE_PNT), _pathsMap[FILE_PNT].file_string()));
			stream << t.cell("Fichier PTF (points de forme)", t.getForm().getTextInput(_getFileParameterName(FILE_PTF), _pathsMap[FILE_PTF].file_string()));
			stream << t.cell("Fichier DIS (distances)", t.getForm().getTextInput(_getFileParameterName(FILE_DIS), _pathsMap[FILE_DIS].file_string()));
			stream << t.cell("Fichier DST (destinations)", t.getForm().getTextInput(_getFileParameterName(FILE_DST), _pathsMap[FILE_DST].file_string()));
			stream << t.cell("Fichier LIG (lignes)", t.getForm().getTextInput(_getFileParameterName(FILE_LIG), _pathsMap[FILE_LIG].file_string()));
			stream << t.cell("Fichier CJV (dates)", t.getForm().getTextInput(_getFileParameterName(FILE_CJV), _pathsMap[FILE_CJV].file_string()));
			stream << t.cell("Fichier CAL (calendriers)", t.getForm().getTextInput(_getFileParameterName(FILE_CAL), _pathsMap[FILE_CAL].file_string()));
			stream << t.cell("Fichier HOR (horaires)", t.getForm().getTextInput(_getFileParameterName(FILE_HOR), _pathsMap[FILE_HOR].file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Effacer données existantes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_OLD_DATA, _cleanOldData));
			stream << t.cell("Ne pas importer données anciennes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_FROM_TODAY, _fromToday));
			stream << t.cell("Effacer arrêts inutilisés", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_UNUSED_STOPS, _cleanUnusedStops));
			stream << t.cell("Réseau (ID)", t.getForm().getTextInput(PARAMETER_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
			stream << t.cell("Affichage arrêts liés", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops));
			stream << t.cell("Import automatique arrêts", t.getForm().getOuiNonRadioInput(PARAMETER_AUTO_IMPORT_STOPS, _autoImportStops));
			stream << t.cell("Commune par défaut (ID)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity.get() ? lexical_cast<string>(_defaultCity->getKey()) : string()));
			stream << t.cell("Temps de transfert par défaut (min)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_stopAreaDefaultTransferDuration.total_seconds() / 60)));
			
			// Line read method
			vector<pair<optional<string>, string> > methods;
			methods.push_back(make_pair(optional<string>(VALUE_CIDX), VALUE_CIDX));
			methods.push_back(make_pair(optional<string>(VALUE_SV), VALUE_SV));
			stream << t.cell("Méthode d'identification des lignes", t.getForm().getSelectInput(PARAMETER_LINE_READ_METHOD, methods, optional<string>(_lineReadMethod)));

			// Line short name field
			vector<pair<optional<string>, string> > fields;
			fields.push_back(make_pair(optional<string>(VALUE_NLGIV), VALUE_NLGIV));
			fields.push_back(make_pair(optional<string>(VALUE_MNLC), VALUE_MNLC));
			stream << t.cell("Champ nom court des lignes", t.getForm().getSelectInput(PARAMETER_LINE_SHORT_NAME_FIELD, fields, optional<string>(_lineShortNameField)));

			// Stop id field
			vector<pair<optional<string>, string> > sfields;
			sfields.push_back(make_pair(optional<string>(VALUE_MNLP), VALUE_MNLP));
			sfields.push_back(make_pair(optional<string>(VALUE_IDENTSMS), VALUE_IDENTSMS));
			stream << t.cell("Champ id arrêt", t.getForm().getSelectInput(PARAMETER_STOP_ID_FIELD, sfields, optional<string>(_stopIdField)));

			// Stop name field
			vector<pair<optional<string>, string> > snfields;
			snfields.push_back(make_pair(optional<string>(VALUE_LIBP), VALUE_LIBP));
			snfields.push_back(make_pair(optional<string>(VALUE_LIBCOM), VALUE_LIBCOM));
			stream << t.cell("Champ nom arrêt", t.getForm().getSelectInput(PARAMETER_STOP_NAME_FIELD, snfields, optional<string>(_stopNameField)));

			// Stop city field
			vector<pair<optional<string>, string> > scfields;
			scfields.push_back(make_pair(optional<string>(string()), "Pas de champ commune"));
			scfields.push_back(make_pair(optional<string>(VALUE_CODE_COMMUNE), VALUE_CODE_COMMUNE));
			stream << t.cell("Champ commune arrêt", t.getForm().getSelectInput(PARAMETER_STOP_CITY_CODE_FIELD, scfields, optional<string>(_stopCityCodeField)));

			// Add wayback to journey pattern code
			stream << t.cell("Ajouter le sens au code de chainage", t.getForm().getOuiNonRadioInput(PARAMETER_ADD_WAYBACK_TO_JOURNEYPATTERN_CODE, _addWaybackToJourneyPatternCode));

			// Journey pattern line overload field
			stream << t.cell("Champ de forçage de ligne dans le fichier girouettes", t.getForm().getTextInput(PARAMETER_JOURNEY_PATTERN_LINE_OVERLOAD_FIELD, _journeyPatternLineOverloadField));

			stream << t.close();
		}



		db::DBTransaction IneoFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			_addRemoveQueries(transaction);

			// Saving of each created or altered objects
			if(_autoImportStops)
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
			BOOST_FOREACH(const Registry<Destination>::value_type& destination, _env.getRegistry<Destination>())
			{
				DestinationTableSync::Save(destination.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<Junction>::value_type& junction, _env.getRegistry<Junction>())
			{
				JunctionTableSync::Save(junction.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<TransportNetwork>::value_type network, _env.getRegistry<TransportNetwork>())
			{
				TransportNetworkTableSync::Save(network.second.get(), transaction);
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
			return transaction;
		}



		void IneoFileFormat::Importer_::_clearFieldsMap() const
		{
			_fieldsMap.clear();
		}



		std::string IneoFileFormat::Importer_::_getValue( const std::string& field ) const
		{
			return _line[field];
		}



		bool IneoFileFormat::Importer_::_readLine(ifstream& file) const
		{
			string line;
			if(!getline(file, line))
			{
				_section.clear();
				return false;
			}
			_loadLine(line);
			return true;
		}



		void IneoFileFormat::Importer_::_loadLine( const std::string& line ) const
		{
			_line.clear();

			vector<string> parts;
			string trim_line(trim_copy(line));
			if (trim_line[0] == ';')
				trim_line = trim_line.substr(1);
			split(parts, trim_line, is_any_of(":"));
			if(parts.size() < 2 || parts[0].empty() || parts[1].empty())
			{
				_section = "#";
				return;
			}
			_section = parts[0];
			if(_section == "F")
			{
				string code(trim_copy(parts[1]));
				vector<string> fields;
				split(fields, parts[2], is_any_of(SEP));
				BOOST_FOREACH(const string& field, fields)
				{
					_fieldsMap[code].push_back(trim_copy(field));
				}
			}
			else
			{
				FieldMaps::const_iterator itFieldsMap(_fieldsMap.find(_section));
				if(itFieldsMap == _fieldsMap.end())
				{
					_section = "#";
					return;
				}
				vector<string> fields;
				string utfLine(IConv::IConv(_dataSource.getCharset(), "UTF-8").convert(parts[1]));
				split(fields, utfLine, is_any_of(SEP));
				const vector<string>& cols(itFieldsMap->second);
				for(size_t i=0; i<fields.size(); ++i)
				{
					_line[cols[i]] = trim_copy(fields[i]);
				}
			}
		}



		util::ParametersMap IneoFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());

			// Network
			if(_network.get())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}

			map.insert(PARAMETER_AUTO_IMPORT_STOPS, _autoImportStops);
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(_defaultCity.get())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity->getKey());
			}
			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}

			// Line read method
			map.insert(PARAMETER_LINE_READ_METHOD, _lineReadMethod);

			// Line short name field
			map.insert(PARAMETER_LINE_SHORT_NAME_FIELD, _lineShortNameField);

			// Stop id field
			map.insert(PARAMETER_STOP_ID_FIELD, _stopIdField);

			// Add wayback to journeypattern code
			map.insert(PARAMETER_ADD_WAYBACK_TO_JOURNEYPATTERN_CODE, _addWaybackToJourneyPatternCode);

			// Stop city field
			map.insert(PARAMETER_STOP_CITY_CODE_FIELD, _stopCityCodeField);

			// Stop handicapped field
			map.insert(PARAMETER_STOP_HANDICAPPED_ACCESSIBILITY_FIELD, _stopHandicappedAccessibilityField);

			// Stop name field
			map.insert(PARAMETER_STOP_NAME_FIELD, _stopNameField);

			// Journey pattern line overload field
			if(!_journeyPatternLineOverloadField.empty())
			{
				map.insert(PARAMETER_JOURNEY_PATTERN_LINE_OVERLOAD_FIELD, _journeyPatternLineOverloadField);
			}

			return map;
		}



		void IneoFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);

			if(map.isDefined(PARAMETER_NETWORK_ID)) try
			{
				_network = TransportNetworkTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), _env);
			}
			catch (ObjectNotFoundException<TransportNetwork>&)
			{
			}

			_autoImportStops = map.getDefault<bool>(PARAMETER_AUTO_IMPORT_STOPS, false);
			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
			}

			// Line read method
			_lineReadMethod = map.getDefault<string>(PARAMETER_LINE_READ_METHOD, VALUE_CIDX);

			// Line short name field
			_lineShortNameField = map.getDefault<string>(PARAMETER_LINE_SHORT_NAME_FIELD, VALUE_NLGIV);

			// Stop area id field
			_stopAreaIdField = map.getDefault<string>(PARAMETER_STOP_AREA_ID_FIELD, VALUE_MNCP);

			// Stop id field
			_stopIdField = map.getDefault<string>(PARAMETER_STOP_ID_FIELD, VALUE_MNLP);

			// Add wayback to journeypattern code
			_addWaybackToJourneyPatternCode = map.getDefault<bool>(PARAMETER_ADD_WAYBACK_TO_JOURNEYPATTERN_CODE, false);

			// Stop city field
			_stopCityCodeField = map.getDefault<string>(PARAMETER_STOP_CITY_CODE_FIELD);

			// Stop handicapped field
			_stopHandicappedAccessibilityField = map.getDefault<string>(PARAMETER_STOP_HANDICAPPED_ACCESSIBILITY_FIELD);

			// Stop name field
			_stopNameField = map.getDefault<string>(PARAMETER_STOP_NAME_FIELD, VALUE_LIBP);

			// Journey pattern line overload field
			_journeyPatternLineOverloadField = map.getDefault<string>(PARAMETER_JOURNEY_PATTERN_LINE_OVERLOAD_FIELD);

			// Calendar dates
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_CAL));
			if(it != _pathsMap.end())
			{
				ifstream inFile;
				inFile.open(it->second.file_string().c_str());
				if(!inFile)
				{
					throw Exception("Could no open the calendar file.");
				}
				_clearFieldsMap();
				_calendar.clear();
				date now(day_clock::local_day());
				while(_readLine(inFile))
				{
					if(_section == "CAL")
					{
						string dateStr(_getValue("DATE"));
						date calDate(
							lexical_cast<int>(dateStr.substr(6,4)),
							lexical_cast<int>(dateStr.substr(3,2)),
							lexical_cast<int>(dateStr.substr(0,2))
						);
						if(!_fromToday || calDate >= now)
						{
							_calendar.setActive(calDate);
							_dates[
								make_pair(
									lexical_cast<int>(_getValue("PH")),
									lexical_cast<int>(_getValue("TYP_JOUR"))
								)
							].push_back(calDate);
						}
					}
				}
			}
		}
}	}
