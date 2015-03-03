
/** POICSVFileFormat class implementation.
	@file POICSVFileFormat.cpp

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

#include "POICSVFileFormat.hpp"

#include "Import.hpp"
#include "ImpExModule.h"
#include "PublicPlace.h"
#include "DataSource.h"
#include "DBModule.h"
#include "CityTableSync.h"
#include "PTUseRuleTableSync.h"
#include "IConv.hpp"

#include "RoadChunkTableSync.h"
#include "PublicPlaceEntranceTableSync.hpp"
#include "EdgeProjector.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace geos::geom;

namespace synthese
{
	using namespace data_exchange;
	using namespace impex;
	using namespace road;
	using namespace util;
	using namespace db;
	using namespace graph;
	using namespace server;
	using namespace geography;



	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,POICSVFileFormat>::FACTORY_KEY("POI-CSV");
	}



	namespace data_exchange
	{
		const string POICSVFileFormat::Importer_::FILE_LIEUX_PUBLICS = "lieux";
		const string POICSVFileFormat::Importer_::PARAMETER_PROJECT_PLACES = "project";
		const string POICSVFileFormat::Importer_::SEP(";");
	}



	namespace impex
	{
		template<> const MultipleFileTypesImporter<POICSVFileFormat>::Files MultipleFileTypesImporter<POICSVFileFormat>::FILES(
			POICSVFileFormat::Importer_::FILE_LIEUX_PUBLICS.c_str(),
		"");
	}



	namespace data_exchange
	{
		bool POICSVFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_LIEUX_PUBLICS));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		POICSVFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<POICSVFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			RoadFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_publicPlaces(*import.get<DataSource>(), env)
		{}



		bool POICSVFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const string& key
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}
			string line;
			_logDebug(
				"Loading file "+ filePath.file_string() +" as "+ key
			);

			DataSource& dataSource(*_import.get<DataSource>());

			// Public Places
			if(key == FILE_LIEUX_PUBLICS)
			{
				// Loop
				while(getline(inFile, line))
				{
					// Strings
					_loadLine(line);
					string name(_getValue(0));
					string x(_getValue(1));
					string y(_getValue(2));
					string cityCode(_getValue(3));
					string code(_getValue(4));

					// City
					shared_ptr<City> cityForPublicPlace;
					RegistryKeyType cityId;
					try
					{
						cityId = lexical_cast<RegistryKeyType>(cityCode);
					}
					catch(bad_lexical_cast &)
					{
						continue;
					}

					cityForPublicPlace = CityTableSync::GetEditable(cityId, _env);
					if(!cityForPublicPlace.get())
					{
						_logError(
							"ERR : City " + cityCode + " not found used by publicPlace : " + name + ",  " + code + ", " + x + ", " + y + "<br />"
						);
						continue;
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
								"Public Place "+ name + "/" + code +" has invalid coordinate"
							);
						}
					}
					else
					{
						_logWarning(
							"Public Place "+ name + "/" + code +" has invalid coordinate"
						);
					}

					// Public Place creation
					_createOrUpdatePublicPlace(
						_publicPlaces,
						code,
						name,
						point,
						*cityForPublicPlace
					);
				}
			}
			return true;
		}



		db::DBTransaction POICSVFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			algorithm::EdgeProjector<boost::shared_ptr<road::RoadChunk> >::CompatibleUserClassesRequired requiredUserClasses;
			requiredUserClasses.insert(USER_PEDESTRIAN);
			requiredUserClasses.insert(USER_CAR);
			requiredUserClasses.insert(USER_BIKE);

			// Saving of each created or altered objects
			BOOST_FOREACH(const Registry<PublicPlace>::value_type& publicPlace, _env.getEditableRegistry<PublicPlace>())
			{
				PublicPlaceTableSync::Save(publicPlace.second.get(), transaction);
			}
			// If requested, computing of each projection of the public places
			if (_projectAllPublicPlaces)
			{
				BOOST_FOREACH(const Registry<PublicPlace>::value_type& publicPlace, _env.getEditableRegistry<PublicPlace>())
				{
					if (publicPlace.second.get()->getPoint())
					{
						PublicPlaceEntrance entrance;
						entrance.set<PublicPlace>(*publicPlace.second.get());

						RoadChunkTableSync::ProjectAddress(
							*publicPlace.second.get()->getPoint(),
							100,
							entrance,
							requiredUserClasses
						);

						// Saving
						PublicPlaceEntranceTableSync::Save(&entrance, transaction);
					}
				}
			}

			return transaction;
		}



		string POICSVFileFormat::Importer_::_getValue( std::size_t rank ) const
		{
			return trim_copy(_line[rank]);
		}



		void POICSVFileFormat::Importer_::_loadLine( const string& line ) const
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



		util::ParametersMap POICSVFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			if (_projectAllPublicPlaces)
			{
				map.insert(PARAMETER_PROJECT_PLACES, _projectAllPublicPlaces ? 1 : 0);
			}
			return map;
		}



		void POICSVFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			_projectAllPublicPlaces = map.getDefault<bool>(PARAMETER_PROJECT_PLACES, false);
		}
}	}
