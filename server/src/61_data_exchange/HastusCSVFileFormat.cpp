
/** HastusCSVFileFormat class implementation.
	@file HastusCSVFileFormat.cpp

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

#include "HastusCSVFileFormat.hpp"

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
#include "DBModule.h"
#include "CityTableSync.h"
#include "JunctionTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "PTUseRuleTableSync.h"
#include "IConv.hpp"
#include "CommercialLineTableSync.h"
#include "RollingStockTableSync.hpp"
#include "Fare.hpp"

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
	using namespace server;
	using namespace geography;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,HastusCSVFileFormat>::FACTORY_KEY("Hastus-CSV");
	}

	namespace data_exchange
	{
		const std::string HastusCSVFileFormat::Importer_::FILE_ARRETS = "arrets";
		const std::string HastusCSVFileFormat::Importer_::FILE_ITINERAIRES = "itineraires";
		const std::string HastusCSVFileFormat::Importer_::FILE_VOYAGES = "voyages";
		const std::string HastusCSVFileFormat::Importer_::SEP(";");

		const std::string HastusCSVFileFormat::Importer_::PARAMETER_NETWORK_ID("network_id");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_ROLLING_STOCK_ID_A("rolling_stock_id_a");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_ROLLING_STOCK_ID_T("rolling_stock_id_t");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_ROLLING_STOCK_ID_DEFAULT("rolling_stock_id_default");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_IMPORT_STOP_AREA("isa");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const string HastusCSVFileFormat::Importer_::PARAMETER_HANDICAPPED_ALLOWED_USE_RULE = "handicapped_allowed_use_rule";
		const string HastusCSVFileFormat::Importer_::PARAMETER_HANDICAPPED_FORBIDDEN_USE_RULE = "handicapped_forbidden_use_rule";
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HastusCSVFileFormat>::Files MultipleFileTypesImporter<HastusCSVFileFormat>::FILES(
			HastusCSVFileFormat::Importer_::FILE_ARRETS.c_str(),
			HastusCSVFileFormat::Importer_::FILE_VOYAGES.c_str(),
			HastusCSVFileFormat::Importer_::FILE_ITINERAIRES.c_str(),
		"");
	}


	namespace data_exchange
	{
		bool HastusCSVFileFormat::Importer_::TripIndex::operator<( const TripIndex& other ) const
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
			if(handicapped != other.handicapped)
			{
				return handicapped < other.handicapped;
			}
			return false;
		}



		bool HastusCSVFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_ARRETS));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_ITINERAIRES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_VOYAGES);
			return true;
		}



		HastusCSVFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<HastusCSVFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_importStopArea(false),
			_interactive(false),
			_displayLinkedStops(false),
			_stopPoints(*import.get<DataSource>(), env)
		{}



		bool HastusCSVFileFormat::Importer_::_parse(
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
			_logDebug(
				"Loading file "+ filePath.string() +" as "+ key
			);

			DataSource& dataSource(*_import.get<DataSource>());

			// Stops
			if(key == FILE_ARRETS)
			{
				// Loop
				while(getline(inFile, line))
				{
					// Strings
					_loadLine(line);
					string code(_getValue(0));
					string name(_getValue(1));
					string x(_getValue(2));
					string y(_getValue(3));
					string cityCode(_getValue(6));
					bool handicapped(_getValue(7) == "1");

					// City
					City* cityForStopAreaAutoGeneration(NULL);
					CityTableSync::SearchResult cities(
						CityTableSync::Search(_env, optional<string>(), optional<string>(), cityCode, 0, 1)
					);
					if(cities.empty())
					{
						_logError(
							"ERR : City " + cityCode + " not found used by stopPoint : " + name + ",  " + code + ", " + x + ", " + y + "<br />"
						);
						continue;
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
								"Stop "+ code +" has invalid coordinate"
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

					RuleUser::Rules handicappedForbiddenRules;
					handicappedForbiddenRules.push_back(NULL);
					handicappedForbiddenRules.push_back(NULL);
					handicappedForbiddenRules.push_back(_handicappedForbiddenUseRule.get());
					handicappedForbiddenRules.push_back(NULL);

					optional<const RuleUser::Rules&> handicappedUseRule(
						handicapped ?
						handicappedRules :
						handicappedForbiddenRules
					);

					// Stop creation // AJOUTER PRISE EN CHARGE _importStopArea
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
				}
			}
			// Services
			else if(key == FILE_VOYAGES)
			{
				while(getline(inFile, line))
				{
					// Strings
					_loadLine(line);

					date day(
						from_string(_getValue(0))
					);
					TripIndex trip;
					trip.lineCode = _getValue(1);
					trip.routeCode = _getValue(4);
					trip.code = _getValue(5);
					trip.team = _getValue(6);
					trip.handicapped = (_getValue(13) == "X");

					size_t pos = trip.team.rfind('-');
					if(pos != string::npos && pos + 1 < trip.team.length())
					{
						trip.team = trim_left_copy(trip.team.substr(pos + 1));
					}

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
					itTrip->second.routeName = _getValue(10);
					itTrip->second.wayBack = (_getValue(12) == "Retour");
					itTrip->second.rollingStock = (_rollingStocks.find(_getValue(3)) == _rollingStocks.end()) ? _defaultRollingStock.get() : _rollingStocks.find(_getValue(3))->second.get();
					_tripsByCode[trip.code].insert(trip);
				}
			}
			// Routes
			else if(key == FILE_ITINERAIRES)
			{
				string lastCode;
				MetricOffset distance(0);

				while(getline(inFile, line))
				{
					// Strings
					_loadLine(line);
					string stopCode(_getValue(0));
					MetricOffset delta(
						lexical_cast<MetricOffset>(
							_getValue(2)
					)	);
					time_duration schedule(
						duration_from_string(
							_getValue(3)
					)	);
					string code(_getValue(4));
					bool withSchedules(_getValue(5) == "REGUL");

					// Object change
					if(lastCode != code)
					{
						lastCode = code;
						distance = 0;
					}

					// Storage
					BOOST_FOREACH(const TripsByCode::mapped_type::value_type& itTrip, _tripsByCode[code])
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
							distance + delta,
							true,
							true,
							withSchedules
						);
						trip.stops.push_back(stop);
					}

					// Distance
					distance += delta;
				}

				// SYNTHESE object construction
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(dataSource, _env);
				BOOST_FOREACH(const Trips::value_type& itTrip, _trips)
				{
					const TripIndex& trip(itTrip.first);
					const TripValues& tripValues(itTrip.second);

					// Line
					if(!lines.contains(trip.lineCode))
					{
						_logWarning(
							"inconsistent line id "+ trip.lineCode +" in the trip "+ trip.code
						);
						continue;
					}
					CommercialLine& line(
						**lines.get(trip.lineCode).begin()
					);

					// Route
					JourneyPattern* route(
						_createOrUpdateRoute(
							line,
							trip.routeCode,
							tripValues.routeName,
							optional<const string&>(),
							optional<Destination*>(),
							optional<const RuleUser::Rules&>(),
							tripValues.wayBack,
							tripValues.rollingStock,
							tripValues.stops,
							dataSource,
							true,
							true,
                            true,
                            true
					)	);
					if(route == NULL)
					{
						_logWarning(
							"Failure at route creation ("+ trip.lineCode +" / "+ trip.routeCode +"/"+ trip.code +")"
						);
						continue;
					}

					// Service
					// Handicapped rules
					RuleUser::Rules handicappedRules;
					handicappedRules.push_back(NULL);
					handicappedRules.push_back(NULL);
					handicappedRules.push_back(_handicappedAllowedUseRule.get());
					handicappedRules.push_back(NULL);
					handicappedRules.push_back(NULL);

					RuleUser::Rules handicappedForbiddenRules;
					handicappedForbiddenRules.push_back(NULL);
					handicappedForbiddenRules.push_back(NULL);
					handicappedForbiddenRules.push_back(_handicappedForbiddenUseRule.get());
					handicappedForbiddenRules.push_back(NULL);
					handicappedForbiddenRules.push_back(NULL);

					optional<const RuleUser::Rules&> handicappedUseRule(
						trip.handicapped ?
						handicappedRules :
						handicappedForbiddenRules
					);

					ScheduledService* service(
						_createOrUpdateService(
							*route,
							tripValues.schedules,
							tripValues.schedules,
							trip.code,
							dataSource,
							optional<const string&>(trip.team),
							handicappedUseRule
					)	);
					if(service)
					{
						*service |= tripValues.calendar;
					}
				}
			}
			return true;
		}



		db::DBTransaction HastusCSVFileFormat::Importer_::_save() const
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



		std::string HastusCSVFileFormat::Importer_::_getValue( std::size_t rank ) const
		{
			return trim_copy(_line[rank]);
		}



		void HastusCSVFileFormat::Importer_::_loadLine( const std::string& line ) const
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



		util::ParametersMap HastusCSVFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());
			map.insert(PARAMETER_IMPORT_STOP_AREA, _importStopArea);
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
			if(_network.get())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			if(_defaultRollingStock.get())
			{
				map.insert(PARAMETER_ROLLING_STOCK_ID_DEFAULT, _defaultRollingStock->getKey());
			}
			if((_rollingStocks.find("A") != _rollingStocks.end()) && _rollingStocks.find("A")->second.get())
			{
				map.insert(PARAMETER_ROLLING_STOCK_ID_A, _rollingStocks.find("A")->second->getKey());
			}
			if((_rollingStocks.find("T") != _rollingStocks.end()) && _rollingStocks.find("T")->second.get())
			{
				map.insert(PARAMETER_ROLLING_STOCK_ID_T, _rollingStocks.find("T")->second->getKey());
			}

			// Handicapped allowed use rule
			if(_handicappedAllowedUseRule.get())
			{
				map.insert(PARAMETER_HANDICAPPED_ALLOWED_USE_RULE, _handicappedAllowedUseRule->getKey());
			}

			// Handicapped forbidden use rule
			if(_handicappedForbiddenUseRule.get())
			{
				map.insert(PARAMETER_HANDICAPPED_FORBIDDEN_USE_RULE, _handicappedForbiddenUseRule->getKey());
			}

			return map;
		}



		void HastusCSVFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
			_importStopArea = map.getDefault<bool>(PARAMETER_IMPORT_STOP_AREA, false);
			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

			if(map.getDefault<RegistryKeyType>(PARAMETER_NETWORK_ID, 0))
			{
				_network = TransportNetworkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), _env);
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID_A, 0))
			{
				_rollingStocks.insert(make_pair("A", RollingStockTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID_A), _env)));
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID_T, 0))
			{
				_rollingStocks.insert(make_pair("T", RollingStockTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID_T), _env)));
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID_DEFAULT, 0))
			{
				_defaultRollingStock = RollingStockTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID_A), _env);
			}

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

			// Handicapped PT forbidden use rule
			RegistryKeyType handicappedPTForbiddenUseRuleId(
				map.getDefault<RegistryKeyType>(PARAMETER_HANDICAPPED_FORBIDDEN_USE_RULE)
			);
			if(handicappedPTForbiddenUseRuleId) try
			{
				_handicappedForbiddenUseRule = PTUseRuleTableSync::GetEditable(handicappedPTForbiddenUseRuleId, _env);
			}
			catch(ObjectNotFoundException<PTUseRule>&)
			{
				throw Exception("No such handicapped forbidden use rule");
			}
		}
}	}
