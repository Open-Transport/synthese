
/** HastusCSVFileFormat class implementation.
	@file HastusCSVFileFormat.cpp

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

#include "HastusCSVFileFormat.hpp"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
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
#include "Importer.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "HastusCSVFileFormat.hpp"
#include "City.h"
#include "PTFileFormat.hpp"
#include "CityTableSync.h"
#include "Junction.hpp"
#include "JunctionTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "PTUseRuleTableSync.h"
#include "IConv.hpp"
#include "CommercialLineTableSync.h"
#include "RollingStock.hpp"
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
		template<> const string FactorableTemplate<FileFormat,HastusCSVFileFormat>::FACTORY_KEY("Hastus-CSV");
	}

	namespace pt
	{
		const std::string HastusCSVFileFormat::Importer_::FILE_ARRETS = "arrets";
		const std::string HastusCSVFileFormat::Importer_::FILE_ITINERAIRES = "itineraires";
		const std::string HastusCSVFileFormat::Importer_::FILE_VOYAGES = "voyages";
		const std::string HastusCSVFileFormat::Importer_::SEP(";");

		const std::string HastusCSVFileFormat::Importer_::PARAMETER_NETWORK_ID("network_id");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_ROLLING_STOCK_ID("rolling_stock_id");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_IMPORT_STOP_AREA("isa");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string HastusCSVFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HastusCSVFileFormat>::Files MultipleFileTypesImporter<HastusCSVFileFormat>::FILES(
			HastusCSVFileFormat::Importer_::FILE_ARRETS.c_str(),
			HastusCSVFileFormat::Importer_::FILE_ITINERAIRES.c_str(),
			HastusCSVFileFormat::Importer_::FILE_VOYAGES.c_str(),
		"");
	}


	namespace pt
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
			return this < &other;
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
			const impex::DataSource& dataSource
		):	MultipleFileTypesImporter<HastusCSVFileFormat>(env, dataSource),
			Importer(env, dataSource),
			PTDataCleanerFileFormat(env, dataSource),
			_importStopArea(false),
			_interactive(false),
			_displayLinkedStops(false),
			_stopPoints(_dataSource, env)
		{}



		bool HastusCSVFileFormat::Importer_::_parse(
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
			stream << "INFO : Loading file " << filePath << " as " << key << "<br />";

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
						stream << "WARN : City " << cityCode << " not found<br />";
					}
					else
					{
						cityForStopAreaAutoGeneration = cities.begin()->get();
					}

					// Point
					shared_ptr<geos::geom::Point> point;
					if(!x.empty() && !y.empty())
					{
						try
						{
							point = _dataSource.getActualCoordinateSystem().createPoint(
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
							stream << "WARN : Stop " << code << " has invalid coordinate<br />";
						}
					}
					else
					{
						stream << "WARN : Stop " << code << " has invalid coordinate<br />";
					}

					// Stop creation
					PTFileFormat::CreateOrUpdateStopPoints(
						_stopPoints,
						code,
						name,
						NULL,
						point.get(),
						cityForStopAreaAutoGeneration,
						_stopAreaDefaultTransferDuration,
						_dataSource,
						_env,
						stream
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
						distance = 0;
					}

					// Storage
					BOOST_FOREACH(const TripsByCode::mapped_type::value_type& itTrip, _tripsByCode[code])
					{
						TripValues& trip(_trips[itTrip]);

						// Distance
						distance += delta;

						// Schedule if necessary
						if(withSchedules)
						{
							trip.schedules.push_back(schedule);
						}

						// Stop
						JourneyPattern::StopWithDepartureArrivalAuthorization stop(
							_stopPoints.get(stopCode),
							distance,
							true,
							true,
							withSchedules
						);
						trip.stops.push_back(stop);
					}
				}

				// SYNTHESE object construction
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);
				BOOST_FOREACH(const Trips::value_type& itTrip, _trips)
				{
					const TripIndex& trip(itTrip.first);
					const TripValues& tripValues(itTrip.second);

					// Line
					if(!lines.contains(trip.lineCode))
					{
						stream << "WARN : inconsistent line id "<< trip.lineCode <<" in the trip "<< trip.code <<"<br />";
						continue;
					}
					CommercialLine& line(
						**lines.get(trip.lineCode).begin()
					);

					// Route
					JourneyPattern* route(
						PTFileFormat::CreateOrUpdateRoute(
							line,
							trip.routeCode,
							tripValues.routeName,
							optional<const string&>(),
							optional<Destination*>(),
							optional<const RuleUser::Rules&>(),
							tripValues.wayBack,
							_rollingStock.get(),
							tripValues.stops,
							_dataSource,
							_env,
							stream,
							true
					)	);
					if(route == NULL)
					{
						stream << "WARN : failure at route creation ("<< trip.lineCode <<" / "<< trip.routeCode <<"/"<< trip.code <<")<br />";
						continue;
					}

					// Service
					// TODO handicapped
					ScheduledService* service(
						PTFileFormat::CreateOrUpdateService(
							*route,
							tripValues.schedules,
							tripValues.schedules,
							trip.code,
							_dataSource,
							_env,
							stream
					)	);
					if(service)
					{
						*service |= tripValues.calendar;
					}
				}
			}
			return true;
		}



		void HastusCSVFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Fichiers</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			t.getForm().addHiddenField(PARAMETER_FROM_TODAY, string("1"));
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Effacer donn�es anciennes", t.getForm().getOuiNonRadioInput(PARAMETER_CLEAN_OLD_DATA, false));
			stream << t.cell("Effacer arr�ts inutilis�s", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_UNUSED_STOPS, _cleanUnusedStops));
			stream << t.title("Fichiers");
			stream << t.cell("Fichier arr�ts", t.getForm().getTextInput(_getFileParameterName(FILE_ARRETS), _pathsMap[FILE_ARRETS].file_string()));
			stream << t.cell("Fichier itin�raires", t.getForm().getTextInput(_getFileParameterName(FILE_ITINERAIRES), _pathsMap[FILE_ITINERAIRES].file_string()));
			stream << t.cell("Fichier voyages", t.getForm().getTextInput(_getFileParameterName(FILE_VOYAGES), _pathsMap[FILE_VOYAGES].file_string()));
			stream << t.title("Param�tres");
			stream << t.cell("Affichage arr�ts li�s", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops));
			stream << t.cell("Import zones d'arr�t", t.getForm().getOuiNonRadioInput(PARAMETER_IMPORT_STOP_AREA, _importStopArea));
			stream << t.cell("R�seau (ID)", t.getForm().getTextInput(PARAMETER_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
			stream << t.cell("Mode de transport (ID)", t.getForm().getTextInput(PARAMETER_ROLLING_STOCK_ID, _rollingStock.get() ? lexical_cast<string>(_rollingStock->getKey()) : string()));
			stream << t.cell("Temps de transfert par d�faut (min)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_stopAreaDefaultTransferDuration.total_seconds() / 60)));
			stream << t.close();
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
				utfline = IConv::IConv(_dataSource.getCharset(), "UTF-8").convert(line);
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
			if(_rollingStock.get())
			{
				map.insert(PARAMETER_ROLLING_STOCK_ID, _rollingStock->getKey());
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

			if(map.getDefault<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID, 0))
			{
				_rollingStock = RollingStockTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID), _env);
			}
		}
}	}
