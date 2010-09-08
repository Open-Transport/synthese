/*
 * ExpatParser.h
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#ifndef EXPATPARSER_H_
#define EXPATPARSER_H_

#include "OSMElements.h"
#include <expat.h>
#include "AttributeMap.h"
namespace synthese {

namespace osm {

class ExpatParser {
public:
   ExpatParser();
   virtual ~ExpatParser();
   virtual NetworkPtr parse(std::istream &data) throw(std::runtime_error);
private:
   static void startElement(void *d, const XML_Char* name, const XML_Char** attrs) throw(Exception);
   static void endElement(void *d, const XML_Char* name) throw(Exception);
   static void characters(void*, const XML_Char* txt, int txtlen);
   typedef struct {
      NetworkPtr network;
      NodePtr curNode;
      WayPtr curWay;
      ElementPtr curElement;
      RelationPtr curRelation;
   } expat_user_data;
   expat_user_data user_data;
   static AttributeMap createAttributeMap(const XML_Char **attrs);
};

}

}

#endif /* EXPATPARSER_H_ */
