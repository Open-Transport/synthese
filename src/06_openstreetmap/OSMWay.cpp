/*
 * Way.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "OSMElements.h"
#include <sstream>
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include "Road.h"
namespace synthese {


namespace osm {

using namespace road;

std::map<std::string,Road::RoadType> Way::highwayTypes =  boost::assign::map_list_of
         ("primary",Road::ROAD_TYPE_PRINCIPLEAXIS)
         ("secondary",Road::ROAD_TYPE_SECONDARYAXIS)
         ("tertiary",Road::ROAD_TYPE_ACCESSROAD)
         ("primary_link",Road::ROAD_TYPE_PRINCIPLEAXIS)
         ("secondary_link",Road::ROAD_TYPE_SECONDARYAXIS)
         ("tertiary_link",Road::ROAD_TYPE_ACCESSROAD)
         ("unclassified",Road::ROAD_TYPE_ACCESSROAD)
         ("road",Road::ROAD_TYPE_ACCESSROAD)
         ("residential",Road::ROAD_TYPE_ACCESSROAD)
         ("living_street",Road::ROAD_TYPE_ACCESSROAD)
         ("service",Road::ROAD_TYPE_ACCESSROAD)
         ("track",Road::ROAD_TYPE_PEDESTRIANPATH)
         ("pedestrian",Road::ROAD_TYPE_PEDESTRIANSTREET)
         ("path",Road::ROAD_TYPE_PEDESTRIANPATH)
         ("cycleway",Road::ROAD_TYPE_PEDESTRIANPATH)
         ("footway",Road::ROAD_TYPE_PEDESTRIANPATH)
         ("bridleway",Road::ROAD_TYPE_PEDESTRIANPATH)
         ("byway",Road::ROAD_TYPE_UNKNOWN)
         ("steps",Road::ROAD_TYPE_STEPS)
         ("unclassified",Road::ROAD_TYPE_UNKNOWN);

Way::Way(AttributeMap &attrs) throw(Exception): Element(attrs) {
   // TODO Auto-generated constructor stub
}

void Way::pushNode(int id ,NodePtr node) {
   std::pair<int, NodePtr> pair(id,node);
   nodes.push_back(pair);
   resetGeometry();
}

void Way::pushNode(NodePtr node) {
   pushNode(node->getId(),node);
}

std::string Way::toWKT() {
   std::stringstream wkt;
   std::list<std::pair<int,NodePtr> >::iterator it = nodes.begin();
   wkt << "LINESTRING(";
   if (it != nodes.end())
      wkt << boost::lexical_cast<std::string>((*it).second->getLon()) << " " << boost::lexical_cast<std::string>((*it).second->getLat());
   while (++it != nodes.end()) {
      wkt << "," << boost::lexical_cast<std::string>((*it).second->getLon()) << " " << boost::lexical_cast<std::string>((*it).second->getLat());
   }
   wkt << ")";
   return wkt.str();
}

const std::list<std::pair<int,NodePtr> >* Way::getNodes() {
   return &nodes;
}

void Way::consolidate(Network* network) throw(RefNotFoundException) {
   std::list<std::pair<int,NodePtr> >::iterator it = nodes.begin();
   while(it != nodes.end()) {
      if(!(it->second)) {
         NodePtr node = network->getNode(it->first);
         it->second = node;
      }
      it++;
   }
}

bool Way::isWalkable() {

   try {
      std::string highway = getTag(TAG_HIGHWAY);
      if(highwayTypes.find(highway) == highwayTypes.end())
         return false;
   } catch (Exception e) { return false; }
   try {
      std::string access = getTag(TAG_FOOT);
      if(access == "no")
         return false;
   } catch (Exception e) {}
   /*
    try {
      std::string area = getTag(TAG_AREA);
      if(area == "yes")
         return false;
   } catch (Exception e) {}
   */
   return true;
}

Road::RoadType Way::getRoadType() {
   Road::RoadType type = Road::ROAD_TYPE_UNKNOWN;
   if(!isWalkable())
      return type;
   try {
      if(isOfType(TAG_BRIDGE)) {
         type = Road::ROAD_TYPE_BRIDGE;
      } else if(isOfType(TAG_TUNNEL)) {
         type = Road::ROAD_TYPE_TUNNEL;
      } else {
         std::string highway = getTag(TAG_HIGHWAY);
         std::map<std::string,Road::RoadType>::iterator it = highwayTypes.find(highway);
         if(it != highwayTypes.end()) type = it->second;
      }
   } catch(Exception e) { }
   return type;
}

void Way::referenceWithNodes() {
   typedef std::pair<int,NodePtr> nodeType;
   BOOST_FOREACH(nodeType node, nodes) {
      node.second->ways.push_back(this);
   }
}

}

}
