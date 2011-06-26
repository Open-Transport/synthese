
/** IneoRealTimeFileFormat class implementation.
	@file IneoRealTimeFileFormat.cpp

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

#include "IneoRealTimeFileFormat.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSource.h"
#include "AdminFunctionRequest.hpp"
#include "DataSourceAdmin.h"
#include "DataSourceTableSync.h"
#include "DBTransaction.hpp"
#include "ImportableTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "StopPointTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "PTFileFormat.hpp"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace gregorian;

namespace synthese
{
	using namespace server;
	using namespace html;
	using namespace util;
	using namespace admin;
	using namespace impex;
	using namespace db;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,pt::IneoRealTimeFileFormat>::FACTORY_KEY("Ineo temps réel");
	}
	
	namespace pt
	{
		const string IneoRealTimeFileFormat::Importer_::PARAMETER_PLANNED_DATASOURCE_ID("ps");

		bool IneoRealTimeFileFormat::Importer_::_read( std::ostream& os, boost::optional<const admin::AdminRequest&> adminRequest ) const
		{
			if(_database.empty() || !_plannedDataSource.get())
			{
				return false;
			}

			date today(day_clock::local_day());
			string todayStr("'"+ to_iso_extended_string(today) +"'");

			// Services linked to the planned source
			ImportableTableSync::ObjectBySource<StopPointTableSync> stops(*_plannedDataSource, _env);
			ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(*_plannedDataSource, _env);

			// 1 : clean the old references to the current source
			ImportableTableSync::ObjectBySource<ScheduledServiceTableSync> sourcedServices(_dataSource, _env);
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<ScheduledServiceTableSync>::Map::value_type& itService, sourcedServices.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<ScheduledServiceTableSync>::Map::mapped_type::value_type& obj, itService.second)
				{
					obj->removeSourceLink(_dataSource);
				}
			}

			// 2 : loop on the services present in the database and link to existing or new services
			stringstream query;
			query << "SELECT ref, chainage, ligne FROM " << _database << ".COURSE WHERE jour=" << todayStr << " AND type='C'";
			DBResultSPtr result(DBModule::GetDB()->execQuery(query.str()));
			while(result->next())
			{
				string serviceRef(result->getText("ref"));
				string chainage(result->getText("chainage"));
				string ligneRef(result->getText("ligne"));

				CommercialLine* line(
					PTFileFormat::GetLine(
						lines,
						ligneRef,
						*_plannedDataSource,
						_env,
						os
				)	);

				stringstream chainageQuery;
				chainageQuery << "SELECT a.mnemol AS mnemol, h.htd AS htd, h.hta AS hta, h.type AS type, c.pos AS pos FROM "
					<< _database << ".ARRETCHN c " <<
					"INNER JOIN " << _database << ".ARRET a ON a.ref=c.arret AND a.jour=c.jour " <<
					"INNER JOIN " << _database << ".HORAIRE h ON h.arretchn=c.ref AND h.jour=a.jour " <<
					"INNER JOIN " << _database << ".COURSE o ON o.chainage=c.chainage AND o.ref=h.course AND c.jour=o.jour " <<
					"WHERE h.course='" << serviceRef << "' AND h.jour=" << todayStr << " ORDER BY c.pos";
				DBResultSPtr chainageResult(DBModule::GetDB()->execQuery(chainageQuery.str()));

				JourneyPattern::StopsWithDepartureArrivalAuthorization servedStops;
				SchedulesBasedService::Schedules departureSchedules;
				SchedulesBasedService::Schedules arrivalSchedules;

				while(chainageResult->next())
				{
					string type(chainageResult->getText("type"));
					string stopCode(chainageResult->getText("mnemol"));
					time_duration departureTime(duration_from_string(chainageResult->getText("htd")));
					time_duration arrivalTime(duration_from_string(chainageResult->getText("hta")));
					MetricOffset stopPos(chainageResult->getInt("pos"));

					std::set<StopPoint*> stopsSet(
						PTFileFormat::GetStopPoints(
							stops,
							stopCode,
							boost::optional<const std::string&>(),
							os
					)	);

					servedStops.push_back(
						JourneyPattern::StopWithDepartureArrivalAuthorization(
							stopsSet,
							stopPos,
							true,
							true,
							optional<bool>()
					)	);

					// Ignoring interpolated times
					if(type != "N")
					{
						// If the bus leaves after midnight, the hours are stored as 0 instead of 24
						if( !departureSchedules.empty() && departureTime < *departureSchedules.rbegin())
						{
							departureTime += hours(24);
						}
						if( !arrivalSchedules.empty() && arrivalTime < *arrivalSchedules.rbegin())
						{
							arrivalTime += hours(24);
						}

						// round of the seconds
						departureTime -= seconds(departureTime.seconds());
						if(arrivalTime.seconds())
						{
							arrivalTime += seconds(60 - arrivalTime.seconds());
						}

						// storage of the times
						departureSchedules.push_back(departureTime);
						arrivalSchedules.push_back(arrivalTime);
					}
				}

				set<JourneyPattern*> routes(
					PTFileFormat::GetRoutes(
						*line,
						servedStops,
						*_plannedDataSource
				)	);

				if(routes.empty())
				{
					int i(0);
					// route creation
				}
				ScheduledService* service(NULL);
				BOOST_FOREACH(JourneyPattern* route, routes)
				{
					BOOST_FOREACH(Service* sservice, route->getServices())
					{
						service = dynamic_cast<ScheduledService*>(sservice);
						if(!service)
						{
							continue;
						}
						if(	service->isActive(today) &&
							service->comparePlannedSchedules(departureSchedules, arrivalSchedules)
						){
							service->setCodeBySource(_dataSource, serviceRef);
							break;
						}
						service = NULL;
					}
					if(service)
					{
						break;
					}
				}

				if(!service)
				{
					int i(0);
					// service creation
				}
			}


			// 3 : loop on the planned services and remove current day of run if not linked to current source
			BOOST_FOREACH(const ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Map::value_type& itLine, lines.getMap())
			{
				BOOST_FOREACH(const ImportableTableSync::ObjectBySource<CommercialLineTableSync>::Map::mapped_type::value_type& obj, itLine.second)
				{
					BOOST_FOREACH(Path* route, obj->getPaths())
					{
						JourneyPattern* jp(static_cast<JourneyPattern*>(route));

						if(!jp->hasLinkWithSource(*_plannedDataSource))
						{
							continue;
						}

						BOOST_FOREACH(const Service* service, jp->getServices())
						{
							const ScheduledService* sservice(dynamic_cast<const ScheduledService*>(service));
							if(	sservice &&
								sservice->isActive(today) &&
								!sservice->hasLinkWithSource(_dataSource)
							){
								const_cast<ScheduledService*>(sservice)->setInactive(today);
							}
						}
					}
			}	}
			return true;
		}



		IneoRealTimeFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	DatabaseReadImporter<IneoRealTimeFileFormat>(env, dataSource)
		{}



		void IneoRealTimeFileFormat::Importer_::displayAdmin( std::ostream& os, const admin::AdminRequest& request ) const
		{
			os << "<h1>Paramètres</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			os << t.open();
			os << t.title("Mode");
			os << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			os << t.title("Bases de données");
			os << t.cell("Base de données SIV", t.getForm().getTextInput(DatabaseReadImporter<IneoRealTimeFileFormat>::PARAMETER_DATABASE, _database));
			os << t.title("Paramètres");
			os << t.cell("Source de données théorique liée", t.getForm().getTextInput(PARAMETER_PLANNED_DATASOURCE_ID, _plannedDataSource.get() ? lexical_cast<string>(_plannedDataSource->getKey()) : string()));
			os << t.close();

		}



		util::ParametersMap IneoRealTimeFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			if(_plannedDataSource.get())
			{
				map.insert(PARAMETER_PLANNED_DATASOURCE_ID, _plannedDataSource->getKey());
			}
			return map;
		}



		void IneoRealTimeFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			if(map.isDefined(PARAMETER_PLANNED_DATASOURCE_ID)) try
			{
				_plannedDataSource = DataSourceTableSync::Get(map.get<RegistryKeyType>(PARAMETER_PLANNED_DATASOURCE_ID), _env);
			}
			catch(ObjectNotFoundException<DataSource>&)
			{
				throw Exception("No such planned data source");
			}
		}



		db::DBTransaction IneoRealTimeFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			return transaction;
		}
	}
}
