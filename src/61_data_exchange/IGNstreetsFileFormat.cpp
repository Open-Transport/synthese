
/** IGNstreetsFileFormat class implementation.
	@file IGNstreetsFileFormat.cpp

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

#include "IGNstreetsFileFormat.hpp"

#include "CrossingTableSync.hpp"
#include "HouseTableSync.hpp"
#include "Import.hpp"
#include "RoadTableSync.h"
#include "RoadChunkTableSync.h"
#include "RoadPlaceTableSync.h"
#include "RoadModule.h"
#include "CityTableSync.h"
#include "DataSource.h"
#include "CoordinatesSystem.hpp"
#include "DBTransaction.hpp"
#include "VirtualShapeVirtualTable.hpp"
#include "FrenchPhoneticString.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <fstream>

using namespace std;
using namespace boost;
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
		template<> const string FactorableTemplate<FileFormat,IGNstreetsFileFormat>::FACTORY_KEY("IGNstreets");
	}

	namespace data_exchange
	{
		const string IGNstreetsFileFormat::Importer_::FILE_ADDRESS("address");
		const string IGNstreetsFileFormat::Importer_::PARAMETER_DISPLAY_STATS("display_stats");
		const string IGNstreetsFileFormat::Importer_::PARAMETER_MAX_HOUSE_DISTANCE("max_house_distance");

		const string IGNstreetsFileFormat::_FIELD_NOM_VOIE("NOM_VOIE");
		const string IGNstreetsFileFormat::_FIELD_NUMERO("NUMERO");
		const string IGNstreetsFileFormat::_FIELD_INSEE_COMM("INSEE_COMM");
		const string IGNstreetsFileFormat::_FIELD_GEOMETRY("Geometry");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<IGNstreetsFileFormat>::Files MultipleFileTypesImporter<IGNstreetsFileFormat>::FILES(
			IGNstreetsFileFormat::Importer_::FILE_ADDRESS.c_str(),
		"");
	}

	namespace data_exchange
	{
		const std::string IGNstreetsFileFormat::Importer_::TAG_MISSING_STREET = "missing_street";
		const std::string IGNstreetsFileFormat::Importer_::ATTR_SOURCE_NAME = "source_name";
		const std::string IGNstreetsFileFormat::Importer_::TAG_CITY = "city";
		const std::string IGNstreetsFileFormat::Importer_::ATTR_IMPORTED_ADDRESSES = "imported_addresses";
		const std::string IGNstreetsFileFormat::Importer_::ATTR_NOT_IMPORTED_CITY_NOT_FOUND = "not_imported_city_not_found";
		const std::string IGNstreetsFileFormat::Importer_::ATTR_NOT_IMPORTED_STREET_NOT_FOUND = "not_imported_street_not_found";
		const std::string IGNstreetsFileFormat::Importer_::ATTR_NOT_IMPORTED_EMPTY_STREET_NAME = "not_imported_empty_street_name";
		const std::string IGNstreetsFileFormat::Importer_::ATTR_NOT_IMPORTED_STREET_TOO_FAR = "not_imported_street_too_far";



		bool IGNstreetsFileFormat::Importer_::_checkPathsMap() const
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



		bool IGNstreetsFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key
		) const {
			// 1 : Administrative areas

			DataSource& dataSource(*_import.get<DataSource>());

			if(key == FILE_ADDRESS)
			{
				// Loading the file into SQLite as virtual table
				VirtualShapeVirtualTable table(
					filePath,
					dataSource.get<Charset>(),
					dataSource.get<CoordinatesSystem>()->getSRID()
				);
				typedef set<pair<City*, string> > MissingStreets;
				MissingStreets missingStreets;
				size_t ok(0);
				size_t streetTooFar(0);
				size_t cityNotFound(0);
				size_t roadNotFound(0);
				size_t emptyStreetName(0);
				size_t badGeometry(0);

				{	// Address
					stringstream query;
					query << "SELECT *, AsText(" << IGNstreetsFileFormat::_FIELD_GEOMETRY << ") AS " << IGNstreetsFileFormat::_FIELD_GEOMETRY << "_ASTEXT" << " FROM " << table.getName();
					DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
					while(rows->next())
					{
						MainRoadChunk::HouseNumber numero(rows->getInt(IGNstreetsFileFormat::_FIELD_NUMERO));
						string nomRue(rows->getText(IGNstreetsFileFormat::_FIELD_NOM_VOIE));
						boost::algorithm::trim(nomRue);
						nomRue = FrenchPhoneticString::to_plain_lower_copy(nomRue);
						if(nomRue.empty())
						{
							++emptyStreetName;
							continue;
						}
						string codeINSEE(rows->getText(IGNstreetsFileFormat::_FIELD_INSEE_COMM));
						boost::shared_ptr<Point> geometry(
							dynamic_pointer_cast<Point, Geometry>(
								rows->getGeometryFromWKT(
									IGNstreetsFileFormat::_FIELD_GEOMETRY+"_ASTEXT",
									dataSource.get<CoordinatesSystem>()->getGeometryFactory()
						)	)	);
						if(!geometry.get())
						{
							_logWarning("Empty geometry.");
							++badGeometry;
							continue;
						}
						boost::shared_ptr<House> house(new House());
						house->setKey(HouseTableSync::getId());
						house->setGeometry(geometry);
						house->setHouseNumber(numero);
						boost::shared_ptr<City> city(CityTableSync::GetEditableFromCode(codeINSEE, _env));
						// ToDo tester si le RoadPlace a été trouvé + City
						if(!city.get())
						{
							++cityNotFound;
							continue;
						}
						boost::shared_ptr<RoadPlace> roadPlace(RoadPlaceTableSync::GetEditableFromCityAndName(city->getKey(),nomRue,_env));
						if(!roadPlace.get())
						{
							++roadNotFound;
							missingStreets.insert(make_pair(city.get(), nomRue));
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
							_logWarning(
								"House "+ lexical_cast<string>(*house->getHouseNumber()) +" could not be projected on "+ nomRue
							);
						}
					}
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



		DBTransaction IGNstreetsFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(Registry<House>::value_type house, _env.getEditableRegistry<House>())
			{
				HouseTableSync::Save(house.second.get(), transaction);
			}
			return transaction;
		}



		util::ParametersMap IGNstreetsFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result;
			result.insert(PARAMETER_DISPLAY_STATS, _displayStats);
			result.insert(PARAMETER_MAX_HOUSE_DISTANCE, _maxHouseDistance);
			return result;
		}



		void IGNstreetsFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			_displayStats = map.getDefault<bool>(PARAMETER_DISPLAY_STATS, false);
			_maxHouseDistance = map.getDefault<double>(PARAMETER_MAX_HOUSE_DISTANCE, 200);
		}



		IGNstreetsFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	impex::Importer(env, import, minLogLevel, logPath, outputStream, pm),
			impex::MultipleFileTypesImporter<IGNstreetsFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			_displayStats(false),
			_maxHouseDistance(200)
		{}
}	}
