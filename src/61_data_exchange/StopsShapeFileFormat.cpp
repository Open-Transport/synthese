
/** StopsShapeFileFormat class implementation.
	@file StopsShapeFileFormat.cpp

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

#include "StopsShapeFileFormat.hpp"

#include "Import.hpp"
#include "ImpExModule.h"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "DataSource.h"
#include "CoordinatesSystem.hpp"
#include "IConv.hpp"
#include "Importer.hpp"
#include "DBModule.h"
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
	using namespace data_exchange;
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace graph;
	using namespace server;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,StopsShapeFileFormat>::FACTORY_KEY("StopsShapefile");
	}

	namespace data_exchange
	{
		const std::string StopsShapeFileFormat::Importer_::FILE_SHAPE("shape");

		const std::string StopsShapeFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");

		const std::string StopsShapeFileFormat::Importer_::PARAMETER_FIELD_STOP_NAME1("stop_name1");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_FIELD_STOP_DIRECTION("stop_direction");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_FIELD_STOP_OPERATOR_CODE("stop_operator_code");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_FIELD_CITY_NAME("city_name");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_FIELD_CITY_CODE("city_code");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_USE_DIRECTION("use_direction");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_VALUE_FORWARD_DIRECTION("value_forward_direction");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_VALUE_BACKWARD_DIRECTION("value_backward_direction");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_VALUE_FORWARD_BACKWARD_DIRECTION("value_forward_backward_direction");
		const std::string StopsShapeFileFormat::Importer_::PARAMETER_UPDATE_ONLY("update_only");
		const std::string StopsShapeFileFormat::Importer_::_FIELD_GEOMETRY("Geometry");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<StopsShapeFileFormat>::Files MultipleFileTypesImporter<StopsShapeFileFormat>::FILES(
			StopsShapeFileFormat::Importer_::FILE_SHAPE.c_str(),
		"");
	}

	namespace data_exchange
	{
		bool StopsShapeFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_SHAPE));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		StopsShapeFileFormat::Importer_::Importer_(
			util::Env& env,
			const Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			MultipleFileTypesImporter<StopsShapeFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_displayLinkedStops(false),
			_stopPoints(*import.get<DataSource>(), _env)
		{}



		bool StopsShapeFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key
		) const {
			PTFileFormat::ImportableStopPoints linkedStopPoints;
			PTFileFormat::ImportableStopPoints nonLinkedStopPoints;

			DataSource& dataSource(*_import.get<DataSource>());

			// Loading the file into SQLite as virtual table
			VirtualShapeVirtualTable table(
				filePath,
				dataSource.get<Charset>(),
				dataSource.get<CoordinatesSystem>()->getSRID()
			);

			stringstream query;
			query << "SELECT *, AsText(" << _FIELD_GEOMETRY << ") AS " << _FIELD_GEOMETRY << "_ASTEXT FROM " << table.getName();

			DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));

			while(rows->next())
			{
				boost::shared_ptr<Point> geometry(
					dynamic_pointer_cast<Point, Geometry>(
						rows->getGeometryFromWKT(
							_FIELD_GEOMETRY+"_ASTEXT",
							dataSource.get<CoordinatesSystem>()->getGeometryFactory()
				)	)	);
				if(!geometry.get())
				{
					_logWarning("Empty geometry.");
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
					stopAreaName = trim_copy(rows->getText(*_stopName1));
					stopPointName = rows->getText(*_stopName1);
				}
				if(_stopDirection && !_useDirection)
				{
					stopPointName += " " + trim_copy(rows->getText(*_stopDirection));
				}

				if(_stopOperatorCode && !_useDirection)
					stopOperatorCode = to_lower_copy(trim_copy(rows->getText(*_stopOperatorCode)));
				else if(_useDirection && _stopDirection)
				{
					stopOperatorCode = to_lower_copy(trim_copy(rows->getText(*_stopName1)));
					if(!_valueForwardDirection || !_valueBackwardDirection)
					{
						_logError("ERR : value Forward or Backward direction is not defined.");
						return false;
					}

					if(trim_copy(rows->getText(*_stopDirection)) == *_valueForwardDirection)
					{
						stopOperatorCode += " A";
					}

					else if (trim_copy(rows->getText(*_stopDirection)) == *_valueBackwardDirection)
					{
						stopOperatorCode += " R";
					}
				}

				// TODO Add a generic function
				replace_all(stopOperatorCode,"é","e");
				replace_all(stopOperatorCode,"è","e");
				replace_all(stopOperatorCode,"ê","e");
				replace_all(stopOperatorCode,"ë","e");
				replace_all(stopOperatorCode,"â","a");
				replace_all(stopOperatorCode,"à","a");
				replace_all(stopOperatorCode,"ô","o");
				replace_all(stopOperatorCode,"'"," ");
				replace_all(stopOperatorCode,"-"," ");
				replace_all(stopOperatorCode,"*","");

				if(_cityCode)
					cityCode = trim_copy(rows->getText(*_cityCode));

				if(_cityName)
					cityName = trim_copy(rows->getText(*_cityName));

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
						_logWarning(
							"City "+ cityName +" / "+ cityCode +" not found"
						);
					}
					else
					{
						city = cities.front();
					}
				}

				PTFileFormat::ImportableStopPoint isp;
				isp.name = stopPointName;
				isp.linkedStopPoints = _stopPoints.get(stopOperatorCode);
				isp.coords = geometry;
				if(city.get())
				{
					isp.cityName = city->getName();
				}
				else if(_defaultCity.get())
				{
					isp.cityName = _defaultCity->getName();
				}

				if(isp.linkedStopPoints.empty())
				{
					nonLinkedStopPoints.insert(
						make_pair(stopOperatorCode, isp)
					);
				}
				else if(_displayLinkedStops)
				{
					linkedStopPoints.insert(
						make_pair(stopOperatorCode, isp)
					);
				}

				const City* cityForStopAreaAutoGeneration;

				if(city.get())
				{
					cityForStopAreaAutoGeneration = city.get();
				}
				else if(_defaultCity.get())
				{
					cityForStopAreaAutoGeneration = _defaultCity.get();
				}
				else
				{
					_logWarning("City not defined");
					continue;
				}

				if(_updateOnly)
				{
					_createOrUpdateStop(
						_stopPoints,
						stopOperatorCode,
						stopPointName,
						optional<const graph::RuleUser::Rules&>(),
						optional<const StopArea*>(),
						geometry.get(),
						dataSource
					);
				}
				else
				{
					_createOrUpdateStopWithStopAreaAutocreation(
						_stopPoints,
						stopOperatorCode,
						stopPointName,
						geometry.get(),
						*cityForStopAreaAutoGeneration,
						_stopAreaDefaultTransferDuration,
						dataSource,
						boost::optional<const graph::RuleUser::Rules&>()
					);
				}
			}

			_exportStopPoints(nonLinkedStopPoints);
			if(_displayLinkedStops)
			{
				_exportStopPoints(linkedStopPoints);
			}

			return true;
		}



		db::DBTransaction StopsShapeFileFormat::Importer_::_save() const
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



		util::ParametersMap StopsShapeFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());
			map.insert(PARAMETER_UPDATE_ONLY, _updateOnly);
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			map.insert(PARAMETER_USE_DIRECTION, _useDirection);
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

			if(_stopDirection)
				map.insert(PARAMETER_FIELD_STOP_DIRECTION,*_stopDirection);

			if(_stopOperatorCode)
				map.insert(PARAMETER_FIELD_STOP_OPERATOR_CODE,*_stopOperatorCode);

			if(_cityName)
				map.insert(PARAMETER_FIELD_CITY_NAME,*_cityName);

			if(_cityCode)
				map.insert(PARAMETER_FIELD_CITY_CODE,*_cityCode);

			if(_valueForwardDirection)
				map.insert(PARAMETER_VALUE_FORWARD_DIRECTION,*_valueForwardDirection);

			if(_valueBackwardDirection)
				map.insert(PARAMETER_VALUE_BACKWARD_DIRECTION,*_valueBackwardDirection);

			if(_valueForwardBackwardDirection)
				map.insert(PARAMETER_VALUE_FORWARD_BACKWARD_DIRECTION,*_valueForwardBackwardDirection);

			return map;
		}



		void StopsShapeFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);

			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 5));
			_updateOnly = map.getDefault<bool>(PARAMETER_UPDATE_ONLY, false);
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);

			_useDirection = map.getDefault<bool>(PARAMETER_USE_DIRECTION, false);

			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);

			_stopName1 = map.getOptional<std::string>(PARAMETER_FIELD_STOP_NAME1);
			_stopDirection = map.getOptional<std::string>(PARAMETER_FIELD_STOP_DIRECTION);
			_stopOperatorCode = map.getOptional<std::string>(PARAMETER_FIELD_STOP_OPERATOR_CODE);
			_cityCode = map.getOptional<std::string>(PARAMETER_FIELD_CITY_CODE);
			_cityName = map.getOptional<std::string>(PARAMETER_FIELD_CITY_NAME);
			_valueForwardDirection = map.getOptional<std::string>(PARAMETER_VALUE_FORWARD_DIRECTION);
			_valueBackwardDirection = map.getOptional<std::string>(PARAMETER_VALUE_BACKWARD_DIRECTION);
			_valueForwardBackwardDirection = map.getOptional<std::string>(PARAMETER_VALUE_FORWARD_BACKWARD_DIRECTION);
		}
}	}
