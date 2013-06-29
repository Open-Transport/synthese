
/** IneoDepartureBoardsFileFormat class implementation.
	@file IneoDepartureBoardsFileFormat.cpp

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

#include "IneoDepartureBoardsFileFormat.hpp"

#include "Import.hpp"
#include "DataSource.h"
#include "DataSourceTableSync.h"
#include "DBTransaction.hpp"
#include "ImportableTableSync.hpp"
#include "DisplayScreenTableSync.h"
#include "StopPointTableSync.hpp"
#include "DisplayTypeTableSync.h"
#include "CommercialLineTableSync.h"
#include "DeparturesTableModule.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace gregorian;
using namespace boost::algorithm;

namespace synthese
{
	using namespace data_exchange;
	using namespace departure_boards;
	using namespace server;
	using namespace util;
	using namespace impex;
	using namespace db;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, IneoDepartureBoardsFileFormat>::FACTORY_KEY("Ineo SIV bornes");
	}

	namespace data_exchange
	{
		const string IneoDepartureBoardsFileFormat::Importer_::PARAMETER_PT_DATASOURCE_ID("ps");
		const string IneoDepartureBoardsFileFormat::Importer_::PARAMETER_DEFAULT_DISPLAY_TYPE_ID("dt");



		bool IneoDepartureBoardsFileFormat::Importer_::_read(
		) const	{
			if(_database.empty() || !_ptDataSource.get())
			{
				return false;
			}

			DataSource& dataSource(*_import.get<DataSource>());

			// Services linked to the planned source
			ImportableTableSync::ObjectBySource<StopPointTableSync> stops(*_ptDataSource, _env);
			ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(*_ptDataSource, _env);
			ImportableTableSync::ObjectBySource<DisplayScreenTableSync> screens(dataSource, _env);

			// 1 : sets existing departure boards offline (will be turned online when reloaded except if does not exists anymore)
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DisplayScreenTableSync>::Map::value_type& itScreen, screens.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<DisplayScreenTableSync>::Map::mapped_type::value_type& obj, itScreen.second)
				{
					obj->setMaintenanceIsOnline(false);
				}
			}

			// 2 : loop on the boards present in the database and load or update
			stringstream query;
			query << "SELECT ref, nom FROM " << _database << ".BORNE ORDER BY ref";
			DBResultSPtr result(DBModule::GetDB()->execQuery(query.str()));
			while(result->next())
			{
				string borneRef(result->getText("ref"));
				string nom(result->getText("nom"));

				// Attempt to load existing screen
				DisplayScreen* screen(NULL);
				ImportableTableSync::ObjectBySource<DisplayScreenTableSync>::Set screenSet(screens.get(borneRef));

				// New screen if not exists
				if(screenSet.empty())
				{
					screen = new DisplayScreen(DisplayScreenTableSync::getId());
					screen->setType(_defaultDisplayType.get());
					screen->addCodeBySource(dataSource, borneRef);
					screen->setAllPhysicalStopsDisplayed(false);
					_env.getEditableRegistry<DisplayScreen>().add(boost::shared_ptr<DisplayScreen>(screen));
				}
				else
				{
					if(screenSet.size() > 1)
					{
						_logWarning(
							"More than one existing screen with datasource link "+ borneRef
						);
					}
					screen = *screenSet.begin();
				}

				// Update of properties
				screen->setName(nom);
				if(!nom.empty())
				{
					vector<string> nomParts;
					split(nomParts, nom, is_any_of(" "));
					screen->setMacAddress(nomParts[0]);
				}

				// Allowed lines
				stringstream linesQuery;
				linesQuery << "SELECT ref_ligne, sens FROM " << _database << ".BORNE_ARRET " <<
					"WHERE ref_borne='" << borneRef << "' GROUP BY ref_ligne";
				DBResultSPtr linesResult(DBModule::GetDB()->execQuery(linesQuery.str()));

				LineFilter lineFilter;
				while(linesResult->next())
				{
					CommercialLine* line(
						_getLine(
							lines,
							linesResult->getText("ref_ligne"),
							*_ptDataSource
					)	);
					if(!line)
					{
						continue;
					}
					lineFilter.insert(
						make_pair(
							line,
							linesResult->getText("sens") != "A"
					)	);
				}
				screen->setAllowedLines(lineFilter);

				// Departure stops
				stringstream stopsQuery;
				stopsQuery << "SELECT a.mnemol FROM " << _database << ".BORNE_ARRET b " <<
					"INNER JOIN " << _database << ".ARRET a ON a.ref=b.ref_arret " <<
					"WHERE b.ref_borne='" << borneRef << "' GROUP BY b.ref_arret";
				DBResultSPtr stopsResult(DBModule::GetDB()->execQuery(stopsQuery.str()));

				ArrivalDepartureTableGenerator::PhysicalStops pstops;
				while(stopsResult->next())
				{
					string stopCode(stopsResult->getText("mnemol"));

					set<StopPoint*> stopSet(
						_getStopPoints(
							stops,
							stopCode,
							optional<const string&>(),
							false
					)	);

					BOOST_FOREACH(StopPoint* stop, stopSet)
					{
						pstops.insert(
							make_pair(
								stop->getKey(),
								stop
						)	);
					}
				}
				screen->setStops(pstops);

				// Detection of the location of the screen from the first displayed stop
				if(!pstops.empty())
				{
					const StopPoint* stop(pstops.begin()->second);

					screen->setDisplayedPlace(stop->getConnectionPlace());
					screen->setRoot(
						DeparturesTableModule::GetPlaceWithDisplayBoards(
							stop->getConnectionPlace(),
							_env
					)	);
					screen->setParent(NULL);
				}

				// Screen activation
				screen->setMaintenanceIsOnline(true);
			}

			return true;
		}



		IneoDepartureBoardsFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			DatabaseReadImporter<IneoDepartureBoardsFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		util::ParametersMap IneoDepartureBoardsFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			if(_ptDataSource.get())
			{
				map.insert(PARAMETER_PT_DATASOURCE_ID, _ptDataSource->getKey());
			}
			if(_defaultDisplayType.get())
			{
				map.insert(PARAMETER_DEFAULT_DISPLAY_TYPE_ID, _defaultDisplayType->getKey());
			}
			return map;
		}



		void IneoDepartureBoardsFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			if(map.isDefined(PARAMETER_PT_DATASOURCE_ID)) try
			{
				_ptDataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_PT_DATASOURCE_ID), _env);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw Exception("No such planned data source");
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_DEFAULT_DISPLAY_TYPE_ID,0)) try
			{
				_defaultDisplayType = DisplayTypeTableSync::Get(map.get<RegistryKeyType>(PARAMETER_DEFAULT_DISPLAY_TYPE_ID), _env);
			}
			catch(ObjectNotFoundException<DisplayType>&)
			{
				throw Exception("No such default display type");
			}
		}



		db::DBTransaction IneoDepartureBoardsFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(const Registry<DisplayScreen>::value_type& screen, _env.getRegistry<DisplayScreen>())
			{
				DisplayScreenTableSync::Save(screen.second.get(), transaction);
			}
			return transaction;
		}
}	}
