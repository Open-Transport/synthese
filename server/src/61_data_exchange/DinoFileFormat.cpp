
/** DinoFileFormat class implementation.
	@file DinoFileFormat.cpp

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

#include "DinoFileFormat.hpp"

#include "Import.hpp"
#include "TransportNetwork.h"
#include "Destination.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ImpExModule.h"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "RollingStock.hpp"
#include "DataSource.h"
#include "Importer.hpp"
#include "DBModule.h"
#include "CityTableSync.h"
#include "JunctionTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "PTUseRuleTableSync.h"
#include "IConv.hpp"
#include "ContinuousService.h"
#include "Path.h"
#include "Fare.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/tokenizer.hpp>

#include <geos/geom/LineString.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Coordinate.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace geos::geom;
using boost::tokenizer;
using boost::escaped_list_separator;

namespace synthese
{
	using namespace data_exchange;
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace calendar;
	using namespace graph;
	using namespace server;
	using namespace geography;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,DinoFileFormat>::FACTORY_KEY("DINO");
	}

	namespace data_exchange
	{
		const std::string DinoFileFormat::Importer_::FILE_STOP_AREAS("rec_stop_area");
		const std::string DinoFileFormat::Importer_::FILE_STOPS("rec_stop");
		const std::string DinoFileFormat::Importer_::FILE_STOPPING_POINTS("rec_stopping_points");
		const std::string DinoFileFormat::Importer_::FILE_BRANCH("branch");
		const std::string DinoFileFormat::Importer_::FILE_LINES("rec_lin_ber");
		const std::string DinoFileFormat::Importer_::FILE_STOP_TIMES("lid_travel_time_type");
		const std::string DinoFileFormat::Importer_::FILE_JOURNEY("lid_course");
		const std::string DinoFileFormat::Importer_::FILE_SERVICE_RESTRICTION("service_restriction");
		const std::string DinoFileFormat::Importer_::FILE_DAY_TYPE_TO_ATTRIBUTE("day_type_2_day_attribute");
		const std::string DinoFileFormat::Importer_::FILE_CALENDAR("set_day_attribute");
		const std::string DinoFileFormat::Importer_::FILE_CALENDAR_DATES("calendar_of_the_company");
		const std::string DinoFileFormat::Importer_::FILE_TRIPS("rec_trip");
		const std::string DinoFileFormat::Importer_::FILE_SERVICE_INTERDICTION("service_interdiction");
		const std::string DinoFileFormat::Importer_::SEP(";");

		const std::string DinoFileFormat::Importer_::PARAMETER_IMPORT_TRANSPORT_NETWORK("itn");
		const std::string DinoFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string DinoFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const string DinoFileFormat::Importer_::PARAMETER_USE_RULE_BLOCK_ID_MASK("use_rule_block_id_mask");
		const std::string DinoFileFormat::Importer_::PARAMETER_IGNORE_SERVICE_NUMBER("ignore_service_number");

		const std::string DinoFileFormat::Exporter_::LABEL_TAD("tad");
		const std::string DinoFileFormat::Exporter_::LABEL_NO_EXPORT_DINO("NO Export DINO");
		const int DinoFileFormat::Exporter_::WGS84_SRID(4326);

		std::map<std::string, util::RegistryKeyType> DinoFileFormat::Exporter_::shapeId;
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<DinoFileFormat>::Files MultipleFileTypesImporter<DinoFileFormat>::FILES(
			DinoFileFormat::Importer_::FILE_STOPS.c_str(),
			DinoFileFormat::Importer_::FILE_STOP_AREAS.c_str(),
			DinoFileFormat::Importer_::FILE_STOPPING_POINTS.c_str(),
			DinoFileFormat::Importer_::FILE_BRANCH.c_str(),
			DinoFileFormat::Importer_::FILE_LINES.c_str(),
			DinoFileFormat::Importer_::FILE_STOP_TIMES.c_str(),
			DinoFileFormat::Importer_::FILE_JOURNEY.c_str(),
			DinoFileFormat::Importer_::FILE_SERVICE_RESTRICTION.c_str(),
			DinoFileFormat::Importer_::FILE_DAY_TYPE_TO_ATTRIBUTE.c_str(),
			DinoFileFormat::Importer_::FILE_CALENDAR.c_str(),
			DinoFileFormat::Importer_::FILE_CALENDAR_DATES.c_str(),
			DinoFileFormat::Importer_::FILE_SERVICE_INTERDICTION.c_str(),
			DinoFileFormat::Importer_::FILE_TRIPS.c_str(),
		"");
	}


	namespace data_exchange
	{
		bool DinoFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_STOPS));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_STOP_AREAS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_STOPPING_POINTS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_BRANCH);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_LINES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_STOP_TIMES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_JOURNEY);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_SERVICE_RESTRICTION);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_DAY_TYPE_TO_ATTRIBUTE);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_CALENDAR);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_CALENDAR_DATES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_SERVICE_INTERDICTION);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_TRIPS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		DinoFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<DinoFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_interactive(false),
			_displayLinkedStops(false),
			_ignoreServiceNumber(false),
			_createNetworks(false),
			_networks(*import.get<DataSource>(), env),
			_stopPoints(*import.get<DataSource>(), env),
			_lines(*import.get<DataSource>(), env),
			_stopAreas(*import.get<DataSource>(), env)
		{}

		void DinoFileFormat::Importer_::_selectAndLoadDinoSchedules(
				DinoSchedulesMap& dinoSchedulesMap,
				const DinoSchedules& schedules,
				const string& schedulesCode
		) const {
			if(schedules.size() < 2)
			{
				_logWarningDetail(
					"SCHEDULES",schedulesCode,string()/*name*/,0,string(),string(), string(),schedules.empty() ? "SERVICE HAS NO SCHEDULES" : "SERVICE HAS ONLY ONE SCHEDULE"
				);
				return;
			}

			DinoSchedules& tempSchedules(
				dinoSchedulesMap.insert(
					make_pair(
						schedulesCode,
						DinoSchedules()
				)	).first->second
			);

			tempSchedules = schedules;
			_logLoadDetail(
				"SCHEDULES",schedulesCode,string(),0,string(),string(), lexical_cast<string>(schedules.size()) + " schedules defined","OK"
			);
		}


		void DinoFileFormat::Importer_::_selectAndLoadJourney(
			JourneysMap& journeys,
			const Journey::StoppingPoints& stoppingPoints,
//			const DinoSchedules& schedules,
			pt::CommercialLine* line,
			const std::string& name,
			bool direction,
			const std::string& journeyCode
		) const	{
			// Check if stopping points is long enough
			if(stoppingPoints.size() < 2)
			{
				_logWarningDetail(
					"JOURNEYPATTERN",journeyCode,string()/*name*/,0,string(),string(), lexical_cast<string>(direction),stoppingPoints.empty() ? "JOURNEYPATTERN HAS NO STOPS" : "JOURNEYPATTERN HAS ONLY ONE STOP"
				);
				return;
			}

			Journey& journey(
				journeys.insert(
					make_pair(
						journeyCode,
						Journey()
				)	).first->second
			);

			journey.code = journeyCode;
			journey.line = line;
			journey.name = name;
			journey.direction = direction;
			journey.stoppingPoints = stoppingPoints;
			_logLoadDetail(
				"JOURNEYPATTERN",journeyCode,name,0,lexical_cast<string>(direction),lexical_cast<string>(stoppingPoints.size()),string(),"OK"
			);

		}

		void DinoFileFormat::Importer_::_selectAndLoadTrip(TripsMap& trips,
				Journey& journey,
				const string& tripCode,
				const string& trainNr,
				const Calendar& calendar,
				const time_duration& startTime,
				const DinoSchedules& schedules
		) const	{
			// Jump over journeys with incomplete schedules
			if(schedules.size() > 0 && schedules.size() != journey.stoppingPoints.size())
			{
				_logWarningDetail(
							"TRIP",tripCode,journey.code,0,trainNr,
							lexical_cast<string>(schedules.size()),
							lexical_cast<string>(journey.stoppingPoints.size()),
							"Bad schedule number compared to stopping point list size"
							);
				return;
			}

			Trip& trip(
				trips.insert(
					make_pair(
						tripCode,
						Trip()
				)	).first->second
			);

			trip.code = tripCode;
			trip.trainNr = trainNr;
			trip.journey = &journey;
			trip.startTime = startTime;
//			trip.interdictions = &interdictions;
			trip.schedules = &schedules;
			trip.calendar = calendar;
			_logLoadDetail(
				"TRIP",tripCode,journey.code,0,
				lexical_cast<string>(startTime.total_seconds()),
				lexical_cast<string>(schedules.size()),
				lexical_cast<string>(journey.stoppingPoints.size()),
				"OK"
			);
		}




		string DinoFileFormat::Importer_::_hexToBinString(const string& s) const
		{
			stringstream ss;
			ss << hex << s;
			unsigned n;
			ss >> n;
			bitset<32> b(n);

			unsigned x = 0;
			if (boost::starts_with(s, "0x") || boost::starts_with(s, "0X")) x = 2;
			return b.to_string().substr(32 - 4*(s.length()-x));
		}



		void DinoFileFormat::Importer_::_fillCalendar(
				Calendar& c,
				const string& bitsetMonthStr,
				int year,
				int month)
		const {
			int jour = 1;
			for(string::const_iterator it = bitsetMonthStr.begin() ; it != bitsetMonthStr.end() ; ++jour, ++it)
			{
				if(lexical_cast<int>(*it) == 1)
				{
					c.setActive(date(year,month,jour));
				}
			}
		}



		void DinoFileFormat::Importer_::_logLoadDetail(
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
		void DinoFileFormat::Importer_::_logWarningDetail(
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

		void DinoFileFormat::Importer_::_logDebugDetail(
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




		bool DinoFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key
		) const {
			ifstream inFile;
			inFile.open(filePath.string().c_str());
			if(!inFile)
			{
				throw Exception("Could not open the file " + filePath.string());
			}
			string line;
			if(!getline(inFile, line))
			{
				return false;
			}
			_loadFieldsMap(line);

			_logInfo("Loading file "+ filePath.string() +" as "+ key);

			DataSource& dataSource(*_import.get<DataSource>());

			// Cities and Stop Areas
			if(key == FILE_STOPS)
			{
				// 2.1 : stop areas and cities
				PTFileFormat::ImportableStopAreas linkedStopAreas;
				PTFileFormat::ImportableStopAreas nonLinkedStopAreas;

				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);
					if(_getValue("STOP_TYPE_NR") != "1")
					{
						continue;
					}

					// City
					string cityName(_getValue("PLACE"));
					string cityCode("0");
					cityCode = _getValue("OCC");
					boost::shared_ptr<City> city;

					DinoCitiesMap::iterator itCity = _cities.find(cityCode);
					if(itCity != _cities.end())
					{
						city = itCity->second;
					}
					else
					{
						CityTableSync::SearchResult citiesRes = CityTableSync::Search(
									_env,
									boost::optional<std::string>(), // exactname
									((cityCode != "0") ? boost::optional<std::string>() : boost::optional<std::string>(cityName)), // likeName
									((cityCode != "0") ? boost::optional<std::string>(cityCode) : boost::optional<std::string>()),
									0, 0, true, true,
									util::UP_LINKS_LOAD_LEVEL // code
									);

						if(citiesRes.empty())
						{
							city = boost::shared_ptr<City>(new City);
							city->set<Name>(cityName);
							city->set<Code>(cityCode);
							city->set<Key>(CityTableSync::getId());
							_env.getEditableRegistry<City>().add(city);
							_cities.insert(make_pair(cityCode, city));
							_logCreation("City [" + cityName + "] code [" + cityCode + "]");
						}
						else
						{
							city = citiesRes.front();
						}
					}

					string id(_getValue("STOP_NR"));
					string name(trim_copy(_getValue("STOP_NAME")));
					// case where "stop name" is "<city name>, <stop name>"
					//if(!name.empty() && name.find(",")!=string::npos)
					//{
					//	vector<string> stopNameFields;
					//	split(stopNameFields, name, is_any_of(","), token_compress_on);
					//	if(trim_copy(stopNameFields[0]) == cityName)
					//	{
					//		name = trim_copy(stopNameFields[1]);
					//	}
					//}

					_logDebug("Working on stop [" + name + "] id [" + id + "]");

					PTFileFormat::ImportableStopArea isa;
					isa.operatorCode = id;
					isa.name = name;
					isa.linkedStopAreas = _stopAreas.get(id);

					if(isa.linkedStopAreas.empty())
					{
						nonLinkedStopAreas.push_back(isa);
					}
					else if(_displayLinkedStops)
					{
						linkedStopAreas.push_back(isa);
					}
					_createOrUpdateStopAreas(
								_stopAreas,
								id,
								name,
								city.get(),
								false,
								_stopAreaDefaultTransferDuration,
								dataSource
								);

					_exportStopAreas(
								nonLinkedStopAreas
								);
					if(_displayLinkedStops)
					{
						_exportStopAreas(
									linkedStopAreas
									);
					}
				}  // end of while
				
			}
			else if(key == FILE_STOP_AREAS)
			{
				// TODO IF NECESSARY
			}
			// stop points
			else if(key == FILE_STOPPING_POINTS)
			{
				
				while(getline(inFile, line))
				{
					_loadLine(line);
					string stopAreaId(_getValue("STOP_NR"));
					string name("");
					string code = stopAreaId + "-" + trim_copy(_getValue("STOPPING_POINT_NR"));
					const StopArea* stopArea(NULL);
					if(_stopAreas.contains(stopAreaId))
					{
						stopArea = *_stopAreas.get(stopAreaId).begin();
						name = stopArea->getName();
					}
					else if(_stopPoints.contains(code))
					{
						name = (*_stopPoints.get(code).begin())->getName();
						stopArea = (*_stopPoints.get(code).begin())->getConnectionPlace();
					}
					else
					{
						_logWarning(
							"inconsistent stop area id "+ stopAreaId +" in the stop point "+ code
						);
						continue;
					}
					// Point
					boost::shared_ptr<geos::geom::Point> point(
						dataSource.getActualCoordinateSystem().createPoint(
							lexical_cast<double>(_getValue("STOPPING_POINT_POS_X")),
							lexical_cast<double>(_getValue("STOPPING_POINT_POS_Y"))
					)	);
					if(point->isEmpty())
					{
						point.reset();
					}

					_createOrUpdateStop(
								_stopPoints,
								code,
								name,
								optional<const RuleUser::Rules&>(),
								stopArea,
								point.get(),
								dataSource
								);
				} // end of while
				
			}
			// Networks
			else if(key == FILE_BRANCH)
			{
				
				while(getline(inFile, line))
				{
					_loadLine(line);

					string networkName(trim_copy(_getValue("BRANCH_NAME")));
					string networkId(_getValue("BRANCH_NR"));
					TransportNetwork* network = NULL;
					TransportNetworkTableSync::SearchResult networksRes = TransportNetworkTableSync::Search(
								_env,
								string(),
								networkId,
								0, boost::optional<std::size_t>(), true, true,
								util::UP_LINKS_LOAD_LEVEL // code
								);


					if(networksRes.empty())
					{
						networksRes = TransportNetworkTableSync::Search(
									_env,
									networkName, // like name
									string(),
									0, boost::optional<std::size_t>(), true, true,
									util::UP_LINKS_LOAD_LEVEL // code
									);

						if(networksRes.empty())
						{
							_missingNetworks.insert(make_pair(networkId, networkName));
							if(_createNetworks)
							{
								network = _createOrUpdateNetwork(
											_networks,
											networkId,
											networkName,
											dataSource
											);
							}
						}
						else
						{
							network = (*(networksRes.begin())).get();
						}
					}
					else
					{
						network = (*(networksRes.begin())).get();
					}
					_networksMap.insert(make_pair(networkId, network));
				} // end of while
				
			} // end of FILE_BRANCH
			// Commercial Lines
			else if(key == FILE_LINES)
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					// Network
					string networkId(_getValue("BRANCH_NR"));
					string id(_getValue("LINE_NR"));
					TransportNetwork* network(NULL);
					if(_networks.contains(networkId))
					{
						network = *_networks.get(networkId).begin();
					}
					else if(_networksMap.find(networkId) != _networksMap.end() && _networksMap.find(networkId)->second)
					{
						network = _networksMap.find(networkId)->second;
					}
					else if(_lines.contains(id))
					{
						network = (*_lines.get(id).begin())->getNetwork();
					}
					else if(_missingNetworks.find(networkId) != _missingNetworks.end())
					{
						_logWarning(
							"SET NETWORK CREATION ON THIS IMPORT to import the missing Network id "+ networkId +" in the line "+ id
						);
						continue;
					}
					else
					{
						_logWarning(
							"Inconsistent network id "+ networkId +" in the line "+ id
						);
						continue;
					}

					std::string lineName(_getValue("LINE_NAME"));
					std::string lineNr(_getValue("LINE_NR"));
					_createOrUpdateLine(
						_lines,
						id,
						lineName,
						lineNr,
						boost::optional<util::RGBColor>(),
						*network,
						dataSource
					);
				} // end of while
			} // end of FILE_LINES
			// Times of travel at every stop point for each service
			else if(key == FILE_STOP_TIMES)
			{
				string lastSchedulesCode;
				DinoSchedules schedules;
				while(getline(inFile, line))
				{
					_loadLine(line);
					// DinoSchedule to create or to update
					string schedulesCode = trim_copy(_getValue("LINE_NR"))
							+ "-" + trim_copy(_getValue("STR_LINE_VAR"))
							+ "-" + trim_copy(_getValue("TIMING_GROUP_NR"));
					// schedulesCode has changed : transform last collected data into a Journey if selected
					if(schedulesCode != lastSchedulesCode)
					{
						_selectAndLoadDinoSchedules(
							_dinoSchedules,
							schedules,
							lastSchedulesCode
						);
					}

					// Entering new journey
					if(schedulesCode != lastSchedulesCode)
					{
						// Beginning load of the next journeyCode and schedules
						lastSchedulesCode = schedulesCode;
						schedules.clear();
					}

					// Schedule for stopping point
					// New schedule
					DinoSchedule& schedule(
								*schedules.insert(
									schedules.end(),
									DinoSchedule()
									)	);

					// Fields load
					schedule.art = seconds(lexical_cast<long int>(_getValue("TT_REL")));
					schedule.wt = seconds(lexical_cast<long int>(_getValue("STOPPING_TIME")));
				} // end of while
				// Load the last journey
				_selectAndLoadDinoSchedules(
						_dinoSchedules,
						schedules,
						lastSchedulesCode
				);
				
			} // end of FILE_STOP_TIMES
			// Initialization of Journey Patterns (code, name, direction, stop points list (metricoffset))
			else if(key == FILE_JOURNEY)
			{
				string lastJourneyCode;
				Journey::StoppingPoints stoppingPoints;
				pt::CommercialLine* commercialLine(NULL);
				string journeyName;
				bool direction(false);
				while(getline(inFile, line))
				{
					_loadLine(line);
					// Journey Pattern to create or to update
					string commercialLineId(trim_copy(_getValue("LINE_NR")));
					string journeyCode = commercialLineId + "-" + trim_copy(_getValue("STR_LINE_VAR"));

					// journey code has changed : transform last collected data into a Journey if selected
					if(commercialLine && journeyCode != lastJourneyCode)
					{
						_selectAndLoadJourney(
							_journeys,
							stoppingPoints,
							commercialLine,
							journeyName,
							direction,
							lastJourneyCode
						);
					}

					// Entering new journey
					if(journeyCode != lastJourneyCode)
					{
						if(!_lines.contains(commercialLineId))
						{
							_logWarning(
										"Inconsistent line id "+ commercialLineId +" in the journey "+ journeyCode
										);
							continue;
						}
						commercialLine = *_lines.get(commercialLineId).begin();
						// Direction
						if(trim_copy(_getValue("LINE_DIR_NR")) == "1")
						{
							direction = true;
						}
						else
						{
							direction = false;
						}

						// Beginning load of the next journeyCode
						lastJourneyCode = journeyCode;
						stoppingPoints.clear();
					}

					// Stoppoint for this journey pattern
					string spCode = trim_copy(_getValue("STOP_NR")) + "-" + trim_copy(_getValue("STOPPING_POINT_NR"));
					if(!_stopPoints.contains(spCode))
					{
						_logWarning("Inconsistent stop point " + spCode + " for Journey "
									+ trim_copy(_getValue("STR_LINE_VAR")) + "of Line " + commercialLineId);
					}
					StoppingPoint& sp(
								*stoppingPoints.insert(
									stoppingPoints.end(),
									StoppingPoint()
					));
					sp.code = spCode;
					sp.stoppoint = *(_stopPoints.get(spCode).begin());
					sp.pos = lexical_cast<int>(trim_copy(_getValue("LINE_CONSEC_NR")));
					sp.offsetFromPreviousStop = lexical_cast<MetricOffset>(trim_copy(_getValue("LENGTH")));
					_logLoadDetail(
						"STOPPOINT",spCode,sp.stoppoint->getName(),0,string(),string(), string(),"OK"
					);
					if(stoppingPoints.size() >= 2)
					{
						string frontName = stoppingPoints.front().getStopPointName();
						string backName = stoppingPoints.back().getStopPointName();
						journeyName = frontName + "-" + backName;
					}
				} // end of while
				// Load the last journey
				_selectAndLoadJourney(
						_journeys,
						stoppingPoints,
						commercialLine,
						journeyName,
						direction,
						lastJourneyCode
				);
				
			} // end of FILE_JOURNEY
			// Specific calendar definitions
			else if(key == FILE_SERVICE_RESTRICTION)
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					string calendarStr; // serialized calendar 
					Calendar c;
					string code = _getValue("RESTRICTION");
//					string name = _getValue("RESTRICT_TEXT1");
					string startDateStr = trim_copy(_getValue("DATE_FROM"));
					string endDateStr = trim_copy(_getValue("DATE_UNTIL"));
					if(startDateStr.size() != 8 || endDateStr.size() != 8)
					{
						_logWarning(
							"Inconsistent dates in service restrictions "+ code +" ("+ startDateStr +" and "+ endDateStr +")"
						);
						continue;
					}

					calendarStr += startDateStr.substr(0,4);

					string days = trim_copy(_getValue("RESTRICTION_DAYS"));
					int intervals = (int) (days.size() / 8);
					int month = lexical_cast<int>(startDateStr.substr(4,2));
					int currentMonth;
					int currentYear = lexical_cast<int>(startDateStr.substr(0,4));
					int position = 0;
					for(int i = 0 ; i < intervals; i++)
					{
						if(month + i > 12 && ((month + i) % 12) == 1)
						{
							// Handle change of year
							currentYear = lexical_cast<int>(startDateStr.substr(0,4)) + 1;
						}
						currentMonth = ((month + i) % 12 == 0 ? 12 : (month + i) % 12);
						string bin = _hexToBinString(days.substr(position, 8));
						position += 8;
						if(((currentMonth) % 2 != 0 && currentMonth < 8)
						   || ((currentMonth) % 2 == 0 && currentMonth >= 8))
						{
							_fillCalendar(c, string (bin.rbegin(), bin.rend() - 1), currentYear, currentMonth);
						}
						else if(currentMonth == 2)
						{
							if(gregorian_calendar::is_leap_year(currentYear))
							{
								_fillCalendar(c, string (bin.rbegin(), bin.rend() - 3), currentYear, currentMonth);
							}
							else
							{
								_fillCalendar(c, string (bin.rbegin(), bin.rend() - 4), currentYear, currentMonth);
							}
						}
						else
						{
							_fillCalendar(c, string (bin.rbegin(), bin.rend() - 2), currentYear, currentMonth);
						}
					}

					DinoCalendar sr;
					sr.code = code;
//					sr.name = name;
					sr.calendar = c;
					_restrictions.insert(make_pair(code, sr));
				} // end of while
			} // end of FILE_SERVICE_RESTRICTION
			else if(key == FILE_DAY_TYPE_TO_ATTRIBUTE)
			{
				set<int> dayAttributes;
				while(getline(inFile, line))
				{
					_loadLine(line);
					dayAttributes.clear();
					int dayType = lexical_cast<int>(_getValue("DAY_TYPE_NR"));
					DayTypes::iterator it = _dayTypes.find(dayType);
					if(it != _dayTypes.end())
					{
						// Found, update it
						set<int>& dayAttr = it->second;
						dayAttr.insert(lexical_cast<int>(_getValue("DAY_ATTRIBUTE_NR")));
					}
					else
					{
						// Not found, create it
						dayAttributes.insert(lexical_cast<int>(_getValue("DAY_ATTRIBUTE_NR")));
						_dayTypes.insert(make_pair(dayType, dayAttributes));
					}
				} // end of while
			} // end of FILE_DAY_TYPE_TO_ATTRIBUTE
			else if(key == FILE_CALENDAR)
			{
				string code;
				while(getline(inFile, line))
				{
					_loadLine(line);
					code = _getValue("DAY_ATTRIBUTE_NR");
					DinoCalendar dc;
					dc.code = code;
					_dayAttributes.insert(make_pair(code, dc));
				}
			}
			else if(key == FILE_CALENDAR_DATES)
			{
				int dayType;
				string dayStr;
				while(getline(inFile, line))
				{
					_loadLine(line);
					dayStr = _getValue("DAY");
					dayType = lexical_cast<int>(_getValue("DAY_TYPE_NR"));
					date day(
						lexical_cast<int>(dayStr.substr(0,4)),
						lexical_cast<int>(dayStr.substr(4,2)),
						lexical_cast<int>(dayStr.substr(6,2))
					);
					DayTypes::iterator it = _dayTypes.find(dayType);
					if(it == _dayTypes.end())
					{
						_logWarningDetail(
							"COMPANYCALENDAR",dayStr,string(),0,string(),string(), lexical_cast<string>(dayType),"Unknown reference to a day type"
						);
					}
					else
					{
						set<int> dayAttributes = it->second;
						BOOST_FOREACH(int dayAttr, it->second)
						{
							DinoCalendars::iterator itDayAttr = _dayAttributes.find(lexical_cast<string>(dayAttr));
							if(itDayAttr == _dayAttributes.end())
							{
								_logWarningDetail(
									"COMPANYCALENDAR",dayStr,string(),0,string(),lexical_cast<string>(dayType), lexical_cast<string>(itDayAttr->second.code),"Unknown reference to a day attribute"
								);
							}
							else
							{
								Calendar& c = itDayAttr->second.calendar;
								c.setActive(day);
							}
						}

					}
				}
			} // end of FILE_CALENDAR_DATES
			// Line stops A/D exceptions bound with a precise service
			else if(key==FILE_SERVICE_INTERDICTION)
			{
				string lastTripCode;
				ServiceInterdictions serviceInterdictions;

				while(getline(inFile, line))
				{
					_loadLine(line);
					string tripCode(trim_copy(_getValue("TRIP_ID")));

					// The trip code has changed : transform last collected data into an interdiction
					if(tripCode != lastTripCode && !lastTripCode.empty())
					{
						_interdictions.insert(make_pair(lastTripCode, serviceInterdictions));

						serviceInterdictions.clear();
					}

					// Getting the new informations for this trip
					string interdictionCode = trim_copy(_getValue("SERVICE_INTERDICTION_CODE"));
					int stopPointRank = lexical_cast<int>(trim_copy(_getValue("LINE_CONSEC_NR")));
					serviceInterdictions.insert(make_pair(stopPointRank, interdictionCode));

					// Now entering in the new trip
					lastTripCode = tripCode;
				} // end of while

				// Load last service interdictions
				_interdictions.insert(make_pair(lastTripCode, serviceInterdictions));

			} // end of FILE_SERVICE_INTERDICTION
			// Journey patterns update / Services initialization
			else if(key == FILE_TRIPS)
			{
				string lastTripCode;
				const DinoSchedules* schedules(NULL);
				const Journey* journey(NULL);
				Calendar calendar;
				time_duration startTime(not_a_date_time);
				string trainNr;

				while(getline(inFile, line))
				{
					_loadLine(line);
					string tripCode(trim_copy(_getValue("TRIP_ID")));

					// The trip code has changed : transform last collected data into a course if selected
					if(	journey &&
						tripCode != lastTripCode &&
						!lastTripCode.empty()
					){
						_selectAndLoadTrip(
							_trips,
							*const_cast<Journey*>(journey),
							lastTripCode,
							trainNr,
							calendar,
							startTime,
//							*interdictions,
							*schedules
						);
					}

					// Entering new trip : check if the journey exists
					if(tripCode != lastTripCode)
					{
						string journeyCode = trim_copy(_getValue("LINE_NR")) + "-" + trim_copy(_getValue("STR_LINE_VAR"));
						string dinoSchedulesCode = journeyCode + "-" + _getValue("TIMING_GROUP_NR");
						int startTimeSeconds = lexical_cast<int>(trim_copy(_getValue("DEPARTURE_TIME")));
						startTime = seconds(startTimeSeconds);
						trainNr = trim_copy(_getValue("TRAIN_NR"));
						string restriction = _getValue("RESTRICTION");
						if(!restriction.empty())
						{
							// Check of the calendar defined by the restriction
							DinoCalendars::const_iterator itRestriction(
								_restrictions.find(
									restriction
							)	);
							if(itRestriction == _restrictions.end())
							{
								_logWarningDetail(
									"TRIP",tripCode,restriction,0,string(),string(), string(),"Bad trip restriction field in trip"
								);
							}
							else
							{
								calendar = itRestriction->second.calendar;
							}

						}
						else
						{
							// Use the calendar defined by the days
							int dayAttribute = lexical_cast<int>(_getValue("DAY_ATTRIBUTE_NR"));
							DinoCalendars::const_iterator itCalendar(
								_dayAttributes.find(
									lexical_cast<string>(dayAttribute)
							)	);
							if(itCalendar == _dayAttributes.end())
							{
								_logWarningDetail(
									"TRIP",tripCode,lexical_cast<string>(dayAttribute),0,string(),string(), string(),"Bad trip day_attribute_nr field in trip"
								);
							}
							else
							{
								calendar = itCalendar->second.calendar;
							}
						}

						// Check of the journey
						JourneysMap::const_iterator itJourney(
							_journeys.find(
								journeyCode
						)	);
						if(itJourney == _journeys.end())
						{
							journey = NULL;
							_logWarningDetail(
								"TRIP",tripCode,journeyCode,0,string(),string(), string(),"Bad journey field in trip"
							);
						}
						else
						{
							journey = &itJourney->second;
						}

						// Check of the dino schedules
						DinoSchedulesMap::const_iterator itSchedules(
							_dinoSchedules.find(
								dinoSchedulesCode
						)	);
						if(itSchedules == _dinoSchedules.end())
						{
							schedules = NULL;
							_logWarningDetail(
								"TRIP",tripCode,dinoSchedulesCode,0,string(),string(), string(),"Bad schedules code in trip"
							);
						}
						else
						{
							schedules = &itSchedules->second;
						}

						// Now entering in the new trip
						lastTripCode = tripCode;
					}

					if(!journey)
					{
						continue;
					}
				} // end of while

				// Load last trip
				if(journey)
				{
					_selectAndLoadTrip(
						_trips,
						*const_cast<Journey*>(journey),
						lastTripCode,
						trainNr,
						calendar,
						startTime,
//						*interdictions,
						*schedules
					);
				}

				// Registering data in ENV
				BOOST_FOREACH(const TripsMap::value_type& trip, _trips)
				{
					JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
					MetricOffset offsetSum(0);
					std::set<StopPoint*> sps;
					size_t rank(0);
					bool isJourneyWithInterdictions(false);
					BOOST_FOREACH(const StoppingPoint& journeyStop, trip.second.journey->stoppingPoints)
					{
						bool isDeparture = rank+1 < trip.second.journey->stoppingPoints.size();
						bool isArrival = rank > 0;
						ServicesInterdictions::const_iterator itSI = _interdictions.find(trip.second.code);
						if(itSI != _interdictions.end())
						{
							isJourneyWithInterdictions = true;
							ServiceInterdictions::const_iterator itInterdiction = itSI->second.find(journeyStop.pos);
							if(itInterdiction != itSI->second.end())
							{
								if(itInterdiction->second == "A")
								{
									isArrival = true;
									isDeparture = false;
								}
								else if(itInterdiction->second == "E")
								{
									isArrival = false;
									isDeparture = true;
								}
								else // "I"
								{
									isArrival = false;
									isDeparture = false;
								}
							}
						}
						offsetSum += journeyStop.offsetFromPreviousStop;
						sps.insert(journeyStop.stoppoint);
						JourneyPattern::StopWithDepartureArrivalAuthorization stop(
							sps,
							offsetSum,
							isDeparture,
							isArrival,
							1 /* with times */
						);
						stops.push_back(stop);
						++rank;
						// make the stop points set begin by the stop point associated to the next journeyStop
						sps.clear();
					}
					// Use rules
					RuleUser::Rules rules(RuleUser::GetEmptyRules());

					std::string routeId(isJourneyWithInterdictions ? trip.second.journey->code + "-" + trip.second.trainNr : trip.second.journey->code);
					JourneyPattern* journeyPattern(
						_createOrUpdateRoute(
							*trip.second.journey->line,
							routeId,
							trip.second.journey->name,
							optional<const string&>(),
							optional<Destination*>(),
							rules,
							trip.second.journey->direction,
							NULL,
							stops,
							dataSource,
							true,
							true,
							true,
							true
					)	);

					// Service
					ScheduledService::Schedules departures;
					ScheduledService::Schedules arrivals;
					time_duration previousDepartureTime = trip.second.startTime;
					time_duration arrivalTime;
					BOOST_FOREACH(const DinoSchedule& dinoSchedule, *trip.second.schedules)
					{
						arrivalTime = previousDepartureTime + dinoSchedule.art;
						arrivals.push_back(arrivalTime);
						previousDepartureTime = arrivalTime + dinoSchedule.wt;
						departures.push_back(previousDepartureTime);
					}

					ScheduledService* service(
						_createOrUpdateService(
							*journeyPattern,
							departures,
							arrivals,
							_ignoreServiceNumber ? string() : trip.second.trainNr,
							dataSource,
							optional<const string&>(),
							optional<const RuleUser::Rules&>(),
							optional<const JourneyPattern::StopsWithDepartureArrivalAuthorization&>(stops),
							trip.second.trainNr
					)	);
					if(service)
					{
						*service |= trip.second.calendar;
					}
				} // end of registering data in ENV
			} // end of FILE_TRIP

			return true;
		}



		db::DBTransaction DinoFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			PTDataCleanerFileFormat::_addRemoveQueries(transaction);

			// Saving of each created or altered objects
			BOOST_FOREACH(const Registry<City>::value_type& city, _env.getEditableRegistry<City>())
			{
				CityTableSync::Save(city.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
			{
				StopAreaTableSync::Save(cstop.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
			{
				StopPointTableSync::Save(stop.second.get(), transaction);
			}
			if(_createNetworks)
			{
				BOOST_FOREACH(Registry<TransportNetwork>::value_type network, _env.getRegistry<TransportNetwork>())
				{
					TransportNetworkTableSync::Save(network.second.get(), transaction);
				}
			}
			BOOST_FOREACH(Registry<CommercialLine>::value_type cline, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(cline.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getEditableRegistry<LineStop>())
			{
				lineStop.second->set<ReservationNeeded>(false);
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const ScheduledService::Registry::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			return transaction;
		}



		void DinoFileFormat::Importer_::_loadFieldsMap( const std::string& line ) const
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



		std::string DinoFileFormat::Importer_::_getValue( const std::string& field ) const
		{
			return trim_copy(_line[_fieldsMap[field]]);
		}



		void DinoFileFormat::Importer_::_loadLine( const std::string& line ) const
		{
			_line.clear();
			if(!line.empty())
			{
				string utfline(
					line[line.size() - 1] == '\r' ?
					line.substr(0, line.size() - 1) :
					line
				);
				utfline = IConv(_import.get<DataSource>()->get<Charset>(), "UTF-8").convert(line);
				split(_line, utfline, is_any_of(SEP));
			}
		}



		util::ParametersMap DinoFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());
			map.insert(PARAMETER_IMPORT_TRANSPORT_NETWORK, _createNetworks);
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
			map.insert(PARAMETER_USE_RULE_BLOCK_ID_MASK, _serializePTUseRuleBlockMasks(_ptUseRuleBlockMasks));
			map.insert(PARAMETER_IGNORE_SERVICE_NUMBER, _ignoreServiceNumber);
			return map;
		}



		void DinoFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
			_createNetworks = map.getDefault<bool>(PARAMETER_IMPORT_TRANSPORT_NETWORK, false);
			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

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
						boost::shared_ptr<const PTUseRule> ptUseRule(
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

			_ignoreServiceNumber = map.getDefault<bool>(PARAMETER_IGNORE_SERVICE_NUMBER, false);
		}



		std::string DinoFileFormat::Importer_::_serializePTUseRuleBlockMasks( const PTUseRuleBlockMasks& object )
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



		string DinoFileFormat::Importer_::StoppingPoint::getStopPointName() const
		{
			return stoppoint->getName();
		}



		// PROVIDING ALL FILES

		util::ParametersMap DinoFileFormat::Exporter_::getParametersMap() const
		{
			ParametersMap result;
			return result;
		}

		void DinoFileFormat::Exporter_::setFromParametersMap(const ParametersMap& map)
		{}

		RegistryKeyType DinoFileFormat::Exporter_::_key(RegistryKeyType key,RegistryKeyType suffix) const
		{
			RegistryKeyType gtfsKey;
			/*gtfsKey = 0;
			if(suffix)
				gtfsKey = suffix << 48;
			gtfsKey |= key & (RegistryKeyType)0xFFFFFFFFFFFF;
			*/
			gtfsKey = key;
			return gtfsKey;
		}

		string DinoFileFormat::Exporter_::_Str(string str)const
		{
			string sub;
			size_t rp;
			size_t lp = str.find("\"", 0);
			while(lp != string::npos)
			{
				rp = str.find("\"", lp+1);
				if(rp != string::npos)
				{
					sub = str.substr(lp, rp);
					sub = "\"" + sub + "\"";
					str.replace(lp, rp, sub);
					lp = rp + 3;
				}
				else
				{
					string sub = "\"\"\"";
					str.replace(lp, 1, sub);
					break;
				}
				lp = str.find("\"", lp);
			}
			if((str.find(",", 0) != string::npos) || (str.find("'", 0) != string::npos))
			{
				str = "\"" + str + "\"";
			}
			return str;
		}

		string DinoFileFormat::Exporter_::_SubLine(string str)const
		{
			size_t lp = str.find(" (");

			if(lp == string::npos)
				return str;
			return str.substr (0, lp);

		}



		// EXPORTER_::BUILD
		void DinoFileFormat::Exporter_::build(ostream& os) const
		{}



		DinoFileFormat::Exporter_::Exporter_(
			const impex::Export& export_
		):	OneFileExporter<DinoFileFormat>(export_)
		{}
}	}
