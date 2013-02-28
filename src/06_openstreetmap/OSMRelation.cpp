/*
 * Relation.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "OSMElements.h"
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <geos/geom/Geometry.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/util/TopologyException.h>
#include <geos/operation/valid/IsValidOp.h>
#include <geos/operation/valid/TopologyValidationError.h>

#include <geos/io/WKTWriter.h>
#include "GeomUtil.h"

namespace synthese {

namespace osm {

Relation::Relation(AttributeMap &attrs): Element(attrs) {
   geometry_computed = false;
}

std::string Relation::toWKT() {
   throw Exception("not implemented");
}


void Relation::add(AttributeMap &attrs, NodePtr node) {
   add(attrs,node->getId(), node);
}

void Relation::add(AttributeMap &attrs, WayPtr way) {
   add(attrs, way->getId(), way);
}

void Relation::add(AttributeMap &attrs, RelationPtr relation) {
   add(attrs, relation->getId(), relation);
}

void Relation::addUnresolvedNode(AttributeMap &attrs, unsigned long long int id) {
   NodePtr node;
   add(attrs,id,node);
}

void Relation::addUnresolvedWay(AttributeMap &attrs, unsigned long long int id) {
   WayPtr way;
   add(attrs,id,way);
}

void Relation::addUnresolvedRelation(AttributeMap &attrs, unsigned long long int id) {
   RelationPtr relation;
   add(attrs,id,relation);
}

void Relation::add(AttributeMap &attrs, unsigned long long int id, NodePtr node) {
   std::string role = attrs.getString(ATTR_ROLE);
   boost::to_lower(role);
   nodes[role][id]=node;
   resetGeometry();
}

void Relation::add(AttributeMap &attrs, unsigned long long int id, WayPtr way) {
   std::string role = attrs.getString(ATTR_ROLE);
   boost::to_lower(role);
   ways[role][id]=way;
   resetGeometry();
}

void Relation::add(AttributeMap &attrs, unsigned long long int id, RelationPtr relation) {
   std::string role = attrs.getString(ATTR_ROLE);
   boost::to_lower(role);
   relations[role][id]=relation;
   resetGeometry();
}

bool Relation::contains(NodePtr &node) {
   typedef std::pair<std::string, std::map<unsigned long long int, NodePtr> > NodeRelationType;
   BOOST_FOREACH(NodeRelationType nrt, nodes) {
      typedef std::pair<unsigned long long int, NodePtr> NodeType;
      BOOST_FOREACH(NodeType nt, nrt.second) {
         if(nt.second == node)
            return true;
      }
   }
   return false;
}

bool Relation::contains(WayPtr &way) {
   typedef std::pair<std::string, std::map<unsigned long long int, WayPtr> > WayRelationType;
   BOOST_FOREACH(WayRelationType wrt, ways) {
      typedef std::pair<unsigned long long int, WayPtr> WayType;
      BOOST_FOREACH(WayType wt, wrt.second) {
         if(wt.second == way)
            return true;
      }
   }
   return false;
}

bool Relation::contains(RelationPtr &rel) {
   typedef std::pair<std::string, std::map<unsigned long long int, RelationPtr> > RelationRelationType;
   BOOST_FOREACH(RelationRelationType rrt, relations) {
      typedef std::pair<unsigned long long int, RelationPtr> RelationType;
      BOOST_FOREACH(RelationType rt, rrt.second) {
         if(rt.second == rel)
            return true;
      }
   }
   return false;
}

void Relation::consolidate(Network *network) throw(RefNotFoundException) {

   std::map<std::string,std::map<unsigned long long int, NodePtr> >::iterator nit1 = nodes.begin();
   while (nit1 != nodes.end()) {
      std::map<unsigned long long int, NodePtr> typeNodes = nit1->second;
      std::map<unsigned long long int, NodePtr>::iterator nit2 = typeNodes.begin();
      while( nit2 != typeNodes.end()) {
         if (!(nit2->second)) {
            NodePtr node = network->getNode(nit2->first);
            nit2->second = node;
         }
         nit2++;
      }
      nit1++;
   }

   std::map<std::string, std::map<unsigned long long int, WayPtr> >::iterator wit1 = ways.begin();
   while (wit1 != ways.end()) {
      std::map<unsigned long long int, WayPtr> typeWays = wit1->second;
      std::map<unsigned long long int, WayPtr>::iterator wit2 = typeWays.begin();
      while (wit2 != typeWays.end()) {
         if (!(wit2->second)) {
            WayPtr way = network->getWay(wit2->first);
            wit2->second = way;
         }
         wit2++;
      }
      wit1++;
   }

   std::map<std::string, std::map<unsigned long long int, RelationPtr> >::iterator rit1 = relations.begin();
   while (rit1 != relations.end()) {
      std::map<unsigned long long int, RelationPtr> typeRelations = rit1->second;
      std::map<unsigned long long int, RelationPtr>::iterator rit2 = typeRelations.begin();
      while (rit2 != typeRelations.end()) {
         if (!(rit2->second)) {
            RelationPtr relation = network->getRelation(rit2->first);
            rit2->second = relation;
         }
         rit2++;
      }
      rit1++;
   }
}

std::list<WayPtr> Relation::getWays(std::string &role) {
   std::list<WayPtr> ret;
   std::map<std::string, std::map<unsigned long long int, WayPtr> >::iterator it1 = ways.find(role);
   if (it1 != ways.end()) {
      std::map<unsigned long long int, WayPtr>::iterator it2 = it1->second.begin();
      while (it2 != it1->second.end()) {
         ret.push_back(it2->second);
         it2++;
      }
   }
   return ret;
}

std::list<NodePtr> Relation::getNodes(std::string &role) {
   std::list<NodePtr> ret;
   std::map<std::string, std::map<unsigned long long int, NodePtr> >::iterator it1 = nodes.find(role);
   if (it1 != nodes.end()) {
      std::map<unsigned long long int, NodePtr>::iterator it2 = it1->second.begin();
      while (it2 != it1->second.end()) {
         ret.push_back(it2->second);
         it2++;
      }
   }
   return ret;
}

/*
std::list<WayPtr> Relation::getWays() {
   std::list<WayPtr> ret;
   std::map<std::string, std::map<unsigned long long int, WayPtr> >::iterator it1 = ways.begin();
   while (it1 != ways.end()) {
      std::map<unsigned long long int, WayPtr>::iterator it2 = it1->second.begin();
      while (it2 != it1->second.end()) {
         ret.push_back(it2->second);
         it2++;
      }
      it1++;
   }
   return ret;
}
*/

std::list<RelationPtr> Relation::getRelations(std::string &role) {
   std::list<RelationPtr> ret;
   std::map<std::string, std::map<unsigned long long int, RelationPtr> >::iterator it1 = relations.find(role);
   if (it1 != relations.end()) {
      std::map<unsigned long long int, RelationPtr>::iterator it2 = it1->second.begin();
      while (it2 != it1->second.end()) {
         ret.push_back(it2->second);
         it2++;
      }
   }
   return ret;
}

std::list<RelationPtr> Relation::getRelations() {
   std::list<RelationPtr> ret;
   std::map<std::string, std::map<unsigned long long int, RelationPtr> >::iterator it1 = relations.begin();
   while (it1 != relations.end()) {
      std::map<unsigned long long int, RelationPtr>::iterator it2 = it1->second.begin();
      while (it2 != it1->second.end()) {
         ret.push_back(it2->second);
         it2++;
      }
      it1++;
   }
   return ret;
}

void Relation::resetGeometry() {
   Element::resetGeometry();
   geometry_computed = false;
}

boost::shared_ptr<const geos::geom::Geometry> Relation::toGeometry() {
   if(!geometry_computed) {
      if(hasTag(TAG_BOUNDARY)) {
         geometry = boost::shared_ptr<geos::geom::Geometry>(extractBoundary());
         geometry_computed = true;
      }
      else {
         throw Exception("toGeometry() only implemented for boundary relations");
      }
   }
   return boost::shared_ptr<const geos::geom::Geometry>(geometry);
}




geos::geom::Geometry* Relation::extractBoundary() {

   const geos::geom::GeometryFactory *geometryFactory = geos::geom::GeometryFactory::getDefaultInstance();
   static std::string sEmpty = "";
   static std::string sEnclave = "enclave";
   static std::string sExclave = "exclave";
   static std::string sOuter = "outer";
   std::list<WayPtr> ways = getWays(sEmpty);
   std::list<WayPtr> waysOuter = getWays(sOuter);
   std::list<WayPtr> exclaves = getWays(sExclave);
   ways.splice(ways.end(), waysOuter);
   ways.splice(ways.end(), exclaves);
   std::list<WayPtr> enclaves = getWays(sEnclave);

   std::vector<geos::geom::Polygon*> *polygons, *polygon_enclaves;
   polygons = GeomUtil::polygonize(ways);

   if(!polygons->size()) {
      delete polygons;
      return NULL;
   }

   geos::io::WKTWriter wktWriter;

   geos::geom::Geometry *poly = geometryFactory->createMultiPolygon((std::vector<geos::geom::Geometry*>*) polygons);
   //the geometries stored in *polygons are now owned by the multipolygon

   if (!poly->isValid()) {
      geos::geom::Geometry *tmp = poly->buffer(0.0);
      geometryFactory->destroyGeometry(poly);
      poly = tmp;
   }

   if (enclaves.size()) {
      polygon_enclaves = GeomUtil::polygonize(enclaves);
      BOOST_FOREACH(geos::geom::Geometry* enc, *polygon_enclaves)
      {
         if (!enc->isValid()) {
            geos::geom::Geometry *tmp = enc->buffer(0.0);
            geometryFactory->destroyGeometry(enc);
            enc = tmp;
         }
         if (poly->intersects(enc)) {
            try {
               geos::geom::Geometry *tmp = poly->difference(enc);
               geometryFactory->destroyGeometry(poly);
               poly = tmp;
            } catch (geos::util::TopologyException e) {
               std::cout << std::endl << e.what() << std::endl;
               geos::operation::valid::IsValidOp ivo(enc);
               geos::operation::valid::TopologyValidationError *tve = ivo.getValidationError();
               const std::string errloc = tve->getCoordinate().toString();
               std::string errmsg(tve->getMessage());
               errmsg += "[" + errloc + "]";

               std::cout << "topolygyexception in relation " << getId() << " subtracting"
                     << std::endl << enc->toText() << std::endl << "from: " << std::endl << poly->toText()
                     << std::endl << "poly valid: " << poly->isValid() << std::endl << "enclave valid: "
                     << errmsg << std::endl;
            }
            geometryFactory->destroyGeometry(enc);
         }
      }
   }
   return poly;
}

//std::list<WayPtr> Relation::getRelatedWays() {
   //std::list<WayPtr> ret = getWays();
   //TODO
   //return ret;
//}



/*
void template<class T> Relation::consolidate(Network *network, T linkedElements) {
   std::map<std::string,std::map<unsigned long long int, T> >::iterator it1 = linkedElements.begin();
   while (it1 != linkedElements.end()) {
      std::map<unsigned long long int, T> typeNodes = it1->second;
      std::map<unsigned long long int, T>::iterator it2 = typeNodes.begin();
      while( it2 != typeNodes.end()) {
         if (!(it2->second)) {
            T node = network->getNode(it2->first);
            it2->second = node;
         }
         it2++;
      }
      it1++;
   }
}
*/


}

}
