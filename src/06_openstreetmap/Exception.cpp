/*
 * Exception.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "Exception.h"

namespace synthese {

namespace osm {

Exception::Exception(const std::string &msg) : runtime_error(msg){}

RefNotFoundException::RefNotFoundException(const std::string &msg) : Exception(msg) {}

UnknownTagException::UnknownTagException(const std::string &tagname) : Exception("Unknown tag \""+tagname+"\"") {}

TopologyException::TopologyException(const std::string &msg) : Exception(msg) {}

}

}
