
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
#include "SQLiteTransaction.h"
#include "JourneyPatternTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "CommercialLineTableSync.h"
#include "LineStopTableSync.h"
#include "Calendar.h"
#include "ImportFunction.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <map>
#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::gregorian;
using namespace boost::posix_time;


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
		const std::string HeuresFileFormat::Importer_::FILE_ITINERAI("1itinerai"); 
		const std::string HeuresFileFormat::Importer_::FILE_TRONCONS("2troncons");
		const std::string HeuresFileFormat::Importer_::FILE_SERVICES("3services");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HeuresFileFormat>::Files MultipleFileTypesImporter<HeuresFileFormat>::FILES(
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
			return true;
		}

		
		
		SQLiteTransaction HeuresFileFormat::Importer_::_save() const
		{
			SQLiteTransaction transaction;
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
				if(service.second->empty())
				{
					ScheduledServiceTableSync::Remove(service.second->getKey(), transaction);
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


			// 1 : Routes
			if(key == FILE_ITINERAI)
			{

			} // 2 : Nodes
			else if(key == FILE_TRONCONS)
			{
				// Cleaning of each service handled by the datasource
				ScheduledServiceTableSync::SearchResult originalServices(
					ScheduledServiceTableSync::Search(
					_env,
					optional<RegistryKeyType>(),
					optional<RegistryKeyType>(),
					_dataSource.getKey(),
					optional<string>(),
					false,
					0,
					optional<size_t>(),
					false,
					false,
					UP_LINKS_LOAD_LEVEL
					)	);
				BOOST_FOREACH(shared_ptr<ScheduledService> service, originalServices)
				{
					for(date d(_startDate); d<=_endDate; d=d+days(1))
					{
						service->setInactive(d);
					}
				}

				string line;
				shared_ptr<ScheduledService> service;
				string serviceNumber;
				string lineNumber;
				int calendarNumber;
				JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
				vector<time_duration> departures;
				vector<time_duration> arrivals;
				Calendar mask;
				for(date curDate(_startDate); curDate <= _endDate; curDate += days(1))
				{
					mask.setActive(curDate);
				}
				bool serviceMustBeAvoided(false);
				map<string, shared_ptr<CommercialLine> > lines;

				while(getline(inFile, line))
				{
					if(line.substr(0,2) == "*Z")
					{
						serviceNumber = line.substr(3,5);
						lineNumber = line.substr(9,6);
						stops.clear();
						departures.clear();
						arrivals.clear();
						serviceMustBeAvoided = false;
					}
					else if(line.substr(0,5) == "*A VE")
					{
						calendarNumber = lexical_cast<int>(line.substr(22,6));
					}
					else if(line.substr(0,1) != "*")
					{
						StopPointTableSync::SearchResult searchStop(
							StopPointTableSync::Search(
							_env, optional<RegistryKeyType>(), line.substr(0,7)
							)	);
						if(searchStop.empty())
						{
							os << "WARN : stop " << line << " not found<br />";
							serviceMustBeAvoided = true;
						}
						else
						{
							string departureTime(line.substr(34,4));
							string arrivalTime(line.substr(29,4));

							JourneyPattern::StopWithDepartureArrivalAuthorization stop;
							BOOST_FOREACH(const StopPointTableSync::SearchResult::value_type& itstop, searchStop)
							{
								stop.stop.insert(itstop.get());
							}
							stop.departure = (departureTime != "9999" && departureTime != "    ");
							stop.arrival = (arrivalTime != "9999" && arrivalTime != "    ");
							stops.push_back(stop);

							arrivals.push_back(
								stop.arrival ?
								hours(lexical_cast<int>(line.substr(29,2))) + minutes(lexical_cast<int>(line.substr(31,2))) :
							minutes(0)
								);
							departures.push_back(
								stop.departure ?
								hours(lexical_cast<int>(line.substr(34,2))) + minutes(lexical_cast<int>(line.substr(36,2))) :
							minutes(0)
								);
						}
					}

					// End of service
					if(line.size() < 54 && !serviceMustBeAvoided)
					{
						// JourneyPattern
						map<string, shared_ptr<CommercialLine> >::const_iterator itLine(lines.find(lineNumber));
						shared_ptr<CommercialLine> cline;
						if(itLine != lines.end())
						{
							cline = itLine->second;
						}
						else
						{
							CommercialLineTableSync::SearchResult lines(
								CommercialLineTableSync::Search(_env, optional<RegistryKeyType>(),optional<string>(), lineNumber)
								);
							if(lines.empty())
							{
								os << "WARN : commercial line with key " << lineNumber << "not found<br />";
								continue;
							}
							if(lines.size() > 1)
							{
								os << "WARN : more than one commercial line with key " << lineNumber << "<br />";
							}
							cline = CommercialLineTableSync::GetEditable(
								lines.front()->getKey(),
								_env,
								UP_LINKS_LOAD_LEVEL
								);

							os << "LOAD : use of existing commercial line" << cline->getKey() << " (" << cline->getName() << ")<br />";

							// Load of existing routes
							JourneyPatternTableSync::SearchResult sroutes(
								JourneyPatternTableSync::Search(_env, cline->getKey(), _dataSource.getKey())
								);
							BOOST_FOREACH(shared_ptr<JourneyPattern> sroute, sroutes)
							{
								LineStopTableSync::Search(
									_env,
									sroute->getKey(),
									optional<RegistryKeyType>(),
									0,
									optional<size_t>(),
									true, true,
									UP_LINKS_LOAD_LEVEL
									);
								ScheduledServiceTableSync::Search(
									_env,
									sroute->getKey(),
									optional<RegistryKeyType>(),
									optional<RegistryKeyType>(),
									optional<string>(),
									false,
									0, optional<size_t>(), true, true,
									UP_LINKS_LOAD_LEVEL
									);
							}
						}

						// Attempting to find an existing route
						shared_ptr<JourneyPattern> route;
						BOOST_FOREACH(const Path* sroute, cline->getPaths())
						{
							const JourneyPattern* lroute(static_cast<const JourneyPattern*>(sroute));
							if(	*lroute == stops
								){
									route = const_pointer_cast<JourneyPattern>(_env.getSPtr(lroute));
									continue;
							}
						}

						// Create a new route if necessary
						if(!route.get())
						{
							string key;
							os << "CREA : Creation of route for " << cline->getName() << "<br />";
							route.reset(new JourneyPattern);
							route->setCommercialLine(cline.get());
							Importable::DataSourceLinks links;
							links.insert(make_pair(&_dataSource, key));
							route->setKey(JourneyPatternTableSync::getId());
							_env.getEditableRegistry<JourneyPattern>().add(route);
							cline->addPath(route.get());

							size_t rank(0);
							BOOST_FOREACH(const JourneyPattern::StopsWithDepartureArrivalAuthorization::value_type& stop, stops)
							{
								shared_ptr<LineStop> ls(new LineStop);
								ls->setLine(route.get());
								ls->setPhysicalStop(*stop.stop.begin());
								ls->setRankInPath(rank);
								ls->setIsArrival(rank > 0 && stop.arrival);
								ls->setIsDeparture(rank+1 < stops.size() && stop.departure);
								ls->setMetricOffset(0);
								ls->setKey(LineStopTableSync::getId());
								route->addEdge(*ls);
								_env.getEditableRegistry<LineStop>().add(ls);

								++rank;
							}
						}
						else
						{
							os << "LOAD : Use of route " << route->getKey() << " (" << route->getName() << ")<br />";
						}
						route->setRollingStock(Env::GetOfficialEnv().getEditable<RollingStock>(13792273858822585).get());

						// Services
						// Creation of the service
						shared_ptr<ScheduledService> service(new ScheduledService);
						service->setPath(route.get());
						service->setPathId(route->getKey());
						service->setServiceNumber(serviceNumber);
						service->setDepartureSchedules(departures);
						service->setArrivalSchedules(arrivals);

						// Search for a corresponding service
						ScheduledService* existingService(NULL);
						BOOST_FOREACH(Service* tservice, route->getServices())
						{
							ScheduledService* curService(dynamic_cast<ScheduledService*>(tservice));

							if(!curService) continue;

							if (*curService == *service)
							{
								os << "LOAD : Use of service " << curService->getKey() << " for " << serviceNumber << " (" << departures[0] << ") on route " << route->getKey() << " (" << route->getName() << ")<br />";
								existingService = curService;
								break;
							}
						}

						// If not found creation
						if(!existingService)
						{
							service->setKey(ScheduledServiceTableSync::getId());
							route->addService(service.get(), false);
							_env.getEditableRegistry<ScheduledService>().add(service);
							existingService = service.get();

							os << "CREA : Creation of service " << service->getServiceNumber() << " for " << serviceNumber << " (" << departures[0] << ") on route " << route->getKey() << " (" << route->getName() << ")<br />";
						}

						// Calendar
						existingService->subDates(mask);
					}
				}
			} // 3 : Services
			else if (key == FILE_SERVICES)
			{
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
			stream << t.title("Propriétés");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Date début", t.getForm().getCalendarInput(PARAMETER_START_DATE, _startDate));
			stream << t.cell("Date fin", t.getForm().getCalendarInput(PARAMETER_END_DATE, _endDate));
			stream << t.title("Données");
			stream << t.cell("Itinerai", t.getForm().getTextInput(_getFileParameterName(FILE_ITINERAI), _pathsMap[FILE_ITINERAI].file_string()));
			stream << t.cell("Troncons", t.getForm().getTextInput(_getFileParameterName(FILE_TRONCONS), _pathsMap[FILE_TRONCONS].file_string()));
			stream << t.cell("Services", t.getForm().getTextInput(_getFileParameterName(FILE_SERVICES), _pathsMap[FILE_SERVICES].file_string()));
			stream << t.close();
		}
}	}
