
/** PTFileFormat class implementation.
	@file PTFileFormat.cpp

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

#include "PTFileFormat.hpp"
#include "LineStop.h"
#include "JourneyPatternTableSync.hpp"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "StopAreaTableSync.hpp"
#include "LineStopTableSync.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "StopPointTableSync.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace impex;

	namespace pt
	{
		boost::shared_ptr<JourneyPattern> PTFileFormat::CreateJourneyPattern(
			const JourneyPattern::StopsWithDepartureArrivalAuthorization& stops,
			CommercialLine& line,
			const impex::DataSource& source,
			Env& env,
			std::ostream& logStream
		){
			shared_ptr<JourneyPattern> route(new JourneyPattern);
			route->setCommercialLine(&line);
			route->setCodeBySource(source, string());
			route->setKey(JourneyPatternTableSync::getId());
			env.getEditableRegistry<JourneyPattern>().add(route);
			line.addPath(route.get());

			size_t rank(0);
			BOOST_FOREACH(const JourneyPattern::StopsWithDepartureArrivalAuthorization::value_type& stop, stops)
			{
				shared_ptr<LineStop> ls(new LineStop);
				ls->setLine(route.get());
				ls->setPhysicalStop(*stop._stop.begin());
				ls->setRankInPath(rank);
				ls->setIsArrival(rank > 0 && stop._arrival);
				ls->setIsDeparture(rank+1 < stops.size() && stop._departure);
				ls->setMetricOffset(stop._metricOffset ? *stop._metricOffset : 0);
				ls->setKey(LineStopTableSync::getId());
				ls->setScheduleInput(stop._withTimes);
				route->addEdge(*ls);
				env.getEditableRegistry<LineStop>().add(ls);

				++rank;
			}

			return route;
		}



		TransportNetwork* PTFileFormat::CreateOrUpdateNetwork(
			impex::ImportableTableSync::ObjectBySource<TransportNetworkTableSync>& networks,
			const std::string& id,
			const std::string& name,
			const impex::DataSource& source,
			util::Env& env,
			std::ostream& logStream
		){
			TransportNetwork* network;
			if(networks.contains(id))
			{
				set<TransportNetwork*> loadedNetworks(networks.get(id));
				if(loadedNetworks.size() > 1)
				{
					logStream << "WARN : more than one network with key " << id << "<br />";
				}
				network = *loadedNetworks.begin();
				logStream << "LOAD : use of existing network " << network->getKey() << " (" << network->getName() << ")<br />";
			}
			else
			{
				network = new TransportNetwork(
					TransportNetworkTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				network->setDataSourceLinks(links);
				env.getEditableRegistry<TransportNetwork>().add(shared_ptr<TransportNetwork>(network));
				networks.add(*network);
				logStream << "CREA : Creation of the network with key " << id << " (" << name <<  ")<br />";
			}
			network->setName(name);
			return network;
		}



		StopArea* PTFileFormat::CreateOrUpdateStopArea(
			impex::ImportableTableSync::ObjectBySource<StopAreaTableSync>& stopAreas,
			const std::string& id,
			const std::string& name,
			const geography::City& city,
			boost::posix_time::time_duration defaultTransferDuration,
			const impex::DataSource& source,
			util::Env& env,
			std::ostream& logStream
		){
			StopArea* stopArea;
			if(stopAreas.contains(id))
			{
				set<StopArea*> loadedStopAreas(stopAreas.get(id));
				if(loadedStopAreas.size() > 1)
				{
					logStream << "WARN : more than one stop area with key " << id << "<br />";
				}
				stopArea = *loadedStopAreas.begin();
			}
			else
			{
				stopArea = new StopArea(
					StopAreaTableSync::getId(),
					true,
					defaultTransferDuration
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				stopArea->setDataSourceLinks(links);
				stopArea->setCity(&city);
				env.getEditableRegistry<StopArea>().add(shared_ptr<StopArea>(stopArea));
				stopAreas.add(*stopArea);
			}
			stopArea->setName(name);
			return stopArea;
		}



		StopPoint* PTFileFormat::CreateOrUpdateStopPoint(
			impex::ImportableTableSync::ObjectBySource<StopPointTableSync>& stopPoints,
			const std::string& id,
			const std::string& name,
			const StopArea& stopArea,
			const StopPoint::Geometry& geometry,
			const impex::DataSource& source,
			util::Env& env,
			std::ostream& logStream
		){
			StopPoint* stopPoint;
			if(stopPoints.contains(id))
			{
				set<StopPoint*> loadedStopPoints(stopPoints.get(id));
				if(loadedStopPoints.size() > 1)
				{
					logStream << "WARN : more than one stop point with key " << id << "<br />";
				}
				stopPoint = *loadedStopPoints.begin();

				logStream
					<< "LOAD : link between stops " << id << " (" << name << ") and "
					<< stopPoint->getKey() << " (" << stopPoint->getConnectionPlace()->getName() << ")<br />";
			}
			else
			{
				stopPoint = new StopPoint(StopPointTableSync::getId());
				stopPoint->setHub(&stopArea);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				stopPoint->setDataSourceLinks(links);
				env.getEditableRegistry<StopPoint>().add(shared_ptr<StopPoint>(stopPoint));
				stopPoints.add(*stopPoint);

				logStream << "CREA : Creation of the physical stop with key " << id << " (" << name <<  ")<br />";
			}

			stopPoint->setName(name);
			stopPoint->setGeometry(
				CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(geometry)
			);
			return stopPoint;
		}



		CommercialLine* PTFileFormat::CreateOrUpdateLine(
			impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync>& lines,
			const std::string& id,
			const std::string& name,
			const std::string& shortName,
			boost::optional<util::RGBColor> color,
			const TransportNetwork& network,
			const impex::DataSource& source,
			util::Env& env,
			std::ostream& logStream
		){
			CommercialLine* line;
			if(lines.contains(id))
			{
				set<CommercialLine*> loadedLines(lines.get(id));
				if(loadedLines.size() > 1)
				{
					logStream << "WARN : more than one line with key " << id << "<br />";
				}
				line = *loadedLines.begin();

				logStream << "LOAD : use of existing commercial line" << line->getKey() << " (" << line->getName() << ")<br />";
			}
			else
			{
				line = new CommercialLine(CommercialLineTableSync::getId());

				logStream << "CREA : Creation of the commercial line with key " << id << " (" << name <<  ")<br />";

				line->setNetwork(&network);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, id));
				line->setDataSourceLinks(links);
				env.getEditableRegistry<CommercialLine>().add(shared_ptr<CommercialLine>(line));
				lines.add(*line);
			}

			line->setName(name);
			line->setShortName(shortName);

			return line;
		}
}	}
