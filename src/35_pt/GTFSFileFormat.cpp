
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
#include "ImpExModule.h"
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
	using namespace html;
	using namespace admin;
	using namespace server;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,GTFSFileFormat>::FACTORY_KEY("GTFS");
	}

	namespace pt
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


	namespace pt
	{
		bool GTFSFileFormat::Importer_::_controlPathsMap() const
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
			const impex::DataSource& dataSource
		):	MultipleFileTypesImporter<GTFSFileFormat>(env, dataSource),
			Importer(env, dataSource),
			_importStopArea(false),
			_interactive(false),
			_displayLinkedStops(false),
			_networks(_dataSource, _env),
			_stopPoints(_dataSource, _env),
			_lines(_dataSource, _env)
		{}



		bool GTFSFileFormat::Importer_::_parse(
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
			string line;
			if(!getline(inFile, line))
			{
				return false;
			}
			_loadFieldsMap(line);

			stream << "INFO : Loading file " << filePath << " as " << key << "<br />";

			// 1 : Routes
			// Stops
			if(key == FILE_STOPS)
			{
				ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(_dataSource, _env);

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
								*_defaultCity,
								_stopAreaDefaultTransferDuration,
								_dataSource,
								_env,
								stream
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
							_dataSource,
							stream
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
								_dataSource,
								stream
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
						stream << "WARN : inconsistent stop area id "<< stopAreaId <<" in the stop point "<< id <<"<br />";
						continue;
					}

					// Point
					shared_ptr<geos::geom::Point> point(
						_dataSource.getCoordinatesSystem()->createPoint(
							lexical_cast<double>(_getValue("stop_lon")),
							lexical_cast<double>(_getValue("stop_lat"))
					)	);
					if(point->isEmpty())
					{
						point.reset();
					}

					if(request)
					{
						PTFileFormat::ImportableStopPoint isp;
						isp.operatorCode = id;
						isp.name = name;
						isp.linkedStopPoints = _stopPoints.get(id);
						isp.stopArea = stopArea;
						isp.coords = point;

						if(isp.linkedStopPoints.empty())
						{
							nonLinkedStopPoints.push_back(isp);
						}
						else if(_displayLinkedStops)
						{
							linkedStopPoints.push_back(isp);
						}
					}
					else
					{
						// Creation or update
						PTFileFormat::CreateOrUpdateStopPoints(
							_stopPoints,
							id,
							name,
							stopArea,
							point.get(),
							NULL,
							optional<time_duration>(),
							_dataSource,
							_env,
							stream
						);
					}
				}

				if(request)
				{
					PTFileFormat::DisplayStopPointImportScreen(
						nonLinkedStopPoints,
						*request,
						_env,
						_dataSource,
						stream
					);
					if(_displayLinkedStops)
					{
						PTFileFormat::DisplayStopPointImportScreen(
							linkedStopPoints,
							*request,
							_env,
							_dataSource,
							stream
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
				//TODO
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
						_dataSource,
						_env,
						stream
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
						stream << "WARN : inconsistent network id "<< networkId <<" in the line "<< id <<"<br />";
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
						stream
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
						stream << "WARN : inconsistent dates in "<< line <<" (" << startDateStr << " and " << endDateStr << ")<br />";
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
					if(dateStr.size() != 8)
					{
						stream << "WARN : inconsistent date in "<< line <<"<br />";
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
						stream << "WARN : inconsistent line id "<< lineCode <<" in the trip "<< id <<"<br />";
						continue;
					}
					trip.line = *_lines.get(lineCode).begin();

					// Calendar
					string calendarCode(_getValue("service_id"));
					Calendars::const_iterator it(_calendars.find(calendarCode));
					if(it == _calendars.end())
					{
						stream << "WARN : inconsistent service id "<< calendarCode <<" in the trip "<< id <<"<br />";
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
					if(tripCode != lastTripCode && !lastTripCode.empty() && !tripDetailVector.empty())
					{
						// Trip
						TripsMap::const_iterator it(_trips.find(lastTripCode));
						if(it == _trips.end())
						{
							stream << "WARN : inconsistent trip id "<< lastTripCode <<" in the trip stops file<br />";
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
								stream
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
								_dataSource,
								_env,
								stream
						)	);
						*service |= trip.calendar;

						tripDetailVector.clear();
					}

					TripDetail tripDetail;
					tripDetail.offsetFromLast = lexical_cast<MetricOffset>(_getValue("shape_dist_traveled"));
					tripDetail.arrivalTime = duration_from_string(_getValue("arrival_time"));
					if(tripDetail.arrivalTime.seconds())
					{
						tripDetail.arrivalTime += seconds(60 - tripDetail.arrivalTime.seconds());
					}
					tripDetail.departureTime = duration_from_string(_getValue("departure_time"));
					if(tripDetail.departureTime.seconds())
					{
						tripDetail.departureTime -= seconds(tripDetail.departureTime.seconds());
					}

					string stopCode(_getValue("stop_id"));
					if(!_stopPoints.contains(stopCode))
					{
						stream << "WARN : inconsistent stop id "<< stopCode <<" in the trip "<< tripCode <<"<br />";
						continue;
					}
					tripDetail.stop = _stopPoints.get(stopCode);

					tripDetailVector.push_back(tripDetail);
					lastTripCode = tripCode;
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



		void GTFSFileFormat::Importer_::displayAdmin(
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
			stream << t.cell("Fichier stops", t.getForm().getTextInput(_getFileParameterName(FILE_STOPS), _pathsMap[FILE_STOPS].file_string()));
			stream << t.cell("Fichier transfers (optionnel)", t.getForm().getTextInput(_getFileParameterName(FILE_TRANSFERS), _pathsMap[FILE_TRANSFERS].file_string()));
			stream << t.cell("Fichier réseaux", t.getForm().getTextInput(_getFileParameterName(FILE_AGENCY), _pathsMap[FILE_AGENCY].file_string()));
			stream << t.cell("Fichier routes", t.getForm().getTextInput(_getFileParameterName(FILE_ROUTES), _pathsMap[FILE_ROUTES].file_string()));
			stream << t.cell("Fichier calendriers", t.getForm().getTextInput(_getFileParameterName(FILE_CALENDAR), _pathsMap[FILE_CALENDAR].file_string()));
			stream << t.cell("Fichier dates", t.getForm().getTextInput(_getFileParameterName(FILE_CALENDAR_DATES), _pathsMap[FILE_CALENDAR_DATES].file_string()));
			stream << t.cell("Fichier voyages", t.getForm().getTextInput(_getFileParameterName(FILE_TRIPS), _pathsMap[FILE_TRIPS].file_string()));
			stream << t.cell("Fichier horaires", t.getForm().getTextInput(_getFileParameterName(FILE_STOP_TIMES), _pathsMap[FILE_STOP_TIMES].file_string()));
			stream << t.cell("Fichier attributs tarification", t.getForm().getTextInput(_getFileParameterName(FILE_FARE_ATTRIBUTES), _pathsMap[FILE_FARE_ATTRIBUTES].file_string()));
			stream << t.cell("Fichier règles tarification", t.getForm().getTextInput(_getFileParameterName(FILE_FARE_RULES), _pathsMap[FILE_FARE_RULES].file_string()));
			stream << t.cell("Fichier géométries", t.getForm().getTextInput(_getFileParameterName(FILE_SHAPES), _pathsMap[FILE_SHAPES].file_string()));
			stream << t.cell("Fichier services continus", t.getForm().getTextInput(_getFileParameterName(FILE_FREQUENCIES), _pathsMap[FILE_FREQUENCIES].file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Affichage arrêts liés", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops));
			stream << t.cell("Import zones d'arrêt", t.getForm().getOuiNonRadioInput(PARAMETER_IMPORT_STOP_AREA, _importStopArea));
			stream << t.cell("Commune par défaut (ID)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity.get() ? lexical_cast<string>(_defaultCity->getKey()) : string()));
			stream << t.cell("Temps de transfert par défaut (min)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_stopAreaDefaultTransferDuration.total_seconds() / 60)));
			stream << t.close();
		}



		db::DBTransaction GTFSFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

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
				utfline = ImpExModule::ConvertChar(line, _dataSource.getCharset(), "UTF-8");
				split(_line, utfline, is_any_of(SEP));
			}
		}



		server::ParametersMap GTFSFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
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
			return map;
		}



		void GTFSFileFormat::Importer_::_setFromParametersMap( const server::ParametersMap& map )
		{
			_importStopArea = map.getDefault<bool>(PARAMETER_IMPORT_STOP_AREA, false);
			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
			}
		}
}	}
