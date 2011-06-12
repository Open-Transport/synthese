
/** HeuresFileFormat class implementation.
	@file HeuresFileFormat.cpp

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

#include "HeuresFileFormat.hpp"
#include "DataSource.h"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "City.h"
#include "CityTableSync.h"
#include "DBTransaction.hpp"
#include "JourneyPatternTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "CommercialLineTableSync.h"
#include "LineStopTableSync.h"
#include "Calendar.h"
#include "ImportFunction.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "ImportableTableSync.hpp"
#include "PTFileFormat.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StopPointTableSync.hpp"
#include "PTPlaceAdmin.h"
#include "StopArea.hpp"
#include "DataSource.h"
#include "ImpExModule.h"
#include "Importer.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "TransportNetworkTableSync.h"
#include "RollingStockTableSync.h"
#include "DesignatedLinePhysicalStop.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <map>
#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	using namespace impex;
	using namespace pt;
	using namespace road;
	using namespace admin;
	using namespace geography;
	using namespace db;
	using namespace graph;
	using namespace calendar;
	using namespace server;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,HeuresFileFormat>::FACTORY_KEY("Heures");
	}

	namespace pt
	{
		const std::string HeuresFileFormat::Importer_::FILE_POINTSARRETS("pointsarrets");
		const std::string HeuresFileFormat::Importer_::FILE_ITINERAI("itinerai");
		const std::string HeuresFileFormat::Importer_::FILE_TRONCONS("troncons");
		const std::string HeuresFileFormat::Importer_::FILE_SERVICES("services");

		const std::string HeuresFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const std::string HeuresFileFormat::Importer_::PARAMETER_END_DATE("end_date");
		const std::string HeuresFileFormat::Importer_::PARAMETER_START_DATE("start_date");
		const std::string HeuresFileFormat::Importer_::PARAMETER_NETWORK_ID("network_id");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HeuresFileFormat>::Files MultipleFileTypesImporter<HeuresFileFormat>::FILES(
			HeuresFileFormat::Importer_::FILE_POINTSARRETS.c_str(),
			HeuresFileFormat::Importer_::FILE_ITINERAI.c_str(),
			HeuresFileFormat::Importer_::FILE_TRONCONS.c_str(),
			HeuresFileFormat::Importer_::FILE_SERVICES.c_str(),
		"");
	}

	namespace pt
	{
		bool HeuresFileFormat::Importer_::_controlPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_ITINERAI));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_TRONCONS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_SERVICES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_POINTSARRETS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		DBTransaction HeuresFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(Registry<CommercialLine>::value_type line, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<JourneyPattern>::value_type route, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(route.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<DesignatedLinePhysicalStop>::value_type lineStop, _env.getRegistry<DesignatedLinePhysicalStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				if(service.second->empty())
				{
					ScheduledServiceTableSync::RemoveRow(service.second->getKey(), transaction);
				}
				else
				{
					ScheduledServiceTableSync::Save(service.second.get(), transaction);
				}
			}
			return transaction;
		}



		bool HeuresFileFormat::Importer_::_parse(
			const path& filePath,
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


			if(key == FILE_POINTSARRETS)
			{
				string line;

				PTFileFormat::ImportableStopPoints linkedStopPoints;
				PTFileFormat::ImportableStopPoints nonLinkedStopPoints;
				ImportableTableSync::ObjectBySource<StopPointTableSync> stopPoints(_dataSource, _env);

				while(getline(inFile, line))
				{
					if(!_dataSource.getCharset().empty())
					{
						line = ImpExModule::ConvertChar(line, _dataSource.getCharset(), "UTF-8");
					}

					string id(boost::algorithm::trim_copy(line.substr(0, 4)));
					if(lexical_cast<int>(id) > 9000)
					{
						continue;
					}

					string name(boost::algorithm::trim_copy(line.substr(5, 50)));

					PTFileFormat::ImportableStopPoint isp;
					isp.operatorCode = id;
					isp.name = name;
					isp.linkedStopPoints = stopPoints.get(id);

					if(isp.linkedStopPoints.empty())
					{
						nonLinkedStopPoints.push_back(isp);
					}
					else if(_displayLinkedStops)
					{
						linkedStopPoints.push_back(isp);
					}
				}
				inFile.close();

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

				if(!nonLinkedStopPoints.empty())
				{
					stream << "ERR  : At least a stop could not be linked.<br/>";
					return false;
				}
			}
			if(key == FILE_ITINERAI) // 1 : Routes
			{
				if(!_network.get())
				{
					stream << "ERR  : The transport network was not specified.<br/>";
					return false;
				}

				// Bus
				RollingStockTableSync::SearchResult rollingstock(RollingStockTableSync::Search(_env, string("Bus")));
				if(rollingstock.empty())
				{
					stream << "ERR  : The bus transport mode is not registered in the table 49.<br />";
					return false;
				}
				RollingStock* bus(rollingstock.front().get());

				// Load of the stops
				ImportableTableSync::ObjectBySource<StopPointTableSync> stops(_dataSource, _env);
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);

				// Parsing the file
				string line;
				CommercialLine* cline(NULL);
				while(getline(inFile, line))
				{
					// Length control
					if(line.length() < 30)
					{
						continue;
					}

					// Route number
					string routeNumber(trim_copy(line.substr(7,2)));
					int technicalLineNumber(lexical_cast<int>(trim_copy(line.substr(4, 3))));

					// Route type
					int routeType(lexical_cast<int>(line.substr(9,1)));
					if(routeType != 0 && routeType != 1)
					{
						_technicalRoutes.insert(
							make_pair(technicalLineNumber, routeNumber)
						);
						continue;
					}

					// Commercial line number
					int commercialLineNumber(lexical_cast<int>(trim_copy(line.substr(0, 4))));

					cline = PTFileFormat::CreateOrUpdateLine(
						lines,
						lexical_cast<string>(commercialLineNumber),
						string(),
						lexical_cast<string>(commercialLineNumber),
						optional<RGBColor>(),
						*_network,
						_dataSource,
						_env,
						stream
					);

					// Stops
					JourneyPattern::StopsWithDepartureArrivalAuthorization servedStops;
					MetricOffset distance(0);
					bool ignoreRoute(false);
					for(size_t i(10); i+1<line.size(); i += 10)
					{
						if(line.size() < i+9)
						{
							stream << "WARN : inconsistent line size " << line << "<br />";
							ignoreRoute = true;
							break;
						}

						// Stop search
						string stopNumber(trim_copy(line.substr(i,4)));

						// Fake stop : girouette
						if(lexical_cast<int>(stopNumber) > 9000)
						{
							*servedStops.rbegin()->_metricOffset += lexical_cast<MetricOffset>(trim_copy(line.substr(i+5,5)));
							continue;
						}

						bool regul(lexical_cast<bool>(line.substr(i+4,1)));
						distance += lexical_cast<MetricOffset>(trim_copy(line.substr(i+5,5)));

						if(!stops.contains(stopNumber))
						{
							stream << "WARN : stop " << stopNumber << " not found<br />";
							ignoreRoute = true;
						}

						servedStops.push_back(
							JourneyPattern::StopWithDepartureArrivalAuthorization(
								stops.get(stopNumber),
								distance,
								true,
								true,
								regul
						)	);
					}

					if(ignoreRoute)
					{
						continue;
					}

					// Route identification
					JourneyPattern* route(
						PTFileFormat::CreateOrUpdateRoute(
							*cline,
							optional<const string&>(routeNumber),
							optional<const string&>(),
							optional<const string&>(),
							true,
							bus,
							servedStops,
							_dataSource,
							_env,
							stream
					)	);

					_routes.insert(
						make_pair(
							make_pair(technicalLineNumber, routeNumber),
							route
					)	);
				}
			} // 2 : Nodes
			else if(key == FILE_TRONCONS)
			{
				string line;
				typedef map<
					pair<JourneyPattern*, string>, // string is service number
					pair<
						pair<ScheduledService::Schedules, ScheduledService::Schedules>, // departure / arrival schedules
						vector<pair<int, int> > // technical line, elementary service number
					>
				> SchedulesMap;
				SchedulesMap services;

				// Reading of the file
				while(getline(inFile, line))
				{
					int lineNumber(lexical_cast<int>(trim_copy(line.substr(0,3))));
					pair<int, int> lineKey(
						make_pair(
							lineNumber,
							lexical_cast<int>(trim_copy(line.substr(3,3)))
					)	);

					for(size_t i(29); i+1<line.size(); ++i)
					{
						string routeNumber(trim_copy(line.substr(i,2)));
						RoutesMap::iterator it(_routes.find(make_pair(lineNumber, routeNumber)));
						if(it == _routes.end())
						{
							if(_technicalRoutes.find(make_pair(lineNumber, routeNumber)) == _technicalRoutes.end())
							{
								stream << "WARN : route not found in service file " << lineNumber << "/" << routeNumber << "<br />";
							}
							for(i+=11; i<line.size() && line[i]!=';'; ++i) ;
							continue;
						}
						JourneyPattern* route(it->second);

						string serviceNumber(trim_copy(line.substr(i+8,3)));
						SchedulesMap::iterator itS(services.find(make_pair(route, serviceNumber)));
						if(itS != services.end())
						{
							if(itS->first.first != route)
							{
								stream << "WARN : inconsistent route in service file " << serviceNumber << "/" << lineNumber << "/" << routeNumber << "<br />";
								for(i+=11; i<line.size() && line[i]!=';'; ++i) ;
								continue;
							}
						}
						else
						{
							itS = services.insert(
								make_pair(
									make_pair(route, serviceNumber),
									SchedulesMap::mapped_type()
							)	).first;
							size_t schedulesNumber(route->getScheduledStopsNumber());
							for(size_t s(0); s<schedulesNumber; ++s)
							{
								itS->second.first.first.push_back(time_duration(not_a_date_time));
								itS->second.first.second.push_back(time_duration(not_a_date_time));
							}
						}

						// Register the line key
						itS->second.second.push_back(lineKey);

						// Read the available schedules
						size_t rank(0);
						for(i+=11; i<line.size() && line[i]!=';'; i+=8, ++rank)
						{
							string arrivalSchedule(line.substr(i, 4));
							string departureSchedule(line.substr(i+4, 4));

							if(departureSchedule != "9999")
							{
								itS->second.first.first[rank] = time_duration(
									lexical_cast<int>(departureSchedule.substr(0,2)),
									lexical_cast<int>(departureSchedule.substr(2,2)),
									0
								);
							}
							if(arrivalSchedule != "9999")
							{
								itS->second.first.second[rank] = time_duration(
									lexical_cast<int>(arrivalSchedule.substr(0,2)),
									lexical_cast<int>(arrivalSchedule.substr(2,2)),
									0
								);
							}
						}
				}	}

				// Storage as ScheduledService
				BOOST_FOREACH(const SchedulesMap::value_type& it, services)
				{
					JourneyPattern* route(it.first.first);

					ScheduledService* service(
						PTFileFormat::CreateOrUpdateService(
							*route,
							it.second.first.first,
							it.second.first.second,
							it.first.second,
							_dataSource,
							_env,
							stream
					)	);

					BOOST_FOREACH(const SchedulesMap::mapped_type::second_type::value_type& itKey, it.second.second)
					{
						_services[itKey].push_back(service);
					}
				}
			} // 3 : Services
			else if (key == FILE_SERVICES)
			{
				if(_startDate.is_not_a_date() || _endDate.is_not_a_date())
				{
					stream << "ERR  : Start date or end date not defined<br />";
					return false;
				}

				string line;
				while(getline(inFile, line))
				{
					// Read of calendar
					vector<bool> days(7, false);
					for(size_t i(0); i<7; ++i)
					{
						days[i] = (line[i==0 ? 12 : i+5] == '1');
					}
					Calendar cal;
					for(gregorian::date d(_startDate); d<=_endDate; d += gregorian::days(1))
					{
						if(days[d.day_of_week()])
						{
							cal.setActive(d);
						}
					}

					// Services list
					for(size_t i(13); i+6<line.size(); i+=29)
					{
						int lineNumber(lexical_cast<int>(trim_copy(line.substr(i,3))));
						int serviceNumber(lexical_cast<int>(trim_copy(line.substr(i+3,3))));

						ServicesMap::iterator itS(_services.find(
								make_pair(lineNumber, serviceNumber)
						)	);
						if(itS == _services.end())
						{
							stream << "WARN : inconsistent service number " << lineNumber << "/" << serviceNumber << " in " << line << "<br />";
							continue;
						}

						BOOST_FOREACH(ScheduledService* service, itS->second)
						{
							*service |= cal;
						}
					}
				}
			}
			inFile.close();

			return true;
		}



		void HeuresFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const AdminRequest& request
		) const {

			stream << "<h1>Horaires</h1>";
			AdminFunctionRequest<DataSourceAdmin> importRequest(request);
			PropertiesHTMLTable t(importRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Données");
			stream << t.cell("Arrêts", t.getForm().getTextInput(_getFileParameterName(FILE_POINTSARRETS), _pathsMap[FILE_POINTSARRETS].file_string()));
			stream << t.cell("Itineraires", t.getForm().getTextInput(_getFileParameterName(FILE_ITINERAI), _pathsMap[FILE_ITINERAI].file_string()));
			stream << t.cell("Troncons", t.getForm().getTextInput(_getFileParameterName(FILE_TRONCONS), _pathsMap[FILE_TRONCONS].file_string()));
			stream << t.cell("Services", t.getForm().getTextInput(_getFileParameterName(FILE_SERVICES), _pathsMap[FILE_SERVICES].file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Affichage arrêts liés", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops));
			stream << t.cell("Date début", t.getForm().getCalendarInput(PARAMETER_START_DATE, _startDate));
			stream << t.cell("Date fin", t.getForm().getCalendarInput(PARAMETER_END_DATE, _endDate));
			stream << t.cell("Réseau", t.getForm().getTextInput(PARAMETER_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
			stream << t.close();
		}


		server::ParametersMap HeuresFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(!_startDate.is_not_a_date())
			{
				map.insert(PARAMETER_START_DATE, _startDate);
			}
			if(!_startDate.is_not_a_date())
			{
				map.insert(PARAMETER_END_DATE, _endDate);
			}
			if(_network.get())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			return map;
		}



		void HeuresFileFormat::Importer_::_setFromParametersMap( const server::ParametersMap& map )
		{
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);
			if(!map.getDefault<string>(PARAMETER_START_DATE).empty())
			{
				_startDate = from_string(map.get<string>(PARAMETER_START_DATE));
			}
			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				_endDate = from_string(map.get<string>(PARAMETER_END_DATE));
			}
			if(map.getOptional<RegistryKeyType>(PARAMETER_NETWORK_ID))
			{
				_network = TransportNetworkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), _env);
			}
		}
}	}
