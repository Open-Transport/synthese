/*
 * AttributeMap.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "AttributeMap.h"
#include <boost/lexical_cast.hpp>
namespace synthese {

namespace osm {

AttributeMap::AttributeMap() {
}

int AttributeMap::getInteger(const std::string &key) throw (Exception) {
   std::string value = this->getString(key);
   return boost::lexical_cast<int>(value);
}

unsigned long long int AttributeMap::getULongLongInt(const std::string &key) throw (Exception) {
   std::string value = this->getString(key);
   return boost::lexical_cast<unsigned long long int>(value);
}

double AttributeMap::getDouble(const std::string &key) throw (Exception) {
   std::string value = this->getString(key);
   return boost::lexical_cast<double>(value);
}

std::string AttributeMap::getString(const std::string &key) throw (Exception) {
   std::map<std::string, std::string>::iterator it = this->find(key);
   if(it == this->end())
      throw Exception("key " + key + " not found in attributes");
   return it->second;
}

}

}
