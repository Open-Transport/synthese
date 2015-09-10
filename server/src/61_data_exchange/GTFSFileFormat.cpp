
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
#include "ZipWriter.hpp"
#include "Path.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"

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
		const std::string GTFSFileFormat::Importer_::PARAMETER_AUTO_CREATE_STOP_AREA("auto_create_stop_area");
		const std::string GTFSFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string GTFSFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string GTFSFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const std::string GTFSFileFormat::Importer_::PARAMETER_USE_RULE_BLOCK_ID_MASK("use_rule_block_id_mask");
		const std::string GTFSFileFormat::Importer_::PARAMETER_USE_LINE_SHORT_NAME_AS_ID("use_line_short_name_as_id");
		const std::string GTFSFileFormat::Importer_::PARAMETER_IGNORE_SERVICE_NUMBER("ignore_service_number");
		const std::string GTFSFileFormat::Importer_::PARAMETER_IGNORE_DIRECTIONS("ignore_directions");

		const std::string GTFSFileFormat::Exporter_::PARAMETER_NETWORK_ID("ni");
		const std::string GTFSFileFormat::Exporter_::LABEL_TAD("tad");
		const std::string GTFSFileFormat::Exporter_::LABEL_NO_EXPORT_GTFS("NO Export GTFS");
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
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<GTFSFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_importStopArea(false),
			_autoCreateStopArea(false),
			_interactive(false),
			_displayLinkedStops(false),
			_useLineShortNameAsId(false),
			_ignoreServiceNumber(false),
			_ignoreDirections(false),
			_networks(*import.get<DataSource>(), env),
			_stopPoints(*import.get<DataSource>(), env),
			_lines(*import.get<DataSource>(), env)
		{}



		bool GTFSFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key
		) const {
			ifstream inFile;
			inFile.open(filePath.string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.string());
			}
			string line;
			if(!getline(inFile, line))
			{
				return false;
			}
			_loadFieldsMap(line);

			_logInfo("Loading file "+ filePath.string() +" as "+ key);

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

						// A stop area must be created for each stop that :
						// - is a stop area (location_type == 1)
						// - has no parent id (GTFS allows a stop to have no stop area, Synthese does not)
						if(_getValue("location_type") != "1" && ! _getValue("parent_station").empty())
						{
							continue;
						}

						string id(_getValue("stop_id"));
						string name(_getValue("stop_name"));

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
						_createOrUpdateStopAreas(
							stopAreas,
							id,
							name,
							_defaultCity.get(),
							false,
							_stopAreaDefaultTransferDuration,
							dataSource
						);
					}

					_exportStopAreas(
						nonLinkedStopAreas
					);
					if(_displayLinkedStops)
					{
						_exportStopAreas(
							linkedStopAreas
						);
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
					if(_getValue("location_type") != "0" && !_autoCreateStopArea)
					{
						continue;
					}

					string id(_getValue("stop_id"));
					string name(_getValue("stop_name"));

					string stopAreaId;

					// Stop area
					// If the stop has no parent station, use its own ID as parent station (because it has been added as such)
					if(_fieldsMap.find("parent_station") != _fieldsMap.end())
						stopAreaId = _getValue("parent_station");
					else
						stopAreaId = id;

					const StopArea* stopArea(NULL);
					if(stopAreas.contains(stopAreaId))
					{
						stopArea = *stopAreas.get(stopAreaId).begin();
					}
					else if(_stopPoints.contains(id))
					{
						stopArea = (*_stopPoints.get(id).begin())->getConnectionPlace();
					}
					else if (!_autoCreateStopArea)
					{
						_logWarning(
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
					isp.name = name;
					isp.linkedStopPoints = _stopPoints.get(id);
					isp.stopArea = stopArea;
					isp.coords = point;

					if(isp.linkedStopPoints.empty())
					{
						nonLinkedStopPoints.insert(
							make_pair(id, isp)
						);
					}
					else if(_displayLinkedStops)
					{
						linkedStopPoints.insert(
							make_pair(id, isp)
						);
					}
					// Creation or update
					if (_autoCreateStopArea)
					{
						_createOrUpdateStopWithStopAreaAutocreation(
							_stopPoints,
							id,
							name,
							point.get(),
							*_defaultCity.get(),
							_stopAreaDefaultTransferDuration,
							dataSource,
							optional<const RuleUser::Rules&>()
						);
					}
					else
					{
						_createOrUpdateStop(
							_stopPoints,
							id,
							name,
							optional<const RuleUser::Rules&>(),
							stopArea,
							point.get(),
							dataSource
						);
					}
				}

				_exportStopPoints(
					nonLinkedStopPoints
				);
				if(_displayLinkedStops)
				{
					_exportStopPoints(
						linkedStopPoints
					);
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

					_createOrUpdateNetwork(
						_networks,
						_getValue("agency_id"),
						_getValue("agency_name"),
						dataSource,
						_getValue("agency_url"),
						_getValue("agency_timezone"),
						_getValue("agency_phone"),
						_getValue("agency_lang"),
						_getValue("agency_fare_url")
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
					if (_useLineShortNameAsId)
					{
						id = _getValue("route_short_name");
					}
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
						_logWarning(
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

					std::string longName(_getValue("route_long_name"));
					std::string shortName(_getValue("route_short_name"));
					_createOrUpdateLine(
						_lines,
						id,
						longName,
						shortName,
						color,
						*network,
						dataSource
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
						_logWarning(
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
						_logWarning(
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
					if (_useLineShortNameAsId)
					{
						vector<string> splitRouteId;
						split(splitRouteId,lineCode,is_any_of("-"));
						lineCode = splitRouteId[0];
					}
					if(!_lines.contains(lineCode))
					{
						_logWarning(
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
						_logWarning(
							"Inconsistent service id "+ calendarCode +" in the trip "+ id
						);
						continue;
					}
					trip.calendar = it->second;

					// Destination
					if (!_ignoreDirections)
					{
						trip.destination = _getValue("trip_headsign");
					}
					else
					{
						trip.destination = "";
					}

					// Direction
					if (_fieldsMap.find("direction_id") != _fieldsMap.end())
					{
						trip.direction = lexical_cast<bool>(_getValue("direction_id"));
					}
					else
					{
						trip.direction = false;
					}

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
							_logWarning(
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
								offsetSum,
								true,
								true,
								tripStop.scheduledStop
							);
							stops.push_back(stop);
						}

						// Use rules
						RuleUser::Rules rules(RuleUser::GetEmptyRules());
						rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = trip.useRule;

						JourneyPattern* route(
							_createOrUpdateRoute(
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
								true,
								true,
								true,
								true
						)	);

						// Service
						ScheduledService::Schedules departures;
						BOOST_FOREACH(const TripDetail& tripStop, tripDetailVector)
						{
							if (tripStop.scheduledStop)
							{
								departures.push_back(tripStop.departureTime);
							}
						}
						ScheduledService::Schedules arrivals;
						BOOST_FOREACH(const TripDetail& tripStop, tripDetailVector)
						{
							if (tripStop.scheduledStop)
							{
								arrivals.push_back(tripStop.arrivalTime);
							}
						}

						ScheduledService* service(
							_createOrUpdateService(
								*route,
								departures,
								arrivals,
								_ignoreServiceNumber ? string() : lastTripCode,
								dataSource,
								optional<const string&>(),
								optional<const RuleUser::Rules&>(),
								optional<const JourneyPattern::StopsWithDepartureArrivalAuthorization&>(stops),
								lastTripCode
						)	);
						if(service)
						{
							*service |= trip.calendar;
						}

						tripDetailVector.clear();
					}

					TripDetail tripDetail;
					if(_fieldsMap.find("shape_dist_traveled") != _fieldsMap.end() &&
						_getValue("shape_dist_traveled") != "")
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
						tripDetail.arrivalTime = not_a_date_time;
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
						tripDetail.departureTime = not_a_date_time;
					}
					
					if (tripDetail.arrivalTime.is_not_a_date_time() &&
						tripDetail.departureTime.is_not_a_date_time())
					{
						tripDetail.scheduledStop = false;
					}
					else
					{
						tripDetail.scheduledStop = true;
					}

					string stopCode(_getValue("stop_id"));
					if(!_stopPoints.contains(stopCode))
					{
						_logWarning(
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
			if(_importStopArea || _autoCreateStopArea)
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
			BOOST_FOREACH(const Registry<ReservationContact>::value_type& contact, _env.getRegistry<ReservationContact>())
			{
				ReservationContactTableSync::Save(contact.second.get(), transaction);
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
			// Check if the field exists, as some fields are not mandatory in GTFS
			if(_fieldsMap.count(field))
			{
				std::size_t pos = _fieldsMap[field];

				// If the field exists in the header but not in the line, just return an empty string
				if(pos >= _line.size())
				{
					return "";
				}
				else
				{
					return trim_copy(_line[pos]);
				}
			}
			else
			{
				return "";
			}
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

				if(_line.size() != _fieldsMap.size())
				{
					_logWarning("Number of fields does not corresponds to header for line '"+line+"'");
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
			map.insert(PARAMETER_USE_LINE_SHORT_NAME_AS_ID, _useLineShortNameAsId);
			map.insert(PARAMETER_IGNORE_SERVICE_NUMBER, _ignoreServiceNumber);
			map.insert(PARAMETER_IGNORE_DIRECTIONS, _ignoreDirections);
			return map;
		}



		void GTFSFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
			_importStopArea = map.getDefault<bool>(PARAMETER_IMPORT_STOP_AREA, false);
			_autoCreateStopArea = map.getDefault<bool>(PARAMETER_AUTO_CREATE_STOP_AREA, false);
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
			
			_useLineShortNameAsId = map.getDefault<bool>(PARAMETER_USE_LINE_SHORT_NAME_AS_ID, false);
			_ignoreServiceNumber = map.getDefault<bool>(PARAMETER_IGNORE_SERVICE_NUMBER, false);
			_ignoreDirections = map.getDefault<bool>(PARAMETER_IGNORE_DIRECTIONS, false);
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

			// Test if at least one point has been added for this shape
			bool hasAtLeastOnePoint = false;

			BOOST_FOREACH(Edge* edge, path->getEdges())
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

					hasAtLeastOnePoint = true;
				}
			}

			if (hasAtLeastOnePoint)
			{
				shapeId[tripName]=shapeIdKey;
				tripTxt << shapeIdKey;
			} else {
				Log::GetInstance().debug("Missing shape for trip " + tripName);
			}
		}

		void GTFSFileFormat::Exporter_::_addTrips(stringstream& trips,
			RegistryKeyType tripId,
			RegistryKeyType service,
			RegistryKeyType route,
			string tripHeadSign,
			bool tripDirection
		) const
		{
			trips << tripId << "," // trip_id
				<< service << "," // service_id
				<< route << "," // route_id
				<< tripHeadSign << "," // trip_head_sign
				<< tripDirection << ","; // direction_id
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
			bool isContinuous,
			bool isReservationMandandatory
		) const
		{
			BOOST_FOREACH(const boost::shared_ptr<LineStop>& ls, linestops)
			{
				boost::shared_ptr<geos::geom::Point> gp;
				string departureTimeStr;
				string arrivalTimeStr;
				boost::posix_time::time_duration arrival;
				boost::posix_time::time_duration departure;

				if (ls->get<RankInPath>() > 0 && ls->get<IsArrival>())
				{
					arrival = service->getArrivalBeginScheduleToIndex(false, ls->get<RankInPath>());
				}
				else
				{
					arrival = service->getDepartureBeginScheduleToIndex(false, ls->get<RankInPath>());
				}

				if (ls->get<RankInPath>()+1 != linestops.size() && ls->get<IsDeparture>())
				{
					departure = service->getDepartureBeginScheduleToIndex(false, ls->get<RankInPath>());
				}
				else
				{
					departure = service->getArrivalBeginScheduleToIndex(false, ls->get<RankInPath>());
				}

				boost::posix_time::time_duration diff = arrival - departure;

				if((diff.hours() == 0) && (diff.minutes() > 0))
				{
					departure = arrival;
				}

				arrivalTimeStr = to_simple_string(arrival);
				departureTimeStr = to_simple_string(departure);
				const StopPoint * stopPoint(dynamic_cast<const StopPoint *>(&*ls->get<LineNode>()));

				if (stopPoint)
				{

					if(stopPoint->hasGeometry())
					{
						gp = CoordinatesSystem::GetCoordinatesSystem(WGS84_SRID).convertPoint(*stopPoint->getGeometry());
					}
					if(gp.get())
					{
						stopTimes <<_key(service->getKey(), 1) << ","
							<< _key(stopPoint->getKey()) << ","
							<< ls->get<RankInPath>() << ","
							<< arrivalTimeStr.substr(0, 8) << ","
							<< departureTimeStr.substr(0, 8) << ","
							<< ","
							<< (ls->get<IsDeparture>() ? (isReservationMandandatory ? "2," : "0,") : "1,") // pickup_type
							<< (ls->get<IsArrival>() ? (isReservationMandandatory ? "2," : "0,") : "1,") // drop_off_type
							<< endl;
						stopTimesExist = true;
					}
				}
				else
				{
					Log::GetInstance().debug("No stop point at rank " + boost::lexical_cast<std::string>(ls->get<RankInPath>()) +
											 " for line stop " + boost::lexical_cast<std::string>(ls->getKey()));
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
			bool tripDirection;

			routeId = _key(static_cast<const JourneyPattern *>(&(*service->getPath()))->getCommercialLine()->getKey());

			const JourneyPattern * line = static_cast<const JourneyPattern *>(&(*service->getPath()));

			string lineDirection(
				line->getDirection().empty() && line->getDirectionObj() ?
				line->getDirectionObj()->get<DisplayedText>() :
				line->getDirection()
			);
			tripHeadSign = _Str(lineDirection.empty() ? line->getDestination()->getConnectionPlace()->getFullName() : lineDirection);

			tripDirection = !line->getWayBack(); 

			journeyName = _SubLine(_Str(line->getName()));

			RegistryKeyType tripId = _key(service->getKey(), 1);
			const Path * path = service->getPath();

			// BEGIN STOP_TIMES.TXT 1/2

			LineStopTableSync::SearchResult lineStops(LineStopTableSync::Search(_env, service->getPath()->getKey()));

			bool isReservationMandandatory = false;
			RollingStock * rs = line->getRollingStock();
			if ((rs != NULL) && (rs->get<IndicatorLabel>()).find(LABEL_TAD) != string::npos)
				isReservationMandandatory = true;

			_addStopTimes(stopTimesTxt,
				lineStops,
				service,
				stopTimesExist,
				isContinuous,
				isReservationMandandatory
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

					_addTrips(tripsTxt, tripId, serviceKey, routeId, tripHeadSign, tripDirection);

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
					_addTrips(tripsTxt, tripId, serviceKey, routeId, tripHeadSign, tripDirection);

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
			agencyTxt << "agency_id,agency_name,agency_url,agency_fare_url,agency_timezone,agency_phone,agency_lang" << endl;
			stopsTxt << "stop_id,stop_code,stop_name,stop_lat,stop_lon,location_type,parent_station" << endl;
			routesTxt << "route_id,agency_id,route_short_name,route_long_name,route_desc,route_type,route_url,route_color,route_text_color" << endl;
			tripsTxt << "trip_id,service_id,route_id,trip_headsign,direction_id,shape_id" << endl;
			stopTimesTxt << "trip_id,stop_id,stop_sequence,arrival_time,departure_time,stop_headsign,pickup_type,drop_off_type,shape_dist_traveled" << endl;
			calendarTxt << "service_id,monday,tuesday,wednesday,thursday,friday,saturday,sunday,start_date,end_date" << endl;
			calendarDatesTxt << "﻿service_id,date,exception_type" << endl;
			shapesTxt << "shape_id,shape_pt_lat,shape_pt_lon,shape_pt_sequence" << endl;
			frequenciesTxt << "trip_id,start_time,end_time,headway_secs" << endl;
			transfersTxt << ".::. NOT YET .::." <<endl;

			// Clear the path ids
			shapeId.clear();

			// BEGIN AGENCY.TXT
			BOOST_FOREACH(Registry<TransportNetwork>::value_type myAgency, Env::GetOfficialEnv().getRegistry<TransportNetwork>())
			{
				// If a fare contact and/or a contact exists, use them
				std::string url = myAgency.second->get<Contact>() ? myAgency.second->get<Contact>()->get<WebsiteURL>() : "";
				std::string phone = myAgency.second->get<Contact>() ? myAgency.second->get<Contact>()->get<PhoneExchangeNumber>() : "";
				std::string fareUrl = myAgency.second->get<FareContact>() ? myAgency.second->get<FareContact>()->get<WebsiteURL>() : "";

				agencyTxt << _key(myAgency.first) << "," // agency_id
					<< _Str(myAgency.second->getName()) << "," // agency_name
					<< url << "," // agency_url
					<< fareUrl << "," // agency_fare_url
					<< myAgency.second->get<Timezone>() <<"," // agency_timezone
					<< phone <<"," // agency_phone
					<< myAgency.second->get<Lang>() // agency_lang
					<< endl;
			}
			// END AGENCY.TXT

			// BEGIN STOPS.TXT
			BOOST_FOREACH(
				Registry<StopPoint>::value_type itps,
				Env::GetOfficialEnv().getRegistry<StopPoint>()
			){
				const StopPoint& stopPoint(*itps.second);
				if (stopPoint.getDepartureEdges().empty() && stopPoint.getArrivalEdges().empty()) 
				{
					LineStopTableSync::SearchResult lineStops(LineStopTableSync::Search(_env, boost::optional<RegistryKeyType>(), stopPoint.getKey()));	
					if (lineStops.empty()) {
						Log::GetInstance().debug("Stop point has no departure/arrival egdes and is not on a trip : " + boost::lexical_cast<std::string>(stopPoint.getKey()));
						continue;
					}
				}

				boost::shared_ptr<geos::geom::Point> gp;
				if(stopPoint.hasGeometry())
				{
					gp = CoordinatesSystem::GetCoordinatesSystem(WGS84_SRID).convertPoint(*stopPoint.getGeometry());
				}

				if(gp.get())
				{
					/* GTFS Format will match commas in operatorCode field as delimiter */
					std::string operatorCodes = stopPoint.getCodeBySources();
					std::replace(operatorCodes.begin(), operatorCodes.end(), ',', '|');
					
					stopsTxt << _key(stopPoint.getKey()) << "," // stop_id
						<< operatorCodes << "," // stop_code
						<< _Str(((stopPoint.getName()) == "" ? stopPoint.getConnectionPlace()->getName():stopPoint.getName())) << "," // stop_name
						<< gp->getY() << "," // stop_lat
						<< gp->getX() << "," // stop_lon
						<< "0," // location_type
						<< _key(stopPoint.getConnectionPlace()->getKey(),2) // parent_station
						<< endl;
				} else {
					Log::GetInstance().debug("Stop point has no geometry : " + boost::lexical_cast<std::string>(stopPoint.getKey()) + " (" + stopPoint.getName() + ")");
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
				} else {
					Log::GetInstance().debug("Stop area and its first stop point have no geometry : " + boost::lexical_cast<std::string>(connPlace->getKey()) + " (" + connPlace->getName() + ")");
				}
			}
			// END STOPS.TXT

			RollingStock * rs = NULL; 

			// BEGIN ROUTES.TXT
			BOOST_FOREACH(Registry<CommercialLine>::value_type  myLine,Env::GetOfficialEnv().getRegistry<CommercialLine>())
			{
				// route_id,agency_id,route_short_name,route_long_name,route_desc,route_type,route_url,route_color,route_text_color

				// Check if Commercial Line musn't exported
				bool mustBeExported = true;
				for (multimap<const DataSource*, string>::const_iterator it= myLine.second->getDataSourceLinks().begin(); it != myLine.second->getDataSourceLinks().end(); ++it) 
				{
					std::string exp_name = it->first->get<Name>();
					Log::GetInstance().debug("Commercial line " + myLine.second->getShortName() + " has datasource " + exp_name + " - " + it->second);
					if(exp_name == LABEL_NO_EXPORT_GTFS)
					{
						mustBeExported = false;
						break;
					}
				}
				if(!mustBeExported) {
					Log::GetInstance().debug("Commercial line has the no export label : " + myLine.second->getShortName() + " (" + boost::lexical_cast<std::string>(myLine.first) + ")");
					continue;
				}

				if((myLine.second->getPaths().begin()) != (myLine.second->getPaths().end()))
				{
					// Take the rolling stock (vehicle type) of the first line in the current commercial line
					// All of the lines of a commercial line does not seems to have a rolling stock. This will create random errors.
					rs = static_cast<const JourneyPattern *>(*myLine.second->getPaths().begin())->getRollingStock();
					if(rs != NULL)
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
							<< rs->getGTFSKey() << "," //route_type
							<< "," //route_url
							<< (color != "" ? color : "000000") << "," //route_color
							<< "FFFFFF" //route_text_color
							<< endl;
					} else {
						Log::GetInstance().debug("No rolling stock for commercial line " + boost::lexical_cast<std::string>(myLine.first) + " (" + myLine.second->getShortName() + ")");
					}
				} else {
					Log::GetInstance().debug("Path have the same beginning and end for commercial line " + boost::lexical_cast<std::string>(myLine.first) + " (" + myLine.second->getShortName() + ")");
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
					const JourneyPattern *sdJourney = static_cast<const JourneyPattern *>(sdService->getPath());
					if (sdJourney)
					{
						rs = sdJourney->getRollingStock();

						// Check if one of the datasources has LABEL_NO_EXPORT_GTFS for name, in which case the schedule is not to be exported
						bool mustBeExported = true;
						const CommercialLine* sdCommercial = sdJourney->getCommercialLine();
						const multimap<const DataSource*, string>& dataSourcesMap = sdCommercial->getDataSourceLinks();
						for (multimap<const DataSource*, string>::const_iterator it = dataSourcesMap.begin(); it != dataSourcesMap.end(); ++it)
						{
							const DataSource* ds = it->first;
							std::string name = ds->get<Name>();
							if(name == LABEL_NO_EXPORT_GTFS)
							{
								mustBeExported = false;
								break;
							}
						}

						if(!mustBeExported) {
							Log::GetInstance().debug(
									"Scheduled service has the no export label : " +
									boost::lexical_cast<std::string>(itsdsrv.first) +
									" (line " + sdJourney->getCommercialLine()->getName() + ")"
							);
							continue;
						}

						// Check if the service is accessible at all
						const PTUseRule* PedestrianUseRule = dynamic_cast<const PTUseRule*>(
							&(sdService)->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
						);

						if ( PedestrianUseRule && PedestrianUseRule->getAccessCapacity().get_value_or(9999) == 0 ) {
							Log::GetInstance().debug(
									"Scheduled service is not accessible for pedestrian: " +
									boost::lexical_cast<std::string>(itsdsrv.first) +
									" (line " + sdJourney->getCommercialLine()->getName() + ")"
							);
							continue;
						}

						// Check if it has a rolling stock
						if(rs != NULL)
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
						} else {
							Log::GetInstance().debug(
									"Scheduled service has no rolling stock : " +
									boost::lexical_cast<std::string>(itsdsrv.first) +
									" (line " + sdJourney->getCommercialLine()->getName() + ")"
							);
						}
					} else {
						Log::GetInstance().debug("Scheduled service has no path : " + boost::lexical_cast<std::string>(itsdsrv.first));
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
					bool mustBeExported = true;
					const multimap<const DataSource*, string>& dataSourcesMap = static_cast<const JourneyPattern *>(csService->getPath())->getCommercialLine()->getDataSourceLinks();
					for (multimap<const DataSource*, string>::const_iterator it = dataSourcesMap.begin(); it != dataSourcesMap.end(); ++it)
					{
						if(it->first->getName() == LABEL_NO_EXPORT_GTFS)
						{
							mustBeExported = false;
							break;
						}
					}

					if(!mustBeExported) {
						Log::GetInstance().debug("Continuous service has the no export label : " + boost::lexical_cast<std::string>(itcssrv.first));
						continue;
					}

					rs = static_cast<const JourneyPattern *>(csService->getPath())->getRollingStock();
					if(rs != NULL)
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
					} else {
						Log::GetInstance().debug("Continuous service has no rolling stock : " + boost::lexical_cast<std::string>(itcssrv.first));
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



		GTFSFileFormat::Exporter_::Exporter_(
			const impex::Export& export_
		):	OneFileExporter<GTFSFileFormat>(export_)
		{}
}	}
