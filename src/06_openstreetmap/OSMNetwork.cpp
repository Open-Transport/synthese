/*
 * Network.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "OSMElements.h"
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "Log.h"
#include "GeomUtil.h"
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>


using boost::shared_ptr;
using boost::lexical_cast;
using std::list;
using std::pair;
using geos::geom::Geometry;

namespace synthese {
namespace osm {

Network::Network() {
   // TODO Auto-generated constructor stub

}

Network::~Network() {
   // TODO Auto-generated destructor stub
}

void Network::addNode(NodePtr node) {
   nodes[node->getId()] = node;
}

void Network::addWay(WayPtr way) {
   ways[way->getId()] = way;
}

void Network::addRelation(RelationPtr relation) {
   relations[relation->getId()] = relation;
}

const std::map<unsigned long long int, NodePtr >* Network::getNodes() {
   return &nodes;
}

const std::map<unsigned long long int, WayPtr >* Network::getWays() {
   return &ways;
}

const std::map<unsigned long long int, RelationPtr >* Network::getRelations() {
   return &relations;
}


NodePtr Network::getNode(unsigned long long int id) throw (RefNotFoundException) {
   std::map<unsigned long long int, NodePtr>::iterator it = nodes.find(id);
   if (it == nodes.end())
      throw RefNotFoundException("node " + lexical_cast<std::string> (id) + " not in network");
   return it->second;
}

WayPtr Network::getWay(unsigned long long int id) throw (RefNotFoundException) {
   std::map<unsigned long long int, WayPtr>::iterator it = ways.find(id);
   if (it == ways.end())
      throw RefNotFoundException("way " + lexical_cast<std::string> (id) + " not in network");
   return it->second;
}

RelationPtr Network::getRelation(unsigned long long int id) throw (RefNotFoundException) {
   std::map<unsigned long long int, RelationPtr>::iterator it = relations.find(id);
   if (it == relations.end())
      throw RefNotFoundException("relation " + lexical_cast<std::string> (id)+ " not in network");
   return it->second;
}

void Network::consolidate(bool discard_if_missing_reference) {
   consolidateWays(discard_if_missing_reference);
   consolidateRelations(discard_if_missing_reference);
}

void Network::consolidateWays(bool discard_if_missing_reference) {
   std::map<unsigned long long int, WayPtr>::iterator wayIt = ways.begin();
   //loop through the ways
   while(wayIt != ways.end()) {
      WayPtr way = wayIt->second;
      try {
         way->consolidate(this);
         wayIt++;
      } catch(RefNotFoundException e) {
         //the way references a node that doesn't exist
         if(discard_if_missing_reference) {
            //discard relations referencing this way
            std::map<unsigned long long int,RelationPtr>::iterator relIt = relations.begin();
            while(relIt != relations.end()) {
               if(relIt->second->contains(way)) {
                  relations.erase(relIt++);
               } else {
                  relIt++;
               }
            }

            //discard the way
            std::map<unsigned long long int,WayPtr>::iterator way_to_delete = wayIt++;
            ways.erase(way_to_delete);
         } else {
            wayIt++;
         }
      }
   }
}

void Network::consolidateRelations(bool discard_if_missing_reference) {
   std::map<unsigned long long int, RelationPtr>::iterator relIt = relations.begin();
   //loop through the relations
   while (relIt != relations.end()) {
      RelationPtr relation = relIt->second;
      try {
         relation->consolidate(this);
         relIt++;
      } catch (RefNotFoundException e) {
         //the relation references a node, way or relation that doesn't exist
         if (discard_if_missing_reference) {
            //discard relations referencing this relation
            std::map<unsigned long long int, RelationPtr>::iterator relIt2 = relations.begin();
            while (relIt2 != relations.end() && relIt2 != relIt) {
               if (relIt2->second->contains(relation)) {
                  relations.erase(relIt2++);
               } else {
                  relIt2++;
               }
            }

            //discard the relation itself
            std::map<unsigned long long int, RelationPtr>::iterator rel_to_delete = relIt++;
            relations.erase(rel_to_delete);
         } else {
            relIt++;
         }
      }
   }
}

std::map<unsigned long long int,WayPtr> Network::getWalkableWays() {
   std::map<unsigned long long int, WayPtr> ret;
   typedef std::pair<unsigned long long int, WayPtr> WayType;
   BOOST_FOREACH(WayType w, ways) {
      if(w.second->isWalkable() && w.second->getNodes()->size() > 1) {
         ret[w.second->getId()] = w.second;
      }
   }
   return ret;
}


std::map<unsigned long long int,std::pair<RelationPtr,std::map<unsigned long long int, WayPtr> > > Network::getWalkableWaysByAdminBoundary(int admin_level) {
   std::map<unsigned long long int, std::pair<RelationPtr, std::map<unsigned long long int, WayPtr> > > ret;
   util::Log::GetInstance().info("extracting administrative boundaries");
   std::map<unsigned long long int, RelationPtr> adminBoundaries = getAdministrativeBoundaries(admin_level);
   util::Log::GetInstance().info("finished extracting administrative boundaries");
   std::map<unsigned long long int, WayPtr> walkableWays = getWalkableWays();

   util::Log::GetInstance().info("extracting ways by administrative boundaries");
   /*
   std::map<unsigned long long int,RelationPtr>::iterator boundaryIterator = adminBoundaries.begin();
   while(boundaryIterator != adminBoundaries.end()) {
      RelationPtr boundary = boundaryIterator->second;
      ret[boundaryIterator->first] = std::pair<RelationPtr,std::map<unsigned long long int,WayPtr> >(boundary, std::map<unsigned long long int,WayPtr>());
      boost::shared_ptr<const geos::geom::prep::PreparedGeometry> boundaryPrepGeom =
            boundary->toPreparedGeometry();
      std::map<unsigned long long int,WayPtr>::iterator wayIterator = walkableWays.begin();
      while(wayIterator != walkableWays.end()) {
         WayPtr way = wayIterator->second;
         boost::shared_ptr<const geos::geom::Geometry> wayGeom = way->toGeometry();
         if(boundaryPrepGeom->covers(wayGeom.get())) {
            //the way is inside this boundary, keep it
            ret[boundaryIterator->first].second[wayIterator->first]=way;

            //mark the nodes of this way for next step reference counting
            way->referenceWithNodes();

            //remove way from list to avoid further superfluous geometry computations
            walkableWays.erase(wayIterator++);

         } else if(boundaryPrepGeom->intersects(wayGeom.get())) {
            //the way isn't inside, but intersects, flag it as invalid
            walkableWays.erase(wayIterator++);
            //TODO: log the way
         } else {
            //no interaction with current boundary
            wayIterator++;
         }
      }
      boundaryIterator++;
   }
   */

   std::map<unsigned long long int, RelationPtr>::iterator boundaryIterator = adminBoundaries.begin();
   while(boundaryIterator != adminBoundaries.end()) {
      RelationPtr boundary = boundaryIterator->second;
      ret[boundaryIterator->first] = std::pair<RelationPtr, std::map<unsigned long long int, WayPtr> >(boundary, std::map<unsigned long long int, WayPtr>());
      boundaryIterator++;
   }

   std::map<unsigned long long int,WayPtr>::iterator wayIterator = walkableWays.begin();
   while(wayIterator != walkableWays.end()) {
      WayPtr way = wayIterator->second;
      boost::shared_ptr<const geos::geom::Geometry> wayGeom = way->toGeometry();
      boundaryIterator = adminBoundaries.begin();
      while(boundaryIterator != adminBoundaries.end()) {
         RelationPtr boundary = boundaryIterator->second;
         boost::shared_ptr<const geos::geom::prep::PreparedGeometry> boundaryPrepGeom = boundary->toPreparedGeometry();
         if(boundaryPrepGeom->covers(wayGeom.get())) {
            //the way is inside this boundary, keep it
            ret[boundaryIterator->first].second[wayIterator->first] = way;

            //mark the nodes of this way for next step reference counting
            way->referenceWithNodes();
            break;

         } else if(boundaryPrepGeom->intersects(wayGeom.get())) {
            //the way isn't inside, but intersects, continue
            break;
            //TODO: log the way
         } else {
            //no interaction with current boundary
            boundaryIterator++;
         }
      }
      ++wayIterator;
   }

   util::Log::GetInstance().info("finished extracting ways by administrative boundaries");
   return ret;
}


std::map<unsigned long long int, std::pair<RelationPtr, std::map<unsigned long long int, WayPtr> > > Network::getWaysByAdminBoundary(int admin_level)
{
	typedef std::map<unsigned long long int, WayPtr> WayMap;
	typedef std::pair<unsigned long long int, WayPtr> WayType;
	typedef std::map<unsigned long long int, RelationPtr> RelationMap;
	typedef std::pair<RelationPtr, WayMap> RelationWayPair;
	typedef std::map<unsigned long long int, RelationWayPair> GlobalMap;

	GlobalMap ret;

	util::Log::GetInstance().info("extracting administrative boundaries");
	RelationMap adminBoundaries = getAdministrativeBoundaries(admin_level);
	util::Log::GetInstance().info("finished extracting administrative boundaries");

	WayMap waysList;
	BOOST_FOREACH(WayType w, ways)
	{
		if(
			w.second->getNodes()->size() > 1 &&
			w.second->hasTag(Element::TAG_HIGHWAY) &&
			Way::highwayTypes.find(w.second->getTag(Element::TAG_HIGHWAY)) != Way::highwayTypes.end()
		)
		{
			waysList[w.second->getId()] = w.second;
		}
	}

	util::Log::GetInstance().info("extracting ways by administrative boundaries");

	RelationMap::iterator boundaryIterator = adminBoundaries.begin();
	while(boundaryIterator != adminBoundaries.end())
	{
		RelationPtr boundary = boundaryIterator->second;
		ret[boundaryIterator->first] = RelationWayPair(boundary, WayMap());
		boundaryIterator++;
	}

	WayMap::iterator wayIterator = waysList.begin();
	while(wayIterator != waysList.end())
	{
		WayPtr way = wayIterator->second;
		boost::shared_ptr<const geos::geom::Geometry> wayGeom = way->toGeometry();

		boundaryIterator = adminBoundaries.begin();
		while(boundaryIterator != adminBoundaries.end())
		{
			RelationPtr boundary = boundaryIterator->second;
			boost::shared_ptr<const geos::geom::prep::PreparedGeometry> boundaryPrepGeom = boundary->toPreparedGeometry();
			if(boundaryPrepGeom->covers(wayGeom.get()) || boundaryPrepGeom->intersects(wayGeom.get()))
			{
				//the way is inside or intersected by the boundary, keep it
				ret[boundaryIterator->first].second[wayIterator->first] = way;
				//mark the nodes of this way for next step reference counting
				way->referenceWithNodes();
				break;
			}
			else //no interaction with current boundary
				boundaryIterator++;
		}
		++wayIterator;
	}

   util::Log::GetInstance().info("finished extracting ways by administrative boundaries");

   return ret;
}




std::map<unsigned long long int,RelationPtr> Network::getAdministrativeBoundaries(int admin_level) {
   std::map<unsigned long long int, RelationPtr> ret;

   std::map<unsigned long long int, RelationPtr>::const_iterator it = relations.begin();

   while (it != relations.end()) {
      RelationPtr relation = it->second;
      if(relation->hasTag(Element::TAG_BOUNDARY) &&
            relation->hasTag(Element::TAG_ADMINLEVEL) &&
            boost::iequals(relation->getTag(Element::TAG_BOUNDARY), "administrative") &&
            relation->getTag(Element::TAG_ADMINLEVEL) == boost::lexical_cast<std::string>(admin_level) ){

         if(relation->toGeometry())
            ret[relation->getId()] = relation;
      }
      it++;
   }
   return ret;
}

}
}
