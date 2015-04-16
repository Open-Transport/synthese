
/** RTMFileFormat class implementation.
	@file RTMFileFormat.cpp

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

#include "RTMFileFormat.hpp"

#include "Import.hpp"
#include "PTModule.h"
#include "TransportNetworkTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ImpExModule.h"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "DataSource.h"
#include "Importer.hpp"
#include "DBModule.h"
#include "CityTableSync.h"
#include "JunctionTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "PTUseRuleTableSync.h"
#include "IConv.hpp"
#include "CommercialLineTableSync.h"
#include "RollingStockTableSync.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace geos::geom;

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

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,RTMFileFormat>::FACTORY_KEY("RTM");
	}

	namespace data_exchange
	{
		const std::string RTMFileFormat::Importer_::FILE_ARRETS = "station.csv";
		const std::string RTMFileFormat::Importer_::FILE_LIGNES_ITI_COURSES = "lepilote.txt";
		const std::string RTMFileFormat::Importer_::SEP(";");

		//const std::string RTMFileFormat::Importer_::PARAMETER_IMPORT_STOP_AREA("isa");
		const std::string RTMFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		//const std::string RTMFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		//const std::string RTMFileFormat::Importer_::PARAMETER_CREATE_LIGNE("#1");
		//const std::string RTMFileFormat::Importer_::PARAMETER_DESCRIPTION_LIGNE("#11");
		const std::string RTMFileFormat::Importer_::PARAMETER_CREATE_ITI("#2");
		const std::string RTMFileFormat::Importer_::PARAMETER_DESCRIPTION_ITI("#3");
		//const std::string RTMFileFormat::Importer_::PARAMETER_CREATE_TM("#4");
		//const std::string RTMFileFormat::Importer_::PARAMETER_TM_LIGNE("#5");
		const std::string RTMFileFormat::Importer_::PARAMETER_CREATE_COURSE("#6");
		const std::string RTMFileFormat::Importer_::PARAMETER_CREATE_TIMETABLE("#7");
		//const std::string RTMFileFormat::Importer_::PARAMETER_COURSE_TIMETABLE("#71");
		const std::string RTMFileFormat::Importer_::PARAMETER_DATE_TM("#8");
		//const std::string RTMFileFormat::Importer_::PARAMETER_TYPICAL_DAY_COURSE("#9");
		//const std::string RTMFileFormat::Importer_::PARAMETER_DATE_COURSE("#91");
		const std::string RTMFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<RTMFileFormat>::Files MultipleFileTypesImporter<RTMFileFormat>::FILES(
			RTMFileFormat::Importer_::FILE_ARRETS.c_str(),
			RTMFileFormat::Importer_::FILE_LIGNES_ITI_COURSES.c_str(),
		"");
	}


	namespace data_exchange
	{
		bool RTMFileFormat::Importer_::TripIndex::operator<( const TripIndex& other ) const
		{
			if(lineCode != other.lineCode)
			{
				return lineCode < other.lineCode;
			}
			if(routeCode != other.routeCode)
			{
				return routeCode < other.routeCode;
			}
			if(code != other.code)
			{
				return code < other.code;
			}
			if(team != other.team)
			{
				return team < other.team;
			}
			if(itiCode != other.itiCode)
			{
				return itiCode < other.itiCode;
			}		
			if(tmCode != other.tmCode)
			{
				return tmCode < other.tmCode;
			}	
			return false;
		}



		bool RTMFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_ARRETS));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_LIGNES_ITI_COURSES);
			return true;
		}



		RTMFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<RTMFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_importStopArea(false),
			_interactive(false),
			_displayLinkedStops(false),
			_stopPoints(*_import.get<DataSource>(), env)
		{}



		bool RTMFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				_logError("Could not open the file " + filePath.file_string());
				throw Exception("Could not open the file " + filePath.file_string());
			}
			string line;
			_logDebug(
				"Loading file "+ filePath.file_string() +" as "+ key
			);

			DataSource& dataSource(*_import.get<DataSource>());

			// Stops
			if(key == FILE_ARRETS)
			{
				cout << "test boucle arrêts" << endl;
				// Loop
				while(getline(inFile, line))
				{
					// Strings
					_loadLine(line);
					string code(_getValue(6));
					string name(_getValue(0));
					string x(_getValue(3));
					string y(_getValue(2));

					// City
					City* cityForStopAreaAutoGeneration(NULL);
					CityTableSync::SearchResult cities(
						CityTableSync::Search(_env, optional<string>(), optional<string>(), string("13055"), 0, 1)
					);
		
					if(cities.empty())
					{
						_logWarning(
							"City Marseille not found"
						);
					}
					else
					{
						cityForStopAreaAutoGeneration = cities.begin()->get();
					}

					// Point
					boost::shared_ptr<geos::geom::Point> point;								
					if(!x.empty() && !y.empty())
					{
						try
						{
							if(x.size()>1)
							{
								x.replace(1, 1, ".");
							}

							if(y.size()>1)
							{
								y.replace(2,1,".");
							}
						 
							point = dataSource.getActualCoordinateSystem().createPoint(
								lexical_cast<double>(x),
								lexical_cast<double>(y)
							);
							if(point->isEmpty())
							{
								point.reset();
							}

						}
						catch(boost::bad_lexical_cast&)
						{
							_logWarning(
								"Stop "+ code +" has invalid coordinate 1"
							);
						}
					}
					else
					{
						_logWarning(
							"Stop "+ code +" has invalid coordinate"
						);
					}

					// Handicapped rules
					RuleUser::Rules handicappedRules;
					handicappedRules.push_back(NULL);
					handicappedRules.push_back(NULL);
					handicappedRules.push_back(_handicappedAllowedUseRule.get());
					handicappedRules.push_back(NULL);
					
					optional<const RuleUser::Rules&> handicappedUseRule(handicappedRules);
					
					// Stop creation
					_createOrUpdateStopWithStopAreaAutocreation(
						_stopPoints,
						code,
						name,
						point.get(),
						*cityForStopAreaAutoGeneration,
						_stopAreaDefaultTransferDuration,
						dataSource,
 						handicappedUseRule
					);
					cout << "test arrets ok" << endl;
				}
			}
			// Services
			else if(key == FILE_LIGNES_ITI_COURSES)
			{
				int i = 0;
				int u = 0;
				JourneyPattern::StopsWithDepartureArrivalAuthorization stops;

				while(getline(inFile, line))
				{
					// Strings
					_loadLine(line);
					string typeObjet(_getValue(0));

//					shared_ptr<RollingStock> rollingStock(RollingStockTableSync::GetEditable(1, _env));
					cout << i << endl;
					i++; 
					// LINES
					if(typeObjet == PARAMETER_CREATE_ITI)
					{
						TripIndex trip;
						trip.lineCode = _getValue(1);
						trip.code = _getValue(2);

						cout << typeObjet << endl;	
											
						// Storage
						Trips::iterator itTrip(
							_trips.find(trip)
						);
						if(itTrip == _trips.end())
						{
							itTrip = _trips.insert(
								make_pair(trip, TripValues())
							).first;
						}
						cout << "test ligne ok" << endl;
										
					}
			
					// ITINERAIRES
					else if(typeObjet == PARAMETER_DESCRIPTION_ITI)
					{
						// Strings
						TripIndex trip;
						trip.stopCode = _getValue(2);
						trip.itiCode = _getValue(1);
						
						// Storage
						Trips::iterator itTrip(
							_trips.find(trip)
						);
						if(itTrip == _trips.end())
						{
							itTrip = _trips.insert(
								make_pair(trip, TripValues())
							).first;
						}
						cout << "test iti ok" << endl;
					}

					// COURSES
					else if(typeObjet == PARAMETER_CREATE_COURSE)
					{
						// Strings
						TripIndex trip;
						trip.itiCode = _getValue(1);
						trip.courseCode = _getValue(2);
						trip.tmCode = _getValue(3);
						
						// Storage
						Trips::iterator itTrip(
							_trips.find(trip)
						);
						if(itTrip == _trips.end())
						{
							itTrip = _trips.insert(
								make_pair(trip, TripValues())
							).first;
						}
						cout << "test course ok" << endl;
					}

					//SCHEDULE
					else if(typeObjet == PARAMETER_CREATE_TIMETABLE)
					{
						TripIndex trip;
						trip.courseCode = _getValue(1);
						string stopCode(_getValue(2));
						
						time_duration schedule(
								duration_from_string(
									_getValue(3)
							)	);	
							
						bool withSchedules(1);
											 

						// Storage
						BOOST_FOREACH(const TripsByCode::mapped_type::value_type& itTrip, _tripsByCode[trip.code])
						{
							TripValues& trip(_trips[itTrip]);

							// Schedule if necessary
							if(withSchedules)
							{
								trip.schedules.push_back(schedule);
							}

							// Stop
							JourneyPattern::StopWithDepartureArrivalAuthorization stop(
								_stopPoints.get(stopCode),
								boost::optional<MetricOffset>(),
								//distance + delta,
								true,
								true,
								withSchedules
								);
								trip.stops.push_back(stop);
						}

							// Distance
//							distance += delta;		
						cout << "test schedule ok" << endl;					
					}
					
					//DATE
					else if(typeObjet == PARAMETER_DATE_TM)
					{
						// Date format
						string frdate(_getValue(2));
						string year(frdate.substr(6,4));
						string month(frdate.substr(2,4));
						string jour(frdate.substr(0,2));
						string dat=year+month+jour;
						date day(
							from_string(dat)
						);	
						
						TripIndex trip;						
						trip.tmCode = _getValue(1);	
											
						// Storage
						Trips::iterator itTrip(
							_trips.find(trip)
						);
						if(itTrip == _trips.end())
						{
							itTrip = _trips.insert(
								make_pair(trip, TripValues())
							).first;
						}						
						itTrip->second.calendar.setActive(day);
						itTrip->second.wayBack = 0;
//						itTrip->second.rollingStock = (_rollingStocks.find(_getValue(3)) == _rollingStocks.end()) ? _defaultRollingStock.get() : _rollingStocks.find(_getValue(3))->second.get();
//						_tripsByCode[trip.code].insert(trip);	

						cout << "test date ok" << endl;											
					}
				}
				// SYNTHESE object construction
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(dataSource, _env);
				BOOST_FOREACH(const Trips::value_type& itTrip, _trips)
				{
					cout << "debut construction" << endl;
					cout << u << endl;
					u++;
					const TripIndex& trip(itTrip.first);

					// Line
					if(!lines.contains(trip.lineCode))
					{
						_logWarning(
							"Inconsistent line id "+ trip.lineCode +" in the trip "+ trip.code
						);
						continue;
					}
					cout << "test construction lignes" << endl;	
/*						
					// Route
					JourneyPattern* route(
						PTFileFormat::CreateOrUpdateRoute(
							line,
							trip.courseCode,
							optional<const string&>(),
							optional<const string&>(),
							optional<Destination*>(),
							optional<const RuleUser::Rules&>(),
							tripValues.wayBack,
							tripValues.rollingStock,
							tripValues.stops,
							_dataSource,
							_env,
							stream,
							true,
							true
					)	);
					if(route == NULL)
					{
						stream << "WARN : failure at route creation ("<< trip.lineCode <<" / "<< trip.routeCode <<"/"<< trip.code <<")<br />";
						continue;
					}

					// Service
					// Handicapped rules
				// Handicapped rules
					RuleUser::Rules handicappedRules;
					handicappedRules.push_back(NULL);
					handicappedRules.push_back(NULL);
					handicappedRules.push_back(_handicappedAllowedUseRule.get());
					handicappedRules.push_back(NULL);
						
					optional<const RuleUser::Rules&> handicappedUseRule(handicappedRules);

					ScheduledService* service(
						PTFileFormat::CreateOrUpdateService(
							*route,
							tripValues.schedules,
							tripValues.schedules,
							trip.code,
							_dataSource,
							_env,
							stream,
							optional<const string&>(trip.team),
							handicappedUseRule
					)	);
					if(service)
					{
						*service |= tripValues.calendar;
					}
*/				}									
			}
			return true;
		}



		db::DBTransaction RTMFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			PTDataCleanerFileFormat::_addRemoveQueries(transaction);

			// Saving of each created or altered objects
			BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
			{
				StopPointTableSync::Save(stop.second.get(), transaction);
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
			BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
			{
				StopAreaTableSync::Save(cstop.second.get(), transaction);
			}
			return transaction;
		}



		std::string RTMFileFormat::Importer_::_getValue( std::size_t rank ) const
		{
			return trim_copy(_line[rank]);
		}



		void RTMFileFormat::Importer_::_loadLine( const std::string& line ) const
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



		util::ParametersMap RTMFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());
			//map.insert(PARAMETER_IMPORT_STOP_AREA, _importStopArea);			
			//map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			//exemple
/*			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
*/			
			return map;
		}



		void RTMFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
			//_importStopArea = map.getDefault<bool>(PARAMETER_IMPORT_STOP_AREA, false);
			//_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			//_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);
			//exemple
			/*if(map.getDefault<RegistryKeyType>(PARAMETER_NETWORK_ID, 0))
			{
				_network = TransportNetworkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), _env);
			}*/


		}
}	}
