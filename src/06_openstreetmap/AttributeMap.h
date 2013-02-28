/*
 * AttributeMap.h
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#ifndef ATTRIBUTEMAP_H_
#define ATTRIBUTEMAP_H_

#include <map>
#include <string>
#include "Exception.h"
namespace synthese {

namespace osm {

/**
 * \brief helper class for extracting values from a std::map<std::string,std::string>
 */
class AttributeMap: public std::map<std::string,std::string> {
public:
   AttributeMap();
   /**
    * \brief return Integer representation of entry
    * \param key The key to lookup from
    * \exception Exception Key is not in map
    */
   int getInteger(const std::string &key) throw(Exception);

   unsigned long long int getULongLongInt(const std::string &key) throw(Exception);

   /**
    * \brief return Double representation of entry
    * \param key The key to lookup from
    * \exception Exception Key is not in map
    */
   double getDouble(const std::string &key) throw (Exception);

   /**
    * \brief return entry
    * \param key The key to lookup from
    * \exception Exception Key is not in map
    */
   std::string getString(const std::string &key) throw(Exception);
};

}

}

#endif /* ATTRIBUTEMAP_H_ */
