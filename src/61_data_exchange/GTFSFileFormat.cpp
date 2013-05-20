
/** GTFSFileFormat class implementation.
	@file GTFSFileFormat.cpp

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

#include "GTFSFileFormat.hpp"

#include "Import.hpp"
#include "TransportNetwork.h"
#include "StopArea.hpp"
#include "Destination.hpp"
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
#include "RollingStock.hpp"
#include "StopArea.hpp"
#include "DataSource.h"
#include "Importer.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "GTFSFileFormat.hpp"
#include "City.h"
#include "PTFileFormat.hpp"
#include "CityTableSync.h"
#include "Junction.hpp"
#include "JunctionTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "PTUseRuleTableSync.h"
#include "IConv.hpp"
#include "ContinuousService.h"
#include "ZipWriter.hpp"
#include "Path.h"

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
	using namespace html;
	using namespace admin;
	using namespace server;
	using namespace geography;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,GTFSFileFormat>::FACTORY_KEY("GTFS");
	}

	namespace data_exchange
	{
		const std::string GTFSFileFormat::Importer_::FILE_STOPS("stops");
		const std::string GTFSFileFormat::Importer_::FILE_TRANSFERS("transfers");
		const std::string GTFSFileFormat::Importer_::FILE_AGENCY("agency");
		const std::string GTFSFileFormat::Importer_::FILE_ROUTES("routes");
		const std::string GTFSFileFormat::Importer_::FILE_CALENDAR("calendar");
		const std::string GTFSFileFormat::Importer_::FILE_CALENDAR_DATES("calendar_dates");
		const std::string GTFSFileFormat::Importer_::FILE_TRIPS("trips");
		const std::string GTFSFileFormat::Importer_::FILE_STOP_TIMES("stop_times");
		const std::string GTFSFileFormat::Importer_::FILE_FARE_ATTRIBUTES("fare_attributes");
		const std::string GTFSFileFormat::Importer_::FILE_FARE_RULES("fare_rules");
		const std::string GTFSFileFormat::Importer_::FILE_SHAPES("shapes");
		const std::string GTFSFileFormat::Importer_::FILE_FREQUENCIES("frequencies");
		const std::string GTFSFileFormat::Importer_::SEP(",");

		const std::string GTFSFileFormat::Importer_::PARAMETER_IMPORT_STOP_AREA("isa");
		const std::string GTFSFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string GTFSFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string GTFSFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const string GTFSFileFormat::Importer_::PARAMETER_USE_RULE_BLOCK_ID_MASK("use_rule_block_id_mask");

		const std::string GTFSFileFormat::Exporter_::PARAMETER_NETWORK_ID("ni");
		const std::string GTFSFileFormat::Exporter_::LABEL_TAD("tad");
		const int GTFSFileFormat::Exporter_::WGS84_SRID(4326);

		std::map<std::string, util::RegistryKeyType> GTFSFileFormat::Exporter_::shapeId;
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<GTFSFileFormat>::Files MultipleFileTypesImporter<GTFSFileFormat>::FILES(
			GTFSFileFormat::Importer_::FILE_STOPS.c_str(),
			GTFSFileFormat::Importer_::FILE_TRANSFERS.c_str(),
			GTFSFileFormat::Importer_::FILE_AGENCY.c_str(),
			GTFSFileFormat::Importer_::FILE_ROUTES.c_str(),
			GTFSFileFormat::Importer_::FILE_CALENDAR.c_str(),
			GTFSFileFormat::Importer_::FILE_CALENDAR_DATES.c_str(),
			GTFSFileFormat::Importer_::FILE_TRIPS.c_str(),
			GTFSFileFormat::Importer_::FILE_STOP_TIMES.c_str(),
			GTFSFileFormat::Importer_::FILE_FARE_ATTRIBUTES.c_str(),
			GTFSFileFormat::Importer_::FILE_FARE_RULES.c_str(),
			GTFSFileFormat::Importer_::FILE_SHAPES.c_str(),
			GTFSFileFormat::Importer_::FILE_FREQUENCIES.c_str(),
		"");
	}


	namespace data_exchange
	{
		bool GTFSFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_AGENCY));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_STOPS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_ROUTES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_TRIPS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_STOP_TIMES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_CALENDAR);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		GTFSFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			const impex::ImportLogger& importLogger
		):	Importer(env, import, importLogger),
			MultipleFileTypesImporter<GTFSFileFormat>(env, import, importLogger),
			PTDataCleanerFileFormat(env, import, importLogger),
			_importStopArea(false),
			_interactive(false),
			_displayLinkedStops(false),
			_networks(*import.get<DataSource>(), env),
			_stopPoints(*import.get<DataSource>(), env),
			_lines(*import.get<DataSource>(), env)
		{}



		bool GTFSFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key,
			boost::optional<const server::Request&> request
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}
			string line;
			if(!getline(inFile, line))
			{
				return false;
			}
			_loadFieldsMap(line);

			_log(ImportLogger::INFO, "Loading file "+ filePath.file_string() +" as "+ key);

			DataSource& dataSource(*_import.get<DataSource>());

			// 1 : Routes
			// Stops
			if(key == FILE_STOPS)
			{
				ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(dataSource, _env);

				// 2.1 : stop areas
				if(_importStopArea)
				{
					PTFileFormat::ImportableStopAreas linkedStopAreas;
					PTFileFormat::ImportableStopAreas nonLinkedStopAreas;

					// Loop
					while(getline(inFile, line))
					{
						_loadLine(line);
						if(_getValue("location_type") != "1")
						{
							continue;
						}

						string id(_getValue("stop_id"));
						string name(_getValue("stop_name"));

						if(request)
						{
							PTFileFormat::ImportableStopArea isa;
							isa.operatorCode = id;
							isa.name = name;
							isa.linkedStopAreas = stopAreas.get(id);

							if(isa.linkedStopAreas.empty())
							{
								nonLinkedStopAreas.push_back(isa);
							}
							else if(_displayLinkedStops)
							{
								linkedStopAreas.push_back(isa);
							}
						}
						else
						{
							PTFileFormat::CreateOrUpdateStopAreas(
								stopAreas,
								id,
								name,
								_defaultCity.get(),
								false,
								_stopAreaDefaultTransferDuration,
								dataSource,
								_env,
								_logger
							);
						}
					}

					if(request)
					{
						PTFileFormat::DisplayStopAreaImportScreen(
							nonLinkedStopAreas,
							*request,
							true,
							false,
							_defaultCity,
							_env,
							dataSource,
							_logger
						);
						if(_displayLinkedStops)
						{
							PTFileFormat::DisplayStopAreaImportScreen(
								linkedStopAreas,
								*request,
								true,
								false,
								_defaultCity,
								_env,
								dataSource,
								_logger
							);
						}
					}
				}

				// 2.2 : stops
				PTFileFormat::ImportableStopPoints linkedStopPoints;
				PTFileFormat::ImportableStopPoints nonLinkedStopPoints;

				// Loop
				inFile.clear();
				inFile.seekg(0, ios::beg);
				getline(inFile, line);
				while(getline(inFile, line))
				{
					_loadLine(line);
					if(_getValue("location_type") != "0")
					{
						continue;
					}

					string id(_getValue("stop_id"));
					string name(_getValue("stop_name"));

					// Stop area
					string stopAreaId(_getValue("parent_station"));
					const StopArea* stopArea(NULL);
					if(stopAreas.contains(stopAreaId))
					{
						stopArea = *stopAreas.get(stopAreaId).begin();
					}
					else if(_stopPoints.contains(id))
					{
						stopArea = (*_stopPoints.get(id).begin())->getConnectionPlace();
					}
					else
					{
						_log(
							ImportLogger::WARN,
							"inconsistent stop area id "+ stopAreaId +" in the stop point "+ id
						);
						continue;
					}

					// Point
					boost::shared_ptr<geos::geom::Point> point(
						dataSource.getActualCoordinateSystem().createPoint(
							lexical_cast<double>(_getValue("stop_lon")),
							lexical_cast<double>(_getValue("stop_lat"))
					)	);
					if(point->isEmpty())
					{
						point.reset();
					}

					PTFileFormat::ImportableStopPoint isp;
					isp.operatorCode = id;
					isp.name = name;
					isp.linkedStopPoints = _stopPoints.get(id);
					isp.stopArea = stopArea;
					isp.coords = point;

					if(request && isp.linkedStopPoints.empty())
					{
						nonLinkedStopPoints.push_back(isp);
					}
					else if(request && _displayLinkedStops)
					{
						linkedStopPoints.push_back(isp);
					}
					else
					{
						// Creation or update
						PTFileFormat::CreateOrUpdateStop(
							_stopPoints,
							id,
							name,
							optional<const RuleUser::Rules&>(),
							stopArea,
							point.get(),
							dataSource,
							_env,
							_logger
						);
					}
				}

				if(request)
				{
					PTFileFormat::DisplayStopPointImportScreen(
						nonLinkedStopPoints,
						*request,
						_env,
						dataSource,
						_logger
					);
					if(_displayLinkedStops)
					{
						PTFileFormat::DisplayStopPointImportScreen(
							linkedStopPoints,
							*request,
							_env,
							dataSource,
							_logger
						);
					}
				}

				if(!nonLinkedStopPoints.empty())
				{
					return false;
				}
			}
			else if(key == FILE_TRANSFERS)
			{
				// TODO
			}
			else if(key == FILE_AGENCY)
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					PTFileFormat::CreateOrUpdateNetwork(
						_networks,
						_getValue("agency_id"),
						_getValue("agency_name"),
						dataSource,
						_env,
						_logger
					);
				}
			}
			// 3 : Lines
			else if(key == FILE_ROUTES)
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					// Network
					string networkId(_getValue("agency_id"));
					string id(_getValue("route_id"));
					TransportNetwork* network(NULL);
					if(_networks.contains(networkId))
					{
						network = *_networks.get(networkId).begin();
					}
					else if(_lines.contains(id))
					{
						network = (*_lines.get(id).begin())->getNetwork();
					}
					else
					{
						_log(
							ImportLogger::WARN,
							"Inconsistent network id "+ networkId +" in the line "+ id
						);
						continue;
					}

					// Color
					optional<RGBColor> color;
					string colorStr(_getValue("route_color"));
					if(colorStr.size() == 6)
					{
						color = RGBColor::FromXMLColor("#"+ colorStr);
					}
					else if(colorStr.size() == 7 && colorStr[0] == '#')
					{
						color = RGBColor::FromXMLColor(colorStr);
					}

					PTFileFormat::CreateOrUpdateLine(
						_lines,
						id,
						_getValue("route_long_name"),
						_getValue("route_short_name"),
						color,
						*network,
						dataSource,
						_env,
						_logger
					);
				}
			}
			// 4 : Calendars
			else if(key == FILE_CALENDAR)
			{
				vector<string> week_days;
				week_days.push_back("sunday");
				week_days.push_back("monday");
				week_days.push_back("tuesday");
				week_days.push_back("wednesday");
				week_days.push_back("thursday");
				week_days.push_back("friday");
				week_days.push_back("saturday");

				while(getline(inFile, line))
				{
					_loadLine(line);

					Calendar c;

					string startDateStr(_getValue("start_date"));
					string endDateStr(_getValue("end_date"));
					if(startDateStr.size() != 8 || endDateStr.size() != 8)
					{
						_log(
							ImportLogger::WARN,
							"Inconsistent dates in "+ line +" ("+ startDateStr +" and "+ endDateStr +")"
						);
						continue;
					}
					date startDate(
						lexical_cast<int>(startDateStr.substr(0,4)),
						lexical_cast<int>(startDateStr.substr(4,2)),
						lexical_cast<int>(startDateStr.substr(6,2))
					);
					date endDate(
						lexical_cast<int>(endDateStr.substr(0,4)),
						lexical_cast<int>(endDateStr.substr(4,2)),
						lexical_cast<int>(endDateStr.substr(6,2))
					);

					for(date curDate(startDate); curDate<=endDate; curDate += days(1))
					{
						if(_getValue(week_days[curDate.day_of_week()]) == "1")
						{
							c.setActive(curDate);
						}
					}

					_calendars[_getValue("service_id")] = c;
				}
			}
			else if(key == FILE_CALENDAR_DATES) // 5
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					std::map<std::string, calendar::Calendar>::iterator it(_calendars.find(_getValue("service_id")));
					if(it == _calendars.end())
					{
						it = _calendars.insert(make_pair(_getValue("service_id"), Calendar())).first;
					}

					string dateStr(_getValue("date"));
					if(dateStr.size() != 8)
					{
						_log(
							ImportLogger::WARN,
							"Inconsistent date in "+ line
						);
						continue;
					}
					date d(
						lexical_cast<int>(dateStr.substr(0,4)),
						lexical_cast<int>(dateStr.substr(4,2)),
						lexical_cast<int>(dateStr.substr(6,2))
					);

					if(_getValue("exception_type") == "1")
					{
						it->second.setActive(d);
					}
					else if(_getValue("exception_type") == "2")
					{
						it->second.setInactive(d);
					}
				}
			}
			// 6 : Routes / Services
			else if(key == FILE_TRIPS)
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					Trip trip;

					// Line
					string id(_getValue("trip_id"));
					string lineCode(_getValue("route_id"));
					if(!_lines.contains(lineCode))
					{
						_log(
							ImportLogger::WARN,
							"Inconsistent line id "+ lineCode +" in the trip "+ id
						);
						continue;
					}
					trip.line = *_lines.get(lineCode).begin();

					// Use rule
					trip.useRule = NULL;
					string blockId(_getValue("block_id"));
					BOOST_FOREACH(const PTUseRuleBlockMasks::value_type& rule, _ptUseRuleBlockMasks)
					{
						if(blockId.size() >= rule.first.size() && blockId.substr(0, rule.first.size()) == rule.first)
						{
							trip.useRule = rule.second;
							break;
						}
					}

					// Calendar
					string calendarCode(_getValue("service_id"));
					Calendars::const_iterator it(_calendars.find(calendarCode));
					if(it == _calendars.end())
					{
						_log(
							ImportLogger::WARN,
							"Inconsistent service id "+ calendarCode +" in the trip "+ id
						);
						continue;
					}
					trip.calendar = it->second;

					// Destination
					trip.destination = _getValue("trip_headsign");

					// Direction
					trip.direction = lexical_cast<bool>(_getValue("direction_id"));

					_trips.insert(make_pair(id, trip));
				}
			}
			else if(key == FILE_STOP_TIMES)
			{
				string lastTripCode;
				time_duration previousArrivalTime, previousDepartureTime;
				TripDetailVector tripDetailVector;

				while(getline(inFile, line))
				{
					_loadLine(line);

					string tripCode(_getValue("trip_id"));
					if(tripCode != lastTripCode && !lastTripCode.empty() && !tripDetailVector.empty())
					{
						// Trip
						TripsMap::const_iterator it(_trips.find(lastTripCode));
						if(it == _trips.end())
						{
							_log(
								ImportLogger::WARN,
								"Inconsistent trip id "+ lastTripCode +" in the trip stops file"
							);
							continue;
						}
						Trip trip(it->second);

						// Route
						JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
						MetricOffset offsetSum(0);
						BOOST_FOREACH(const TripDetail& tripStop, tripDetailVector)
						{
							offsetSum += tripStop.offsetFromLast;
							JourneyPattern::StopWithDepartureArrivalAuthorization stop(
								tripStop.stop,
								offsetSum
							);
							stops.push_back(stop);
						}

						// Use rules
						RuleUser::Rules rules(RuleUser::GetEmptyRules());
						rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = trip.useRule;

						JourneyPattern* route(
							PTFileFormat::CreateOrUpdateRoute(
								*trip.line,
								optional<const string&>(),
								optional<const string&>(),
								optional<const string&>(trip.destination),
								optional<Destination*>(),
								rules,
								trip.direction,
								NULL,
								stops,
								dataSource,
								_env,
								_logger,
								true,
								true
						)	);

						// Service
						ScheduledService::Schedules departures;
						BOOST_FOREACH(const TripDetail& tripStop, tripDetailVector)
						{
							departures.push_back(tripStop.departureTime);
						}
						ScheduledService::Schedules arrivals;
						BOOST_FOREACH(const TripDetail& tripStop, tripDetailVector)
						{
							arrivals.push_back(tripStop.arrivalTime);
						}

						ScheduledService* service(
							PTFileFormat::CreateOrUpdateService(
								*route,
								departures,
								arrivals,
								lastTripCode,
								dataSource,
								_env,
								_logger
						)	);
						if(service)
						{
							*service |= trip.calendar;
						}

						tripDetailVector.clear();
					}

					TripDetail tripDetail;
					if(_getValue("shape_dist_traveled") != "")
					{
						tripDetail.offsetFromLast = lexical_cast<MetricOffset>(_getValue("shape_dist_traveled"));
					}
					else
					{
						tripDetail.offsetFromLast = 0;
					}
					stringstream arr_stream(_getValue("arrival_time"));
					if(arr_stream.str() != "" && arr_stream >> tripDetail.arrivalTime) // Invalid time duration
					{
						if(tripDetail.arrivalTime.seconds())
						{
							tripDetail.arrivalTime += seconds(60 - tripDetail.arrivalTime.seconds());
						}
						previousArrivalTime = tripDetail.arrivalTime;
					}
					else  // Invalid time duration
					{
						tripDetail.arrivalTime = previousArrivalTime; // Copy previous regulation stop
					}

					stringstream dep_stream(_getValue("departure_time"));
					if(dep_stream.str() != "" && dep_stream >> tripDetail.departureTime)
					{
						if(tripDetail.departureTime.seconds())
						{
							tripDetail.departureTime -= seconds(tripDetail.departureTime.seconds());
						}
						previousDepartureTime = tripDetail.departureTime;
					}
					else // Invalid time duration
					{
						tripDetail.departureTime = previousDepartureTime; // Copy previous regulation stop
					}

					string stopCode(_getValue("stop_id"));
					if(!_stopPoints.contains(stopCode))
					{
						_log(
							ImportLogger::WARN,
							"inconsistent stop id "+ stopCode +" in the trip "+ tripCode
						);
						continue;
					}
					tripDetail.stop = _stopPoints.get(stopCode);

					tripDetailVector.push_back(tripDetail);
					lastTripCode = tripCode;
				}
			}
			else if(key == FILE_FARE_ATTRIBUTES)
			{
				// TODO
			}
			else if(key == FILE_FARE_RULES)
			{
				// TODO
			}
			else if(key == FILE_SHAPES)
			{
				// TODO
			}
			else if(key == FILE_FREQUENCIES)
			{
				// TODO
			}
			return true;
		}



		db::DBTransaction GTFSFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			PTDataCleanerFileFormat::_addRemoveQueries(transaction);

			// Saving of each created or altered objects
			if(_importStopArea)
			{
				BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
				{
					StopAreaTableSync::Save(cstop.second.get(), transaction);
				}
			}
			BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
			{
				StopPointTableSync::Save(stop.second.get(), transaction);
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
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			return transaction;
		}



		void GTFSFileFormat::Importer_::_loadFieldsMap( const std::string& line ) const
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



		std::string GTFSFileFormat::Importer_::_getValue( const std::string& field ) const
		{
			return trim_copy(_line[_fieldsMap[field]]);
		}



		void GTFSFileFormat::Importer_::_loadLine( const std::string& line ) const
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

				tokenizer<escaped_list_separator<char> > tok(utfline, escaped_list_separator<char>('\\', ',', '\"'));
				for(tokenizer<escaped_list_separator<char> >::iterator beg=tok.begin(); beg!=tok.end(); ++beg)
				{
					_line.push_back(*beg);
				}
			}
		}



		util::ParametersMap GTFSFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());
			map.insert(PARAMETER_IMPORT_STOP_AREA, _importStopArea);
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(_defaultCity.get())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity->getKey());
			}
			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
			map.insert(PARAMETER_USE_RULE_BLOCK_ID_MASK, _serializePTUseRuleBlockMasks(_ptUseRuleBlockMasks));
			return map;
		}



		void GTFSFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
			_importStopArea = map.getDefault<bool>(PARAMETER_IMPORT_STOP_AREA, false);
			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
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
		}



		std::string GTFSFileFormat::Importer_::_serializePTUseRuleBlockMasks( const PTUseRuleBlockMasks& object )
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

		// PROVIDING ALL FILES

		util::ParametersMap GTFSFileFormat::Exporter_::getParametersMap() const
		{
			ParametersMap result;
			if (_network.get() != NULL)
			{
				result.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			return result;
		}

		void GTFSFileFormat::Exporter_::setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_network = TransportNetworkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), Env::GetOfficialEnv());
			}
			catch (...)
			{
				throw Exception("Transport network " + lexical_cast<string>(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID)) + " not found");
			}
		}

		RegistryKeyType GTFSFileFormat::Exporter_::_key(RegistryKeyType key,RegistryKeyType suffix) const
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

		string GTFSFileFormat::Exporter_::_Str(string str)const
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

		string GTFSFileFormat::Exporter_::_SubLine(string str)const
		{
			size_t lp = str.find(" (");

			if(lp == string::npos)
				return str;
			return str.substr (0, lp);

		}

		void GTFSFileFormat::Exporter_::_addShapes(const Path * path,
			RegistryKeyType shapeIdKey,
			stringstream& shapeTxt,
			stringstream& tripTxt,
			string tripName
		) const
		{
			if(shapeId.find(tripName) != shapeId.end())
			{
				tripTxt << shapeId[tripName];
				return;
			}

			int cpt_seq = 0;
			double lastx = 0.0;
			double lasty = 0.0;

			BOOST_FOREACH(Edge* edge, path->getAllEdges())
			{
				if( ! edge->getNext())
					break;

				boost::shared_ptr<geos::geom::LineString> lineStr = edge->getRealGeometry();

				if(!lineStr.get())continue;

				boost::shared_ptr<geos::geom::Geometry> prGeom(CoordinatesSystem::GetCoordinatesSystem(WGS84_SRID).convertGeometry(*lineStr));

				size_t nb_Points = lineStr->getNumPoints();

				for( size_t i=0; i< nb_Points; i++)
				{
					const Coordinate& pt(prGeom->getCoordinates()->getAt(i));

					if(( pt.y == lasty ) && (pt.x == lastx))
						continue;

					lastx = pt.x;
					lasty = pt.y;

					shapeTxt << shapeIdKey << ","
						<< pt.y << ","
						<< pt.x << ","
						<< cpt_seq++
						<< endl;
				}
			}
			shapeId[tripName]=shapeIdKey;
			tripTxt << shapeIdKey;
		}

		void GTFSFileFormat::Exporter_::_addTrips(stringstream& trips,
			RegistryKeyType tripId,
			RegistryKeyType service,
			RegistryKeyType route,
			string tripHeadSign
		) const
		{
			trips << tripId << "," // trip_id
				<< service << "," // service_id
				<< route << "," // route_id
				<< tripHeadSign << ","; // trip_head_sign
		}

		void GTFSFileFormat::Exporter_::_addCalendars( stringstream& calendar,
			stringstream& calendarDates,
			const SchedulesBasedService* service,
			RegistryKeyType serviceId,
			bool isContinuous
		) const
		{
			try
			{
				boost::gregorian::date currentDay, firstActiveDay, lastActiveDay;
				try
					{
					lastActiveDay = service->getLastActiveDate();
				}
				catch(...)
				{
					throw Exception("Service LastActiveDate is corrupted");
				}
				bool weekDays [7];

				try
				{
					currentDay = firstActiveDay = service->getFirstActiveDate();
				}
				catch(...)
				{
					throw Exception("Service FirstActiveDate is corrupted");
				}

				boost::gregorian::date::day_of_week_type dayOfWeek = firstActiveDay.day_of_week();
				unsigned int firstActiveDayIndex = (dayOfWeek + 6) % 7; // 0 -> Mon; 1 -> Tues; ...; 6 -> Sun

				// 0 -> Mon; 1 -> Tues; ...; 6 -> Sun
				for(int i = 0; i<7; i++)
				{
					weekDays[(i+firstActiveDayIndex) % 7] = service->isActive(firstActiveDay + date_duration(i));
				}

				calendar << serviceId << ",";

				for(int i=0; i<7; i++)
				{
					calendar << weekDays[i] << ",";
				}

				calendar << to_iso_string(service->getFirstActiveDate()) << ","
					<< to_iso_string(service->getLastActiveDate())
					<< endl;

				// END CALENDAR.TXT

				// BEGIN CALENDAR_DATES.TXT
				for(int i = 0; currentDay <= lastActiveDay;i++)
				{
					bool isNormalyActive = weekDays[(firstActiveDayIndex + i) % 7];
					if(isNormalyActive != service->isActive(currentDay))
					{
						calendarDates << serviceId << ","
							<< to_iso_string(currentDay) << ","
							<< (2 - service->isActive(currentDay))
							<< endl;
					}
					currentDay += date_duration(1);
				}
			}
			catch(const Exception & e)
			{
				throw Exception("Exception when wrinting calendars for service " + lexical_cast<string>(serviceId) + " : " + e.getMessage());
			}
			catch(...)
			{
				throw Exception("Unknown exception when wrinting calendars for service " + lexical_cast<string>(serviceId));
			}
		}

		void GTFSFileFormat::Exporter_::_addFrequencies(stringstream& frequencies,
			RegistryKeyType tripId,
			const ContinuousService* service
		) const
		{
			boost::posix_time::time_duration headway = service->getMaxWaitingTime ();
			boost::posix_time::time_duration DBSTI = service->getDepartureBeginScheduleToIndex(false, 0);
			boost::posix_time::time_duration DESTI = service->getDepartureEndScheduleToIndex(false, 0);

			frequencies << tripId << "," // trip_id
				<< DBSTI << "," // start_time
				<< DESTI << "," // start_time
				<< headway.total_seconds() // headway_secs
				<< endl;
		}

		void GTFSFileFormat::Exporter_::_addStopTimes(stringstream& stopTimes,
			const LineStopTableSync::SearchResult linestops,
			const SchedulesBasedService* service,
			bool& stopTimesExist,
			bool isContinuous
		) const
		{
			bool passMidnight = false;

			BOOST_FOREACH(const boost::shared_ptr<LineStop>& ls, linestops)
			{
				boost::shared_ptr<geos::geom::Point> gp;
				string departureTimeStr;
				string arrivalTimeStr;
				boost::posix_time::time_duration arrival;
				boost::posix_time::time_duration departure;

				if (ls->getRankInPath() > 0 && ls->isArrival())
				{
					arrival = Service::GetTimeOfDay(service->getArrivalBeginScheduleToIndex(false, ls->getRankInPath()));
				}
				else
				{
					arrival = Service::GetTimeOfDay(service->getDepartureBeginScheduleToIndex(false, ls->getRankInPath()));
				}

				if (ls->getRankInPath()+1 != linestops.size() && ls->isDeparture())
				{
					departure = Service::GetTimeOfDay(service->getDepartureBeginScheduleToIndex(false, ls->getRankInPath()));
				}
				else
				{
					departure = Service::GetTimeOfDay(service->getArrivalBeginScheduleToIndex(false, ls->getRankInPath()));
				}

				// Correct trips over midnight
				if(arrival.hours() >= 22 || departure.hours() >= 22)
				passMidnight = true;

				if(passMidnight)
				{
					if(arrival.hours() < 12)
						arrival = arrival + hours(24);
					if(departure.hours() < 12)
						departure = departure + hours(24);
				}

				boost::posix_time::time_duration diff = arrival - departure;

				if((diff.hours() == 0) && (diff.minutes() > 0))
				{
					departure = arrival;
				}

				arrivalTimeStr = to_simple_string(arrival);
				departureTimeStr = to_simple_string(departure);
				const StopPoint * stopPoint(static_cast<const StopPoint *>(ls->getFromVertex()));

				if(stopPoint->hasGeometry())
				{
					gp = CoordinatesSystem::GetCoordinatesSystem(WGS84_SRID).convertPoint(*stopPoint->getGeometry());
				}

				if(gp.get())
				{
					stopTimes <<_key(service->getKey(), 1) << ","
						<< _key(stopPoint->getKey()) << ","
						<< ls->getRankInPath() << ","
						<< arrivalTimeStr.substr(0, 8) << ","
						<< departureTimeStr.substr(0, 8) << ","
						<< ","
						<< ","
						<< ","
						<< endl;
					stopTimesExist = true;
				}
			}
		}

		void GTFSFileFormat::Exporter_::_filesProvider(const SchedulesBasedService* service,
			stringstream& stopTimesTxt,
			stringstream& tripsTxt,
			stringstream& shapesTxt,
			stringstream& calendarTxt,
			stringstream& calendarDatesTxt,
			stringstream& frequenciesTxt,
			list < pair<const Calendar *, RegistryKeyType> > & calendarMap,
			bool isContinuous
		) const
		{
			try
			{
				RegistryKeyType serviceKey;
				RegistryKeyType routeId;
				string tripHeadSign,journeyName;
				bool stopTimesExist = false;

				routeId = _key(static_cast<const JourneyPattern *>(&(*service->getPath()))->getCommercialLine()->getKey());

				const JourneyPattern * line = static_cast<const JourneyPattern *>(&(*service->getPath()));

				string lineDirection(
					line->getDirection().empty() && line->getDirectionObj() ?
					line->getDirectionObj()->getDisplayedText() :
					line->getDirection()
				);
				tripHeadSign = _Str(lineDirection.empty() ? line->getDestination()->getConnectionPlace()->getFullName() : lineDirection);

				journeyName = _SubLine(_Str(line->getName()));

				RegistryKeyType tripId = _key(service->getKey(), 1);
				const Path * path = service->getPath();

				// BEGIN STOP_TIMES.TXT 1/2

				LineStopTableSync::SearchResult lineStops(LineStopTableSync::Search(_env, service->getPath()->getKey()));

				_addStopTimes(stopTimesTxt,
					lineStops,
					service,
					stopTimesExist,
					isContinuous
				);

				// END STOP_TIMES.TXT 1/2

				const Calendar * currentCal = static_cast<const Calendar *>(service);
				list <pair<const Calendar *, RegistryKeyType> >::iterator itCal = calendarMap.begin();
				bool alreadyExist = false;
				while(itCal != calendarMap.end())
				{
					if(*(itCal->first) == *currentCal)
					{
						alreadyExist = true;
						break;
					}
					itCal++;
				}

				if(!alreadyExist)
				{
					serviceKey = _key(service->getKey());

					calendarMap.push_back(make_pair(currentCal, serviceKey));

					// BEGIN TRIPS.TXT 1.1
					// trip_id,service_id,route_id,trip_headsign
					if(stopTimesExist) // only trips wich have stops_times will be added
					{
						if(isContinuous)
						{
							// BEGIN FREQUENCIES.TXT 1_2

							_addFrequencies(frequenciesTxt, tripId, static_cast<const ContinuousService *>(service));

							// END FREQUENCIES.TXT 1_2
						}

						_addTrips(tripsTxt, tripId, serviceKey, routeId, tripHeadSign);

						// BEGIN SHAPES.TXT 1.1

						_addShapes(path, tripId, shapesTxt, tripsTxt, journeyName);

						// END SHAPES.TXT 1.1

						tripsTxt << endl;

						// END TRIPS.TXT 1.1
					}

					// BEGIN CALENDAR.TX & CALENDAR_DATES 1/2

					_addCalendars(calendarTxt, calendarDatesTxt, service, serviceKey, isContinuous);

					// END CALENDAR.TX & CALENDAR_DATES 1/2
				}
				else
				{
					serviceKey = itCal->second;

					// BEGIN TRIPS.TXT 1.2
					// trip_id,service_id,route_id,trip_headsign
					if(stopTimesExist)
					{
						_addTrips(tripsTxt, tripId, serviceKey, routeId, tripHeadSign);

						if(isContinuous)
						{
							// BEGIN FREQUENCIES.TXT 2_2

							_addFrequencies(frequenciesTxt, tripId, static_cast<const ContinuousService *>(service));

							// END FREQUENCIES.TXT 2_2
						}

						// BEGIN SHAPES.TXT 1.2

						_addShapes(path, tripId, shapesTxt, tripsTxt, journeyName);

						// END SHAPES.TXT 1.2

						tripsTxt << endl;
					}
					// END TRIPS.TXT
				}
			}
			catch (const Exception & e)
			{
				Log::GetInstance().warn("Exception in GTFSFileFormat::Exporter_::_filesProvider: " + e.getMessage() + ", service will be ignored !");
				throw Exception("Exception in GTFSFileFormat::Exporter_::_filesProvider: " + e.getMessage() + ", service will be ignored !");
			}
			catch (...)
			{
				throw Exception("Unknown Exception in GTFSFileFormat::Exporter_::_filesProvider");
			}
		}

		// EXPORTER_::BUILD

		void GTFSFileFormat::Exporter_::build(ostream& os) const
		{
			stringstream agencyTxt;
			stringstream stopsTxt;
			stringstream routesTxt;
			stringstream tripsTxt;
			stringstream stopTimesTxt;
			stringstream calendarTxt;
			stringstream calendarDatesTxt;
			stringstream shapesTxt;
			stringstream frequenciesTxt;
			stringstream transfersTxt;

			// Add header line to each file
			agencyTxt << "agency_id,agency_name,agency_url,agency_timezone,agency_phone,agency_lang" << endl;
			stopsTxt << "stop_id,stop_code,stop_name,stop_lat,stop_lon,location_type,parent_station" << endl;
			routesTxt << "route_id,agency_id,route_short_name,route_long_name,route_desc,route_type,route_url,route_color,route_text_color" << endl;
			tripsTxt << "trip_id,service_id,route_id,trip_headsign,shape_id" << endl;
			stopTimesTxt << "trip_id,stop_id,stop_sequence,arrival_time,departure_time,stop_headsign,pickup_type,drop_off_type,shape_dist_traveled" << endl;
			calendarTxt << "service_id,monday,tuesday,wednesday,thursday,friday,saturday,sunday,start_date,end_date" << endl;
			calendarDatesTxt << "﻿service_id,date,exception_type" << endl;
			shapesTxt << "shape_id,shape_pt_lat,shape_pt_lon,shape_pt_sequence" << endl;
			frequenciesTxt << "trip_id,start_time,end_time,headway_secs" << endl;
			transfersTxt << ".::. NOT YET .::." <<endl;

			// BEGIN AGENCY.TXT
			BOOST_FOREACH(Registry<TransportNetwork>::value_type myAgency, Env::GetOfficialEnv().getRegistry<TransportNetwork>())
			{
				agencyTxt << _key(myAgency.first) << "," // agency_id
					<< _Str(myAgency.second->getName()) << "," // agency_name
					<< "," // agency_url
					<< "," // agency_timezone
					<< "," // agency_phone
					<< endl; // agency_lang
				}
				// END AGENCY.TXT

			// BEGIN STOPS.TXT
			BOOST_FOREACH(
				Registry<StopPoint>::value_type itps,
				Env::GetOfficialEnv().getRegistry<StopPoint>()
			){
				const StopPoint& stopPoint(*itps.second);
				if (stopPoint.getDepartureEdges().empty() && stopPoint.getArrivalEdges().empty()) continue;

				boost::shared_ptr<geos::geom::Point> gp;
				if(stopPoint.hasGeometry())
				{
					gp = CoordinatesSystem::GetCoordinatesSystem(WGS84_SRID).convertPoint(*stopPoint.getGeometry());
				}

				if(gp.get())
				{
					stopsTxt << _key(stopPoint.getKey()) << "," // stop_id
						<< stopPoint.getCodeBySources() << "," // stop_code
						<< _Str(((stopPoint.getName()) == "" ? stopPoint.getConnectionPlace()->getName():stopPoint.getName())) << "," // stop_name
						<< gp->getY() << "," // stop_lat
						<< gp->getX() << "," // stop_lon
						<< "0," // location_type
						<< _key(stopPoint.getConnectionPlace()->getKey(),2) // parent_station
						<< endl;
				}
			}

			BOOST_FOREACH(
				Registry<StopArea>::value_type itcp,
				Env::GetOfficialEnv().getRegistry<StopArea>()
			){
				const StopArea* connPlace(itcp.second.get());

				boost::shared_ptr<geos::geom::Point> gp;
				if(connPlace->getPoint().get() && !connPlace->getPoint()->isEmpty())
				{
					gp = CoordinatesSystem::GetCoordinatesSystem(WGS84_SRID).convertPoint(*connPlace->getPoint());
				}
				else // get first point coordinates
				{
					const StopArea::PhysicalStops& stops(connPlace->getPhysicalStops());
					if(!stops.empty())
					{
						if(stops.begin()->second->hasGeometry())
						{
							gp = CoordinatesSystem::GetCoordinatesSystem(WGS84_SRID).convertPoint(*stops.begin()->second->getGeometry());
						}
					}
				}

				if(gp.get())
				{
					stopsTxt << _key(connPlace->getKey(),2) << "," //stop_id
						<< "," //stop_code
						<< _Str(connPlace->getName()) << "," //stop_name
						<< gp->getY() << "," //stop_lat
						<< gp->getX() << "," //stop_lon
						<< "1," //location_type
						<< endl; //StopArea does not have parentStation !
				}
			}
			// END STOPS.TXT

			RollingStock * rs = NULL; 

			// BEGIN ROUTES.TXT
			BOOST_FOREACH(Registry<CommercialLine>::value_type  myLine,Env::GetOfficialEnv().getRegistry<CommercialLine>())
			{
				// route_id,agency_id,route_short_name,route_long_name,route_desc,route_type,route_url,route_color,route_text_color

				if((myLine.second->getPaths().begin()) != (myLine.second->getPaths().end()))
				{
					rs = static_cast<const JourneyPattern *>(*myLine.second->getPaths().begin())->getRollingStock();
					if((rs != NULL) && (rs->getIndicator()).find(LABEL_TAD) == string::npos)
					{
						string color;

						if(!(myLine.second->getColor()))
						{
							color = "";
						}
						else
						{
							color = _Str(myLine.second->getColor()->toXMLColor());

							if(color[0] == '#')
							{
								color = color.substr(1);
							}
						}
						routesTxt << _key(myLine.first) << "," //route_id
							<< _key(myLine.second->getNetwork()->getKey()) << "," //agency_id
							<< _Str(myLine.second->getShortName()) << "," //route_short_name
							<< _Str(myLine.second->getLongName()) << "," //route_long_name
							<< _Str(myLine.second->getRuleUserName()) << "," //route_desc
							<< static_cast<const JourneyPattern *>(*myLine.second->getPaths().begin())->getRollingStock()->getGTFSKey() << "," //route_type
							<< "," //route_url
							<< (color != "" ? color : "000000") << "," //route_color
							<< "FFFFFF" //route_text_color
							<< endl;
					}
				}
			}

			// END ROUTES.TXT

			// BEGIN STOP_TIMES.TXT + CALENDAR + TRIPS.TXT # SERVICES 1/2

			list < pair<const Calendar *, RegistryKeyType> > calendarMap;

			BOOST_FOREACH(Registry<ScheduledService>::value_type itsdsrv, Env::GetOfficialEnv().getRegistry<ScheduledService>())
			{
				const ScheduledService* sdService(itsdsrv.second.get());

				if(sdService)
				{
					rs = static_cast<const JourneyPattern *>(sdService->getPath())->getRollingStock();

					if((rs != NULL) && (rs->getIndicator()).find(LABEL_TAD) == string::npos)
					{
						_filesProvider(sdService,
							stopTimesTxt,
							tripsTxt,
							shapesTxt,
							calendarTxt,
							calendarDatesTxt,
							frequenciesTxt,
							calendarMap,
							false);
					}
				}
				else
				{
					synthese::util::Log::GetInstance().warn("Unavailable Scheduled Service");
					break;
				}
			}

			// END STOP_TIMES.TXT + CALENDAR + TRIPS.TXT # SERVICES 1/2

			// BEGIN STOP_TIMES.TXT + CALENDAR + TRIPS.TXT # SERVICES 2/2

			BOOST_FOREACH(Registry<ContinuousService>::value_type itcssrv, Env::GetOfficialEnv().getRegistry<ContinuousService>())
			{
				const ContinuousService* csService(itcssrv.second.get());

				if(csService)
				{
					rs = static_cast<const JourneyPattern *>(csService->getPath())->getRollingStock();

					if((rs != NULL) && (rs->getIndicator()).find(LABEL_TAD) == string::npos)
					{
						_filesProvider(csService,
							stopTimesTxt,
							tripsTxt,
							shapesTxt,
							calendarTxt,
							calendarDatesTxt,
							frequenciesTxt,
							calendarMap,
							true);
					}
				}
				else
				{
					synthese::util::Log::GetInstance().warn("Unavailable Continuous Service");
					break;
				}
			}

			// END CALENDAR & TRIPS.TXT # SERVICES

			ZipWriter * zip = new ZipWriter(os);

			zip->Write("agency.txt", agencyTxt);
			zip->Write("routes.txt", routesTxt);
			zip->Write("stops.txt", stopsTxt);
			zip->Write("trips.txt", tripsTxt);
			zip->Write("stop_times.txt", stopTimesTxt);
			zip->Write("calendar.txt", calendarTxt);
			zip->Write("calendar_dates.txt", calendarDatesTxt);
			zip->Write("shapes.txt", shapesTxt);
			zip->Write("frequencies.txt", frequenciesTxt);

			zip->WriteDirectory();

			os << flush;
		}
	}
}
