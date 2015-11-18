
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

#include "OSMCityBoundariesFileFormat.hpp"
#include "CityTableSync.h"
#include "FrenchPhoneticString.h"
#include "Import.hpp"
#include "OSMParser.hpp"
#include "OSMEntityHandler.hpp"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#include <geos/geom/Polygon.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/CoordinateSequence.h>


namespace synthese 
{
	using namespace geography;

	namespace util
	{
		template<>
		const string FactorableTemplate<impex::FileFormat, data_exchange::OSMCityBoundariesFileFormat>::FACTORY_KEY("OpenStreetMapCityBoundaries");
	}

	namespace data_exchange
	{

		class OSMCitiesHandler : public OSMEntityHandler
		{
			private:

				const impex::Importer& _importer;

				util::Env& _env;

			public:

				OSMCitiesHandler(const impex::Importer& importer, util::Env& env):
					_importer(importer),
					_env(env)
				{ }


				void handleCity(
					const std::string&    cityName,
					const std::string&    cityCode,
					geos::geom::Geometry* boundary)
				{
					_importer._logDebug("Processing city " + cityName);

					boost::shared_ptr<City> city;
					std::string normalizedCityName = boost::to_upper_copy(lexical_matcher::FrenchPhoneticString::to_plain_lower_copy(cityName));
					geos::geom::Polygon* polygonBoundary = NULL;

					if(NULL != boundary)
					{
						// This block converts a geometry object into a polygon
						// Note : this should be a multi-polygon instead, because some cities consist of multiple disjoint areas
						geos::geom::CoordinateSequence *cs = boundary->getCoordinates();

						if(0 < cs->size())
						{
							if(!cs->back().equals(cs->front()))
							{
								cs->add(cs->front());
							}
							geos::geom::LinearRing *lr = CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory().createLinearRing(cs);
							polygonBoundary = CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory().createPolygon(lr, NULL);
						}
					}

					CityTableSync::SearchResult cities = CityTableSync::Search(
						_env,
						boost::optional<std::string>(), // exactname
						((cityCode != "0") ? boost::optional<std::string>() : boost::optional<std::string>(normalizedCityName)), // likeName
						((cityCode != "0") ? boost::optional<std::string>(cityCode) : boost::optional<std::string>()),
						0, 0, true, true,
						util::UP_LINKS_LOAD_LEVEL // code
					);

					if(cities.empty())
					{
						// No matching city was found, create a new one
						_importer._logCreation("New city " + normalizedCityName + " (" + cityCode + ")");

						city = boost::shared_ptr<City>(new City);
						city->set<Name>(normalizedCityName);
						city->set<Code>(cityCode);
						city->set<Key>(CityTableSync::getId());

						if(NULL != polygonBoundary)
						{
							city->set<PolygonGeometry>(boost::shared_ptr<geos::geom::Polygon>(polygonBoundary));
						}
						else
						{
							_importer._logWarning("City " + normalizedCityName + " has no geometry");
						}

						// Add the new city to the registry
						_env.getEditableRegistry<City>().add(city);
					}

					else
					{
						// At least one matching city found, update the first one
						city = cities.front();

						if(NULL != polygonBoundary)
						{
							city->set<PolygonGeometry>(boost::shared_ptr<geos::geom::Polygon>(polygonBoundary));
						}
						else
						{
							_importer._logWarning("City " + normalizedCityName + " has no geometry");
						}
					}
				}

		};


		bool OSMCityBoundariesFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath
		) const {
			boost::filesystem::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);
			boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
			if(!file.good())
			{
				_logError("Unable to open file");
				throw std::runtime_error("Unable to open file");
			}
			std::string ext = boost::filesystem::extension(filePath);
			OSMCitiesHandler handler(*this, _env);
			OSMParser parser(*_fileStream, handler);
			if(ext == ".bz2")
			{
				in.push(boost::iostreams::bzip2_decompressor());
				in.push(file);
				std::istream data(&in);
				parser.parse(data);
			}
			else
			{
				parser.parse(file);
			}

			_logDebug("Finished work on city boundaries");

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
			util::ParametersMap result;
			return result;
		}



		void OSMCityBoundariesFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
		}



		db::DBTransaction OSMCityBoundariesFileFormat::Importer_::_save() const
		{
			db::DBTransaction transaction;
			BOOST_FOREACH(const util::Registry<geography::City>::value_type& city, _env.getEditableRegistry<geography::City>())
			{
				geography::CityTableSync::Save(city.second.get(), transaction);
			}

			return transaction;
		}
	}
}
