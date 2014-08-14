/*
 * GeomUtil.cpp
 *
 *  Created on: Jan 27, 2010
 *      Author: tbonfort
 */

#include "GeomUtil.h"
#include <boost/foreach.hpp>
#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/operation/linemerge/LineMerger.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/LinearRing.h>
#include <geos/io/WKTWriter.h>

namespace synthese {
namespace osm {

std::vector<geos::geom::Polygon*>* GeomUtil::polygonize(std::list<WayPtr> &ways) {
   geos::geom::Geometry* g = NULL;
   std::vector<geos::geom::Polygon*>* ret = new std::vector<geos::geom::Polygon*>();
   const geos::geom::GeometryFactory *gf = geos::geom::GeometryFactory::getDefaultInstance();
   if(ways.size() >= 2) {
      /*
      geos::operation::polygonize::Polygonizer p;

     geos::geom::Geometry* g = NULL;
     //std::vector<const geos::geom::Geometry*> geometries;
      bool first = true;
      BOOST_FOREACH(WayPtr w, ways) {

         // filter out bogus ways
         if(w->getNodes()->size() < 2)
            continue;

         //geometries.push_back(w->toGeometry()->buffer(0));
         if (first) {
            g = w->toGeometry().get()->clone();
            first = false;
         } else {
            geos::geom::Geometry* tmp = g->Union(w->toGeometry().get());
            delete g;
            g = tmp;
         }

      }
      if(g) {
         p.add(g);
         ret = p.getPolygons();
         delete g;
      }
      */
      geos::operation::linemerge::LineMerger lm;
      BOOST_FOREACH(WayPtr w, ways) {
         if(w->getNodes()->size() < 2)
            continue;
         lm.add(w->toGeometry().get());
      }
      std::vector< geos::geom::LineString * > *lss = lm.getMergedLineStrings();
      BOOST_FOREACH(geos::geom::LineString *ls, *lss) {
         if(ls->getNumPoints()>3 && ls->isClosed()) {
            geos::geom::Polygon *p = gf->createPolygon(gf->createLinearRing(ls->getCoordinates()),0);
            ret->push_back(p);
         }
         delete ls;
      }
      lss->clear();
      delete lss;
   } else if(ways.size() == 1){
      WayPtr w = ways.front();
      const std::list<std::pair<unsigned long long int,NodePtr> > *nodes = w->getNodes();
      if(nodes->size()>3 && nodes->front().first == nodes->back().first) {
         //we have a closed way, return it
         g = w->toGeometry().get()->clone();
         geos::geom::CoordinateSequence *cs = g->getCoordinates();
         geos::geom::LinearRing *lr = gf->createLinearRing(cs);
         //std::vector<geos::geom::Geometry*>* holes = new std::vector<geos::geom::Geometry*>();

         geos::geom::Polygon *p = gf->createPolygon(lr,NULL);
         ret->push_back(p);
         delete g;
      }
   }

   return ret;
}

/*
 std::list<WayPtr> GeomUtil::mergeConnected(std::list<WayPtr> &ways) {
 std::list<WayPtr> ret;
 std::list<WayPtr>::iterator it1 = ways.begin();
 while(it1 != ways.end()) {
 WayPtr newway = boost::shared_ptr<Way>(new Way(*it1->get()));
 std::list<WayPtr>::iterator it2 = ret.begin();
 while(it2 != ret.end()) {
 if(connectable(newway,*it2)) {
 connect(newway,*it2);
 //newway now contains the nodes of *it2
 ret.erase(it2++);
 continue;
 }
 it2++;
 }
 ret.push_back(*it1);
 it1++;
 }
 return ret;
 }

 bool GeomUtil::connectable(WayPtr &way1, WayPtr &way2) {
 if(way1 == way2)
 return false; //duh
 const std::list<std::pair<int,NodePtr> > *nodes1,*nodes2;
 nodes1 = way1->getNodes();
 nodes2 = way2->getNodes();
 return (
 (nodes1->front().first == nodes2->front().first) ||
 (nodes1->front().first == nodes2->back().first) ||
 (nodes1->back().first == nodes2->front().first) ||
 (nodes1->back().first == nodes2->back().first));
 }

 void GeomUtil::connect(WayPtr &from, WayPtr &to) {
 std::list<std::pair<int,NodePtr> > *fromnodes, *tonodes;
 fromnodes = from->getNodes();
 tonodes = to->getNodes();
 if(tonodes->size() == 0) {
 throw TopologyException("cannot connect to way with no nodes");
 }

 if(fromnodes->front().first == tonodes->front().first) {
 //reverse from nodes, and push them to the beginning of tonodes
 tonodes->insert(tonodes->begin(),fromnodes->rbegin(), --(fromnodes->rend()));
 } else if (fromnodes->front().first == tonodes->back().first) {
 //append fromnodes to the end of tonodes
 tonodes->insert(tonodes->end(), ++(fromnodes->begin()), fromnodes->end());
 } else if (fromnodes->back().first == tonodes->front().first) {
 //insert fromnodes to the beginning of tonodes
 tonodes->insert(tonodes->begin(), fromnodes->begin(), --(fromnodes->end()));
 } else if (fromnodes->back().first == tonodes->back().first) {
 //append reversed fromnodes to the end of tonodes
 tonodes->insert(tonodes->end(), ++(fromnodes->rbegin()), fromnodes->rend());
 } else {
 throw TopologyException("cannot join unconnected ways");
 }
 }

 */

}
}
