
/** OsmFileFormatTest class implementation.
	@file OsmFileFormatTest.cpp
	@author Marc Jambert

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <boost/test/auto_unit_test.hpp>

#include <expat.h>
#include <map>
#include <set>
#include <vector>
#include <string>

#include <fstream>
#include <iostream>

#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/prep/PreparedPolygon.h>
#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/operation/linemerge/LineMerger.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/util/TopologyException.h>
#include <geos/operation/valid/IsValidOp.h>
#include <geos/operation/valid/TopologyValidationError.h>
#include <geos/io/WKTReader.h>

//#include <geos.h>

typedef std::map<std::string, std::string> AttributesMap;
typedef long OSMId;


struct OSMNode
{
	double latitude;
	double longitude;

    bool operator==(const OSMNode& rhs) const {
    	return (latitude == rhs.latitude) && (longitude == rhs.longitude); }

};

struct OSMWay
{
	OSMId id;
	std::vector<OSMId> nodeRefs;
	std::vector<OSMNode> nodes;

    void reset() 
    { 
    	nodeRefs.clear(); 
    	nodes.clear(); 
    }

    bool operator==(const OSMWay& rhs) const {
    	return (id == rhs.id) && (nodeRefs == rhs.nodeRefs); }
};

struct OSMMember
{
	OSMId ref;
	std::string role;
};


OSMWay NoOsmWay;

struct OSMRelation
{
	typedef std::map<std::string, std::string> TagsMap;

   private :
 
     TagsMap _tags;
     std::vector<OSMMember> _wayMembers;

   public :

     void reset();

     void addTag(const std::string& key, const std::string& value);

     void addWayMember(OSMId wayMemberId, const std::string& role);

     std::vector<OSMMember> getWayMembers() const;

     std::string getValueOrEmpty(const std::string& tag) const;

};

class OSMEntityHandler
{
protected:

	virtual ~OSMEntityHandler() {}

public:

	virtual void handleCity(const std::string& cityName, const std::string& cityCode, geos::geom::Geometry* boundary) = 0;

};

class FakeEntityHandler : public OSMEntityHandler
{
public:
	std::vector<boost::tuple<std::string, std::string> > handledCities;

	virtual void handleCity(const std::string& cityName, const std::string& cityCode, geos::geom::Geometry* boundary)
	{
		std::cerr << "Create city " << cityName << ", code = " << cityCode << ", boundary = " << 
			(boundary == 0 ? " none" : boundary->toString()) << std::endl;
		handledCities.push_back(boost::make_tuple(cityName, cityCode));
	}

};


FakeEntityHandler fakeEntityHandler;

class OSMParser
{
private:

	const std::string _cityCodeTag;

	OSMRelation _currentRelation;
	bool _inRelation;

	OSMWay _currentBoundaryWay;
	bool _inBoundaryWay;

	std::map<OSMId, OSMNode> _nodes;
	std::map<OSMId, OSMWay> _boundaryWays;
	int _passCount;

public:
	OSMParser(const std::string& cityCodeTag = std::string("ref:INSEE"));

	void parse(std::istream& osmInput);

private:
	void parseOnce(std::istream& osmInput);

	AttributesMap makeAttributesMap(const XML_Char **attrs);

    std::string currentRelationName() const;

    std::string currentRelationCode() const;

    bool currentRelationIsAdministrativeBoundary() const;

    bool currentRelationIsCityAdministrativeLevel() const;

	geos::geom::Geometry* makeGeometryFrom(OSMWay* way);
	geos::geom::Geometry* makeGeometryFrom(const std::vector<OSMWay*>& outerWays, const std::vector<OSMWay*>& innerWays);
	std::string makeWKTFrom(OSMWay* way);

	std::vector<geos::geom::Polygon*>* polygonize(const std::vector<OSMWay*>& ways);


	void handleStartElement(const XML_Char* name, const XML_Char** attrs);
	void handleEndElement(const XML_Char* name);

	void firstPassStartElement(const XML_Char* name, const XML_Char** attrs);
	void firstPassEndElement(const XML_Char* name);

	void secondPassStartElement(const XML_Char* name, const XML_Char** attrs);
	void secondPassEndElement(const XML_Char* name);

    friend void startElement(void* userData, const XML_Char* name, const XML_Char** attrs);
    friend void endElement(void* userData, const XML_Char* name);
    friend void characters(void* userData, const XML_Char* txt, int txtlen);
};


OSMParser::OSMParser(const std::string& cityCodeTag)
: _cityCodeTag(cityCodeTag)
, _passCount(0)
, _inRelation(false)
, _inBoundaryWay(false)
{
}


void OSMParser::handleStartElement(const XML_Char* name, const XML_Char** attrs)
{
	if (_passCount == 0)
	{
		firstPassStartElement(name, attrs);
	}
	else if (_passCount == 1)
	{
		secondPassStartElement(name, attrs);
	}

}


void OSMParser::handleEndElement(const XML_Char* name)
{
	if (_passCount == 0)
	{
		firstPassEndElement(name);
	}
	else if (_passCount == 1)
	{
		secondPassEndElement(name);
	}
}


void OSMParser::firstPassStartElement(const XML_Char* name, const XML_Char** attrs)
{
	if (!std::strcmp(name, "relation")) 
	{
		_currentRelation.reset();
		_inRelation = true;
	}
	else if (!std::strcmp(name, "node")) 
	{
        AttributesMap attributes = makeAttributesMap(attrs);
		OSMNode node;
		node.latitude = boost::lexical_cast<double>(attributes["lat"]);
		node.longitude = boost::lexical_cast<double>(attributes["lon"]);
		_nodes.insert(std::make_pair(boost::lexical_cast<long>(attributes["id"]), node));
	}
	else if (_inRelation && !std::strcmp(name, "tag")) 
	{
        AttributesMap attributes = makeAttributesMap(attrs);
		_currentRelation.addTag(attributes["k"], attributes["v"]);
	}
	else if (_inRelation && !std::strcmp(name, "member")) 
	{
        AttributesMap attributes = makeAttributesMap(attrs);
		if("way" == attributes["type"])
		{
			_currentRelation.addWayMember(boost::lexical_cast<OSMId>(attributes["ref"]), attributes["role"]);
		} 
	}
}


void OSMParser::firstPassEndElement(const XML_Char* name)
{
	if (!std::strcmp(name, "relation") &&
		currentRelationIsAdministrativeBoundary() && 
		currentRelationIsCityAdministrativeLevel())
	{
		std::vector<OSMMember> currentRelationWayMembers = _currentRelation.getWayMembers();
		std::cerr << "firstPassEndElement : found city relation with " << currentRelationWayMembers.size() << " way members" << std::endl;
		BOOST_FOREACH(OSMMember wayMember, currentRelationWayMembers)
		{
			_boundaryWays.insert(std::make_pair(wayMember.ref, NoOsmWay));
		}
		_inRelation = false;
	}
	else if (!std::strcmp(name, "osm")) 
	{
		++_passCount;
	}
}


void OSMParser::secondPassStartElement(const XML_Char* name, const XML_Char** attrs)
{
	if (!std::strcmp(name, "relation")) 
	{
		_currentRelation.reset();
		_inRelation = true;
	}
	else if (_inRelation && !std::strcmp(name, "tag")) 
	{
        AttributesMap attributes = makeAttributesMap(attrs);
		_currentRelation.addTag(attributes["k"], attributes["v"]);
	}
	else if (!std::strcmp(name, "way")) 
	{
        AttributesMap attributes = makeAttributesMap(attrs);
        OSMId wayId = boost::lexical_cast<OSMId>(attributes["id"]);
        if (_boundaryWays.find(wayId) != _boundaryWays.end())
        {
			_currentBoundaryWay.reset();
	        _currentBoundaryWay.id = wayId;
			_inBoundaryWay = true;
        }
	}
	else if (_inBoundaryWay && !std::strcmp(name, "nd")) 
	{
        AttributesMap attributes = makeAttributesMap(attrs);
		_currentBoundaryWay.nodeRefs.push_back(boost::lexical_cast<OSMId>(attributes["ref"]));
	}
	else if (_inRelation && !std::strcmp(name, "member")) 
	{
        AttributesMap attributes = makeAttributesMap(attrs);
		if("way" == attributes["type"])
		{
			_currentRelation.addWayMember(boost::lexical_cast<OSMId>(attributes["ref"]), attributes["role"]);
		} 
	}
}


void OSMParser::secondPassEndElement(const XML_Char* name)
{
	if (!std::strcmp(name, "way") &&
		_inBoundaryWay)
	{
		BOOST_FOREACH(OSMId nodeRef, _currentBoundaryWay.nodeRefs)
		{
			//if (_nodes.find(nodeRef) == _nodes.end()) continue; // TODO !!!!
			_currentBoundaryWay.nodes.push_back(_nodes[nodeRef]);
		}
		_boundaryWays[_currentBoundaryWay.id] = _currentBoundaryWay;
		_inBoundaryWay = false;

		//std::cerr << "new boundaryWay ! " << _currentBoundaryWay.id << std::endl;
	}
	else if (!std::strcmp(name, "relation") &&
		currentRelationIsAdministrativeBoundary() && 
		currentRelationIsCityAdministrativeLevel())
	{
		std::vector<OSMWay*> innerWays;
		std::vector<OSMWay*> outerWays;
		BOOST_FOREACH(OSMMember wayMember, _currentRelation.getWayMembers())
		{
			std::map<OSMId, OSMWay>::iterator it = _boundaryWays.find(wayMember.ref);
			if (it == _boundaryWays.end()) continue;
			if (it->second == NoOsmWay) continue;
			if (wayMember.role == "outer")
			{
				outerWays.push_back(&it->second);
			}
			else if (wayMember.role == "inner")
			{
				innerWays.push_back(&it->second);
			}
		}
		geos::geom::Geometry* boundary = makeGeometryFrom(outerWays, innerWays);
		//std::cerr << "OB " << outerWays.size() << "  IB " << innerWays.size() << std::endl;
		fakeEntityHandler.handleCity(currentRelationName(), currentRelationCode(), boundary);
	}
	else if (!std::strcmp(name, "osm")) 
	{
		++_passCount;
	}
}


std::vector<geos::geom::Polygon*>* 
OSMParser::polygonize(const std::vector<OSMWay*>& ways) {
   geos::geom::Geometry* g = NULL;
   BOOST_FOREACH(OSMWay* w, ways) {
   		//geos::geom::Geometry* wayGeom = makeGeometryFrom(w);
   		std::string wkt = makeWKTFrom(w);
   		//std::cerr << (wayGeom == 0 ? "bouh" : wayGeom->toText()) << " ";
   		std::cerr << wkt << " ";
	}
	std::cerr << std::endl;
	std::cerr << std::endl;

   std::vector<geos::geom::Polygon*>* ret = new std::vector<geos::geom::Polygon*>();
   const geos::geom::GeometryFactory *gf = geos::geom::GeometryFactory::getDefaultInstance();
   if(ways.size() >= 2) {
      geos::operation::linemerge::LineMerger lm;
      BOOST_FOREACH(OSMWay* w, ways) {
         if(w->nodes.size() < 2)
            continue;
         lm.add(makeGeometryFrom(w));
      }
      std::vector< geos::geom::LineString * > *lss = lm.getMergedLineStrings();
      BOOST_FOREACH(geos::geom::LineString *ls, *lss) {
		std::cerr << " ls " << ls << std::endl;
		std::cerr << " ls->isClosed() " << ls->isClosed() << std::endl;
      	std::cerr << " ls->getNumPoints() " << ls->getNumPoints() << std::endl;
      	 if(ls->getNumPoints()>3 && ls->isClosed()) {
            geos::geom::Polygon *p = gf->createPolygon(gf->createLinearRing(ls->getCoordinates()),0);
			ret->push_back(p);
         }
         delete ls;
      }
      lss->clear();
      delete lss;
   } else if(ways.size() == 1){
      OSMWay* w = ways.front();
      const std::vector<OSMNode>& nodes = w->nodes;
      if (nodes.size() > 3 && nodes.front() == nodes.back()) {
         //we have a closed way, return it
         g = makeGeometryFrom(w);
         geos::geom::CoordinateSequence *cs = g->getCoordinates();
         geos::geom::LinearRing *lr = gf->createLinearRing(cs);
         //std::vector<geos::geom::Geometry*>* holes = new std::vector<geos::geom::Geometry*>();

         geos::geom::Polygon *p = gf->createPolygon(lr,NULL);
         ret->push_back(p);
         delete g;
      }
   }
   return ret;
}


std::string
OSMParser::makeWKTFrom(OSMWay* way)
{
	std::stringstream wkt;
	std::vector<OSMNode>::const_iterator it = way->nodes.begin();
	wkt << "LINESTRING(";

	if (it != way->nodes.end())
		wkt << it->longitude << " " << it->latitude;

	while (++it != way->nodes.end()) 
	{
		wkt << "," << it->longitude << " " << it->latitude;
	}

	wkt << ")";
   return wkt.str();
}

geos::geom::Geometry*
OSMParser::makeGeometryFrom(OSMWay* way)
{
   static geos::io::WKTReader wktReader;
	std::stringstream wkt;
	std::vector<OSMNode>::const_iterator it = way->nodes.begin();
	wkt << "LINESTRING(";

	if (it != way->nodes.end())
		wkt << it->longitude << " " << it->latitude;

	while (++it != way->nodes.end()) 
	{
		wkt << "," << it->longitude << " " << it->latitude;
	}

	wkt << ")";
   //std::cerr << "=========== " << wkt.str() << std::endl;
   return wktReader.read(wkt.str());
}

geos::geom::Geometry*
OSMParser::makeGeometryFrom(const std::vector<OSMWay*>& outerWays, const std::vector<OSMWay*>& innerWays)
{
   const geos::geom::GeometryFactory *geometryFactory = geos::geom::GeometryFactory::getDefaultInstance();
   std::vector<geos::geom::Polygon*>* polygons, *polygon_enclaves;

   std::cerr << "=================================== " << outerWays.size() << " outer ways ; " <<  innerWays.size() << " inner ways" << std::endl;
   //if (!outerWays.empty()) std::cerr << "FIRST BOUNDARY WAY = " << std::endl << makeGeometryFrom(outerWays.at(0))->toText() << std::endl;

   polygons = polygonize(outerWays);

   if(!polygons->size()) {
      delete polygons;
      return NULL;
   }

   geos::geom::Geometry* poly = geometryFactory->createMultiPolygon((std::vector<geos::geom::Geometry*>*) polygons);
   //the geometries stored in *polygons are now owned by the multipolygon

   if (!poly->isValid()) {
      geos::geom::Geometry *tmp = poly->buffer(0.0);
      geometryFactory->destroyGeometry(poly);
      poly = tmp;
   }

   if (innerWays.size()) {
      polygon_enclaves = polygonize(innerWays);
      BOOST_FOREACH(geos::geom::Geometry* enc, *polygon_enclaves)
      {
         if (!enc->isValid()) {
            geos::geom::Geometry *tmp = enc->buffer(0.0);
            geometryFactory->destroyGeometry(enc);
            enc = tmp;
         }
         if (poly->intersects(enc)) {
            try {
               geos::geom::Geometry *tmp = poly->difference(enc);
               geometryFactory->destroyGeometry(poly);
               poly = tmp;
            } catch (geos::util::TopologyException e) {
               std::cout << std::endl << e.what() << std::endl;
               geos::operation::valid::IsValidOp ivo(enc);
               geos::operation::valid::TopologyValidationError *tve = ivo.getValidationError();
               const std::string errloc = tve->getCoordinate().toString();
               std::string errmsg(tve->getMessage());
               errmsg += "[" + errloc + "]";

            }
            geometryFactory->destroyGeometry(enc);
         }
      }
   }
   return poly;

}



void startElement(void* userData, const XML_Char* name, const XML_Char** attrs)
{
	OSMParser* osmParser = static_cast<OSMParser*>(userData);
	osmParser->handleStartElement(name, attrs);
}

void endElement(void* userData, const XML_Char* name)
{
	OSMParser* osmParser = static_cast<OSMParser*>(userData);
	osmParser->handleEndElement(name);
}


void characters(void* userData, const XML_Char* txt, int txtlen)
{

}


AttributesMap 
OSMParser::makeAttributesMap(const XML_Char **attrs) {
   int count = 0;
   AttributesMap attributesMap;
   while (attrs[count]) {
      attributesMap[attrs[count]] = attrs[count+1];
      //std::cerr << attributesMap[attrs[count]] << attrs[count+1] << std::endl;
      count += 2;
   }
   return attributesMap;
}


void
OSMParser::parseOnce(std::istream& osmInput)
{
   const XML_Char * encoding = NULL;
   XML_Parser expatParser = XML_ParserCreate(encoding);
   XML_SetUserData(expatParser, this);

   if (!expatParser) {
      throw std::runtime_error("error creating expat parser");
   }
   XML_SetElementHandler(expatParser, startElement, endElement);
   XML_SetCharacterDataHandler(expatParser, characters);

   long long int count = 0;
   int done, n = 0;
   char buf[1024*1024*4];
   do {
      osmInput.read(buf, 1024 * 1024 * 4);
      n = osmInput.gcount();
      done = (n != 1024 * 1024 * 4);
      if (XML_Parse(expatParser, buf, n, done) == XML_STATUS_ERROR) {
         XML_Error errorCode = XML_GetErrorCode(expatParser);
         int errorLine = XML_GetCurrentLineNumber(expatParser);
         long errorCol = XML_GetCurrentColumnNumber(expatParser);
         const XML_LChar *errorString = XML_ErrorString(errorCode);
         std::stringstream errorDesc;
         errorDesc << "XML parsing error at line " << errorLine << ":"
               << errorCol;
         errorDesc << ": " << errorString;
         throw std::runtime_error(errorDesc.str());
      }
      count += n;
      std::cout << "Read " << (count / (1024 * 1024)) << " MB " << std::endl;
   } while (!done);

   XML_ParserFree(expatParser);

}


void
OSMParser::parse(std::istream& osmInput)
{
   parseOnce(osmInput);
   
   osmInput.clear();
   osmInput.seekg(0);

   parseOnce(osmInput);
}

std::string 
OSMParser::currentRelationName() const
{
	return _currentRelation.getValueOrEmpty("name");
}


std::string 
OSMParser::currentRelationCode() const
{
	return _currentRelation.getValueOrEmpty(_cityCodeTag);
}


bool 
OSMParser::currentRelationIsAdministrativeBoundary() const
{
	return "administrative" == _currentRelation.getValueOrEmpty("boundary");
}


bool 
OSMParser::currentRelationIsCityAdministrativeLevel() const
{
	return "8" == _currentRelation.getValueOrEmpty("admin_level");
}



void 
OSMRelation::reset()
{
   _tags.clear();
   _wayMembers.clear();
}


void 
OSMRelation::addTag(const std::string& key, const std::string& value)
{
	_tags[key] = value;
}


void 
OSMRelation::addWayMember(OSMId wayMemberId, const std::string& role)
{
	OSMMember member;
	member.ref = wayMemberId;
	member.role = role;
	if ((member.role.empty()) || (member.role == "exclave")) 
	{
		member.role = "outer";
	}
	else if (member.role == "enclave")
	{
		member.role = "inner";
	}
	_wayMembers.push_back(member);
}

std::vector<OSMMember>
OSMRelation::getWayMembers() const
{
    return _wayMembers;
}


std::string OSMRelation::getValueOrEmpty(const std::string& tag) const
{
	TagsMap::const_iterator it = _tags.find(tag);
	return (_tags.end() == it) ? "" : it->second;
}



/*
BOOST_AUTO_TEST_CASE (should_parse_five_cities_from_osm_file)
{
	//std::ifstream osmStream("/home/mjambert/workspace/rcsmobility/gitlab/switzerland-tests/robot/resources/data/swiss.osm");
	std::ifstream osmStream("/home/mjambert/workspace/rcsmobility/git/synthese3/server/build/test/61_data_exchange/5_cities.osm");
	OSMParser parser("swisstopo:BFS_NUMMER");
	parser.parse(osmStream);
   osmStream.close();

   BOOST_CHECK_EQUAL(5, fakeEntityHandler.handledCities.size());
   int cityIndex = 0;
   BOOST_CHECK_EQUAL("Hauterive (NE)", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6454", fakeEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Neuchâtel", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6458", fakeEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Saint-Blaise", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6459", fakeEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Valangin", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6485", fakeEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Val-de-Ruz", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6487", fakeEntityHandler.handledCities[cityIndex++].get<1>());

}
*/

BOOST_AUTO_TEST_CASE (should_parse_city_boundary)
{
	//std::ifstream osmStream("/home/mjambert/workspace/rcsmobility/git/synthese3/server/build/test/61_data_exchange/monaco.osm");
	std::ifstream osmStream("/home/mjambert/workspace/rcsmobility/git/synthese3/server/build/test/61_data_exchange/capendu.osm");
	OSMParser parser;
	parser.parse(osmStream);
    osmStream.close();

   //BOOST_CHECK_EQUAL(5, fakeEntityHandler.handledCities.size());
   int cityIndex = 0;
   BOOST_CHECK_EQUAL("Hauterive (NE)", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6454", fakeEntityHandler.handledCities[cityIndex++].get<1>());

/*
   BOOST_CHECK_EQUAL("Neuchâtel", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6458", fakeEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Saint-Blaise", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6459", fakeEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Valangin", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6485", fakeEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Val-de-Ruz", fakeEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6487", fakeEntityHandler.handledCities[cityIndex++].get<1>());
*/
}

