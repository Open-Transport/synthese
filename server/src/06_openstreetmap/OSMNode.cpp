/*
 * Node.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "OSMElements.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <iostream>
#include <cmath>


using std::string;

namespace synthese {
namespace osm {

Node::Node(AttributeMap &attrs) throw(Exception): Element(attrs) {
   this->lon = attrs.getDouble(ATTR_LONGITUDE);
   this->lat = attrs.getDouble(ATTR_LATITUDE);
}


std::string Node::toWKT() {
   std::string wkt = "POINT("+boost::lexical_cast<string>(lon)+" "+boost::lexical_cast<string>(lat)+")";
   return wkt;
}

double Node::getLon() {
   return lon;
}

double Node::getLat() {
   return lat;
}


int Node::numConnectedWay() {
   return ways.size();
}

bool Node::isStop() {
   //TODO: lookup tags

   if(hasTag(TAG_HIGHWAY)) {
      std::string highway = getTag(TAG_HIGHWAY);
      if(highway=="bus_stop" || highway == "stop")
         return true;
   }

   if(hasTag(TAG_RAILWAY)) {
      std::string railway = getTag(TAG_RAILWAY);
      if(railway=="station" || railway == "stop")
         return true;
   }


   return false;
}

double Node::distance(NodePtr &other) {
   //TODO real distance, projections
   double dlon = other->getLon() - getLon();
   double dlat = other->getLat() - getLat();
   return 100000*std::sqrt(dlon*dlon+dlat*dlat);
}

}
}
