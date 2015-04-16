/*
 * Exception.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "OSMException.h"

namespace synthese {

namespace osm {

OSMException::OSMException(const std::string &msg) : runtime_error(msg){}

RefNotFoundException::RefNotFoundException(const std::string &msg) : OSMException(msg) {}

UnknownTagException::UnknownTagException(const std::string &tagname) : OSMException("Unknown tag \""+tagname+"\"") {}

TopologyException::TopologyException(const std::string &msg) : OSMException(msg) {}

}

}
