
/** EdgeProjector class implementation.
	@file EdgeProjector.cpp

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

#include "EdgeProjector.hpp"
#include "PathGroup.h"
#include "Path.h"
#include "Edge.h"
#include "Vertex.h"
#include "Log.h"

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

#include <geos/geom/Geometry.h>
#include <geos/geom/Point.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/linearref/LinearLocation.h>
#include <geos/linearref/LengthIndexedLine.h>

using namespace geos::geom;

namespace synthese
{
	using namespace graph;

	namespace algorithm
	{
		EdgeProjector::EdgeProjector(
			const EdgeProjector::From& from,
			double distanceLimit
		):	_from(from),
			_distanceLimit(distanceLimit)
		{}



		EdgeProjector::~EdgeProjector() {
		}


		EdgeProjector::PathNearby EdgeProjector::projectEdge(
			const Coordinate& pt
		) const {
		   PathsNearby candidates = getPathsByDistance(pt);
		   if(candidates.empty())
		   {
			   throw NotFoundException();
		   }
		   EdgeProjector::PathNearby bestRoadNearby = candidates.begin()->second;
		   return bestRoadNearby;

		#if 0
		   Coordinate bestPoint = bestRoadNearby.get<0>();
		   Path* bestRoad = bestRoadNearby.get<1>();
		   double projectedOffset = bestRoadNearby.get<2>();

		   assert(stop->getGeometry()->distance(bestPoint.getGeometry().get()) <= distance_limit);

		   //look if we already have an address that corresponds to this stop
		   std::vector<graph::Edge*> edges = bestRoad->getEdges();
		   boost::shared_ptr<Address> address;

		   BOOST_FOREACH(graph::Edge* e, edges) {
			  if(e->getFromVertex()->getDistanceTo(bestPoint) == candidates.begin()->first) {
				 address.reset(reinterpret_cast<Address*>(e->getFromVertex()));
			  }
		   }

		   if(!address) {
			  address.reset(new Address);
			  address->setXY(bestPoint.getX(),bestPoint.getY());
			  address->setKey(AddressTableSync::getId());
			  _env->getEditableRegistry<Address>().add(address);
		   }

		   PublicTransportStopZoneConnectionPlace* place(const_cast<PublicTransportStopZoneConnectionPlace*>(stop->getConnectionPlace()));
		   pt::PhysicalStopTableSync::Search(*_env, place->getKey());
		   AddressTableSync::Search(*_env, place->getKey());
		   place->addAddress(address.get());
		   BOOST_FOREACH(const Address* add2, place->getAddresses())
		   {
			  if(add2 == address.get())
			  {
				 place->addTransferDelay(add2->getKey(), address->getKey(), boost::posix_time::minutes(0));
			  }
			  else
			  {
				 add2->getKey();
				 address->getKey();
				 place->addForbiddenTransferDelay(add2->getKey(), address->getKey());
				 place->addForbiddenTransferDelay(address->getKey(), add2->getKey());
			  }
		   }
		   BOOST_FOREACH(const PublicTransportStopZoneConnectionPlace::PhysicalStops::value_type& ps2, place->getPhysicalStops())
		   {
			  if(ps2.second == stop.get())
			  {
				 place->addTransferDelay(
					ps2.second->getKey(),
					address->getKey(),
					boost::posix_time::minutes(0)
				 );
				 place->addTransferDelay(
					address->getKey(),
					ps2.second->getKey(),
					boost::posix_time::minutes(0)
				 );
			  }
			  else
			  {
				 place->addForbiddenTransferDelay(ps2.second->getKey(), address->getKey());
				 place->addForbiddenTransferDelay(address->getKey(), ps2.second->getKey());
			  }
		   }
		   address->setHub(place);


		   double beforeLength = bestRoad->getGeometry()->getLength();
		   RoadChunk* newRoadChunk = splitRoad(bestRoad, projectedOffset, address.get());
		   double afterLength = bestRoad->getGeometry()->getLength();
		   assert(std::abs(beforeLength-afterLength)<0.1);
		   const graph::Vertex* pv = newRoadChunk->getFromVertex();
		   double pd = pv->getDistanceTo(*stop);
		   assert(pd <= distance_limit);

		   std::stringstream debugmsg;
		   debugmsg << "Projected stop " << stop->getName() << " (" << stop->getKey() << ") onto road " << bestRoad->getRoadPlace()->getName() << " (" << bestRoad->getKey() << ")";
		   util::Log::GetInstance().info(debugmsg.str());
		   return bestRoad;
		#endif
		}

		#if 0
		EdgeProjector::RoadsNearby EdgeProjector::getRoadsByDistance(boost::shared_ptr<pt::PhysicalStop> stop, double distance_limit) {
		   City* city = const_cast<City*>(stop->getConnectionPlace()->getCity());

		   std::map<util::RegistryKeyType,std::vector<Road*> >::iterator it = roadsByCity.find(city->getKey());
		   if(it == roadsByCity.end()) {
			  util::Log::GetInstance().info("extracting roads of "+city->getName());
			  std::vector<Road*> roads;

		   #if 0
			  //get roads for city
			  const lexmatcher::LexicalMatcher<const NamedPlace*>::Map roadsMap = city->getLexicalMatcher("RoadPlace").entries();
			  //geometry::Point2D pt = city->getPoint();
			  typedef std::pair<lexmatcher::FrenchSentence, const NamedPlace*> LmEntry;
			  BOOST_FOREACH(LmEntry lmEntry,roadsMap) {
				 road::RoadPlace *rp = const_cast<road::RoadPlace*>(dynamic_cast<const road::RoadPlace*>(lmEntry.second));
				 const graph::PathGroup::Paths rp_paths = rp->getPaths();
				 BOOST_FOREACH(graph::Path *path, rp_paths) {
					roads.push_back(reinterpret_cast<Road*>(path));
				 }
			  }
		   #endif


			  BOOST_FOREACH(util::Registry<Road>::value_type road, _env->getEditableRegistry<Road>()) {
				 boost::shared_ptr<Road> r = road.second;
				 if(r->getRoadPlace()->getCity() == city) {
					roads.push_back(r.get());
				 }
			  }

			  /*
			  BOOST_FOREACH(util::Registry<RoadPlace>::value_type roadplace, _env->getEditableRegistry<RoadPlace>())
			  {
				 boost::shared_ptr<RoadPlace> rp = roadplace.second;
				 if(rp->getCity()->getKey() == city->getKey()) {
					cityRoadPlaces.push_back(rp);
					const graph::PathGroup::Paths rp_paths = rp->getPaths();
					BOOST_FOREACH(util::Registry<Road>::value_type road, _env->getEditableRegistry<Road>())
					{
					   boost::shared_ptr<Road> r = road.second;
					   if(r->getRoadPlace()->getKey() == rp->getKey())
						  roads.push_back(r.get());
					}
				 }
			  }
			  */
			  roadsByCity[city->getKey()] = roads;
			  util::Log::GetInstance().info("finished extracting roads of "+city->getName());
		   }




		   //Point2D cityPt = city->getPoint();
		   return getRoadsByDistance(stop.get(), roadsByCity[city->getKey()], distance_limit);
		}
		#endif


		EdgeProjector::PathsNearby EdgeProjector::getPathsByDistance(
			const Coordinate& pt
		) const {
		   PathsNearby ret;
		   GeometryFactory geometryFactory;
		   geos::geom::Point* ptGeom(geometryFactory.createPoint(pt));
		   BOOST_FOREACH(const Path* road, _from) {
			  //TODO optimization: first compute distance to road bbox
			  boost::shared_ptr<geos::geom::Geometry> roadGeom = road->getGeometry();
			  double bboxdistance = roadGeom->getEnvelope()->distance(ptGeom);
			  if(_distanceLimit != -1 && bboxdistance >= _distanceLimit)
				 continue;
			  double distance = roadGeom->distance(ptGeom);
			  if(_distanceLimit == -1 || distance < _distanceLimit) {
				 geos::linearref::LengthIndexedLine lil(roadGeom.get());
				 const geos::geom::Coordinate *ptCoords = ptGeom->getCoordinate();
				 geos::geom::Coordinate coord(*ptCoords);
				 double index = lil.project(coord);
				 geos::geom::Coordinate projectedCoords = lil.extractPoint(index);
				 road->validateGeometry();
				 ret.insert(std::make_pair(
					   distance,
					   boost::make_tuple(
							 projectedCoords,
							 road,
							 index
				 )	)	);
			  }
		   }
		   return ret;
		}

		#if 0
		RoadChunk* EdgeProjector::splitRoad(Road *road, double metricOffset, Address *pt) {

		   road->validateGeometry();
		   boost::shared_ptr<geos::geom::Geometry> roadGeom = road->getGeometry();
		   geos::linearref::LengthIndexedLine lil(roadGeom.get());
		   geos::geom::Coordinate c = lil.extractPoint(metricOffset);
		   assert(c.x == pt->getX() && c.y == pt->getY());
		   assert(roadGeom->distance(pt->getGeometry().get())<0.01);

		   graph::Edge* edgeToSplitPtr = road->getEdgeAtOffset(metricOffset);
		   boost::shared_ptr<RoadChunk> edgeToSplit = _env->getEditable<RoadChunk>(edgeToSplitPtr->getKey());

		   if(std::abs(metricOffset-edgeToSplit->getMetricOffset())<0.5) {
			  //we already have a vertex at the place of insertion, do nothing
			  assert(edgeToSplit->getFromVertex()->getDistanceTo(*pt)<0.5);
			  return edgeToSplit.get();
		   } else {
			  assert(edgeToSplit->getRankInPath() < (road->getEdges().size()-1) );
			  const geometry::Point2D *prev = dynamic_cast<const geometry::Point2D*>(edgeToSplit->getFromVertex());
			  double edgeOffset = metricOffset - edgeToSplit->getMetricOffset(); //offset inside the edge to split



			  //check we're inside the segment
			  assert(metricOffset < road->getEdge(edgeToSplit->getRankInPath()+1)->getMetricOffset()
					&& metricOffset > edgeToSplit->getMetricOffset());

			  const std::vector<const geometry::Point2D*> viaPoints = edgeToSplit->getViaPoints();
			  int nViaPoints = viaPoints.size();
			  std::vector<geometry::Point2D*> firstChunkViaPoints,secondChunkViaPoints;
			  double firstChunkLength=0,secondChunkLength=0, viaPointOffset=0;
			  std::vector<const geometry::Point2D*>::const_iterator viaPointIterator = viaPoints.begin();

			  //extract the viaPoints and length of the first roadChunk
			  while(viaPointIterator != viaPoints.end()) {
				 double segmentLength = prev->getDistanceTo(**viaPointIterator);
				 viaPointOffset += segmentLength;
				 if(viaPointOffset < edgeOffset) {
					firstChunkLength += segmentLength;
					firstChunkViaPoints.push_back(new Point2D(**viaPointIterator));
					prev = *viaPointIterator;
					++viaPointIterator;
				 } else {
					break;
				 }
			  } // at the end of this loop, viaPointIterator points to the first viapoint after the point to insert,
				// i.e. the first viaPoint of the roadChunk to create.
				// prev points to the last viaPoint (or the edge/roadchunk) of the first roadchunk
			  double remaininglength = prev->getDistanceTo(*pt);
			  firstChunkLength += remaininglength;
			  assert(std::abs(firstChunkLength-edgeOffset)<0.1);

			  //add viaPoints and calculate length of second roadChunk
			  prev = dynamic_cast<const geometry::Point2D*>(pt);
			  while(viaPointIterator != viaPoints.end()) {
				 secondChunkLength+=prev->getDistanceTo(**viaPointIterator);
				 secondChunkViaPoints.push_back(new Point2D(**viaPointIterator));
				 prev = *viaPointIterator;
				 ++viaPointIterator;
			  }
			  const graph::Vertex *next = road->getEdge(edgeToSplit->getRankInPath()+1)->getFromVertex();
			  secondChunkLength += prev->getDistanceTo(*next);

			  edgeToSplit->clearViaPoints();
			  BOOST_FOREACH(Point2D *vp, firstChunkViaPoints) {
				 edgeToSplit->addViaPoint(*vp);
			  }

			  boost::shared_ptr<RoadChunk> newRoadChunk(new RoadChunk);
			  newRoadChunk->setRoad(road);
			  newRoadChunk->setFromCrossing(pt);
			  newRoadChunk->setRankInPath(edgeToSplit->getRankInPath() + 1);
			  newRoadChunk->setMetricOffset(edgeToSplit->getMetricOffset()+firstChunkLength);
			  newRoadChunk->setKey(RoadChunkTableSync::getId());
			  BOOST_FOREACH(Point2D *vp, secondChunkViaPoints) {
				 newRoadChunk->addViaPoint(*vp);
			  }

			  assert(nViaPoints == (newRoadChunk->getViaPoints().size() +edgeToSplit->getViaPoints().size() ));

			  road->addRoadChunk(newRoadChunk.get(),secondChunkLength);

			  _env->getEditableRegistry<RoadChunk>().add(newRoadChunk);
			  _env->getEditableRegistry<RoadChunk>().replace(edgeToSplit);

			  return newRoadChunk.get();
		   }
		}
		#endif
}	}
