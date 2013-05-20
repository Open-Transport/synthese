
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
		template<> const string FactorableTemplate<FileFormat,IGNstreetsFileFormat>::FACTORY_KEY("IGNstreets");
	}

	namespace road
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

	namespace road
	{
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
			const std::string& key,
			boost::optional<const server::Request&> adminRequest
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

				if(!missingStreets.empty())
				{
					stringstream os;
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
					_logger.logRaw(os.str());
				}

				if(_displayStats)
				{
					size_t total(ok + cityNotFound + roadNotFound + emptyStreetName + badGeometry + streetTooFar);

					stringstream os;
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
					_logger.logRaw(os.str());
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



		void IGNstreetsFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const server::Request& request
		) const	{
			AdminFunctionRequest<DataSourceAdmin> importRequest(request);
			PropertiesHTMLTable t(importRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Propriétés");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Distance maximale adresse rue", t.getForm().getTextInput(PARAMETER_MAX_HOUSE_DISTANCE, lexical_cast<string>(_maxHouseDistance)));
			stream << t.title("Données");
			stream << t.cell("Adresses (address)", t.getForm().getTextInput(_getFileParameterName(FILE_ADDRESS), _pathsMap[FILE_ADDRESS].file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Afficher statistiques", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_STATS, _displayStats));
			stream << t.close();
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
			const impex::ImportLogger& logger
		):	impex::Importer(env, import, logger),
			impex::MultipleFileTypesImporter<IGNstreetsFileFormat>(env, import, logger),
			_displayStats(false),
			_maxHouseDistance(200)
		{}
}	}
