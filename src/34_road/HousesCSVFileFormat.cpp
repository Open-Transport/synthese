
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
#include "DataSourceAdmin.h"
#include "ImportFunction.h"
#include "PropertiesHTMLTable.h"
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
	using namespace road;
	using namespace util;
	using namespace impex;
	using namespace geography;
	using namespace graph;
	using namespace db;
	using namespace admin;
	using namespace server;
	using namespace html;
	using namespace lexical_matcher;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,HousesCSVFileFormat>::FACTORY_KEY("HousesCSV");
	}

	namespace road
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
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HousesCSVFileFormat>::Files MultipleFileTypesImporter<HousesCSVFileFormat>::FILES(
			HousesCSVFileFormat::Importer_::FILE_ADDRESS.c_str(),
		"");
	}

	namespace road
	{
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
			std::ostream& os,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> adminRequest
		) const {
			// 1 : Administrative areas

			if(key == FILE_ADDRESS)
			{
				typedef set<pair<City*, string> > MissingStreets;
				MissingStreets missingStreets;
				size_t ok(0);
				size_t streetTooFar(0);
				size_t cityNotFound(0);
				size_t roadNotFound(0);
				size_t emptyStreetName(0);
				size_t badGeometry(0);

				{
					ifstream inFile;
					string line;

					os << "INFO : Loading file " << filePath << "<br />";
					inFile.open(filePath.file_string().c_str());
					if(!inFile)
					{
						os << "Could no open the file " << filePath << "<br />";
						return false;
					}
					// Ignore header lines
					for(int i = 0; i < _numberOfLinesToIgnore; i++)
					{
						if(getline(inFile, line))
							_loadLine(line);
						else
						{
							os << "Error with the number of lines to ignore : " << _numberOfLinesToIgnore << "<br />";
							return false;
						}
					}

					if(!_cityCodeField || !_cityNameField)
					{
						os << "ERROR : city field not defined";
						return false;
					}
					if(!_roadNameField)
					{
						os << "ERROR : road name field not defined";
						return false;
					}
					if(!_numberField)
					{
						os << "ERROR : number field not defined";
						return false;
					}
					if(!_geometryXField)
					{
						os << "ERROR : geometry x field not defined";
						return false;
					}
					if(!_geometryYField)
					{
						os << "ERROR : geometry y field not defined";
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
						shared_ptr<Point> geometry(_dataSource.getActualCoordinateSystem().createPoint(x, y));

						roadName = FrenchPhoneticString::to_plain_lower_copy(roadName);
						if(roadName.empty())
						{
							++emptyStreetName;
							continue;
						}

						if(!geometry.get())
						{
							os << "ERR : Empty geometry.<br />";
							++badGeometry;
							continue;
						}

						shared_ptr<House> house(new House());
						house->setKey(HouseTableSync::getId());
						house->setGeometry(geometry);
						house->setHouseNumber(number);
						shared_ptr<City> city(CityTableSync::GetEditableFromCode(cityCode, _env));

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
						BOOST_FOREACH(const shared_ptr<Path>& path, paths)
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
							os << "WARN : house " << *house->getHouseNumber() << " could not be projected on " << roadName << "<br />";
						}
					}

				}

				if(!missingStreets.empty())
				{
					os << "<h1>Rues non trouvées</h1>";

					// Header
					HTMLTable::ColsVector c;
					c.push_back("Commune");
					c.push_back("Rue");

					// Table
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					os << t.open();
					BOOST_FOREACH(const MissingStreets::value_type& missingStreet, missingStreets)
					{
						os << t.row();
						os << t.col() << missingStreet.first->getName();
						os << t.col() << missingStreet.second;
					}
					os << t.close();
				}

				if(_displayStats)
				{
					size_t total(ok + cityNotFound + roadNotFound + emptyStreetName + badGeometry + streetTooFar);

					os << "<h1>Statistiques</h1>";

					// Header
					HTMLTable::ColsVector c;
					c.push_back("Métrique");
					c.push_back("Nombre");
					c.push_back("%");

					// Table
					HTMLTable t(c, ResultHTMLTable::CSS_CLASS);
					os << t.open();
					os << t.row();
					os << t.col() << "Adresses importées";
					os << t.col() << ok;
					os << t.col() << floor(100 * (double(ok) / double(total)));
					os << t.row();
					os << t.col() << "Commune non trouvée";
					os << t.col() << cityNotFound;
					os << t.col() << floor(100 * (double(cityNotFound) / double(total)));
					os << t.row();
					os << t.col() << "Rue non trouvée";
					os << t.col() << roadNotFound;
					os << t.col() << floor(100 * (double(roadNotFound) / double(total)));
					os << t.row();
					os << t.col() << "Nom de rue vide";
					os << t.col() << emptyStreetName;
					os << t.col() << floor(100 * (double(emptyStreetName) / double(total)));
					os << t.row();
					os << t.col() << "Rue trop éloignée";
					os << t.col() << streetTooFar;
					os << t.col() << floor(100 * (double(streetTooFar) / double(total)));
					os << t.close();
				}
			}

			os << "<b>SUCCESS : Data loaded</b><br />";

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



		void HousesCSVFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			AdminFunctionRequest<DataSourceAdmin> importRequest(request);
			PropertiesHTMLTable t(importRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Propriétés");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Distance maximale adresse rue", t.getForm().getTextInput(PARAMETER_MAX_HOUSE_DISTANCE, lexical_cast<string>(_maxHouseDistance)));
			stream << t.title("Données");
			stream << t.cell("Fichier Adresses", t.getForm().getTextInput(_getFileParameterName(FILE_ADDRESS), _pathsMap[FILE_ADDRESS].file_string()));
			stream << t.cell("Nombre de lignes d'entête à ignorer", t.getForm().getTextInput(PARAMETER_NUMBER_OF_LINES_TO_IGNORE, lexical_cast<string>(_numberOfLinesToIgnore)));
			stream << t.title("Numéro des champs");
			stream << t.cell("Code de la ville", t.getForm().getTextInput(PARAMETER_FIELD_CITY_CODE, _cityCodeField ? lexical_cast<string>(*_cityCodeField) : string()));
			stream << t.cell("Nom de la ville", t.getForm().getTextInput(PARAMETER_FIELD_CITY_NAME, _cityNameField ? lexical_cast<string>(*_cityNameField) : string()));
			stream << t.cell("Nom de la rue", t.getForm().getTextInput(PARAMETER_FIELD_ROAD_NAME, _roadNameField ? lexical_cast<string>(*_roadNameField) : string()));
			stream << t.cell("Numéro de l'adresse", t.getForm().getTextInput(PARAMETER_FIELD_NUMBER, _numberField ? lexical_cast<string>(*_numberField) : string()));
			stream << t.cell("Coordonnée X", t.getForm().getTextInput(PARAMETER_FIELD_GEOMETRY_X, _geometryXField ? lexical_cast<string>(*_geometryXField) : string()));
			stream << t.cell("Coordonnée Y", t.getForm().getTextInput(PARAMETER_FIELD_GEOMETRY_Y, _geometryYField ? lexical_cast<string>(*_geometryYField) : string()));
			stream << t.title("Paramètres");
			stream << t.cell("Afficher statistiques", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_STATS, _displayStats));
			stream << t.close();
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
				utfline = IConv(_dataSource.getCharset(), "UTF-8").convert(line);
				split(_line, utfline, is_any_of(SEP));
			}
		}
}	}
