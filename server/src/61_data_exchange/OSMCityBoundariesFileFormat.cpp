
/** OSMCityBoundariesFileFormat class implementation.
	@file OSMCityBoundariesFileFormat.cpp

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

#include "OSMExpatParser.h"
#include "OSMCityBoundariesFileFormat.hpp"

#include "AdminFunctionRequest.hpp"
#include "AllowedUseRule.h"
#include "CityTableSync.h"
#include "Crossing.h"
#include "CrossingTableSync.hpp"
#include "DataSource.h"
#include "DataSourceAdmin.h"
#include "EdgeProjector.hpp"
#include "ForbiddenUseRule.h"
#include "FrenchPhoneticString.h"
#include "Import.hpp"
#include "PropertiesHTMLTable.h"
#include "RoadPath.hpp"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"
#include "RoadChunkEdge.hpp"
#include "RoadChunkTableSync.h"
#include "StopAreaTableSync.hpp"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#include <geos/geom/Point.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/prep/PreparedGeometry.h>
#include <geos/operation/distance/DistanceOp.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::operation;
using namespace geos::geom::prep;

namespace synthese 
{
	using namespace admin;
	using namespace algorithm;
	using namespace data_exchange;
	using namespace db;
	using namespace geography;
	using namespace graph;
	using namespace impex;
	using namespace osm;
	using namespace road;
	using namespace util;

	namespace util
	{
		template<>
		const string FactorableTemplate<FileFormat, OSMCityBoundariesFileFormat>::FACTORY_KEY("OpenStreetMapCityBoundaries");
	}

	namespace data_exchange
	{
		bool OSMCityBoundariesFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath
		) const {
			DataSource& dataSource(*_import.get<DataSource>());

			NetworkPtr network;
			boost::filesystem::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);
			boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
			if(!file.good())
			{
				_logError("Unable to open file");
				throw std::runtime_error("unable to open file");
			}
			std::string ext = boost::filesystem::extension(filePath);
			ExpatParser parser;
			if(ext == ".bz2")
			{
				in.push(boost::iostreams::bzip2_decompressor());
				in.push(file);
				std::istream data(&in);
				network = parser.parse(data);
			}
			else
			{
				network = parser.parse(file);
			}

			network->consolidate(true);

			_logDebug("finished parsing osm xml");

			typedef std::map<unsigned long long int, RelationPtr> RelationMap;
			RelationMap boundaries = network->getAdministrativeBoundaries(8);

			_logDebug("Extracted boundaries");

			// cities
			RelationMap::iterator boundaryIterator = boundaries.begin();
			while(boundaryIterator != boundaries.end())
			{
				RelationPtr boundary = boundaryIterator->second;
				string cityId("0");
				if(boundary->hasTag("ref:INSEE"))
					cityId = boundary->getTag("ref:INSEE");
				std::string cityCode = cityId;
				std::string cityName = to_upper_copy(lexical_matcher::FrenchPhoneticString::to_plain_lower_copy(boundary->getTag(Element::TAG_NAME)));
				_logDebug("treating boundary of " + cityName);
				CityTableSync::SearchResult cities = CityTableSync::Search(
					_env,
					boost::optional<std::string>(), // exactname
					((cityId != "0") ? boost::optional<std::string>() : boost::optional<std::string>(cityName)), // likeName
					((cityId != "0") ? boost::optional<std::string>(cityId) : boost::optional<std::string>()),
					0, 0, true, true,
					util::UP_LINKS_LOAD_LEVEL // code
				);
				boost::shared_ptr<City> city;

				if(cities.empty())
				{
					_logCreation("New City " + cityName + "(" + cityCode + ")");
					city = boost::shared_ptr<City>(new City);
					city->set<Name>(cityName);
					city->set<Code>(cityCode);
					city->set<Key>(CityTableSync::getId());
					_env.getEditableRegistry<City>().add(city);
				}
				else
				{
					city = cities.front();
				}
				geos::geom::CoordinateSequence *cs = boundary->toGeometry()->getCoordinates();
				if (!cs->back().equals(cs->front()))
				{
					cs->add(cs->front());
				}
				geos::geom::LinearRing *lr = CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory().createLinearRing(cs);
				geos::geom::Polygon *p = CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory().createPolygon(lr,NULL);
				city->set<PolygonGeometry>(boost::shared_ptr<geos::geom::Polygon>(p));
				boundaryIterator++;
			}

			_logDebug("finished work on city boundaries");

			return true;
		}



		OSMCityBoundariesFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			OneFileTypeImporter<OSMCityBoundariesFileFormat>(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		util::ParametersMap OSMCityBoundariesFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result;

			return result;
		}



		void OSMCityBoundariesFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
		}



		db::DBTransaction OSMCityBoundariesFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(const Registry<City>::value_type& city, _env.getEditableRegistry<City>())
			{
				CityTableSync::Save(city.second.get(), transaction);
			}
			return transaction;
		}
	}
}
