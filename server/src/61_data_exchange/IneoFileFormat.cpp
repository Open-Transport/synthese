
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

#include "AllowedUseRule.h"
#include "CityTableSync.h"
#include "DataSource.h"
#include "DBModule.h"
#include "DeadRunTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "DriverAllocationTableSync.hpp"
#include "DriverServiceTableSync.hpp"
#include "Fare.hpp"
#include "ForbiddenUseRule.h"
#include "IConv.hpp"
#include "ImpExModule.h"
#include "Import.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "PTModule.h"
#include "PTUseRuleTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "StopAreaTableSync.hpp"
#include "TransportNetwork.h"
#include "UserTableSync.h"
#include "VehicleServiceTableSync.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace geos::geom;

namespace synthese
{
	using namespace calendar;
	using namespace data_exchange;
	using namespace db;
	using namespace geography;
	using namespace graph;
	using namespace impex;
	using namespace pt;
	using namespace pt_operation;
	using namespace security;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,IneoFileFormat>::FACTORY_KEY = "Ineo";
	}

	namespace data_exchange
	{
		const string IneoFileFormat::Importer_::FILE_PTF = "ptf";
		const string IneoFileFormat::Importer_::FILE_PNT = "pnt";
		const string IneoFileFormat::Importer_::FILE_DIS = "dis";
		const string IneoFileFormat::Importer_::FILE_DST = "dst";
		const string IneoFileFormat::Importer_::FILE_LIG = "lig";
		const string IneoFileFormat::Importer_::FILE_CJV = "cjv";
		const string IneoFileFormat::Importer_::FILE_HOR = "hor";
		const string IneoFileFormat::Importer_::FILE_CAL = "cal";
		const string IneoFileFormat::Importer_::SEP = ";";

		const string IneoFileFormat::Importer_::PARAMETER_NETWORK_ID = "net";
		const string IneoFileFormat::Importer_::PARAMETER_TRANSPORT_MODE_TYPE_LG_MASK = "transport_mode_type_lg_mask";
		const string IneoFileFormat::Importer_::PARAMETER_AUTO_IMPORT_STOPS("isa");
		const string IneoFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const string IneoFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const string IneoFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const string IneoFileFormat::Importer_::PARAMETER_LINE_READ_METHOD("line_read_method");
		const string IneoFileFormat::Importer_::VALUE_CIDX("CIDX");
		const string IneoFileFormat::Importer_::VALUE_SV("SV");
		const string IneoFileFormat::Importer_::PARAMETER_LINE_SHORT_NAME_FIELD = "line_short_name_field";
		const string IneoFileFormat::Importer_::VALUE_NLGIV = "NLGIV";
		const string IneoFileFormat::Importer_::VALUE_MNLC = "MNLC";
		const string IneoFileFormat::Importer_::PARAMETER_ADD_WAYBACK_TO_JOURNEYPATTERN_CODE = "add_wayback_to_journeypattern_code";
		const string IneoFileFormat::Importer_::PARAMETER_STOP_AREA_ID_FIELD = "stop_area_id_field";
		const string IneoFileFormat::Importer_::VALUE_MNCP = "MNCP";
		const string IneoFileFormat::Importer_::PARAMETER_STOP_ID_FIELD = "stop_id_field";
		const string IneoFileFormat::Importer_::VALUE_MNLP = "MNLP";
		const string IneoFileFormat::Importer_::VALUE_IDENTSMS = "IdentSMS";
		const string IneoFileFormat::Importer_::MNLP_PREFIX = "MNLP_**_";
		const string IneoFileFormat::Importer_::PARAMETER_STOP_CITY_CODE_FIELD = "stop_city_code_field";
		const string IneoFileFormat::Importer_::VALUE_CODE_COMMUNE = "code_commune";
		const string IneoFileFormat::Importer_::PARAMETER_STOP_NAME_FIELD = "stop_name_field";
		const string IneoFileFormat::Importer_::VALUE_LIBP = "LIBP";
		const string IneoFileFormat::Importer_::VALUE_LIBCOM = "LIBCOM";
		const string IneoFileFormat::Importer_::PARAMETER_STOP_HANDICAPPED_ACCESSIBILITY_FIELD = "stop_handicapped_accessibility_field";
		const string IneoFileFormat::Importer_::VALUE_UFR = "UFR";
		const string IneoFileFormat::Importer_::PARAMETER_JOURNEY_PATTERN_LINE_OVERLOAD_FIELD = "journey_pattern_line_overload_field";
		const string IneoFileFormat::Importer_::PARAMETER_HANDICAPPED_ALLOWED_USE_RULE = "handicapped_allowed_use_rule";
		const string IneoFileFormat::Importer_::PARAMETER_FORBIDDEN_SERVICE_USE_RULE = "forbidden_service_use_rule";
		const string IneoFileFormat::Importer_::PARAMETER_VEHICLE_SERVICE_SUFFIX = "vehicle_service_suffix";
		const string IneoFileFormat::Importer_::PARAMETER_DEPOT_TO_STOP_IS_HLP = "depot_to_stop_is_hlp";
		const string IneoFileFormat::Importer_::PARAMETER_STOP_TO_DEPOT_IS_HLP = "stop_to_depot_is_hlp";
		const string IneoFileFormat::Importer_::PARAMETER_ALLOW_DIFFERENT_STOP_POINTS_IN_SAME_STOP_AREA = "allow_different_stop_points_in_same_stop_area";
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


	namespace data_exchange
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
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<IneoFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTOperationFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_autoImportStops(false),
			_displayLinkedStops(false),
			_depotToStopIsHLP(false),
			_stopToDepotIsHLP(false),
			_allowDifferentStopPointsInSameStopArea(true),
			_interactive(false),
			_addWaybackToJourneyPatternCode(false),
			_destinations(*import.get<DataSource>(), _env),
			_stopPoints(*import.get<DataSource>(), _env),
			_depots(*import.get<DataSource>(), _env),
			_lines(*import.get<DataSource>(), _env)
		{}



		bool IneoFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const string& key
		) const {
			ifstream inFile;
			inFile.open(filePath.string().c_str());
			if(!inFile)
			{
				_logError(
					"Could not open the file " + filePath.string()
				);
				throw Exception("Could not open the file " + filePath.string());
			}
			_clearFieldsMap();
			date now(day_clock::local_day());

			DataSource& dataSource(*_import.get<DataSource>());

			// 1 : Stops
			if(key == FILE_PNT)
			{
				ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(dataSource, _env);
				ImportableTableSync::ObjectBySource<DepotTableSync> depots(dataSource, _env);

				// 1.1 Stop areas
				if(_defaultCity.get())
				{
					// Loop
					while(_readLine(inFile))
					{
						if(_section != "P")
						{
							continue;
						}

						//////////////////////////////////////////////////////////////////////////
						// Depots

						// Avoid existing depots
						if(depots.contains(_mnlp_prefix + _getValue(VALUE_MNLP)))
						{
							continue;
						}

						//////////////////////////////////////////////////////////////////////////
						// Fields

						// Code field
						string stopAreaCode(_getValue(_stopAreaIdField));

						// Name field
						string name(_getValue(_stopNameField));

						// City
						boost::shared_ptr<const City> city;
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
						_createOrUpdateStopAreas(
							stopAreas,
							stopAreaCode,
							name,
							city.get(),
							!_stopCityCodeField.empty(),
							_stopAreaDefaultTransferDuration,
							dataSource
						);
				}	}


				// 1.2 : stops
				StopsMap stops;
				inFile.clear();
				inFile.seekg(0, ios::beg);
				string fakeLine;
				getline(inFile, fakeLine);
				while(_readLine(inFile))
				{
					// Read only stop section
					if(_section != "P")
					{
						continue;
					}

					//////////////////////////////////////////////////////////////////////////
					// Fields reading

					// MNLP code
					string mnlp(_getValue(VALUE_MNLP));

					// Name field
					string name(_getValue(_stopNameField));

					// Code field
					string id(_getValue(_stopIdField));
					if(id.empty())
					{
						id = mnlp;
					}

					// Point
					boost::shared_ptr<Point> geometry;
					if(!_getValue("X").empty() && !_getValue("Y").empty())
					{
						try
						{
							geometry = dataSource.getActualCoordinateSystem().createPoint(
								lexical_cast<double>(_getValue("X")),
								lexical_cast<double>(_getValue("Y"))
							);
							if(geometry->isEmpty())
							{
								geometry.reset();
							}
							else
							{
								_points.insert(
									make_pair(
										mnlp,
										geometry
								)	);
							}
						}
						catch(boost::bad_lexical_cast&)
						{
						}
					}

					//////////////////////////////////////////////////////////////////////////
					// Depots

					// Search for existing depots.
					// Depots are not created here because they cannot be distinguished from stops
					// with the informations present in the file
					if(depots.contains(_mnlp_prefix + mnlp))
					{
						// Gets the depot
						Depot* depot = *depots.get(_mnlp_prefix + mnlp).begin();

						// Properties update
						depot->setName(name);
						depot->setGeometry(geometry);

						// Jump to next record
						continue;
					}


					//////////////////////////////////////////////////////////////////////////
					// Stops

					// Search for already defined stop
					StopsMap::iterator it(stops.find(id));
					if(it != stops.end())
					{
						it->second.codes.insert(_mnlp_prefix + mnlp);
						continue;
					}

					// Declaration
					Stop stop;
					stop.codes.insert(_mnlp_prefix + mnlp);
					stop.geometry = geometry;

					// Name field
					stop.name = name;

					// Handicapped
					if(!_stopHandicappedAccessibilityField.empty())
					{
						stop.ufr = (_getValue(_stopHandicappedAccessibilityField) == "O");
					}

					// Stop area
					string stopAreaCode(_getValue(_stopAreaIdField));
					set<StopArea*> stopAreasSet(stopAreas.get(stopAreaCode));
					if(stopAreasSet.empty())
					{
						_logWarning(
							"Stop area "+ stopAreaCode +" was not found for stop "+ id +". Stop update or creation is ignored."
						);
						continue;
					}
					stop.stopArea = *stopAreasSet.begin();

					stops.insert(
						make_pair(
							id,
							stop
					)	);
				}

				// Handicapped rules
				RuleUser::Rules handicappedRules;
				handicappedRules.push_back(NULL);
				handicappedRules.push_back(NULL);
				handicappedRules.push_back(_handicappedAllowedUseRule.get());
				handicappedRules.push_back(NULL);

				RuleUser::Rules handicappedForbiddenRules;
				handicappedForbiddenRules.push_back(NULL);
				handicappedForbiddenRules.push_back(NULL);
				handicappedForbiddenRules.push_back(NULL);
				handicappedForbiddenRules.push_back(NULL);

				// Stop removals
				typedef map<string, StopPoint*> ToRemove;
				ToRemove toRemove;
				BOOST_FOREACH(const impex::ImportableTableSync::ObjectBySource<StopPointTableSync>::Map::value_type& itStop, _stopPoints.getMap())
				{
					BOOST_FOREACH(StopPoint* stop, itStop.second)
					{
						BOOST_FOREACH(const std::string& code, stop->getCodesBySource(dataSource))
						{
							if(!_mnlp_prefix.empty() && code.size() > _mnlp_prefix.size() && code.substr(0, _mnlp_prefix.size()) == _mnlp_prefix)
							{
								stop->removeSourceLink(dataSource, code);
								toRemove.insert(make_pair(code, stop));
								continue;
							}
							StopsMap::const_iterator it(stops.find(code));
							if(it == stops.end())
							{
								stop->removeSourceLink(dataSource, code);
								toRemove.insert(make_pair(code, stop));
							}
						}
					}
				}

				// Code removal
				BOOST_FOREACH(ToRemove::value_type& it, toRemove)
				{
					_stopPoints.remove(it.first, *it.second);
				}

				// Stop updates and creations
				BOOST_FOREACH(const StopsMap::value_type& itStop, stops)
				{
					optional<const RuleUser::Rules&> handicapped(
						itStop.second.ufr ?
						handicappedRules :
						handicappedForbiddenRules
					);

					// Create or update stop
					set<StopPoint*> matchingStops(
						_createOrUpdateStop(
							_stopPoints,
							itStop.first,
							itStop.second.name,
							handicapped,
							itStop.second.stopArea,
							itStop.second.geometry.get(),
							dataSource
					)	);

					if(_stopIdField != VALUE_MNLP)
					{
						// Adding of the code to the stop if not already exists
						BOOST_FOREACH(const string& code, itStop.second.codes)
						{
							(*matchingStops.begin())->addCodeBySource(dataSource, code);
						}
						_stopPoints.add(**matchingStops.begin());
					}
				}
			}
			else if(key == FILE_PTF)
			{
				Geometries::key_type key;
				CoordinateSequence* sequence(NULL);
				string lastCode;
				while(_readLine(inFile))
				{
					if(_section == "NLP")
					{
						if(sequence)
						{
							if(_points[lastCode].get())
							{
								sequence->add(
									*_points[lastCode]->getCoordinate()
								);
							}
							if(sequence->getSize() > 1)
							{
								_geometries.insert(
									make_pair(
										key,
										boost::shared_ptr<LineString>(
											dataSource.getActualCoordinateSystem().getGeometryFactory().createLineString(sequence)
								)	)	);
							}
						}
						key = make_pair(
							_mnlp_prefix + _getValue("NLP1"),
							_mnlp_prefix + _getValue("NLP2")
						);
						sequence = dataSource.getActualCoordinateSystem().getGeometryFactory().getCoordinateSequenceFactory()->create(0, 2);

						if(_points[_getValue("NLP1")].get())
						{
							sequence->add(
								*_points[_getValue("NLP1")]->getCoordinate()
							);
						}
						lastCode = _getValue("NLP2");
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
				if(sequence)
				{
					if(_points[lastCode].get())
					{
						sequence->add(
							*_points[lastCode]->getCoordinate()
						);
					}
					if(sequence->getSize() > 1)
					{
						_geometries.insert(
							make_pair(
								key,
								boost::shared_ptr<LineString>(
									dataSource.getActualCoordinateSystem().getGeometryFactory().createLineString(sequence)
						)	)	);
					}
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
									_mnlp_prefix + _getValue("MNEO"),
									_mnlp_prefix + _getValue("MNED")
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
							_createOrUpdateDestination(
								_destinations,
								_getValue("NDSTG"),
								_getValue("DSTBL"),
								_getValue("DSTTS"),
								dataSource
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
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(dataSource, _env);
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
							_createOrUpdateRoute(
								overloadedLine ? *overloadedLine : *line,
								optional<const string&>(),
								jpName,
								optional<const string&>(), // destination
								destination,
								optional<const RuleUser::Rules&>(),
								jpWayback,
								NULL,
								stops,
								dataSource,
								true,
								true,
								true,
								_allowDifferentStopPointsInSameStopArea
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

						std::string longName(_getValue("LIBLG"));
						std::string shortName(_getValue(_lineShortNameField));
						line = _createOrUpdateLine(
							lines,
							lineId,
							longName,
							shortName,
							optional<RGBColor>(),
							*_network,
							dataSource
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
								_logWarning(
									"Destination "+ destinationCode +" not found in journey pattern "+ jpKey
								);
							}
							else
							{
								destination = *destinations.begin();
								map<Destination*,string>::const_iterator it(_destinationLineOverloads.find(destination));
								if(it != _destinationLineOverloads.end())
								{
									overloadedLine = _getLine(
										lines,
										it->second,
										dataSource
									);
								}
							}
						}
					}
					else if(_section == "PC")
					{
						string stopCode(_mnlp_prefix + _getValue("MNL"));
						boost::shared_ptr<LineString> lineString;
						if(!lastStopCode.empty())
						{
							// Distance
							std::map<std::pair<string, string>, graph::MetricOffset>::const_iterator it(_distances.find(
									make_pair(lastStopCode, stopCode)
							)	);
							if(it != _distances.end())
							{
								dst += it->second;
							}
							else
							{
								_logWarning(
									"Distance between "+ lastStopCode +" and "+ stopCode +" not found."
								);
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
							// Probably a dead run
							atLeastAnInexistantStop = true;
						}
						else
						{
							if(lineString.get() && !atLeastAnInexistantStop)
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
				ImportableTableSync::ObjectBySource<DeadRunTableSync> deadRuns(dataSource, _env);
				ImportableTableSync::ObjectBySource<VehicleServiceTableSync> vehicleServices(dataSource, _env);
				TCOUValues tcou;
				JourneyPattern* route(NULL);
				VehicleService* vehicleService(NULL);
				ScheduledService::Schedules departureSchedules;
				ScheduledService::Schedules arrivalSchedules;
				int ph(0);
				vector<date> dates;
				time_duration lastTd(minutes(0));
				string lineNum;
				vector<string> deadRunStops;
				string cidx;
				bool afterMidnight(false);
				while(true)
				{
					_readLine(inFile);

					if(	(_section == "C" || _section.empty() || _section == "SV") &&
						!departureSchedules.empty()
					){
						SchedulesBasedService* service(NULL);
						if(	route &&
							(tcou == TCOU_Commercial || tcou == TCOU_HLP ||
							 (tcou == TCOU_DepotToStop && _depotToStopIsHLP) ||
							 (tcou == TCOU_StopToDepot && _stopToDepotIsHLP))
						){
							RuleUser::Rules forbiddenServiceRules;
							if (tcou == TCOU_HLP ||
								(tcou == TCOU_DepotToStop && _depotToStopIsHLP) ||
								(tcou == TCOU_StopToDepot && _stopToDepotIsHLP))
							{
								forbiddenServiceRules.push_back(_forbiddenServiceUseRule.get());
								forbiddenServiceRules.push_back(_forbiddenServiceUseRule.get());
								forbiddenServiceRules.push_back(_forbiddenServiceUseRule.get());
								forbiddenServiceRules.push_back(_forbiddenServiceUseRule.get());
							} else {
								forbiddenServiceRules.push_back(NULL);
								forbiddenServiceRules.push_back(NULL);
								forbiddenServiceRules.push_back(NULL);
								forbiddenServiceRules.push_back(NULL);
							}
							
							service = _createOrUpdateService(
								*route,
								departureSchedules,
								arrivalSchedules,
								string(),
								dataSource,
								NULL,
								forbiddenServiceRules
							);
						}
						else if(
							(tcou == TCOU_DepotToStop || tcou == TCOU_StopToDepot) &&
							deadRunStops.size() == 2
						){
							// Load or creation of the object
							DeadRun* deadRun(
								_loadOrCreateObject(
									deadRuns,
									cidx,
									dataSource,
									"haut le pied"
							)	);

							// Depot code
							string depotCode(_mnlp_prefix + deadRunStops[tcou == TCOU_DepotToStop ? 0 : 1]);
							set<Depot*> linkedDepots(
								_depots.get(
									depotCode
							)	);

							// Stop code
							string stopCode(_mnlp_prefix + deadRunStops[tcou == TCOU_DepotToStop ? 1 : 0]);
							set<StopPoint*> linkedStops(
								_stopPoints.get(
									stopCode
							)	);

							if(!linkedDepots.empty() && !linkedStops.empty())
							{
								// Distance
								graph::MetricOffset dst(0);
								std::map<std::pair<string, string>, graph::MetricOffset>::const_iterator it(
									_distances.find(
										make_pair(
											_mnlp_prefix + deadRunStops[0],
											_mnlp_prefix + deadRunStops[1]
								)	)	);
								if(it != _distances.end())
								{
									dst = it->second;
								}

								// Route
								deadRun->setRoute(
									**linkedDepots.begin(),
									**linkedStops.begin(),
									dst,
									tcou == TCOU_DepotToStop
								);

								// Schedules
								deadRun->setDataSchedules(
									departureSchedules,
									arrivalSchedules
								);

								// Network
								deadRun->setTransportNetwork(_network.get());

								service = deadRun;
							}
						}
						if(service)
						{
							BOOST_FOREACH(const date& dat, dates)
							{
								service->setActive(dat);
								if(vehicleService)
								{
									vehicleService->setActive(dat);
								}
							}
							if(vehicleService)
							{
								vehicleService->insert(*service);
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
						string mnesv(_getValue("MNESV"));
						if(_lineReadMethod == VALUE_SV)
						{
							lineNum.clear();
							if(mnesv.length() >= 3)
							{
								lineNum = mnesv.substr(0, mnesv.size() - 2);
							}
						}
					}
					else if(_section == "C")
					{
						tcou = static_cast<TCOUValues>(
							lexical_cast<int>(
								_getValue("TCOU")
						)	);
						route = NULL;

						// Line
						if(tcou == TCOU_Commercial || tcou == TCOU_HLP ||
							(tcou == TCOU_DepotToStop && _depotToStopIsHLP) ||
							(tcou == TCOU_StopToDepot && _stopToDepotIsHLP))
						{
							if(_lineReadMethod == VALUE_CIDX)
							{
								lineNum = lexical_cast<string>(lexical_cast<int>(_getValue("CIDX").substr(5,2)));
							}

							// Journey pattern
							string jpNum(
								_addWaybackToJourneyPatternCode ?
								_getValue("SENS") + _getValue("ORD") :
								_getValue("ORD")
							);
							if(!lineNum.empty() && !jpNum.empty())
							{
								route = _journeyPatterns[make_pair(lineNum, jpNum)];
							}
						}
						else
						{
							deadRunStops.clear();
							cidx = _getValue("CIDX");
						}

						// Vehicle service
						string sb(_getValue("SB"));
						vehicleService = _createOrUpdateVehicleService(
							vehicleServices,
							sb + "/" + lexical_cast<string>(ph) + (_vehicleServiceSuffix.empty() ? string() : ("/"+ _vehicleServiceSuffix)),
							optional<optional<OperationUnit&> >()
						);
						vehicleService->set<Name>(sb);

						// Schedules initialization
						departureSchedules.clear();
						arrivalSchedules.clear();
						dates.clear();
						lastTd = minutes(0);
						afterMidnight = (_getValue("APM") == "O");

						// Calendar
						BOOST_FOREACH(int day, _calendars[_getValue("CJDV")])
						{
							BOOST_FOREACH(const date& dat, _dates[make_pair(ph, day)])
							{
								dates.push_back(dat);
							}
						}
					}
					else if(_section == "H")
					{
						string timeStr(_getValue("HOR"));
						time_duration td(
							lexical_cast<int>(timeStr.substr(0,2)),
							lexical_cast<int>(timeStr.substr(2,2)),
							(timeStr.size() >= 6) ? lexical_cast<int>(timeStr.substr(4,2)) : 0
						);
						if(afterMidnight)
						{
							td += hours(24);
						}
						if(td < lastTd)
						{
							td += hours(24);
						}

						arrivalSchedules.push_back(td.seconds() ? td + seconds(60 - td.seconds()) : td);

						// TBAT
						string tbatStr(_getValue("TBAT"));
						if(!tbatStr.empty())
						{
							td += minutes(lexical_cast<double>(tbatStr));
						}
						departureSchedules.push_back(td - seconds(td.seconds()));

						lastTd = td;

						// Pick up the points for dead runs
						if(tcou == TCOU_DepotToStop || tcou == TCOU_StopToDepot)
						{
							deadRunStops.push_back(_getValue("MNL"));
						}
					}
					if(_section.empty())
					{
						break;
					}
				}
			}
			return true;
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
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<VehicleService>::value_type& vservice, _env.getRegistry<VehicleService>())
			{
				VehicleServiceTableSync::Save(vservice.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<Depot>::value_type& depot, _env.getRegistry<Depot>())
			{
				DepotTableSync::Save(depot.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<DeadRun>::value_type& deadRun, _env.getRegistry<DeadRun>())
			{
				DeadRunTableSync::Save(deadRun.second.get(), transaction);
			}
			return transaction;
		}



		void IneoFileFormat::Importer_::_clearFieldsMap() const
		{
			_fieldsMap.clear();
		}



		string IneoFileFormat::Importer_::_getValue( const string& field ) const
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



		void IneoFileFormat::Importer_::_loadLine( const string& line ) const
		{
			_line.clear();

			string trim_line(trim_copy(line));
			if (trim_line[0] == ';')
				trim_line = trim_line.substr(1);

			size_t separator(trim_line.find_first_of(":"));
			if(separator == string::npos || separator == 0 || separator == trim_line.size()-1)
			{
				_section = "#";
				return;
			}

			_section = trim_line.substr(0, separator);
			if(_section == "F")
			{
				string content(trim_line.substr(separator+1));
				vector<string> parts;
				split(parts, content, is_any_of(":"));
				string code(trim_copy(parts[0]));
				vector<string> fields;
				split(fields, parts[1], is_any_of(SEP));
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
				string utfLine(IConv(_import.get<DataSource>()->get<Charset>(), "UTF-8").convert(trim_line.substr(separator+1)));
				split(fields, utfLine, is_any_of(SEP));
				const vector<string>& cols(itFieldsMap->second);
				for(size_t i=0; i<fields.size() && i<cols.size(); ++i)
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

			// Vehicle service suffix
			if(!_vehicleServiceSuffix.empty())
			{
				map.insert(PARAMETER_VEHICLE_SERVICE_SUFFIX, _vehicleServiceSuffix);
			}

			// Line read method
			map.insert(PARAMETER_LINE_READ_METHOD, _lineReadMethod);

			// Line short name field
			map.insert(PARAMETER_LINE_SHORT_NAME_FIELD, _lineShortNameField);

			// Stop area id field
			map.insert(PARAMETER_STOP_AREA_ID_FIELD, _stopAreaIdField);

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

			// Handicapped allowed use rule
			if(_handicappedAllowedUseRule.get())
			{
				map.insert(PARAMETER_HANDICAPPED_ALLOWED_USE_RULE, _handicappedAllowedUseRule->getKey());
			}
			
			// Forbidden service use rule
			if(_forbiddenServiceUseRule.get())
			{
				map.insert(PARAMETER_FORBIDDEN_SERVICE_USE_RULE, _forbiddenServiceUseRule->getKey());
			}

			// Forbidden service use rule
			if(_forbiddenServiceUseRule.get())
			{
				map.insert(PARAMETER_FORBIDDEN_SERVICE_USE_RULE, _forbiddenServiceUseRule->getKey());
			}

			// Journey pattern line overload field
			if(!_journeyPatternLineOverloadField.empty())
			{
				map.insert(PARAMETER_JOURNEY_PATTERN_LINE_OVERLOAD_FIELD, _journeyPatternLineOverloadField);
			}
			
			map.insert(PARAMETER_ALLOW_DIFFERENT_STOP_POINTS_IN_SAME_STOP_AREA, _allowDifferentStopPointsInSameStopArea);

			map.insert(PARAMETER_DEPOT_TO_STOP_IS_HLP, _depotToStopIsHLP);
			map.insert(PARAMETER_STOP_TO_DEPOT_IS_HLP, _stopToDepotIsHLP);
			map.insert(PARAMETER_ALLOW_DIFFERENT_STOP_POINTS_IN_SAME_STOP_AREA, _allowDifferentStopPointsInSameStopArea);

			return map;
		}



		void IneoFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);

			// Network
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

			// Vehicle service suffix
			_vehicleServiceSuffix = map.getDefault<string>(PARAMETER_VEHICLE_SERVICE_SUFFIX);

			// Line read method
			_lineReadMethod = map.getDefault<string>(PARAMETER_LINE_READ_METHOD, VALUE_CIDX);

			// Line short name field
			_lineShortNameField = map.getDefault<string>(PARAMETER_LINE_SHORT_NAME_FIELD, VALUE_NLGIV);

			// Stop area id field
			_stopAreaIdField = map.getDefault<string>(PARAMETER_STOP_AREA_ID_FIELD, VALUE_MNCP);

			// Stop id field
			_stopIdField = map.getDefault<string>(PARAMETER_STOP_ID_FIELD, VALUE_MNLP);
			if(_stopIdField != VALUE_MNLP)
			{
				_mnlp_prefix = MNLP_PREFIX;
			}

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

			// Handicapped PT use rule
			RegistryKeyType handicappedPTUseRuleId(
				map.getDefault<RegistryKeyType>(PARAMETER_HANDICAPPED_ALLOWED_USE_RULE)
			);
			if(handicappedPTUseRuleId) try
			{
				_handicappedAllowedUseRule = PTUseRuleTableSync::GetEditable(handicappedPTUseRuleId, _env);
			}
			catch(ObjectNotFoundException<PTUseRule>&)
			{
				throw Exception("No such handicapped use rule");
			}
			
			// Forbidden Service use rule
			RegistryKeyType forbiddenServiceUseRuleId(
				map.getDefault<RegistryKeyType>(PARAMETER_FORBIDDEN_SERVICE_USE_RULE)
			);
			if(forbiddenServiceUseRuleId) try
			{
				_forbiddenServiceUseRule = PTUseRuleTableSync::GetEditable(forbiddenServiceUseRuleId, _env);
			}
			catch(ObjectNotFoundException<PTUseRule>&)
			{
				throw Exception("No such forbidden service use rule");
			}

			_depotToStopIsHLP = map.getDefault<bool>(PARAMETER_DEPOT_TO_STOP_IS_HLP, false);
			_stopToDepotIsHLP = map.getDefault<bool>(PARAMETER_STOP_TO_DEPOT_IS_HLP, false);
			_allowDifferentStopPointsInSameStopArea = map.getDefault<bool>(PARAMETER_ALLOW_DIFFERENT_STOP_POINTS_IN_SAME_STOP_AREA, true);

			// Calendar dates
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_CAL));
			if(it != _pathsMap.end())
			{
				ifstream inFile;
				inFile.open(it->second.string().c_str());
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
