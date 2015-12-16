
/** PubliBikeCSVFileFormat class implementation.
	@file PubliBikeCSVFileFormat.cpp

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

#include "PubliBikeCSVFileFormat.hpp"

#include "Import.hpp"
#include "ImpExModule.h"
#include "DataSource.h"
#include "DBModule.h"
#include "CityTableSync.h"
#include "IConv.hpp"
#include "RoadChunkTableSync.h"
#include "PublicBikeNetworkTableSync.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace geos::geom;

namespace synthese
{
	namespace util
	{
		template<> const string FactorableTemplate<impex::FileFormat,data_exchange::PubliBikeCSVFileFormat>::FACTORY_KEY("PubliBikeCSV");
	}

	namespace data_exchange
	{
		const string PubliBikeCSVFileFormat::Importer_::FILE_PUBLIBIKE = "publibike";
		const string PubliBikeCSVFileFormat::Importer_::PARAMETER_PROJECT_STATIONS = "project";
		const string PubliBikeCSVFileFormat::Importer_::SEP(",");
	}

	namespace data_exchange
	{
		PubliBikeCSVFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			OneFileTypeImporter<PubliBikeCSVFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			_stations(*import.get<impex::DataSource>(), env),
			_networks(*import.get<impex::DataSource>(), env)
		{}



		bool PubliBikeCSVFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath
		) const {
			ifstream inFile;
			inFile.open(filePath.string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.string());
			}
			string line;
			_logDebug(
				"Loading file "+ filePath.string()
			);

			impex::DataSource& dataSource(*_import.get<impex::DataSource>());

			// Loop
			bool first(true);
			while(getline(inFile, line))
			{
				if (first)
				{
					// Ignore first line (header of file)
					first = false;
					continue;
				}
				// Strings
				_loadLine(line);
				string networkName(_getValue(0));
				string networkNumber(_getValue(2));
				string stationNumber(_getValue(3));
				string cityName(_getValue(4));
				string stationName(_getValue(5));
				string x(_getValue(14));
				string y(_getValue(13));

				// City
				std::string normalizedCityName = boost::to_upper_copy(lexical_matcher::FrenchPhoneticString::to_plain_lower_copy(cityName));
				geography::City* cityForStation(NULL);
				geography::CityTableSync::SearchResult cities(
					geography::CityTableSync::Search(_env, optional<string>(), normalizedCityName, optional<string>(), 0, 1)
				);

				if(cities.empty())
				{
					_logError(
						"City " + cityName + " not found"
					);
					return false;
				}
				else
				{
					cityForStation = cities.begin()->get();
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
							"Station "+ stationName + "/" + stationNumber +" has invalid coordinates"
						);
					}
				}
				else
				{
					_logWarning(
						"Public Place "+ stationName + "/" + stationNumber +" has invalid coordinates"
					);
				}

				public_biking::PublicBikeNetwork* network = _createOrUpdatePublicBikeNetwork(
					_networks,
					networkNumber,
					networkName
				);

				// Creates the point in the instance coordinate system (instead of datasource coordinates system)
				boost::shared_ptr<geos::geom::Point> convertedPoint =
					CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*point);

				// Public Place creation
				_createOrUpdatePublicBikeStation(
					_stations,
					*network,
					stationNumber,
					stationName,
					convertedPoint,
					*cityForStation
				);
			}
			return true;
		}



		db::DBTransaction PubliBikeCSVFileFormat::Importer_::_save() const
		{
			db::DBTransaction transaction;

			// If requested, computing projection of the stations
			if (_projectStations)
			{
				BOOST_FOREACH(const util::Registry<public_biking::PublicBikeStation>::value_type& publicBikeStation, _env.getEditableRegistry<public_biking::PublicBikeStation>())
				{
					if (publicBikeStation.second.get()->hasGeometry())
					{
						road::Address address;
						road::RoadChunkTableSync::ProjectAddress(
							*publicBikeStation.second.get()->getGeometry(),
							100,
							address
						);

						if(address.getRoadChunk())
						{
							publicBikeStation.second->setProjectedPoint(address);
						}
					}
				}
			}

			// Saving of each created or altered objects
			BOOST_FOREACH(const util::Registry<public_biking::PublicBikeNetwork>::value_type& publicBikeNetwork, _env.getEditableRegistry<public_biking::PublicBikeNetwork>())
			{
				public_biking::PublicBikeNetworkTableSync::Save(publicBikeNetwork.second.get(), transaction);
			}
			BOOST_FOREACH(const util::Registry<public_biking::PublicBikeStation>::value_type& publicBikeStation, _env.getEditableRegistry<public_biking::PublicBikeStation>())
			{
				public_biking::PublicBikeStationTableSync::Save(publicBikeStation.second.get(), transaction);
			}

			return transaction;
		}



		string PubliBikeCSVFileFormat::Importer_::_getValue( std::size_t rank ) const
		{
			return trim_copy(_line[rank]);
		}



		void PubliBikeCSVFileFormat::Importer_::_loadLine( const string& line ) const
		{
			_line.clear();
			if(!line.empty())
			{
				string utfline(
					line[line.size() - 1] == '\r' ?
					line.substr(0, line.size() - 1) :
					line
				);
				utfline = util::IConv(_import.get<impex::DataSource>()->get<Charset>(), "UTF-8").convert(line);
				split(_line, utfline, is_any_of(SEP));
			}
		}



		util::ParametersMap PubliBikeCSVFileFormat::Importer_::_getParametersMap() const
		{
			util::ParametersMap map;
			if (_projectStations)
			{
				map.insert(PARAMETER_PROJECT_STATIONS, _projectStations ? 1 : 0);
			}
			return map;
		}



		void PubliBikeCSVFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			_projectStations = map.getDefault<bool>(PARAMETER_PROJECT_STATIONS, false);
		}

		public_biking::PublicBikeNetwork* PubliBikeCSVFileFormat::Importer_::_createOrUpdatePublicBikeNetwork(
			impex::ImportableTableSync::ObjectBySource<public_biking::PublicBikeNetworkTableSync>& publicBikeNetworks,
			const std::string& networkNumber,
			const std::string& networkName
		) const {
			public_biking::PublicBikeNetwork* network(NULL);

			// Search for a network linked with the datasource
			if(publicBikeNetworks.contains(networkNumber))
			{
				set<public_biking::PublicBikeNetwork*> loadedNetworks(publicBikeNetworks.get(networkNumber));
				if(loadedNetworks.size() > 1)
				{
					_logWarning(
						"More than one network with key "+ networkNumber
					);
				}
				network = *loadedNetworks.begin();
				_logLoad(
					"Use of existing network "+ lexical_cast<string>(network->getKey()) +" ("+ network->getName() +")"
				);
			}
			else
			{
				network = new public_biking::PublicBikeNetwork(
					public_biking::PublicBikeNetworkTableSync::getId()
				);
				impex::Importable::DataSourceLinks links;
				links.insert(make_pair(&*_import.get<impex::DataSource>(), networkNumber));
				network->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<public_biking::PublicBikeNetwork>().add(boost::shared_ptr<public_biking::PublicBikeNetwork>(network));
				publicBikeNetworks.add(*network);
				_logCreation(
					"Creation of the network with key "+ networkNumber +" ("+ networkName +")"
				);
			}

			// Properties update
			network->setName(networkName);

			return network;
		}

		public_biking::PublicBikeStation* PubliBikeCSVFileFormat::Importer_::_createOrUpdatePublicBikeStation(
			impex::ImportableTableSync::ObjectBySource<public_biking::PublicBikeStationTableSync>& publicBikeStations,
			const public_biking::PublicBikeNetwork& network,
			const std::string& stationNumber,
			const std::string& stationName,
			boost::optional<boost::shared_ptr<geos::geom::Point> > geometry,
			const geography::City& city
		) const {
			public_biking::PublicBikeStation* station(NULL);

			// Search for a station linked with the datasource
			if(publicBikeStations.contains(stationNumber))
			{
				set<public_biking::PublicBikeStation*> loadedStations(publicBikeStations.get(stationNumber));
				if(loadedStations.size() > 1)
				{
					_logWarning(
						"More than one station with key "+ stationNumber
					);
				}
				station = *loadedStations.begin();
				_logLoad(
					"Use of existing station "+ lexical_cast<string>(station->getKey()) +" ("+ station->getName() +")"
				);
			}
			else
			{
				station = new public_biking::PublicBikeStation(
					public_biking::PublicBikeStationTableSync::getId()
				);
				impex::Importable::DataSourceLinks links;
				links.insert(make_pair(&*_import.get<impex::DataSource>(), stationNumber));
				station->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<public_biking::PublicBikeStation>().add(boost::shared_ptr<public_biking::PublicBikeStation>(station));
				publicBikeStations.add(*station);
				_logCreation(
					"Creation of the station with key "+ stationNumber +" ("+ city.getName() +" "+ stationName +")"
				);
			}

			// Properties update
			station->setName(stationName);
			station->setCity(const_cast<geography::City*>(&city));
			station->setNetwork(const_cast<public_biking::PublicBikeNetwork*>(&network));
			if(geometry)
			{
				station->setGeometry(*geometry);
			}

			return station;
		}
}	}
