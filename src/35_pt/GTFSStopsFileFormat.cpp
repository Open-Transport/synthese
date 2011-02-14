
/** GTFSStopsFileFormat class implementation.
	@file GTFSStopsFileFormat.cpp

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

#include "GTFSStopsFileFormat.hpp"
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
#include "ImpExModule.h"
#include "Importer.hpp"
#include "StopPointMoveAction.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "GTFSFileFormat.hpp"
#include "City.h"
#include "PTFileFormat.hpp"
#include "CityTableSync.h"
#include "Junction.hpp"
#include "JunctionTableSync.hpp"
#include "ImpExModule.h"

#include <geos/geom/Point.h>
#include <geos/opDistance.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <fstream>

using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace html;
	using namespace admin;
	using namespace impex;
	using namespace util;
	using namespace db;
	using namespace pt;
	using namespace server;
	using namespace geography;
	
	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,GTFSStopsFileFormat>::FACTORY_KEY("GTFSStops");
	}

	namespace pt
	{
		const std::string GTFSStopsFileFormat::Importer_::FILE_STOPS("0stops");
		const std::string GTFSStopsFileFormat::Importer_::FILE_TRANSFERS("1transfers");

		const std::string GTFSStopsFileFormat::Importer_::PARAMETER_IMPORT_STOP_AREA("isa");
		const std::string GTFSStopsFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string GTFSStopsFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");

		const std::string GTFSStopsFileFormat::Importer_::SEP(",");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<GTFSStopsFileFormat>::Files MultipleFileTypesImporter<GTFSStopsFileFormat>::FILES(
			GTFSStopsFileFormat::Importer_::FILE_STOPS.c_str(),
			GTFSStopsFileFormat::Importer_::FILE_TRANSFERS.c_str(),
		"");
	}

	namespace pt
	{
		GTFSStopsFileFormat::Importer_::Importer_(
			Env& env,
			const DataSource& dataSource
		):	MultipleFileTypesImporter<GTFSStopsFileFormat>(env, dataSource),
			_importStopArea(false),
			_interactive(false),
			_stopPoints(_dataSource, _env)
 		{}



		bool GTFSStopsFileFormat::Importer_::_controlPathsMap() const
		{
			FilePathsMap::const_iterator it;
			it = _pathsMap.find(FILE_STOPS);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		bool GTFSStopsFileFormat::Importer_::_parse(
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
			if(!getline(inFile, line))
			{
				return false;
			}
			_loadFieldsMap(line);

			// Stops
			if(key == FILE_STOPS)
			{
				ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(_dataSource, _env);
				
				// 2.1 : stop areas
				if(_importStopArea)
				{
					PTFileFormat::ImportableStopAreas linkedStopAreas;
					PTFileFormat::ImportableStopAreas nonLinkedStopAreas;

					// Loop
					while(getline(inFile, line))
					{
						_loadLine(line);
						if(_getValue("location_type") != "1")
						{
							continue;
						}

						string id(_getValue("stop_id"));
						string name(_getValue("stop_name"));

						if(request)
						{
							PTFileFormat::ImportableStopArea isa;
							isa.operatorCode = id;
							isa.name = name;
							isa.linkedStopAreas = stopAreas.get(id);
								
							if(isa.linkedStopAreas.empty())
							{
								nonLinkedStopAreas.push_back(isa);
							}
							else
							{
								linkedStopAreas.push_back(isa);
							}
						}
						else
						{
							PTFileFormat::CreateOrUpdateStopAreas(
								stopAreas,
								id,
								name,
								*_defaultCity,
								_stopAreaDefaultTransferDuration,
								_dataSource,
								_env,
								stream
							);
						}
					}

					if(request)
					{
						PTFileFormat::DisplayStopAreaImportScreen(
							nonLinkedStopAreas,
							*request,
							true,
							false,
							_defaultCity,
							_env,
							_dataSource,
							stream
						);
						PTFileFormat::DisplayStopAreaImportScreen(
							linkedStopAreas,
							*request,
							true,
							false,
							_defaultCity,
							_env,
							_dataSource,
							stream
						);
					}
				}

				// 2.2 : stops
				PTFileFormat::ImportableStopPoints linkedStopPoints;
				PTFileFormat::ImportableStopPoints nonLinkedStopPoints;

				// Loop
				inFile.clear();
				inFile.seekg(0, ios::beg);
				getline(inFile, line);
				while(getline(inFile, line))
				{
					_loadLine(line);
					if(_getValue("location_type") != "0")
					{
						continue;
					}

					string id(_getValue("stop_id"));
					string name(_getValue("stop_name"));

					// Stop area
					string stopAreaId(_getValue("parent_station"));
					const StopArea* stopArea(NULL);
					if(stopAreas.contains(stopAreaId))
					{
						stopArea = *stopAreas.get(stopAreaId).begin();
					}
					else if(_stopPoints.contains(id))
					{
						stopArea = (*_stopPoints.get(id).begin())->getConnectionPlace();
					}
					else
					{
						stream << "WARN : inconsistent stop area id "<< stopAreaId <<" in the stop point "<< id <<"<br />";
						continue;
					}

					// Point
					shared_ptr<geos::geom::Point> point(
						CoordinatesSystem::GetCoordinatesSystem(4326).createPoint(
							lexical_cast<double>(_getValue("stop_lon")),
							lexical_cast<double>(_getValue("stop_lat"))
					)	);
					if(point->isEmpty())
					{
						point.reset();
					}

					if(request)
					{
						PTFileFormat::ImportableStopPoint isp;
						isp.operatorCode = id;
						isp.name = name;
						isp.linkedStopPoints = _stopPoints.get(id);
						isp.stopArea = stopArea;
						isp.coords = point;

						if(isp.linkedStopPoints.empty())
						{
							nonLinkedStopPoints.push_back(isp);
						}
						else
						{
							linkedStopPoints.push_back(isp);
						}
					}
					else
					{
						// Creation or update
						PTFileFormat::CreateOrUpdateStopPoints(
							_stopPoints,
							id,
							name,
							*stopArea,
							point.get(),
							_dataSource,
							_env,
							stream
						);
					}
				}

				if(request)
				{
					PTFileFormat::DisplayStopPointImportScreen(
						linkedStopPoints,
						*request,
						_env,
						_dataSource,
						stream
					);
					PTFileFormat::DisplayStopPointImportScreen(
						nonLinkedStopPoints,
						*request,
						_env,
						_dataSource,
						stream
					);
				}
			}
			else if(key == FILE_TRANSFERS)
			{
				//TODO
			}

			inFile.close();

			return false;
		}



		void GTFSStopsFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Fichiers</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Fichiers");
			stream << t.cell("Fichier stops", t.getForm().getTextInput(_getFileParameterName(FILE_STOPS), _pathsMap[FILE_STOPS].file_string()));
			stream << t.cell("Fichier transfers (optionnel)", t.getForm().getTextInput(_getFileParameterName(FILE_TRANSFERS), _pathsMap[FILE_TRANSFERS].file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Import zones d'arrêt", t.getForm().getOuiNonRadioInput(PARAMETER_IMPORT_STOP_AREA, _importStopArea));
			stream << t.cell("Commune par défaut (ID)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity.get() ? lexical_cast<string>(_defaultCity->getKey()) : string()));
			stream << t.cell("Temps de transfert par défaut (min)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_stopAreaDefaultTransferDuration.total_seconds() / 60)));
			stream << t.close();
		}



		db::SQLiteTransaction GTFSStopsFileFormat::Importer_::_save() const
		{
			SQLiteTransaction transaction;
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
			BOOST_FOREACH(const Registry<Junction>::value_type& junction, _env.getRegistry<Junction>())
			{
				JunctionTableSync::Save(junction.second.get(), transaction);
			}
			return transaction;
		}


		void GTFSStopsFileFormat::Importer_::_loadFieldsMap( const std::string& line ) const
		{
			vector<string> cols;
			split(cols, line, is_any_of(SEP));
			_fieldsMap.clear();
			size_t rank(0);
			BOOST_FOREACH(const string& col, cols)
			{
				_fieldsMap[col] = rank;
				++rank;
			}
		}



		std::string GTFSStopsFileFormat::Importer_::_getValue( const std::string& field ) const
		{
			return trim_copy(_line[_fieldsMap[field]]);
		}



		void GTFSStopsFileFormat::Importer_::_loadLine( const std::string& line ) const
		{
			_line.clear();
			string utfLine(ImpExModule::ConvertChar(line, _dataSource.getCharset(), "UTF-8"));
			split(_line, utfLine, is_any_of(SEP));
		}



		server::ParametersMap GTFSStopsFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_IMPORT_STOP_AREA, _importStopArea);
			if(_defaultCity.get())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity->getKey());
			}
			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
			return map;
		}



		void GTFSStopsFileFormat::Importer_::_setFromParametersMap( const server::ParametersMap& map )
		{
			_importStopArea = map.getDefault<bool>(PARAMETER_IMPORT_STOP_AREA, false);
			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));

			if(map.isDefined(PARAMETER_STOP_AREA_DEFAULT_CITY))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
			}
		}
}	}
