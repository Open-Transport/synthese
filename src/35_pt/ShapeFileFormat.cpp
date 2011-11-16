
/** ShapeFileFormat class implementation.
	@file ShapeFileFormat.cpp

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

#include "ShapeFileFormat.hpp"
#include "StopArea.hpp"
#include "PTFileFormat.hpp"
#include "ImpExModule.h"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
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
#include "CoordinatesSystem.hpp"
#include "IConv.hpp"
#include "Importer.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "ShapeFileFormat.hpp"
#include "City.h"
#include "PTFileFormat.hpp"
#include "CityTableSync.h"
#include "VirtualShapeVirtualTable.hpp"

#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace geos::geom;

namespace synthese
{
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace graph;
	using namespace html;
	using namespace admin;
	using namespace server;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,ShapeFileFormat>::FACTORY_KEY("Shapefile");
	}

	namespace pt
	{
		const std::string ShapeFileFormat::Importer_::FILE_SHAPE("shape");

		const std::string ShapeFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string ShapeFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string ShapeFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");

		const std::string ShapeFileFormat::Importer_::PARAMETER_FIELD_STOP_NAME1("stop_name1");
		const std::string ShapeFileFormat::Importer_::PARAMETER_FIELD_STOP_NAME2("stop_name2");
		const std::string ShapeFileFormat::Importer_::PARAMETER_FIELD_STOP_OPERATOR_CODE("stop_operator_code");
		const std::string ShapeFileFormat::Importer_::PARAMETER_FIELD_CITY_NAME("city_name");
		const std::string ShapeFileFormat::Importer_::PARAMETER_FIELD_CITY_CODE("city_code");

		const std::string ShapeFileFormat::Importer_::_FIELD_GEOMETRY("Geometry");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<ShapeFileFormat>::Files MultipleFileTypesImporter<ShapeFileFormat>::FILES(
			ShapeFileFormat::Importer_::FILE_SHAPE.c_str(),
		"");
	}


	namespace pt
	{
		bool ShapeFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_SHAPE));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		ShapeFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	MultipleFileTypesImporter<ShapeFileFormat>(env, dataSource),
			PTDataCleanerFileFormat(env, dataSource),
			Importer(env, dataSource),
			_displayLinkedStops(false),
			_stopPoints(_dataSource, _env)
		{}



		bool ShapeFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> request
		) const {

			ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(_dataSource, _env);
			map<string, StopArea*> stopAreasByName;

			// 2.2 : stops
			PTFileFormat::ImportableStopPoints linkedStopPoints;
			PTFileFormat::ImportableStopPoints nonLinkedStopPoints;


			// Loading the file into SQLite as virtual table
			VirtualShapeVirtualTable table(filePath, _dataSource.getCharset() , _dataSource.getCoordinatesSystem()->getSRID());
			size_t badGeometry(0);

			stringstream query;
			query << "SELECT *, AsText(" << _FIELD_GEOMETRY << ") AS " << _FIELD_GEOMETRY << "_ASTEXT FROM " << table.getName();

			DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));

			while(rows->next())
			{
				shared_ptr<Point> geometry(
					dynamic_pointer_cast<Point, Geometry>(
						rows->getGeometryFromWKT(
							_FIELD_GEOMETRY+"_ASTEXT",
							_dataSource.getCoordinatesSystem()->getGeometryFactory()
				)	)	);
				if(!geometry.get())
				{
					stream << "ERR : Empty geometry.<br />";
					++badGeometry;
					continue;
				}

				string stopAreaName;
				string stopPointName;
				string stopOperatorCode;
				string cityCode;
				string cityName;
				boost::shared_ptr<geography::City> city;

				if(_stopName1)
				{
					stopAreaName = rows->getText(*_stopName1);
					stopPointName = rows->getText(*_stopName1);
				}
				if(_stopName2)
					stopPointName += " " + rows->getText(*_stopName2);

				if(_stopOperatorCode)
					stopOperatorCode = rows->getText(*_stopOperatorCode);

				if(_cityCode)
					cityCode = rows->getText(*_cityCode);

				if(_cityName)
					cityName = rows->getText(*_cityName);

				if(_cityCode || _cityName)
				{
					CityTableSync::SearchResult cities(
						CityTableSync::Search(
							_env,
							optional<std::string>(),
							_cityName ? cityName : optional<std::string>(),
							_cityCode ? cityCode : optional<std::string>(),
							0,
							1)
					);
					if(cities.empty())
					{
						stream << "WARN : City " << cityName << " / " << cityCode << " not found<br />";
					}
					else
					{
						city = cities.front();
					}
				}

				if(request)
				{
					PTFileFormat::ImportableStopPoint isp;
					isp.operatorCode = stopOperatorCode;
					isp.name = stopPointName;
					isp.linkedStopPoints = _stopPoints.get(stopOperatorCode);
					isp.coords = geometry;
					if(city.get())
						isp.cityName = city->getName();
					else if(_defaultCity.get())
						isp.cityName = _defaultCity->getName();

					if(isp.linkedStopPoints.empty())
					{
						nonLinkedStopPoints.push_back(isp);
					}
					else if(_displayLinkedStops)
					{
						linkedStopPoints.push_back(isp);
					}
				}

				if(_stopPoints.contains(stopOperatorCode))
				{
					BOOST_FOREACH(StopPoint* stopPoint, _stopPoints.get(stopOperatorCode))
					{
						stopPoint->setName(stopPointName);
						if(geometry.get())
						{
							stopPoint->setGeometry(geometry);
						}
						const_cast<StopArea*>(stopPoint->getConnectionPlace())->setName(stopAreaName);
					}
				}
				else
				{
					StopArea* stopArea(NULL);
					 // Search in the last created stop areas map
					map<string, StopArea*>::const_iterator it(stopAreasByName.find(stopAreaName));
					if(it != stopAreasByName.end())
					{
						stopArea = it->second;
					}
					// Search in the database
					if(!stopArea)
					{
						StopAreaTableSync::SearchResult stopAreas(
							StopAreaTableSync::Search(
								_env,
								(city.get() ? city->getKey() : (_defaultCity.get() ? _defaultCity->getKey() : boost::optional<util::RegistryKeyType>())),
								logic::indeterminate,
								optional<string>(),
								stopAreaName
						)	);
						if(!stopAreas.empty())
						{
							stopArea = stopAreas.begin()->get();
						}
					}
					// Creation of the stop area
					if(!stopArea)
					{
						stopArea = new StopArea(
							StopAreaTableSync::getId(),
							true,
							_stopAreaDefaultTransferDuration
						);
						if(city.get())
							stopArea->setCity(city.get());
						else if(_defaultCity.get())
							stopArea->setCity(_defaultCity.get());
						stopArea->setName(stopAreaName);
						_env.getEditableRegistry<StopArea>().add(shared_ptr<StopArea>(stopArea));
						stopAreasByName.insert(make_pair(stopAreaName, stopArea));
					}
					PTFileFormat::CreateOrUpdateStop(
						_stopPoints,
						stopOperatorCode,
						stopPointName,
						NULL,
						stopArea,
						geometry.get(),
						_dataSource,
						_env,
						stream
					);
				}
			}

			if(request)
			{
				PTFileFormat::DisplayStopPointImportScreen(
					nonLinkedStopPoints,
					*request,
					_env,
					_dataSource,
					stream
					);
				if(_displayLinkedStops)
				{
					PTFileFormat::DisplayStopPointImportScreen(
						linkedStopPoints,
						*request,
						_env,
						_dataSource,
						stream
					);
				}
			}
			return true;
		}



		void ShapeFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Fichiers</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			t.getForm().addHiddenField(PTDataCleanerFileFormat::PARAMETER_FROM_TODAY, string("1"));
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Effacer données existantes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_OLD_DATA, false));
			stream << t.cell("Effacer arrêts inutilisés", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_UNUSED_STOPS, _cleanUnusedStops));
			stream << t.title("Fichiers");
			stream << t.cell("Fichier ShapeFile (arrêts)", t.getForm().getTextInput(_getFileParameterName(FILE_SHAPE), _pathsMap[FILE_SHAPE].file_string()));
			stream << t.title("Paramètres Généraux");
			stream << t.cell("Affichage arrêts liés", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops));
			stream << t.cell("Commune par défaut (ID)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity.get() ? lexical_cast<string>(_defaultCity->getKey()) : string()));
			stream << t.cell("Temps de transfert par défaut (min)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_stopAreaDefaultTransferDuration.total_seconds() / 60)));
			stream << t.title("Attributs du fichier arrêts");
			stream << t.cell("Nom principal de l'arrêt", t.getForm().getTextInput(PARAMETER_FIELD_STOP_NAME1, _stopName1 ? *_stopName1 : string()));
			stream << t.cell("Nom complémentaire de l'arrêt physique", t.getForm().getTextInput(PARAMETER_FIELD_STOP_NAME2,  _stopName2 ? *_stopName2 : string()));
			stream << t.cell("Code Opérateur de l'arrêt", t.getForm().getTextInput(PARAMETER_FIELD_STOP_OPERATOR_CODE,  _stopOperatorCode ? *_stopOperatorCode : string()));
			stream << t.cell("Nom de la commune", t.getForm().getTextInput(PARAMETER_FIELD_CITY_NAME, _cityName ? *_cityName : string()));
			stream << t.cell("Code de la commune", t.getForm().getTextInput(PARAMETER_FIELD_CITY_CODE, _cityCode ? *_cityCode : string()));
			stream << t.close();
		}



		db::DBTransaction ShapeFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			PTDataCleanerFileFormat::_addRemoveQueries(transaction);

			// Saving of each created or altered objects
			BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
			{
				StopAreaTableSync::Save(cstop.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
			{
				StopPointTableSync::Save(stop.second.get(), transaction);
			}

			return transaction;
		}



		util::ParametersMap ShapeFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			if(_defaultCity.get())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity->getKey());
			}
			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}

			if(_stopName1)
				map.insert(PARAMETER_FIELD_STOP_NAME1,*_stopName1);

			if(_stopName2)
				map.insert(PARAMETER_FIELD_STOP_NAME2,*_stopName2);

			if(_stopOperatorCode)
				map.insert(PARAMETER_FIELD_STOP_OPERATOR_CODE,*_stopOperatorCode);

			if(_cityName)
				map.insert(PARAMETER_FIELD_CITY_NAME,*_cityName);

			if(_cityCode)
				map.insert(PARAMETER_FIELD_CITY_CODE,*_cityCode);

			return map;
		}



		void ShapeFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);

			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);

			_stopName1 = map.getOptional<std::string>(PARAMETER_FIELD_STOP_NAME1);
			_stopName2 = map.getOptional<std::string>(PARAMETER_FIELD_STOP_NAME2);
			_stopOperatorCode = map.getOptional<std::string>(PARAMETER_FIELD_STOP_OPERATOR_CODE);
			_cityCode = map.getOptional<std::string>(PARAMETER_FIELD_CITY_CODE);
			_cityName = map.getOptional<std::string>(PARAMETER_FIELD_CITY_NAME);
		}
}	}
