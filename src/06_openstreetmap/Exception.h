/*
 * Exception.h
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <stdexcept>
#include <string>

namespace synthese {
namespace osm {

/**
 * \brief application specific exception
 */
class Exception: public std::runtime_error {
public:
   /**
    * \param msg Textual representation of error
    */
   explicit Exception(const std::string &msg);
};


class RefNotFoundException: public Exception {
public:
   explicit RefNotFoundException(const std::string &msg);
};

class UnknownTagException: public Exception {
public:
   explicit UnknownTagException(const std::string &tagname);
};

class TopologyException: public Exception {
public:
   explicit TopologyException(const std::string &msg);
};





}
}

#endif /* EXCEPTION_H_ */
