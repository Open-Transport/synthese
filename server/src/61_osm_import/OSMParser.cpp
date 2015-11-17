/** OsmParser class implementation.
	@file OsmParser.cpp
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
#include "OSMParser.hpp"
#include "OSMEntityHandler.hpp"

#include <expat.h>

#include <iostream>
#include <limits>

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


namespace synthese
{
namespace data_exchange
{

class OSMParserImpl
{
private:
	typedef std::map<std::string, std::string> AttributesMap;

	typedef long OSMId;

	static OSMId ToOSMId(const std::string& osmIdStr);
	static OSMId ParseOSMId(const XML_Char** attrs);

	struct OSMNode
	{
		static OSMNode EMPTY;

		double latitude;
		double longitude;

		OSMNode(): latitude(0.0), longitude(0.0) {}

		bool operator==(const OSMNode& rhs) const {
			return (latitude == rhs.latitude) && (longitude == rhs.longitude); }

	};

	struct OSMWay
	{
		static OSMWay EMPTY;

		OSMId id;
		std::vector<OSMId> nodeRefs;
		std::vector<OSMNode> nodes;

		OSMWay(): id(0) {}

		bool operator==(const OSMWay& rhs) const {
			return (id == rhs.id); }
	};

	struct OSMMember
	{
		static OSMMember EMPTY;

		OSMMember(): ref(0) {}

		OSMId ref;
		std::string role;
	};


	struct OSMRelation
	{
	private :

		typedef std::map<std::string, std::string> TagsMap;

		TagsMap _tags;
		std::vector<OSMMember> _wayMembers;

	public :
		static OSMRelation EMPTY;

		OSMId id;

		OSMRelation(): id(0) {}

		void addTag(const std::string& key, const std::string& value);

		void addWayMember(OSMId wayMemberId, const std::string& role);

		std::vector<OSMMember> getWayMembers() const;

		std::string getName() const;
		bool isAdministrativeBoundary() const;
		bool isCityAdministrativeLevel() const;

		std::string getValueOrEmpty(const std::string& tag) const;

		bool operator==(const OSMRelation& rhs) const {
			return (id == rhs.id);
		}

	};

	geos::io::WKTReader _wktReader;

	std::ostream& _logStream;
	OSMEntityHandler& _osmEntityHandler;
	const std::string _cityCodeTag;

	OSMRelation _currentRelation;
	OSMWay _currentBoundaryWay;

	std::map<OSMId, OSMNode> _nodes;
	std::map<OSMId, OSMWay> _boundaryWays;
	int _passCount;

public:
	OSMParserImpl(std::ostream& logStream,
				  OSMEntityHandler& osmEntityHandler,
				  const std::string& cityCodeTag);

	void parse(std::istream& osmInput);

private:

	bool inRelation() const { return !(_currentRelation == OSMRelation::EMPTY); }
	bool inBoundaryWay() const { return !(_currentBoundaryWay == OSMWay::EMPTY); }

	void parseOnce(std::istream& osmInput);

	AttributesMap makeAttributesMap(const XML_Char **attrs, size_t nFirst = std::numeric_limits<int>::max());
	geos::geom::Geometry* makeGeometryFrom(OSMWay* way);
	geos::geom::Geometry* makeGeometryFrom(const std::vector<OSMWay*>& outerWays, const std::vector<OSMWay*>& innerWays);
	std::string makeWKTFrom(OSMWay* way);

	std::vector<geos::geom::Polygon*>* polygonize(const std::vector<OSMWay*>& ways);

	void handleStartElement(const XML_Char* name, const XML_Char** attrs);
	void handleEndElement(const XML_Char* name);

	void firstPassStartElement(const XML_Char* name, const XML_Char** attrs);
	void firstPassEndElement(const XML_Char* name);

	bool startNode(const XML_Char* name);
	void handleStartNode(const XML_Char* name, const XML_Char** attrs);

	void secondPassStartElement(const XML_Char* name, const XML_Char** attrs);
	void secondPassEndElement(const XML_Char* name);

	bool passEnd(const XML_Char* name) const;

	bool startRelation(const XML_Char* name);
	void handleStartRelation(const XML_Char* name, const XML_Char** attrs);

	bool startRelationTag(const XML_Char* name);
	void handleStartRelationTag(const XML_Char* name, const XML_Char** attrs);

	bool startWay(const XML_Char* name);
	void handleStartWay(const XML_Char* name, const XML_Char** attrs);

	bool startBoundaryWayNodeRef(const XML_Char* name);
	void handleStartBoundaryWayNodeRef(const XML_Char* name, const XML_Char** attrs);

	bool startRelationMember(const XML_Char* name);
	void handleStartRelationMember(const XML_Char* name, const XML_Char** attrs);

	bool endBoundaryWay(const XML_Char* name) const;
	void handleEndBoundaryWay();
	bool endCityRelation(const XML_Char* name) const;
	void handleEndCityRelation();

	friend void startElement(void* userData, const XML_Char* name, const XML_Char** attrs);
	friend void endElement(void* userData, const XML_Char* name);
};


OSMParserImpl::OSMNode OSMParserImpl::OSMNode::EMPTY;


OSMParserImpl::OSMWay OSMParserImpl::OSMWay::EMPTY;

OSMParserImpl::OSMMember OSMParserImpl::OSMMember::EMPTY;

OSMParserImpl::OSMRelation OSMParserImpl::OSMRelation::EMPTY;

void
OSMParserImpl::OSMRelation::addTag(const std::string& key, const std::string& value)
{
	_tags[key] = value;
}


void
OSMParserImpl::OSMRelation::addWayMember(OSMId wayMemberId, const std::string& role)
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

std::vector<OSMParserImpl::OSMMember>
OSMParserImpl::OSMRelation::getWayMembers() const
{
	return _wayMembers;
}

bool
OSMParserImpl::OSMRelation::isAdministrativeBoundary() const
{
	return "administrative" == getValueOrEmpty("boundary");
}

bool
OSMParserImpl::OSMRelation::isCityAdministrativeLevel() const
{
	return "8" == getValueOrEmpty("admin_level");
}

std::string
OSMParserImpl::OSMRelation::getName() const
{
	return getValueOrEmpty("name");
}

std::string
OSMParserImpl::OSMRelation::getValueOrEmpty(const std::string& tag) const
{
	TagsMap::const_iterator it = _tags.find(tag);
	return (_tags.end() == it) ? "" : it->second;
}


OSMParserImpl::OSMParserImpl(std::ostream& logStream,
							 OSMEntityHandler& osmEntityHandler,
							 const std::string& cityCodeTag)
	: _logStream(logStream)
	, _osmEntityHandler(osmEntityHandler)
	, _cityCodeTag(cityCodeTag)
	, _currentRelation(OSMRelation::EMPTY)
	, _currentBoundaryWay(OSMWay::EMPTY)
	, _passCount(0)
{
}


OSMParserImpl::OSMId
OSMParserImpl::ToOSMId(const std::string& osmIdStr)
{
	return boost::lexical_cast<OSMId>(osmIdStr);
}

void
OSMParserImpl::handleStartElement(const XML_Char* name, const XML_Char** attrs)
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


void
OSMParserImpl::handleEndElement(const XML_Char* name)
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


void
OSMParserImpl::firstPassStartElement(const XML_Char* name, const XML_Char** attrs)
{
	if (startRelation(name))
	{
		handleStartRelation(name, attrs);
	}
	else if (startNode(name))
	{
		handleStartNode(name, attrs);
	}
	else if (startRelationTag(name))
	{
		handleStartRelationTag(name, attrs);
	}
	else if (startRelationMember(name))
	{
		handleStartRelationMember(name, attrs);
	}
}


void
OSMParserImpl::firstPassEndElement(const XML_Char* name)
{
	if (endCityRelation(name))
	{
		std::vector<OSMMember> currentRelationWayMembers = _currentRelation.getWayMembers();
		BOOST_FOREACH(OSMMember wayMember, currentRelationWayMembers)
		{
			_boundaryWays.insert(std::make_pair(wayMember.ref, OSMWay::EMPTY));
		}
		_currentRelation = OSMRelation::EMPTY;
	}
	else if (passEnd(name))
	{
		_logStream << "Found " << _nodes.size() << " OSM nodes" << std::endl;
		++_passCount;
	}
}

bool
OSMParserImpl::startNode(const XML_Char* name)
{
	return !std::strcmp(name, "node");
}

void
OSMParserImpl::handleStartNode(const XML_Char* name, const XML_Char** attrs)
{
	AttributesMap attributes = makeAttributesMap(attrs, 3);
	OSMNode node;
	node.latitude = boost::lexical_cast<double>(attributes["lat"]);
	node.longitude = boost::lexical_cast<double>(attributes["lon"]);
	_nodes.insert(std::make_pair(ToOSMId(attributes["id"]), node));
}


bool
OSMParserImpl::startRelation(const XML_Char* name)
{
	return !std::strcmp(name, "relation");
}

OSMParserImpl::OSMId
OSMParserImpl::ParseOSMId(const XML_Char** attrs)
{
	if(!strcmp("id", attrs[0])) return (OSMId) atol(attrs[1]);
	int count = 2;
	while (attrs[count]) {
		if(!strcmp("id", attrs[count])) return (OSMId) atol(attrs[count+1]);
		count += 2;
	}
	return -1;
}

void
OSMParserImpl::handleStartRelation(const XML_Char* name, const XML_Char** attrs)
{
	_currentRelation = OSMRelation::EMPTY;
	_currentRelation.id = ParseOSMId(attrs);
}

bool
OSMParserImpl::startRelationTag(const XML_Char* name)
{
	return inRelation() && !std::strcmp(name, "tag");
}

void
OSMParserImpl::handleStartRelationTag(const XML_Char* name, const XML_Char** attrs)
{
	AttributesMap attributes = makeAttributesMap(attrs);
	_currentRelation.addTag(attributes["k"], attributes["v"]);
}

bool
OSMParserImpl::startWay(const XML_Char* name)
{
	return !std::strcmp(name, "way");
}

void
OSMParserImpl::handleStartWay(const XML_Char* name, const XML_Char** attrs)
{
	OSMId wayId = ParseOSMId(attrs);
	if (_boundaryWays.find(wayId) != _boundaryWays.end())
	{
		_currentBoundaryWay = OSMWay::EMPTY;
		_currentBoundaryWay.id = wayId;
	}
}

bool
OSMParserImpl::startBoundaryWayNodeRef(const XML_Char* name)
{
	return inBoundaryWay() && !std::strcmp(name, "nd");
}

void
OSMParserImpl::handleStartBoundaryWayNodeRef(const XML_Char* name, const XML_Char** attrs)
{
	AttributesMap attributes = makeAttributesMap(attrs);
	_currentBoundaryWay.nodeRefs.push_back(ToOSMId(attributes["ref"]));
}

bool
OSMParserImpl::startRelationMember(const XML_Char* name)
{
	return inRelation() && !std::strcmp(name, "member");
}

void
OSMParserImpl::handleStartRelationMember(const XML_Char* name, const XML_Char** attrs)
{
	AttributesMap attributes = makeAttributesMap(attrs);
	if("way" == attributes["type"])
	{
		_currentRelation.addWayMember(ToOSMId(attributes["ref"]), attributes["role"]);
	}
}


void
OSMParserImpl::secondPassStartElement(const XML_Char* name, const XML_Char** attrs)
{
	if (startRelation(name))
	{
		handleStartRelation(name, attrs);
	}
	else if (startRelationTag(name))
	{
		handleStartRelationTag(name, attrs);
	}
	else if (startWay(name))
	{
		handleStartWay(name, attrs);
	}
	else if (startBoundaryWayNodeRef(name))
	{
		handleStartBoundaryWayNodeRef(name, attrs);
	}
	else if (startRelationMember(name))
	{
		handleStartRelationMember(name, attrs);
	}
}


bool
OSMParserImpl::passEnd(const XML_Char* name) const
{
	return !std::strcmp(name, "osm");
}


void
OSMParserImpl::secondPassEndElement(const XML_Char* name)
{
	if (endBoundaryWay(name))
	{
		handleEndBoundaryWay();
	}
	else if (endCityRelation(name))
	{
		handleEndCityRelation();
	}
	else if (passEnd(name))
	{
		++_passCount;
	}
}

bool
OSMParserImpl::endBoundaryWay(const XML_Char* name) const
{
	return (!std::strcmp(name, "way") && inBoundaryWay());
}

void
OSMParserImpl::handleEndBoundaryWay()
{
	bool completeBoundaryWay = true;
	BOOST_FOREACH(OSMId nodeRef, _currentBoundaryWay.nodeRefs)
	{
		if (_nodes.find(nodeRef) == _nodes.end())
		{
			_logStream << "Ignoring incomplete city boundary way " << _currentBoundaryWay.id << std::endl;
			completeBoundaryWay = false;
			break;
		}
		_currentBoundaryWay.nodes.push_back(_nodes[nodeRef]);
	}
	if (completeBoundaryWay)
	{
		_logStream << "Found complete city boundary way " << _currentBoundaryWay.id << std::endl;
		_boundaryWays[_currentBoundaryWay.id] = _currentBoundaryWay;
	}
	_currentBoundaryWay = OSMWay::EMPTY;
}


bool
OSMParserImpl::endCityRelation(const XML_Char* name) const
{
	return !std::strcmp(name, "relation") &&
			 _currentRelation.isAdministrativeBoundary() &&
			 _currentRelation.isCityAdministrativeLevel();
}

void
OSMParserImpl::handleEndCityRelation()
{
	std::vector<OSMWay*> innerWays;
	std::vector<OSMWay*> outerWays;
	bool completeRelation = true;
	BOOST_FOREACH(OSMMember wayMember, _currentRelation.getWayMembers())
	{
		std::map<OSMId, OSMWay>::iterator it = _boundaryWays.find(wayMember.ref);
		if ((it == _boundaryWays.end()) || (it->second == OSMWay::EMPTY))
		{
			completeRelation = false;
			_logStream << "Ignoring incomplete city boundary in relation " << _currentRelation.id << std::endl;
			break;
		}
		if (wayMember.role == "outer")
		{
			outerWays.push_back(&it->second);
		}
		else if (wayMember.role == "inner")
		{
			innerWays.push_back(&it->second);
		}
	}

	std::string cityName = _currentRelation.getName();
	std::string cityCode = _currentRelation.getValueOrEmpty(_cityCodeTag);
	if (completeRelation)
	{
		_logStream << "Found city with boundary : name =  " << cityName << " ; code = " << cityCode << std::endl;
		geos::geom::Geometry* boundary = makeGeometryFrom(outerWays, innerWays);
		_osmEntityHandler.handleCity(cityName, cityCode, boundary);
	}
	else
	{
		_logStream << "Found city without boundary : name =  " << cityName << " ; code = " << cityCode << std::endl;
		_osmEntityHandler.handleCity(cityName, cityCode, 0);
	}
}



std::vector<geos::geom::Polygon*>*
OSMParserImpl::polygonize(const std::vector<OSMWay*>& ways) {
	geos::geom::Geometry* g = NULL;

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
OSMParserImpl::makeWKTFrom(OSMWay* way)
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
OSMParserImpl::makeGeometryFrom(OSMWay* way)
{
	return _wktReader.read(makeWKTFrom(way));
}

geos::geom::Geometry*
OSMParserImpl::makeGeometryFrom(const std::vector<OSMWay*>& outerWays, const std::vector<OSMWay*>& innerWays)
{
	const geos::geom::GeometryFactory *geometryFactory = geos::geom::GeometryFactory::getDefaultInstance();
	std::vector<geos::geom::Polygon*>* polygons, *polygon_enclaves;
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
					_logStream << std::endl << e.what() << std::endl;
				}
				geometryFactory->destroyGeometry(enc);
			}
		}
	}
	return poly;

}


OSMParserImpl::AttributesMap
OSMParserImpl::makeAttributesMap(const XML_Char **attrs, size_t nFirst) {
	int count = 0;
	AttributesMap attributesMap;
	while (attrs[count]) {
		attributesMap[attrs[count]] = attrs[count+1];
		count += 2;
		if (attributesMap.size() == nFirst) break;
	}
	return attributesMap;
}



void startElement(void* userData, const XML_Char* name, const XML_Char** attrs)
{
	OSMParserImpl* osmParser = static_cast<OSMParserImpl*>(userData);
	osmParser->handleStartElement(name, attrs);
}

void endElement(void* userData, const XML_Char* name)
{
	OSMParserImpl* osmParser = static_cast<OSMParserImpl*>(userData);
	osmParser->handleEndElement(name);
}


void
OSMParserImpl::parseOnce(std::istream& osmInput)
{
	const XML_Char * encoding = NULL;
	XML_Parser expatParser = XML_ParserCreate(encoding);
	XML_SetUserData(expatParser, this);

	if (!expatParser) {
		throw std::runtime_error("error creating expat parser");
	}
	XML_SetElementHandler(expatParser, startElement, endElement);

	long long int count = 0;
	int done, n = 0;
	const int bufferSize = 4 * 1024;
	char buf[bufferSize];
	do {
		osmInput.read(buf, bufferSize);
		n = osmInput.gcount();
		done = (n != bufferSize);
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
		//if (count % (1024 * 4 * 1024) == 0)
		//std::cout << "Read " << (count / (1024 * 1024)) << " MB " << std::endl;
	} while (!done);

	XML_ParserFree(expatParser);

}


void
OSMParserImpl::parse(std::istream& osmInput)
{
	_logStream << "Starting first pass over OSM input..." << std::endl;
	parseOnce(osmInput);
	_logStream << "First pass done" << std::endl;;

	osmInput.clear();
	osmInput.seekg(0);

	_logStream << "Starting second pass over OSM input..." << std::endl;;
	parseOnce(osmInput);
	_logStream << "Second pass done" << std::endl;;
}


OSMParser::OSMParser(std::ostream& logStream,
					 OSMEntityHandler& osmEntityHandler,
					 const std::string& cityCodeTag)
	: _pimpl(new OSMParserImpl(logStream, osmEntityHandler, cityCodeTag))
{
}


OSMParser::~OSMParser()
{
	delete _pimpl;
}

void
OSMParser::parse(std::istream& osmInput)
{
	_pimpl->parse(osmInput);
}


}
}


