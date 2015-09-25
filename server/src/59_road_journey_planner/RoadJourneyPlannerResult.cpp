
/** PTRoutePlannerResult class implementation.
	@file PTRoutePlannerResult.cpp

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

#include "RoadJourneyPlannerResult.h"
#include "Journey.h"
#include "Edge.h"
#include "Crossing.h"
#include "ResultHTMLTable.h"
#include "Road.h"
#include "RoadPath.hpp"
#include "City.h"
#include "RoadPlace.h"
#include "Service.h"
#include "Vertex.h"
#include "DBModule.h"

#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/LineString.h>
#include <geos/geom/CoordinateSequenceFactory.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace algorithm;
	using namespace html;
	using namespace db;

	namespace road_journey_planner
	{

		RoadJourneyPlannerResult::RoadJourneyPlannerResult(
			const geography::Place* departurePlace,
			const geography::Place* arrivalPlace,
			bool samePlaces,
			const TimeSlotRoutePlanner::Result& journeys
		):	_departurePlace(departurePlace),
			_arrivalPlace(arrivalPlace),
			_samePlaces(samePlaces),
			_journeys(journeys)
		{}



		void RoadJourneyPlannerResult::displayHTMLTable(
			ostream& stream
		) const {

			if(_journeys.empty())
			{
				stream << "Aucun r&eacute;sultat trouv&eacute; de " << (
						dynamic_cast<const NamedPlace*>(_departurePlace) ?
						dynamic_cast<const NamedPlace*>(_departurePlace)->getFullName() :
						dynamic_cast<const City*>(_departurePlace)->getName()
					) << " &agrave; " << (
						dynamic_cast<const NamedPlace*>(_arrivalPlace) ?
						dynamic_cast<const NamedPlace*>(_arrivalPlace)->getFullName() :
						dynamic_cast<const City*>(_arrivalPlace)->getName()
					);
				return;
			}

			HTMLTable::ColsVector v;
			v.push_back("&Eacute;tapes");
			v.push_back("Distance");
			v.push_back("Distance");
			v.push_back("Heure");
			HTMLTable t(v, ResultHTMLTable::CSS_CLASS);

			// Solutions display loop
			ptime now(second_clock::local_time());
			stream << t.open();
			for (RoadJourneyPlannerResult::Journeys::const_iterator it(_journeys.begin()); it != _journeys.end(); ++it)
			{
				Journey::ServiceUses::const_iterator its(it->getServiceUses().begin());
				stream << t.row();
				stream << t.col() << (dynamic_cast<const NamedPlace*>(_departurePlace) ?
				   dynamic_cast<const NamedPlace*>(_departurePlace)->getFullName() :
				dynamic_cast<const City*>(_departurePlace)->getName());
				stream << t.col() << "0m";
				stream << t.col() << "0m";
				stream << t.col() << "<b>" << its->getDepartureDateTime() << "</b>";
				double accumDistance = 0;
				double totalDistance(0);
				while(true)
				{
				   const Road* road(dynamic_cast<const RoadPath*>(its->getService()->getPath())->getRoad());
				   double dst = its->getDistance();

				   std::string roadName = road->get<RoadPlace>()->getName();
				   if(roadName.empty()) {
					  if(	road->get<RoadTypeField>() == ROAD_TYPE_PEDESTRIANPATH ||
							road->get<RoadTypeField>() == ROAD_TYPE_PEDESTRIANSTREET
					  ){
						 roadName="Chemin Pi&eacute;ton";
					  }
					  else if(road->get<RoadTypeField>() == ROAD_TYPE_STEPS) {
						 roadName="Escaliers";
					  }
					  else if(road->get<RoadTypeField>() == ROAD_TYPE_BRIDGE) {
						 roadName="Pont / Passerelle";
					  }
					  else if(road->get<RoadTypeField>() == ROAD_TYPE_TUNNEL) {
						 roadName="Tunnel";
					  }
					  else {
						 roadName="Route sans nom";
					  }
				   }
				   Journey::ServiceUses::const_iterator next = its+1;
				   if(next != it->getServiceUses().end())
				   {
					  string nextRoadName(
						  dynamic_cast<const RoadPath*>(next->getService()->getPath())->getRoad()->get<RoadPlace>()->getName()
					  );
					  if(!roadName.compare(nextRoadName))
					  {
						 accumDistance += dst;
						 ++its;
						 continue;
					  }
				   }
				   dst += accumDistance;
				   totalDistance += dst;
				   accumDistance = 0;
				   stream << t.row();
				   stream << t.col() << roadName;
				   stream << t.col() << ((int)dst) << "m";
				   stream << t.col() << ((int) totalDistance) << "m";

				   // Next service use
				   if(its == (it->getServiceUses().end()-1)) break;
				   // Arrival
				   stream << t.col() << its->getArrivalDateTime();
				   ++its;
				}

				// Final arrival
				stream << t.row();
				stream << t.col() << 	(dynamic_cast<const NamedPlace*>(_arrivalPlace) ?
					dynamic_cast<const NamedPlace*>(_arrivalPlace)->getFullName() :
				dynamic_cast<const City*>(_arrivalPlace)->getName());
				stream << t.col() << "0m";
				stream << t.col() << ((int) totalDistance) << "m";
				stream << t.col() << "<b>" << its->getArrivalDateTime() << "</b>";
			}
			stream << t.close();

		}



		std::string RoadJourneyPlannerResult::getTripWKT() const
		{
			const GeometryFactory& gf(CoordinatesSystem::GetDefaultGeometryFactory());
			std::vector<geos::geom::Geometry*> geoms;

			if(!_journeys.empty()) {
				for (RoadJourneyPlannerResult::Journeys::const_iterator it(_journeys.begin()); it != _journeys.end(); ++it) {
					geos::geom::CoordinateSequence *coords(gf.getCoordinateSequenceFactory()->create((size_t)0, 2));

					BOOST_FOREACH(const graph::ServicePointer& su,it->getServiceUses())
					{
						for(const graph::Edge*e(su.getDepartureEdge()); e != su.getArrivalEdge(); e = e->getFollowingArrivalForFineSteppingOnly())
						{
							boost::shared_ptr<LineString> geometry(e->getRealGeometry());
							if(geometry.get())
							{
								for(size_t i(0); i<geometry->getCoordinatesRO()->getSize(); ++i)
								{
									coords->add(geometry->getCoordinatesRO()->getAt(i), false);
								}
							}
						}
					}
					geoms.push_back(dynamic_cast<geos::geom::Geometry*>(gf.createLineString(coords)));
				}
			}
			geos::geom::GeometryCollection *geom = gf.createGeometryCollection(geoms);
			return geom->toText();
		}
	}
}
