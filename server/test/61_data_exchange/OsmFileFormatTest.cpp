
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

typedef std::map<std::string, std::string> AttributesMap;
typedef long OSMId;


struct OSMNode
{
	double latitude;
	double longitude;
};

struct OSMWay
{
	OSMId id;
	std::vector<OSMId> nodeRefs;

    void reset() { nodeRefs.clear(); }
};

OSMWay NoOsmWay;

struct OSMRelation
{
	typedef std::map<std::string, std::string> TagsMap;

   private :
 
     TagsMap _tags;
     std::vector<OSMId> _wayMemberIds;

   public :

     void reset();

     void addTag(const std::string& key, const std::string& value);

     void addWayMember(OSMId wayMemberId);

     std::vector<OSMId> getWayMemberIds() const;

     std::string getValueOrEmpty(const std::string& tag) const;

};

class OSMEntityHandler
{
protected:

	virtual ~OSMEntityHandler() {}

public:

	virtual void handleCity(const std::string& cityName, const std::string& cityCode, std::string cityBoundaries) = 0;

};

class FakeEntityHandler : public OSMEntityHandler
{
public:
	std::vector<boost::tuple<std::string, std::string> > handledCities;

	virtual void handleCity(const std::string& cityName, const std::string& cityCode, std::string cityBoundaries)
	{
		std::cerr << "Create city " << cityName << ", code = " << cityCode << ", boundaries = " << cityBoundaries << std::endl;
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
			_currentRelation.addWayMember(boost::lexical_cast<OSMId>(attributes["ref"]));
		} 
	}
}


void OSMParser::firstPassEndElement(const XML_Char* name)
{
	if (!std::strcmp(name, "relation") &&
		currentRelationIsAdministrativeBoundary() && 
		currentRelationIsCityAdministrativeLevel())
	{
		std::vector<OSMId> currentRelationWayMemberIds = _currentRelation.getWayMemberIds();
		std::cerr << "firstPassEndElement : found city relation with " << currentRelationWayMemberIds.size() << " way members" << std::endl;
		BOOST_FOREACH(OSMId id, currentRelationWayMemberIds)
		{
			_boundaryWays.insert(std::make_pair(id, NoOsmWay));
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
		std::cerr << "way! " << _boundaryWays.size() << std::endl;
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
}


void OSMParser::secondPassEndElement(const XML_Char* name)
{
	if (!std::strcmp(name, "way") &&
		_inBoundaryWay)
	{
		_boundaryWays[_currentBoundaryWay.id] = _currentBoundaryWay;
		_inBoundaryWay = false;
		std::cerr << "new boundaryWay ! " << _currentBoundaryWay.id << std::endl;
	}
	else if (!std::strcmp(name, "relation") &&
		currentRelationIsAdministrativeBoundary() && 
		currentRelationIsCityAdministrativeLevel())
	{
		fakeEntityHandler.handleCity(currentRelationName(), currentRelationCode(), "");
	}
	else if (!std::strcmp(name, "osm")) 
	{
		++_passCount;
	}
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
   _wayMemberIds.clear();
}


void 
OSMRelation::addTag(const std::string& key, const std::string& value)
{
	_tags[key] = value;
}


void 
OSMRelation::addWayMember(OSMId wayMemberId)
{
	_wayMemberIds.push_back(wayMemberId);
}

std::vector<OSMId>
OSMRelation::getWayMemberIds() const
{
    return _wayMemberIds;
}


std::string OSMRelation::getValueOrEmpty(const std::string& tag) const
{
	TagsMap::const_iterator it = _tags.find(tag);
	return (_tags.end() == it) ? "" : it->second;
}




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

