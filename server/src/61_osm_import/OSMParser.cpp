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
#include "OSMLocale.hpp"

#include <expat.h>

#include <iostream>
#include <limits>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/shared_ptr.hpp>

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/prep/PreparedPolygon.h>
#include <geos/operation/polygonize/Polygonizer.h>
#include <geos/operation/linemerge/LineMerger.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/Coordinate.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/Point.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/util/TopologyException.h>
#include <geos/operation/valid/IsValidOp.h>
#include <geos/operation/valid/TopologyValidationError.h>
#include <geos/io/WKTReader.h>


namespace synthese
{
namespace data_exchange
{

class OSMParserImpl : public OSMEntityHandler
{
private:
	typedef std::map<std::string, std::string> AttributesMap;
	typedef boost::tuple<road::RoadType, bool, bool, bool> RoadCharacteristics;

	static OSMId ToOSMId(const std::string& osmIdStr);
	static OSMId ParseOSMId(const XML_Char** attrs);

	struct OSMNode
	{
		static OSMNode EMPTY;

		OSMId id;
		double latitude;
		double longitude;
		std::string houseNumberTag;
		std::string streetNameTag;
		unsigned char connectedWaysCount;

		OSMNode(): latitude(0.0), longitude(0.0), houseNumberTag(""), streetNameTag(""), connectedWaysCount(0) {}

		bool hasAddress() const { return houseNumberTag != "" && streetNameTag != ""; }

		bool operator==(const OSMNode& rhs) const {
			return (latitude == rhs.latitude) && (longitude == rhs.longitude); }

		bool isIntersection() const { return (1 < connectedWaysCount); }

	};

	struct OSMWay
	{
		static OSMWay EMPTY;

		static std::map<std::string, RoadCharacteristics> highwayTypes;

		OSMId id;
		std::vector<OSMId> nodeRefs;
		std::vector<OSMNode> nodes;
		bool isBoundaryWay;

		std::string nameTag;
		std::string highwayTag;
		std::string onewayTag;
		std::string junctionTag;
		std::string maxSpeedTag;


		OSMWay(): id(0), isBoundaryWay(false), nameTag(""), highwayTag(""), onewayTag(""), junctionTag(""), maxSpeedTag("") {}

		bool isHighway() const { return highwayTag != ""; }

		std::string getName() const;
		RoadCharacteristics getRoadCharacteristics() const;

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
		std::vector<OSMMember> _nodeMembers;

	public :
		static OSMRelation EMPTY;

		OSMId id;

		OSMRelation(): id(0) {}

		void addTag(const std::string& key, const std::string& value);

		void addWayMember(OSMId wayMemberId, const std::string& role);
		void addNodeMember(OSMId nodeMemberId, const std::string& role);

		std::vector<OSMMember> getWayMembers() const;
		std::vector<OSMMember> getNodeMembers() const;

		std::string getName() const;
		bool isAdministrativeBoundary() const;
		bool isCityAdministrativeLevel() const;
		bool isAssociatedSteet() const;

		std::string getValueOrEmpty(const std::string& tag) const;

		bool operator==(const OSMRelation& rhs) const {
			return (id == rhs.id);
		}

	};

	geos::io::WKTReader _wktReader;

	std::ostream& _logStream;
    const geos::geom::GeometryFactory& _geometryFactory;
	const CoordinatesSystem& _sourceCoordinatesSystem;
	OSMEntityHandler& _osmEntityHandler;
	const OSMLocale& _osmLocale;

	OSMNode _currentNode;
	OSMRelation _currentRelation;
	OSMWay _currentWay;

	std::map<OSMId, OSMNode> _nodes;
	std::map<OSMId, OSMWay> _boundaryWays;
	int _passCount;

public:
	OSMParserImpl(std::ostream& logStream,
				  const geos::geom::GeometryFactory& geometryFactory,
				  const CoordinatesSystem& sourceCoordinatesSystem,
				  OSMEntityHandler& osmEntityHandler,
				  const OSMLocale& osmLocale);

	void parse(std::istream& osmInput);

private:

	bool inNode() const { return !(_currentNode == OSMNode::EMPTY); }
	bool inRelation() const { return !(_currentRelation == OSMRelation::EMPTY); }
	bool inWay() const { return !(_currentWay == OSMWay::EMPTY); }
	bool inBoundaryWay() const { return inWay() && _currentWay.isBoundaryWay; }
	bool inHighway() const { return inWay() && _currentWay.isHighway(); }

	void parseOnce(std::istream& osmInput);

	AttributesMap makeAttributesMap(const XML_Char **attrs);
	geos::geom::Point* makeGeometryFrom(OSMNode* way);
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
	bool endNode(const XML_Char* name);
	void handleEndNode(const XML_Char* name);

	bool startNodeTag(const XML_Char* name);
	void handleStartNodeTag(const XML_Char* name, const XML_Char** attrs);

	void secondPassStartElement(const XML_Char* name, const XML_Char** attrs);
	void secondPassEndElement(const XML_Char* name);

	void thirdPassStartElement(const XML_Char* name, const XML_Char** attrs);
	void thirdPassEndElement(const XML_Char* name);

	bool passEnd(const XML_Char* name) const;
	void handleFirstPassEnd();
	void handleSecondPassEnd();
	void handleThirdPassEnd();

	bool startRelation(const XML_Char* name);
	void handleStartRelation(const XML_Char* name, const XML_Char** attrs);

	bool startRelationTag(const XML_Char* name);
	void handleStartRelationTag(const XML_Char* name, const XML_Char** attrs);

	bool startWayTag(const XML_Char* name);
	void handleStartWayTag(const XML_Char* name, const XML_Char** attrs);

	bool startWay(const XML_Char* name);
	void handleStartWay(const XML_Char* name, const XML_Char** attrs);

	bool startWayNodeRef(const XML_Char* name);
	void handleStartWayNodeRef(const XML_Char* name, const XML_Char** attrs);

	bool startRelationMember(const XML_Char* name);
	void handleStartRelationMember(const XML_Char* name, const XML_Char** attrs);

	bool endBoundaryWay(const XML_Char* name) const;
	void handleEndBoundaryWay();
	bool endHighway(const XML_Char* name);
	void handleFirstPassEndHighway(const XML_Char* name);
	void handleThirdPassEndHighway(const XML_Char* name);
	bool endCityRelation(const XML_Char* name) const;
	void handleEndCityRelation();
	bool endAssociatedStreetRelation(const XML_Char* name) const;
	void handleEndAssociatedStreetRelation();

	void handleCity(const std::string& cityName,
					const std::string& cityCode,
					boost::shared_ptr<geos::geom::Geometry> boundary);

	void handleRoad(const OSMId& roadSourceId, 
					const std::string& name,
					const road::RoadType& roadType, 
					boost::shared_ptr<geos::geom::Geometry> path);

	void handleCrossing(const OSMId& crossingSourceId, boost::shared_ptr<geos::geom::Point> point);

	void handleRoadChunk(size_t rank,
						 graph::MetricOffset metricOffset,
						 TrafficDirection trafficDirection,
						 double maxSpeed,
						 bool isDrivable,
						 bool isBikable,
						 bool isWalkable,
						 boost::shared_ptr<geos::geom::LineString> path);

	void handleHouse(const HouseNumber& houseNumber,
					 const std::string& streetName,
					 boost::shared_ptr<geos::geom::Point> boundary);

	void handleHouse(const HouseNumber& houseNumber,
					 const OSMId& roadSourceId,
					 boost::shared_ptr<geos::geom::Point> point);

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

void
OSMParserImpl::OSMRelation::addNodeMember(OSMId nodeMemberId, const std::string& role)
{
	OSMMember member;
	member.ref = nodeMemberId;
	member.role = role;
	_nodeMembers.push_back(member);
}


std::vector<OSMParserImpl::OSMMember>
OSMParserImpl::OSMRelation::getWayMembers() const
{
	return _wayMembers;
}

std::vector<OSMParserImpl::OSMMember>
OSMParserImpl::OSMRelation::getNodeMembers() const
{
	return _nodeMembers;
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

bool
OSMParserImpl::OSMRelation::isAssociatedSteet() const {
	return "associatedStreet" == getValueOrEmpty("type");
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
					         const geos::geom::GeometryFactory& geometryFactory,
							 const CoordinatesSystem& sourceCoordinatesSystem,
							 OSMEntityHandler& osmEntityHandler,
							 const OSMLocale& osmLocale)
	: _logStream(logStream)
	, _geometryFactory(geometryFactory)
	, _sourceCoordinatesSystem(sourceCoordinatesSystem)
	, _osmEntityHandler(osmEntityHandler)
	, _osmLocale(osmLocale)
	, _currentRelation(OSMRelation::EMPTY)
	, _currentWay(OSMWay::EMPTY)
	, _passCount(0)
{
}


OSMId
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
	else if (_passCount == 2)
	{
		thirdPassStartElement(name, attrs);
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
	else if (_passCount == 2)
	{
		thirdPassEndElement(name);
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
	else if (startNodeTag(name))
	{
		handleStartNodeTag(name, attrs);
	}
	else if (startRelationTag(name))
	{
		handleStartRelationTag(name, attrs);
	}
	else if (startWay(name))
	{
		handleStartWay(name, attrs);
	}
	else if (startWayNodeRef(name))
	{
		handleStartWayNodeRef(name, attrs);
	}
	else if (startWayTag(name))
	{
		handleStartWayTag(name, attrs);
	}
	else if (startRelationMember(name))
	{
		handleStartRelationMember(name, attrs);
	}
}


void
OSMParserImpl::firstPassEndElement(const XML_Char* name)
{
	if (endNode(name))
	{
		handleEndNode(name);
	}
	else if (endHighway(name))
	{
		handleFirstPassEndHighway(name);
	}
	else if (endCityRelation(name))
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
		handleFirstPassEnd();
	}
}

void 
OSMParserImpl::handleFirstPassEnd()
{
	_logStream << "Found " << _nodes.size() << " OSM nodes" << std::endl;
	++_passCount;
}

void 
OSMParserImpl::handleSecondPassEnd()
{
	++_passCount;
}

void
OSMParserImpl::handleThirdPassEnd()
{
	// Here we are sure that all roads and road chunks have been created
	// Therefore we can project houses on road chunks
	std::map<OSMId, OSMNode>::iterator nodeIt = _nodes.begin();
	while (nodeIt != _nodes.end())
	{
		if (nodeIt->second.hasAddress())
		{
			boost::shared_ptr<geos::geom::Point> nodeGeometry(makeGeometryFrom(&nodeIt->second));
			handleHouse(nodeIt->second.houseNumberTag, nodeIt->second.streetNameTag, nodeGeometry);
		}
		++nodeIt;
	}
	++_passCount;
}

bool
OSMParserImpl::startNode(const XML_Char* name)
{
	return !std::strcmp(name, "node");
}

void
OSMParserImpl::handleStartNode(const XML_Char* name, const XML_Char** attrs)
{
	AttributesMap attributes = makeAttributesMap(attrs);
	_currentNode.id = boost::lexical_cast<OSMId>(attributes["id"]);
	_currentNode.latitude = boost::lexical_cast<double>(attributes["lat"]);
	_currentNode.longitude = boost::lexical_cast<double>(attributes["lon"]);
}

bool
OSMParserImpl::startNodeTag(const XML_Char* name)
{
	return inNode() && !std::strcmp(name, "tag");
}

bool
OSMParserImpl::endNode(const XML_Char* name)
{
	return !std::strcmp(name, "node");
}

void
OSMParserImpl::handleEndNode(const XML_Char* name)
{
	_nodes.insert(std::make_pair(_currentNode.id, _currentNode));
	_currentNode = OSMNode::EMPTY;
}


void
OSMParserImpl::handleStartNodeTag(const XML_Char* name, const XML_Char** attrs)
{
	AttributesMap attributes = makeAttributesMap(attrs);
	if (attributes["k"] == "addr:housenumber")
	{
		_currentNode.houseNumberTag = attributes["v"];
	}
	else if (attributes["k"] == "addr:street")
	{
		_currentNode.streetNameTag = attributes["v"];
	}
}


bool
OSMParserImpl::startRelation(const XML_Char* name)
{
	return !std::strcmp(name, "relation");
}

OSMId
OSMParserImpl::ParseOSMId(const XML_Char** attrs)
{
	if(!strcmp("id", attrs[0])) return boost::lexical_cast<OSMId>(attrs[1]);
	int count = 2;
	while (attrs[count]) {
		if(!strcmp("id", attrs[count])) return boost::lexical_cast<OSMId>(attrs[count+1]);
		count += 2;
	}
	return 0;
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
OSMParserImpl::startWayTag(const XML_Char* name)
{
	return inWay() && !std::strcmp(name, "tag");
}

void
OSMParserImpl::handleStartWayTag(const XML_Char* name, const XML_Char** attrs)
{
	AttributesMap attributes = makeAttributesMap(attrs);
	if (attributes["k"] == "name")
	{
		_currentWay.nameTag = attributes["v"];
	}
	else if (attributes["k"] == "highway")
	{
		_currentWay.highwayTag = attributes["v"];
	}
	else if (attributes["k"] == "oneway")
	{
		_currentWay.onewayTag = attributes["v"];
	}
	else if (attributes["k"] == "junction")
	{
		_currentWay.junctionTag = attributes["v"];
	}
	else if (attributes["k"] == "maxspeed")
	{
		_currentWay.maxSpeedTag = attributes["v"];
	}
}


bool
OSMParserImpl::startWay(const XML_Char* name)
{
	return !std::strcmp(name, "way");
}

bool
OSMParserImpl::endHighway(const XML_Char* name)
{
	return !std::strcmp(name, "way") && inHighway();
}

void
OSMParserImpl::handleStartWay(const XML_Char* name, const XML_Char** attrs)
{
	OSMId wayId = ParseOSMId(attrs);
	_currentWay = OSMWay::EMPTY;
	_currentWay.id = wayId;
	if (_boundaryWays.find(wayId) != _boundaryWays.end())
	{
		_currentWay.isBoundaryWay = true;
	}
}

bool
OSMParserImpl::startWayNodeRef(const XML_Char* name)
{
	return inWay() && !std::strcmp(name, "nd");
}

void
OSMParserImpl::handleStartWayNodeRef(const XML_Char* name, const XML_Char** attrs)
{
	AttributesMap attributes = makeAttributesMap(attrs);
	_currentWay.nodeRefs.push_back(ToOSMId(attributes["ref"]));
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
	if("node" == attributes["type"])
	{
		_currentRelation.addNodeMember(ToOSMId(attributes["ref"]), attributes["role"]);
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
	else if (startWayNodeRef(name))
	{
		handleStartWayNodeRef(name, attrs);
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
	else if (endAssociatedStreetRelation(name))
	{
		handleEndAssociatedStreetRelation();
	}
	else if (passEnd(name))
	{
		handleSecondPassEnd();
	}
}


void
OSMParserImpl::thirdPassStartElement(const XML_Char* name, const XML_Char** attrs)
{
	if (startWay(name))
	{
		handleStartWay(name, attrs);
	}
	else if (startWayNodeRef(name))
	{
		handleStartWayNodeRef(name, attrs);
	}
	else if (startWayTag(name))
	{
		handleStartWayTag(name, attrs);
	}
}


void
OSMParserImpl::thirdPassEndElement(const XML_Char* name)
{
	// TODO : handle associatedStreet relations ?
	if (endHighway(name))
	{
		handleThirdPassEndHighway(name);
	}
	else if (passEnd(name))
	{
		handleThirdPassEnd();
	}
}


std::map<std::string, OSMParserImpl::RoadCharacteristics>
OSMParserImpl::OSMWay::highwayTypes = boost::assign::map_list_of
	// Roads													driveable / walkable / bikable
	("motorway",		boost::make_tuple(road::ROAD_TYPE_MOTORWAY,		true, false, false))
	("trunk",			boost::make_tuple(road::ROAD_TYPE_PRINCIPLEAXIS, true, false, false))
	("primary",			boost::make_tuple(road::ROAD_TYPE_PRINCIPLEAXIS, true, true, true))
	("secondary",		boost::make_tuple(road::ROAD_TYPE_SECONDARYAXIS, true, true, true))
	("tertiary",		boost::make_tuple(road::ROAD_TYPE_HIGHWAY,		true, true, true))
	("unclassified",	boost::make_tuple(road::ROAD_TYPE_STREET,		true, true, true))
	("residential",		boost::make_tuple(road::ROAD_TYPE_STREET,		true, true, true))

	// Link roads
	("motorway_link",	boost::make_tuple(road::ROAD_TYPE_ACCESSROAD,	true, false, false))
	("trunk_link",		boost::make_tuple(road::ROAD_TYPE_ACCESSROAD,	true, false, false))
	("primary_link",	boost::make_tuple(road::ROAD_TYPE_ACCESSROAD,	true, true, true))
	("secondary_link",	boost::make_tuple(road::ROAD_TYPE_ACCESSROAD,	true, true, true))
	("tertiary_link",	boost::make_tuple(road::ROAD_TYPE_ACCESSROAD,	true, true, true))

	// Special road types
	("living_street",	boost::make_tuple(road::ROAD_TYPE_STREET,			true, true, true))
	("pedestrian",		boost::make_tuple(road::ROAD_TYPE_PEDESTRIANSTREET,	false, true, true))
	("road",			boost::make_tuple(road::ROAD_TYPE_STREET,			true, true, true))

	// Paths
	("footway",			boost::make_tuple(road::ROAD_TYPE_PEDESTRIANPATH,	false, true, true))
	("steps",			boost::make_tuple(road::ROAD_TYPE_STEPS,			false, true, false))

	// Cycleway
	("cycleway",		boost::make_tuple(road::ROAD_TYPE_PEDESTRIANPATH,	false, true, true))
;


std::string
OSMParserImpl::OSMWay::getName() const
{
	return nameTag;
}


OSMParserImpl::RoadCharacteristics
OSMParserImpl::OSMWay::getRoadCharacteristics() const
{
	RoadCharacteristics characteristics = boost::make_tuple(road::ROAD_TYPE_UNKNOWN, false, false, false);
	if(highwayTag != "")
	{
		std::map<std::string, RoadCharacteristics>::iterator it = highwayTypes.find(highwayTag);
		if(it != highwayTypes.end())
			characteristics = it->second;
	}
	return characteristics;
}


void
OSMParserImpl::handleFirstPassEndHighway(const XML_Char* name)
{
	RoadCharacteristics roadCharacteristics = _currentWay.getRoadCharacteristics();
	road::RoadType roadType = roadCharacteristics.get<0>();

	if(_currentWay.nodeRefs.size() < 2)
	{
		_logStream << "Ignoring way with less than 2 nodes : " << _currentWay.id << std::endl;
		return;
	}

	if (road::ROAD_TYPE_UNKNOWN == roadType)
	{
		_logStream << "Ignoring way type " << _currentWay.highwayTag << " for : " << _currentWay.id << std::endl;
		return;
	}

	bool completeWayPath = true;
	BOOST_FOREACH(OSMId nodeRef, _currentWay.nodeRefs)
	{
		std::map<OSMId, OSMNode>::iterator itNode = _nodes.find(nodeRef);
		if (itNode == _nodes.end())
		{
			completeWayPath = false;
			break;
		}
		_currentWay.nodes.push_back(itNode->second);
	}

	if (!completeWayPath)
	{
		_logStream << "Ignoring way with unresolved nodes references : " << _currentWay.id << std::endl;
		return;
	}

	BOOST_FOREACH(OSMNode& currentWayNode, _currentWay.nodes)
	{
		_nodes[currentWayNode.id].connectedWaysCount += 1;
	}

	_currentWay = OSMWay::EMPTY;
}


void
OSMParserImpl::handleThirdPassEndHighway(const XML_Char* name)
{
	RoadCharacteristics roadCharacteristics = _currentWay.getRoadCharacteristics();
	road::RoadType roadType = roadCharacteristics.get<0>();
	bool isDrivable = roadCharacteristics.get<1>();
	bool isWalkable = roadCharacteristics.get<2>();
	bool isBikable = roadCharacteristics.get<3>();

	if((_currentWay.nodeRefs.size() < 2) || (road::ROAD_TYPE_UNKNOWN == roadType))
	{
		return;
	}

	bool completeWayPath = true;
	BOOST_FOREACH(OSMId nodeRef, _currentWay.nodeRefs)
	{
		std::map<OSMId, OSMNode>::iterator itNode = _nodes.find(nodeRef);
		if (itNode == _nodes.end())
		{
			completeWayPath = false;
			break;
		}
		_currentWay.nodes.push_back(itNode->second);
	}

	if (!completeWayPath)
	{
		return;
	}

	std::string roadName = _currentWay.getName();
	if (roadName == "")
	{
		std::map<std::string, std::string>::const_iterator it = _osmLocale.getDefaultRoadNames().find(_currentWay.highwayTag);
		if (it != _osmLocale.getDefaultRoadNames().end()) roadName = it->second;
	}

	TrafficDirection trafficDirection = TWO_WAYS;
	if ((_currentWay.highwayTag == "motorway") || (_currentWay.highwayTag == "motorway_link"))
	{
		trafficDirection = ONE_WAY;
	}
	if ((_currentWay.onewayTag == "true") || (_currentWay.onewayTag == "yes") || (_currentWay.onewayTag == "1"))
	{
		trafficDirection = ONE_WAY;
	}
	else if ((_currentWay.onewayTag == "false") || (_currentWay.onewayTag == "no") || (_currentWay.onewayTag == "0"))
	{
		trafficDirection = TWO_WAYS;
	}
	else if (_currentWay.onewayTag == "-1")
	{
		trafficDirection = REVERSED_ONE_WAY;
	}

	if(_currentWay.junctionTag == "roundabout")
	{
		trafficDirection = ONE_WAY;
	}

	double maxSpeed = 0.0;
	if (_currentWay.maxSpeedTag == "")
	{
		// TODO : we should handle default speed here
		maxSpeed = 50.0;
	}
	else if(_osmLocale.getImplicitSpeeds().find(_currentWay.maxSpeedTag) != _osmLocale.getImplicitSpeeds().end())
	{
		maxSpeed = _osmLocale.getImplicitSpeeds().find(_currentWay.maxSpeedTag)->second;
	}
	else
	{
		try
		{
			maxSpeed = boost::lexical_cast<double>(_currentWay.maxSpeedTag);
		}
		catch(boost::bad_lexical_cast &)
		{
			_logStream << "Cannot parse maxspeed tag value : " << _currentWay.maxSpeedTag << " ; using default speed instead" << std::endl;
		}
	}

	boost::shared_ptr<geos::geom::Geometry> roadGeometry(makeGeometryFrom(&_currentWay));
	handleRoad(_currentWay.id, roadName, roadType, roadGeometry);

	boost::shared_ptr<geos::geom::CoordinateSequence> cs(_geometryFactory.
		getCoordinateSequenceFactory()->create(0, 2));
	size_t rank(0);
	graph::MetricOffset metricOffset(0);

	int nodeCount(_currentWay.nodes.size());
	int i(0);
	BOOST_FOREACH(const OSMNode& node, _currentWay.nodes)
	{
		i++;
		boost::shared_ptr<geos::geom::Point> sourcePoint =
			_sourceCoordinatesSystem.createPoint(node.longitude, node.latitude);
		boost::shared_ptr<geos::geom::Point> convertedPoint =
			CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*sourcePoint);

		cs->add(*convertedPoint->getCoordinate());
		if(i == 1)
		{
			handleCrossing(node.id, convertedPoint);
			continue;
		}
		bool isLast = i == nodeCount;
		if(!node.isIntersection() && !isLast)
		{
			// Just extend the current geometry.
			continue;
		}
		boost::shared_ptr<geos::geom::LineString> roadChunkPath(_geometryFactory.createLineString(*cs));
		handleRoadChunk(rank, metricOffset, 
			trafficDirection, maxSpeed, isDrivable, isBikable, isWalkable, roadChunkPath);

		metricOffset += roadChunkPath->getLength();
		handleCrossing(node.id, convertedPoint);
		if(!isLast)
		{
			cs.reset(_geometryFactory.getCoordinateSequenceFactory()->create(0, 2));
			cs->add(*convertedPoint->getCoordinate());
		}
		++rank;
	}
	handleRoadChunk(rank, metricOffset, 
		trafficDirection, maxSpeed, isDrivable, isBikable, isWalkable, boost::shared_ptr<geos::geom::LineString>());

	_currentWay = OSMWay::EMPTY;
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
	BOOST_FOREACH(OSMId nodeRef, _currentWay.nodeRefs)
	{
		if (_nodes.find(nodeRef) == _nodes.end())
		{
			_logStream << "Ignoring incomplete city boundary way " << _currentWay.id << std::endl;
			completeBoundaryWay = false;
			break;
		}
		_currentWay.nodes.push_back(_nodes[nodeRef]);
	}
	if (completeBoundaryWay)
	{
		_logStream << "Found complete city boundary way " << _currentWay.id << std::endl;
		_boundaryWays[_currentWay.id] = _currentWay;
	}
	_currentWay = OSMWay::EMPTY;
}


bool
OSMParserImpl::endCityRelation(const XML_Char* name) const
{
	return !std::strcmp(name, "relation") &&
			 _currentRelation.isAdministrativeBoundary() &&
			 _currentRelation.isCityAdministrativeLevel();
}


bool
OSMParserImpl::endAssociatedStreetRelation(const XML_Char* name) const
{
	return !std::strcmp(name, "relation") &&
			 _currentRelation.isAssociatedSteet();
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
	std::string cityCode = _currentRelation.getValueOrEmpty(_osmLocale.getCityCodeTag());
	if (completeRelation)
	{
		try
		{
			boost::shared_ptr<geos::geom::Geometry> boundary(makeGeometryFrom(outerWays, innerWays));
			handleCity(cityName, cityCode, boundary);
		}
		catch (std::exception& e)
		{
			handleCity(cityName, cityCode, boost::shared_ptr<geos::geom::Geometry>());
		}
	}
	else
	{
		handleCity(cityName, cityCode, boost::shared_ptr<geos::geom::Geometry>());
	}
}


void 
OSMParserImpl::handleEndAssociatedStreetRelation()
{
	BOOST_FOREACH(OSMMember nodeMember, _currentRelation.getNodeMembers())
	{
		if (nodeMember.role == "house")
		{
			if (_nodes.find(nodeMember.ref) == _nodes.end())
			{
				_logStream << "Ignoring unknown house " << nodeMember.ref << " in street association " << _currentRelation.id << std::endl;
				continue;
			} 
			OSMNode& houseNode = _nodes[nodeMember.ref];
			BOOST_FOREACH(OSMMember wayMember, _currentRelation.getWayMembers())
			{
				boost::shared_ptr<geos::geom::Point> houseGeometry(makeGeometryFrom(&houseNode));
				handleHouse(houseNode.houseNumberTag, wayMember.ref, houseGeometry);
			}
		}
	}
}


std::vector<geos::geom::Polygon*>*
OSMParserImpl::polygonize(const std::vector<OSMWay*>& ways) {
	geos::geom::Geometry* g = NULL;

	std::vector<geos::geom::Polygon*>* ret = new std::vector<geos::geom::Polygon*>();
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
				geos::geom::Polygon *p = _geometryFactory.createPolygon(
					_geometryFactory.createLinearRing(ls->getCoordinates()),0);
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
			geos::geom::LinearRing *lr = _geometryFactory.createLinearRing(cs);
			geos::geom::Polygon *p = _geometryFactory.createPolygon(lr,NULL);
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


geos::geom::Point*
OSMParserImpl::makeGeometryFrom(OSMNode* node)
{
	return _geometryFactory.createPoint(geos::geom::Coordinate(node->longitude, node->latitude));
}


geos::geom::Geometry*
OSMParserImpl::makeGeometryFrom(OSMWay* way)
{
	return _wktReader.read(makeWKTFrom(way));
}

geos::geom::Geometry*
OSMParserImpl::makeGeometryFrom(const std::vector<OSMWay*>& outerWays, const std::vector<OSMWay*>& innerWays)
{
	std::vector<geos::geom::Polygon*> *polygons(NULL), *polygon_enclaves(NULL);
	polygons = polygonize(outerWays);

	if(!polygons->size()) {
		delete polygons;
		return NULL;
	}

	geos::geom::Geometry* poly = _geometryFactory.createMultiPolygon((std::vector<geos::geom::Geometry*>*) polygons);
	// The geometries stored in *polygons are now owned by the multipolygon

	geos::operation::valid::IsValidOp outerValidator(poly);
	if (!outerValidator.isValid()) {
		_logStream << "Outer multipolygon is not valid : " << outerValidator.getValidationError()->toString() << std::endl;
		BOOST_FOREACH(OSMWay* outerWay, outerWays)
		{
			_logStream << makeWKTFrom(outerWay) << " ";
		}
		_logStream << std::endl;

		geos::geom::Geometry *tmp = poly->buffer(0.0);
		_geometryFactory.destroyGeometry(poly);
		poly = tmp;
	}

	if (innerWays.size()) {
		polygon_enclaves = polygonize(innerWays);
		BOOST_FOREACH(geos::geom::Geometry* enc, *polygon_enclaves)
		{
			geos::operation::valid::IsValidOp encValidator(enc);
			if (!encValidator.isValid()) {
				_logStream << "Enclave multipolygon is not valid : " << encValidator.getValidationError()->toString() << std::endl;
				geos::geom::Geometry *tmp = enc->buffer(0.0);
				_geometryFactory.destroyGeometry(enc);
				enc = tmp;
			}

			if (poly->intersects(enc)) {
				try {
					geos::geom::Geometry *tmp = poly->difference(enc);
					_geometryFactory.destroyGeometry(poly);
					poly = tmp;
				} catch (const geos::util::TopologyException& te) {
					_logStream << "Failed to subtract enclave polygon : " << te.what() << std::endl;
				}
				_geometryFactory.destroyGeometry(enc);
			}
		}
	}

	return poly;
}



OSMParserImpl::AttributesMap
OSMParserImpl::makeAttributesMap(const XML_Char **attrs) {
	int count = 0;
	AttributesMap attributesMap;
	while (attrs[count]) {
		attributesMap[attrs[count]] = attrs[count+1];
		count += 2;
	}
	return attributesMap;
}


void OSMParserImpl::handleCity(const std::string& cityName, 
                    const std::string& cityCode, 
					boost::shared_ptr<geos::geom::Geometry> boundary)
{
	_logStream << "Found city with " << ((boundary != 0) ? "complete" : "incomplete") 
			<< " boundary : name =  " << cityName << " ; code = " << cityCode << std::endl;
	_osmEntityHandler.handleCity(cityName, cityCode, boundary);

}

void OSMParserImpl::handleRoad(const OSMId& roadSourceId, 
								const std::string& name,
								const road::RoadType& roadType, 
								boost::shared_ptr<geos::geom::Geometry> path)
{
	_logStream << "Handling Road { "
		<< "roadSourceId = " << roadSourceId << ", "
		<< "name = " << name << ", "
		<< "roadType = " << roadType << ", "
		<< "path = " << ((path != 0) ? "<found>" : "<none>")
		<< " }" << std::endl;
	_osmEntityHandler.handleRoad(roadSourceId, name, roadType, path);
}

void OSMParserImpl::handleCrossing(const OSMId& crossingSourceId, boost::shared_ptr<geos::geom::Point> point)
{
	_logStream << "Handling Crossing { "
		<< "crossingSourceId = " << crossingSourceId << ", "
		<< "point = " << ((point != 0) ? "<found>" : "<none>")
		<< " }" << std::endl;
	_osmEntityHandler.handleCrossing(crossingSourceId, point);
}

void OSMParserImpl::handleRoadChunk(size_t rank, 
							 graph::MetricOffset metricOffset,
							 TrafficDirection trafficDirection,
							 double maxSpeed,
		                     bool isDrivable,
		                     bool isBikable,
		                     bool isWalkable,
							 boost::shared_ptr<geos::geom::LineString> path)
{
	_logStream << "Handling RoadChunk { "
		<< "metricOffset = " << metricOffset << ", "
		<< "trafficDirection = " << trafficDirection << ", "
		<< "maxSpeed = " << maxSpeed << ", "
		<< "isDrivable = " << isDrivable << ", "
		<< "isBikable = " << isBikable << ", "
		<< "isWalkable = " << isWalkable << ", "
		<< "path = " << ((path != 0) ? "<found>" : "<none>")
		<< " }" << std::endl;
	_osmEntityHandler.handleRoadChunk(rank, metricOffset, 
		trafficDirection, maxSpeed, isDrivable, isBikable, isWalkable, path);
}

void 
OSMParserImpl::handleHouse(const HouseNumber& houseNumber,
						 const std::string& streetName,
						 boost::shared_ptr<geos::geom::Point> point)
{
	_logStream << "Handling House { "
		<< "houseNumber = " << houseNumber << ", "
		<< "streetName = " << streetName << ", "
		<< "point = " << ((point != 0) ? "<found>" : "<none>")
		<< " }" << std::endl;
	_osmEntityHandler.handleHouse(houseNumber, streetName, point);
}


void 
OSMParserImpl::handleHouse(const HouseNumber& houseNumber,
							 const OSMId& roadSourceId,
							 boost::shared_ptr<geos::geom::Point> point)
{
	_logStream << "Handling House { "
		<< "houseNumber = " << houseNumber << ", "
		<< "roadSourceId = " << roadSourceId << ", "
		<< "point = " << ((point != 0) ? "<found>" : "<none>")
		<< " }" << std::endl;
	_osmEntityHandler.handleHouse(houseNumber, roadSourceId, point);

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
		if (count % (1024 * 4 * 1024) == 0)
		std::cerr << "Read " << (count / (1024 * 1024)) << " MB " << std::endl;
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
	_logStream << "Second pass done" << std::endl;

	osmInput.clear();
	osmInput.seekg(0);

	_logStream << "Starting third pass over OSM input..." << std::endl;;
	parseOnce(osmInput);
	_logStream << "Third pass done" << std::endl;

	_nodes.clear();
}


OSMParser::OSMParser(std::ostream& logStream,
					 const geos::geom::GeometryFactory& geometryFactory,
					 const CoordinatesSystem& sourceCoordinatesSystem,
					 OSMEntityHandler& osmEntityHandler,
					 const OSMLocale& osmLocale)
	: _pimpl(new OSMParserImpl(logStream, geometryFactory, sourceCoordinatesSystem, osmEntityHandler, osmLocale))
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


