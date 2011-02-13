
/** GTFSFileFormat class implementation.
	@file GTFSFileFormat.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "TransportNetwork.h"
#include "StopArea.hpp"
#include "PTFileFormat.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ImpExModule.h"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace calendar;
	using namespace graph;
	
	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,GTFSFileFormat>::FACTORY_KEY("GTFS");
	}
	
	namespace pt
	{
		const std::string GTFSFileFormat::Importer_::FILE_AGENCY("0agency");
		const std::string GTFSFileFormat::Importer_::FILE_ROUTES("1routes");
		const std::string GTFSFileFormat::Importer_::FILE_CALENDAR("2calendar");
		const std::string GTFSFileFormat::Importer_::FILE_CALENDAR_DATES("3calendar_dates");
		const std::string GTFSFileFormat::Importer_::FILE_TRIPS("4trips");
		const std::string GTFSFileFormat::Importer_::FILE_STOP_TIMES("5stop_times");
		const std::string GTFSFileFormat::Importer_::FILE_FARE_ATTRIBUTES("6fare_attributes");
		const std::string GTFSFileFormat::Importer_::FILE_FARE_RULES("7fare_rules");
		const std::string GTFSFileFormat::Importer_::FILE_SHAPES("8shapes");
		const std::string GTFSFileFormat::Importer_::FILE_FREQUENCIES("9frequencies");
		const std::string GTFSFileFormat::Importer_::SEP(",");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<GTFSFileFormat>::Files MultipleFileTypesImporter<GTFSFileFormat>::FILES(
			GTFSFileFormat::Importer_::FILE_AGENCY.c_str(),
			GTFSFileFormat::Importer_::FILE_ROUTES.c_str(),
			GTFSFileFormat::Importer_::FILE_TRIPS.c_str(),
			GTFSFileFormat::Importer_::FILE_STOP_TIMES.c_str(),
			GTFSFileFormat::Importer_::FILE_CALENDAR.c_str(),
			GTFSFileFormat::Importer_::FILE_CALENDAR_DATES.c_str(),
			GTFSFileFormat::Importer_::FILE_FARE_ATTRIBUTES.c_str(),
			GTFSFileFormat::Importer_::FILE_FARE_RULES.c_str(),
			GTFSFileFormat::Importer_::FILE_SHAPES.c_str(),
			GTFSFileFormat::Importer_::FILE_FREQUENCIES.c_str(),
		"");
	}


	namespace pt
	{
		bool GTFSFileFormat::Importer_::_controlPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_AGENCY));
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
			const impex::DataSource& dataSource
		):	MultipleFileTypesImporter<GTFSFileFormat>(env, dataSource),
			_networks(_dataSource, _env),
			_stopPoints(_dataSource, _env),
			_lines(_dataSource, _env)
		{}



		bool GTFSFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& os,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> adminRequest
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

			// 1 : Routes
			if(key == FILE_AGENCY)
			{
				while(getline(inFile, line))
				{
					_loadLine(line);

					PTFileFormat::CreateOrUpdateNetwork(
						_networks,
						_getValue("agency_id"),
						_getValue("agency_name"),
						_dataSource,
						_env,
						os
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
					const TransportNetwork* network;
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
						os << "WARN : inconsistent network id "<< networkId <<" in the line "<< id <<"<br />";
						continue;
					}

					PTFileFormat::CreateOrUpdateLine(
						_lines,
						id,
						_getValue("route_long_name"),
						_getValue("route_short_name"),
						_getValue("route_color").size() == 7 ? optional<RGBColor>(RGBColor::FromXMLColor(_getValue("route_color"))) : optional<RGBColor>(),
						*network,
						_dataSource,
						_env,
						os
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
					date startDate(
						lexical_cast<int>(startDateStr.substr(0,4)),
						lexical_cast<int>(startDateStr.substr(4,2)),
						lexical_cast<int>(startDateStr.substr(6,2))
					);
					string endDateStr(_getValue("end_date"));
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
			else if(key == FILE_CALENDAR_DATES) //5
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
						os << "WARN : inconsistent line id "<< lineCode <<" in the trip "<< id <<"<br />";
						continue;
					}
					trip.line = *_lines.get(lineCode).begin();

					// Calendar
					string calendarCode(_getValue("service_id"));
					Calendars::const_iterator it(_calendars.find(calendarCode));
					if(it == _calendars.end())
					{
						os << "WARN : inconsistent service id "<< calendarCode <<" in the trip "<< id <<"<br />";
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
				TripDetailVector tripDetailVector;

				while(getline(inFile, line))
				{
					_loadLine(line);

					string tripCode(_getValue("trip_id"));
					if(tripCode != lastTripCode && !lastTripCode.empty())
					{
						// Trip
						TripsMap::const_iterator it(_trips.find(tripCode));
						if(it == _trips.end())
						{
							os << "WARN : inconsistent trip id "<< tripCode <<" in the trip stops file<br />";
							continue;
						}
						Trip trip(it->second);

						// Route
						JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
						Edge::MetricOffset offsetSum(0);
						BOOST_FOREACH(const TripDetail& tripStop, tripDetailVector)
						{
							offsetSum += tripStop.offsetFromLast;
							JourneyPattern::StopWithDepartureArrivalAuthorization stop(
								tripStop.stop,
								offsetSum
							);
							stops.push_back(stop);
						}
						
						JourneyPattern* route(
							PTFileFormat::CreateOrUpdateRoute(
								*trip.line,
								optional<const string&>(),
								optional<const string&>(),
								optional<const string&>(trip.destination),
								trip.direction,
								NULL,
								stops,
								_dataSource,
								_env,
								os
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
								tripCode,
								_dataSource,
								_env,
								os
						)	);
						*service |= trip.calendar;
						
						tripDetailVector.clear();
					}

					TripDetail tripDetail;
					tripDetail.offsetFromLast = lexical_cast<Edge::MetricOffset>(_getValue("shape_dist_traveled"));
					tripDetail.arrivalTime = duration_from_string(_getValue("arrival_time"));
					tripDetail.departureTime = duration_from_string(_getValue("departure_time"));

					string stopCode(_getValue("stop_id"));
					if(!_stopPoints.contains(stopCode))
					{
						os << "WARN : inconsistent stop id "<< stopCode <<" in the trip "<< tripCode <<"<br />";
						continue;
					}
					tripDetail.stop = _stopPoints.get(stopCode);
					
					tripDetailVector.push_back(tripDetail);
				}
			}
			else if(key == FILE_FARE_ATTRIBUTES)
			{
				//TODO
			}
			else if(key == FILE_FARE_RULES)
			{
				//TODO
			}
			else if(key == FILE_SHAPES)
			{
				//TODO
			}
			else if(key == FILE_FREQUENCIES)
			{
				//TODO
			}
			return true;
		}



		void GTFSFileFormat::Importer_::displayAdmin( std::ostream& os, const admin::AdminRequest& request ) const
		{

		}



		db::SQLiteTransaction GTFSFileFormat::Importer_::_save() const
		{
			SQLiteTransaction transaction;

			// Saving of each created or altered objects
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
			split(cols, line, is_any_of(SEP));
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
			string utfLine(ImpExModule::ConvertChar(line, _dataSource.getCharset(), "UTF-8"));
			split(_line, utfLine, is_any_of(SEP));
		}
}	}
