
/** HousesCSVFileFormat class implementation.
	@file HousesCSVFileFormat.cpp

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

#include "HousesCSVFileFormat.hpp"

#include "CrossingTableSync.hpp"
#include "HouseTableSync.hpp"
#include "Import.hpp"
#include "RoadTableSync.h"
#include "RoadChunkTableSync.h"
#include "RoadPlaceTableSync.h"
#include "RoadModule.h"
#include "CityTableSync.h"
#include "DataSource.h"
#include "Crossing.h"
#include "CoordinatesSystem.hpp"
#include "DBTransaction.hpp"
#include "VirtualShapeVirtualTable.hpp"
#include "AdminFunctionRequest.hpp"
#include "FrenchPhoneticString.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <fstream>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace geos::geom;

namespace synthese
{
	using namespace data_exchange;
	using namespace road;
	using namespace util;
	using namespace impex;
	using namespace geography;
	using namespace graph;
	using namespace db;
	using namespace server;
	using namespace lexical_matcher;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,HousesCSVFileFormat>::FACTORY_KEY("HousesCSV");
	}

	namespace data_exchange
	{
		const string HousesCSVFileFormat::Importer_::SEP(";");

		const string HousesCSVFileFormat::Importer_::FILE_ADDRESS = "address";
		const string HousesCSVFileFormat::Importer_::PARAMETER_DISPLAY_STATS = "display_stats";
		const string HousesCSVFileFormat::Importer_::PARAMETER_MAX_HOUSE_DISTANCE = "max_house_distance";
		const string HousesCSVFileFormat::Importer_::PARAMETER_NUMBER_OF_LINES_TO_IGNORE = "nb_lines_to_ignore";

		const string HousesCSVFileFormat::PARAMETER_FIELD_CITY_CODE = "field_city_code";
		const string HousesCSVFileFormat::PARAMETER_FIELD_CITY_NAME = "field_city_name";
		const string HousesCSVFileFormat::PARAMETER_FIELD_ROAD_NAME = "field_road_name";
		const string HousesCSVFileFormat::PARAMETER_FIELD_NUMBER = "field_number";
		const string HousesCSVFileFormat::PARAMETER_FIELD_GEOMETRY_X = "field_x";
		const string HousesCSVFileFormat::PARAMETER_FIELD_GEOMETRY_Y = "field_y";

		const std::string HousesCSVFileFormat::Importer_::TAG_MISSING_CITY = "missing_city";
		const std::string HousesCSVFileFormat::Importer_::TAG_MISSING_STREET = "missing_street";
		const std::string HousesCSVFileFormat::Importer_::ATTR_SOURCE_NAME = "source_name";
		const std::string HousesCSVFileFormat::Importer_::TAG_CITY = "city";
		const std::string HousesCSVFileFormat::Importer_::ATTR_IMPORTED_ADDRESSES = "imported_addresses";
		const std::string HousesCSVFileFormat::Importer_::ATTR_NOT_IMPORTED_CITY_NOT_FOUND = "not_imported_city_not_found";
		const std::string HousesCSVFileFormat::Importer_::ATTR_NOT_IMPORTED_STREET_NOT_FOUND = "not_imported_street_not_found";
		const std::string HousesCSVFileFormat::Importer_::ATTR_NOT_IMPORTED_EMPTY_STREET_NAME = "not_imported_empty_street_name";
		const std::string HousesCSVFileFormat::Importer_::ATTR_NOT_IMPORTED_STREET_TOO_FAR = "not_imported_street_too_far";
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HousesCSVFileFormat>::Files MultipleFileTypesImporter<HousesCSVFileFormat>::FILES(
			HousesCSVFileFormat::Importer_::FILE_ADDRESS.c_str(),
		"");
	}

	namespace data_exchange
	{
		HousesCSVFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	impex::Importer(env, import, minLogLevel, logPath, outputStream, pm),
			impex::MultipleFileTypesImporter<HousesCSVFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			_displayStats(false),
			_maxHouseDistance(200)
		{}



		bool HousesCSVFileFormat::Importer_::_checkPathsMap() const
		{
			// ADDRESS
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_ADDRESS));
			if(it == _pathsMap.end() || it->second.empty() || !exists(it->second))
			{
				return false;
			}

			// OK
			return true;
		}



		bool HousesCSVFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key
		) const {
			// 1 : Administrative areas

			DataSource& dataSource(*_import.get<DataSource>());

			if(key == FILE_ADDRESS)
			{
				typedef set<pair<City*, string> > MissingStreets;
				MissingStreets missingStreets;
				set<string> missingCities;
				size_t ok(0);
				size_t streetTooFar(0);
				size_t cityNotFound(0);
				size_t roadNotFound(0);
				size_t emptyStreetName(0);
				size_t badGeometry(0);

				{
					ifstream inFile;
					string line;

					_logDebug("Loading file "+ filePath.file_string());
					inFile.open(filePath.file_string().c_str());
					if(!inFile)
					{
						_logError("Could not open the file "+ filePath.file_string());
						return false;
					}
					// Ignore header lines
					for(int i = 0; i < _numberOfLinesToIgnore; i++)
					{
						if(getline(inFile, line))
							_loadLine(line);
						else
						{
							_logError(
								"Error with the number of lines to ignore : "+ lexical_cast<string>(_numberOfLinesToIgnore)
							);
							return false;
						}
					}

					if(!_cityCodeField || !_cityNameField)
					{
						_logError("City field not defined");
						return false;
					}
					if(!_roadNameField)
					{
						_logError("Road name field not defined");
						return false;
					}
					if(!_numberField)
					{
						_logError("Number field not defined");
						return false;
					}
					if(!_geometryXField)
					{
						_logError("Geometry x field not defined");
						return false;
					}
					if(!_geometryYField)
					{
						_logError("Geometry y field not defined");
						return false;
					}

					while(getline(inFile, line))
					{
						_loadLine(line);

						string cityCode;
						string cityName;
						string roadName;
						MainRoadChunk::HouseNumber number;
						double x;
						double y;

						if(_line.size() > *_cityCodeField)
							cityCode = _getValue(*_cityCodeField);
						else
							continue;

						if(_line.size() > *_cityNameField)
							cityName = _getValue(*_cityNameField);
						else
							continue;

						if(_line.size() > *_roadNameField)
							roadName = _getValue(*_roadNameField);
						else
							continue;
						boost::algorithm::trim(roadName);

						if(_line.size() > *_numberField)
							number = lexical_cast<int>(_getValue(*_numberField));
						else
							continue;
						
						if(_line.size() > *_geometryXField)
							x = lexical_cast<int>(_getValue(*_geometryXField));
						else
							continue;

						if(_line.size() > *_geometryYField)
							y = lexical_cast<int>(_getValue(*_geometryYField));
						else
							continue;					
						boost::shared_ptr<Point> geometry(CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
							*dataSource.getActualCoordinateSystem().createPoint(
								x,
								y
							)
						));

						roadName = FrenchPhoneticString::to_plain_lower_copy(roadName);
						if(roadName.empty())
						{
							++emptyStreetName;
							continue;
						}

						if(!geometry.get())
						{
							_logWarning("Empty geometry");
							++badGeometry;
							continue;
						}

						boost::shared_ptr<House> house(new House());
						house->setKey(HouseTableSync::getId());
						house->setGeometry(geometry);
						house->setHouseNumber(number);
						boost::shared_ptr<City> city(CityTableSync::GetEditableFromCode(cityCode, _env));

						if(!city.get())
						{
							CityTableSync::SearchResult cities = CityTableSync::Search(
								_env,
								boost::optional<std::string>(), // exactname
								boost::optional<std::string>(cityName), // likeName
								boost::optional<std::string>(),
								0, 0, true, true,
								util::UP_LINKS_LOAD_LEVEL
								);

							if(cities.empty())
							{
								++cityNotFound;
								missingCities.insert(cityName);
								continue;
							}
							else
							{
								city = cities.front();
							}
						}

						boost::shared_ptr<RoadPlace> roadPlace(RoadPlaceTableSync::GetEditableFromCityAndName(city->getKey(),roadName,_env));
						if(!roadPlace.get())
						{
							++roadNotFound;
							missingStreets.insert(make_pair(city.get(), roadName));
							continue;
						}

						RoadTableSync::SearchResult paths(RoadTableSync::Search(_env, roadPlace->getKey()));
						BOOST_FOREACH(const boost::shared_ptr<Path>& path, paths)
						{
							RoadChunkTableSync::Search(_env, path->getKey());
						}

						try
						{
							house->setRoadChunkFromRoadPlace(roadPlace, _maxHouseDistance);
							_env.getEditableRegistry<House>().add(house);
							++ok;
						}
						catch(...)
						{
							++streetTooFar;
							_logWarning("House "+ lexical_cast<string>(*house->getHouseNumber()) +" could not be projected on "+ roadName);
						}
					}

				}

				// Export of missing cities
				BOOST_FOREACH(const string missingCity, missingCities)
				{
					// Parameters map
					boost::shared_ptr<ParametersMap> cityPM(new ParametersMap);
					_pm.insert(TAG_MISSING_CITY, cityPM);

					// Name
					cityPM->insert(ATTR_SOURCE_NAME, missingCity);
				}

				// Export of missing streets
				BOOST_FOREACH(const MissingStreets::value_type& missingStreet, missingStreets)
				{
					// Parameters map
					boost::shared_ptr<ParametersMap> streetPM(new ParametersMap);
					_pm.insert(TAG_MISSING_STREET, streetPM);

					// Name
					streetPM->insert(ATTR_SOURCE_NAME, missingStreet.second);

					// City
					boost::shared_ptr<ParametersMap> cityPM(new ParametersMap);
					missingStreet.first->toParametersMap(*streetPM);
					streetPM->insert(TAG_CITY, cityPM);
				}

				if(_displayStats)
				{
					_pm.insert(ATTR_IMPORTED_ADDRESSES, ok);
					_pm.insert(ATTR_NOT_IMPORTED_CITY_NOT_FOUND, cityNotFound);
					_pm.insert(ATTR_NOT_IMPORTED_STREET_NOT_FOUND, roadNotFound);
					_pm.insert(ATTR_NOT_IMPORTED_EMPTY_STREET_NAME, emptyStreetName);
					_pm.insert(ATTR_NOT_IMPORTED_STREET_TOO_FAR, streetTooFar);	
				}
			}

			_logDebug("<b>SUCCESS : Data loaded</b>");

			return true;
		}



		DBTransaction HousesCSVFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(Registry<House>::value_type house, _env.getEditableRegistry<House>())
			{
				HouseTableSync::Save(house.second.get(), transaction);
			}
			return transaction;
		}



		util::ParametersMap HousesCSVFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result;
			result.insert(PARAMETER_DISPLAY_STATS, _displayStats);
			result.insert(PARAMETER_MAX_HOUSE_DISTANCE, _maxHouseDistance);
			result.insert(PARAMETER_NUMBER_OF_LINES_TO_IGNORE, _numberOfLinesToIgnore);

			if(_cityCodeField)
				result.insert(PARAMETER_FIELD_CITY_CODE, *_cityCodeField);
			if(_cityNameField)
				result.insert(PARAMETER_FIELD_CITY_NAME, *_cityNameField);
			if(_roadNameField)
				result.insert(PARAMETER_FIELD_ROAD_NAME, *_roadNameField);
			if(_numberField)
				result.insert(PARAMETER_FIELD_NUMBER, *_numberField);
			if(_geometryXField)
				result.insert(PARAMETER_FIELD_GEOMETRY_X, *_geometryXField);
			if(_geometryYField)
				result.insert(PARAMETER_FIELD_GEOMETRY_Y, *_geometryYField);

			return result;
		}



		void HousesCSVFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			_displayStats = map.getDefault<bool>(PARAMETER_DISPLAY_STATS, false);
			_maxHouseDistance = map.getDefault<double>(PARAMETER_MAX_HOUSE_DISTANCE, 200);
			_numberOfLinesToIgnore = map.getDefault<int>(PARAMETER_NUMBER_OF_LINES_TO_IGNORE,0);

			_cityCodeField = map.getOptional<size_t>(PARAMETER_FIELD_CITY_CODE);
			_cityNameField = map.getOptional<size_t>(PARAMETER_FIELD_CITY_NAME);
			_roadNameField = map.getOptional<size_t>(PARAMETER_FIELD_ROAD_NAME);
			_numberField = map.getOptional<size_t>(PARAMETER_FIELD_NUMBER);
			_geometryXField = map.getOptional<size_t>(PARAMETER_FIELD_GEOMETRY_X);
			_geometryYField = map.getOptional<size_t>(PARAMETER_FIELD_GEOMETRY_Y);
		}



		std::string HousesCSVFileFormat::Importer_::_getValue( const std::size_t field ) const
		{
			return trim_copy(_line[field]);
		}



		void HousesCSVFileFormat::Importer_::_loadLine( const std::string& line ) const
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
}	}
