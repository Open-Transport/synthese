/*
 * Element.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "OSMElements.h"
#include <boost/lexical_cast.hpp>
#include <geos/io/WKTReader.h>

namespace synthese {
namespace osm {

const std::string Element::TAG_HIGHWAY("highway");
const std::string Element::TAG_RAILWAY("railway");
const std::string Element::TAG_AMENITY("amenity");
const std::string Element::TAG_JUNCTION("junction");
const std::string Element::TAG_SERVICE("service");
const std::string Element::TAG_FOOT("foot");
const std::string Element::TAG_BICYCLE("bicycle");
const std::string Element::TAG_MOTOR_VEHICLE("motor_vehicle");
const std::string Element::TAG_MOTORCAR("motorcar");
const std::string Element::TAG_ADMINLEVEL("admin_level");
const std::string Element::TAG_BOUNDARY("boundary");
const std::string Element::TAG_NAME("name");
const std::string Element::TAG_ACCESS("access");
const std::string Element::TAG_AREA("area");
const std::string Element::TAG_BRIDGE("bridge");
const std::string Element::TAG_TUNNEL("tunnel");



const std::string Element::ATTR_LONGITUDE("lon");
const std::string Element::ATTR_LATITUDE("lat");
const std::string Element::ATTR_TYPE("type");
const std::string Element::ATTR_ROLE("role");
const std::string Element::ATTR_ID("id");
const std::string Element::ATTR_KEY("k");
const std::string Element::ATTR_VALUE("v");
const std::string Element::ATTR_REF("ref");

const projPJ Element::originalProjection = pj_init_plus("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs");



Element::Element(AttributeMap &attributes) throw(Exception) {
   this->id = attributes.getULongLongInt(ATTR_ID);
}

Element::~Element() {
}

unsigned long long int Element::getId() {
   return id;
}

void Element::addTag(const std::string &key, const std::string &value) {
   tags[key] = value;
}

bool Element::hasTag(const std::string &key) {
   std::map<std::string,std::string>::iterator it = tags.find(key);
   if(it == tags.end())
      return false;
   return true;
}

std::string Element::getTag(const std::string &key) throw(Exception){
   std::map<std::string,std::string>::iterator it = tags.find(key);
   if(it == tags.end()) {
      throw Exception("tag " + key + " not in tags");
   }
   return it->second;
}

boost::shared_ptr<const geos::geom::Geometry> Element::toGeometry() {
   static geos::io::WKTReader wktReader;
   if(!geometry)
      geometry = boost::shared_ptr<geos::geom::Geometry>(wktReader.read(toWKT()));
   return boost::shared_ptr<const geos::geom::Geometry>(geometry);

}

boost::shared_ptr<const geos::geom::prep::PreparedGeometry> Element::toPreparedGeometry() {
   if(!preparedGeometry) {
      preparedGeometry = boost::shared_ptr<geos::geom::prep::PreparedGeometry> (
            new geos::geom::prep::BasicPreparedGeometry(toGeometry().get()));
   }
   return boost::shared_ptr<const geos::geom::prep::PreparedGeometry>(preparedGeometry);
}

void Element::resetGeometry() {
   geometry.reset();
   preparedGeometry.reset();
}

bool Element::isOfType(const std::string &tag) {
   try {
      std::string tagValue = getTag(tag);
      if(!tagValue.compare("no") || !tagValue.compare("No") ||!tagValue.compare("false") || !tagValue.compare("0") || !tagValue.compare("False"))
         return false;
   } catch(Exception e) {
      return false;
   }
   return true;
}


}}
