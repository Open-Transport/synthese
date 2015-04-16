/*
 * ExpatParser.cpp
 *
 *  Created on: Jan 25, 2010
 *      Author: tbonfort
 */

#include "OSMExpatParser.h"

#include <sstream>
#include <cstring>
#include <boost/lexical_cast.hpp>
namespace synthese {
namespace osm {

ExpatParser::ExpatParser() {
   // TODO Auto-generated constructor stub

}

ExpatParser::~ExpatParser() {
   // TODO Auto-generated destructor stub
}

NetworkPtr ExpatParser::parse(std::istream &data) throw(std::runtime_error) {
   int done, count = 0, n;
   char buf[4096];

   XML_Parser p = XML_ParserCreate(NULL);
   if (!p) {
      throw std::runtime_error("error creating expat parser");
   }


   XML_SetElementHandler(p, ExpatParser::startElement,ExpatParser::endElement);
   XML_SetCharacterDataHandler(p, ExpatParser::characters);

   NetworkPtr network(new Network());

   user_data.network = network;
   user_data.curNode.reset();
   user_data.curWay.reset();
   user_data.curRelation.reset();
   //user_data.curNode=NULL;
   //user_data.curWay=NULL;
   //user_data.curRelation=NULL;
   XML_SetUserData(p,&user_data);
   // straight from example
   do {
      data.read(buf, 4096);
      n = data.gcount();
      done = (n != 4096);
      if (XML_Parse(p, buf, n, done) == XML_STATUS_ERROR) {
         XML_Error errorCode = XML_GetErrorCode(p);
         int errorLine = XML_GetCurrentLineNumber(p);
         long errorCol = XML_GetCurrentColumnNumber(p);
         const XML_LChar *errorString = XML_ErrorString(errorCode);
         std::stringstream errorDesc;
         errorDesc << "XML parsing error at line " << errorLine << ":"
               << errorCol;
         errorDesc << ": " << errorString;
         throw std::runtime_error(errorDesc.str());
      }
      count += n;
   } while (!done);

   XML_ParserFree(p);





   user_data.network.reset();
   user_data.curNode.reset();
   user_data.curWay.reset();
   user_data.curRelation.reset();
   //user_data.curNode=NULL;
   //user_data.curWay=NULL;
   //user_data.curRelation=NULL;

   return network;
}

void ExpatParser::startElement(void *d, const XML_Char* name,
      const XML_Char** attrs) throw(Exception) {
   expat_user_data *user_data = (expat_user_data*) d;

   AttributeMap attributes = ExpatParser::createAttributeMap(attrs);
   if(!user_data->curElement) {
      //no current element, can be a node, way, or relation

      if (!std::strcmp(name, "node")) {
         user_data->curNode.reset(new Node(attributes));
         //user_data->curNode=new Node(attributes);
         user_data->curElement = user_data->curNode;
      } else if (!std::strcmp(name, "way")) {
         user_data->curWay.reset(new Way(attributes));
         //user_data->curWay=new Way(attributes);
         user_data->curElement = user_data->curWay;
      } else if (!std::strcmp(name, "relation")) {
         user_data->curRelation.reset(new Relation(attributes));
         //user_data->curRelation=new Relation(attributes);
         user_data->curElement = user_data->curRelation;
      } else if (!std::strcmp(name, "osm")) {

      } else if (!std::strcmp(name, "bounds") || !std::strcmp(name, "bound")) {

      } else {
         throw UnknownTagException(name);
      }
   } else {
      //we are inside a node, way or relation

      if (!std::strcmp(name, "tag")) {
         std::string key = attributes.getString(Element::ATTR_KEY);
         std::string value = attributes.getString(Element::ATTR_VALUE);
         user_data->curElement->addTag(key,value);
      } else {
         //here are the tags specific to each element type

         if(user_data->curWay) {
            if (!std::strcmp(name, "nd")) {
               unsigned long long int refNode = attributes.getULongLongInt(Element::ATTR_REF);
               try {
                  NodePtr node = user_data->network->getNode(refNode);
                  user_data->curWay->pushNode(node);
               } catch (RefNotFoundException e) {
                  NodePtr emptyNode;
                  user_data->curWay->pushNode(refNode,emptyNode);
                  //user_data->curWay->pushNode(refNode,(NodePtr)NULL);
               }
            } else {
              throw UnknownTagException(name);
            }
         } else if(user_data->curNode) {
            //nothing to do here
            throw UnknownTagException(name);
         } else if(user_data->curRelation) {
            //parse members
            if (!std::strcmp(name, "member")) {
               unsigned long long int refId = attributes.getULongLongInt(Element::ATTR_REF);
               std::string type = attributes.getString(Element::ATTR_TYPE);
               if (type=="way") {
                  try {
                     WayPtr way = user_data->network->getWay(refId);
                     user_data->curRelation->add(attributes, way);
                  } catch (RefNotFoundException e) {
                     user_data->curRelation->addUnresolvedWay(attributes, refId);
                     //user_data->curRelation->add(attributes, refId, (WayPtr)NULL);
                  }
               } else if(type=="node") {
                  try {
                     NodePtr node = user_data->network->getNode(refId);
                     user_data->curRelation->add(attributes,node);
                  } catch (RefNotFoundException e) {
                     user_data->curRelation->addUnresolvedNode(attributes, refId);
                     //user_data->curRelation->add(attributes, refId,(NodePtr)NULL);
                  }
               } else if(type=="relation") {
                  try {
                     RelationPtr rel = user_data->network->getRelation(refId);
                     user_data->curRelation->add(attributes,rel);
                  } catch (RefNotFoundException e) {
                     user_data->curRelation->addUnresolvedRelation(attributes, refId);
                     //user_data->curRelation->add(attributes, refId,(RelationPtr)NULL);
                  }
               } else {
                  throw Exception("unknown relation type " + type);
               }
            } else {
               throw UnknownTagException(name);
            }
         }
      }
   }

}
void ExpatParser::endElement(void *d, const XML_Char* name) throw(Exception) {
   expat_user_data *user_data = (expat_user_data*)d;
   if (!std::strcmp(name, "node")) {
      user_data->network->addNode(user_data->curNode);
      user_data->curNode.reset();
      user_data->curElement.reset();
      //user_data->curNode=NULL;
      //user_data->curElement=NULL;
   } else if (!std::strcmp(name, "way")) {
      user_data->network->addWay(user_data->curWay);
      user_data->curWay.reset();
      user_data->curElement.reset();
      //user_data->curWay=NULL;
      //user_data->curElement=NULL;
   } else if (!std::strcmp(name, "relation")) {
      user_data->network->addRelation(user_data->curRelation);
      user_data->curRelation.reset();
      user_data->curElement.reset();
      //user_data->curRelation=NULL;
      //user_data->curElement=NULL;
   }
}
void ExpatParser::characters(void*, const XML_Char* txt, int txtlen) {

}

AttributeMap ExpatParser::createAttributeMap(const XML_Char **attrs) {
   int count = 0;
   AttributeMap attributes;
   while (attrs[count]) {
      attributes[attrs[count]] = attrs[count+1];
      count += 2;
   }
   return attributes;
}

}}
