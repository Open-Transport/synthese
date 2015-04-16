/*
 * Exception.h
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#ifndef OSM_EXCEPTION_H_
#define OSM_EXCEPTION_H_

#include <stdexcept>
#include <string>

namespace synthese {
namespace osm {

/**
 * \brief application specific exception
 */
class OSMException: public std::runtime_error {
public:
   /**
    * \param msg Textual representation of error
    */
   explicit OSMException(const std::string &msg);
};


class RefNotFoundException: public OSMException {
public:
   explicit RefNotFoundException(const std::string &msg);
};

class UnknownTagException: public OSMException {
public:
   explicit UnknownTagException(const std::string &tagname);
};

class TopologyException: public OSMException {
public:
   explicit TopologyException(const std::string &msg);
};





}
}

#endif /* EXCEPTION_H_ */
