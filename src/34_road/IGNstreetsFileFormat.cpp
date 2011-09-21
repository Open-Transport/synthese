
/** IGNstreetsFileFormat class implementation.
	@file IGNstreetsFileFormat.cpp

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

#include "IGNstreetsFileFormat.hpp"
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

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,IGNstreetsFileFormat>::FACTORY_KEY("IGNstreets");
	}

	namespace road
	{
		const string IGNstreetsFileFormat::Importer_::FILE_ADDRESS("address");

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
			std::ostream& os,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> adminRequest
		) const {
			// 1 : Administrative areas

			if(key == FILE_ADDRESS)
			{
				// Loading the file into SQLite as virtual table
				VirtualShapeVirtualTable table(filePath, _dataSource.getCharset() , _dataSource.getCoordinatesSystem()->getSRID());

				const GeometryFactory& geometryFactory(CoordinatesSystem::GetCoordinatesSystem(4326).getGeometryFactory());

				{	// Address
					stringstream query;
					query << "SELECT *, AsText(" << IGNstreetsFileFormat::_FIELD_GEOMETRY << ") AS " << IGNstreetsFileFormat::_FIELD_GEOMETRY << "_ASTEXT" << " FROM " << table.getName();
					DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
					while(rows->next())
					{
						MainRoadChunk::HouseNumber numero(rows->getInt(IGNstreetsFileFormat::_FIELD_NUMERO));
						string nomRue(rows->getText(IGNstreetsFileFormat::_FIELD_NOM_VOIE));
						string codeINSEE(rows->getText(IGNstreetsFileFormat::_FIELD_INSEE_COMM));
						shared_ptr<Point> geometry(
							dynamic_pointer_cast<Point, Geometry>(
							rows->getGeometryFromWKT(IGNstreetsFileFormat::_FIELD_GEOMETRY+"_ASTEXT", geometryFactory)
						)	);
						if(!geometry.get())
						{
							os << "ERR : Empty geometry.<br />";
							continue;
						}
						shared_ptr<House> house(new House());
						house->setKey(HouseTableSync::getId());
						house->setGeometry(geometry);
						house->setHouseNumber(numero);
						shared_ptr<City> city(CityTableSync::GetEditableFromCode(codeINSEE, _env));
						// ToDo tester si le RoadPlace a été trouvé + City
						if(city.get())
						{
							boost::shared_ptr<RoadPlace> roadPlace(RoadPlaceTableSync::GetEditableFromCityAndName(city->getKey(),nomRue,_env));
							if(roadPlace.get()) house->setRoadChunkFromRoadPlace(roadPlace);
						}
						_env.getEditableRegistry<House>().add(house);
					}
				}
			}

			os << "<b>SUCCESS : Data loaded</b><br />";

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
			const admin::AdminRequest& request
		) const	{
			AdminFunctionRequest<DataSourceAdmin> importRequest(request);
			PropertiesHTMLTable t(importRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Propriétés");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Données");
			stream << t.cell("Adresses (address)", t.getForm().getTextInput(_getFileParameterName(FILE_ADDRESS), _pathsMap[FILE_ADDRESS].file_string()));
			stream << t.close();
		}



		util::ParametersMap IGNstreetsFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result;
			return result;
		}



		void IGNstreetsFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
		}
}	}
