
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

#include <fstream>
#include <iostream>

#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>

typedef std::map<std::string, std::string> AttributesMap;

struct OSMNode
{
	double latitude;
	double longitude;
};


struct OSMRelation
{
	typedef std::map<std::string, std::string> TagsMap;

   private :
 
     TagsMap tags;

   public :

     void reset();

     void addTag(const std::string& key, const std::string& value);

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
	typedef long OSMId;

	const std::string _cityCodeTag;
	OSMRelation currentRelation;
	bool inRelation;
	std::map<OSMId, OSMNode> _nodes;

public:
	OSMParser(const std::string& cityCodeTag = std::string("ref:INSEE"));

	void parse(std::istream& osmInput);

private:
	AttributesMap makeAttributesMap(const XML_Char **attrs);

    std::string currentRelationName() const;

    std::string currentRelationCode() const;

    bool currentRelationIsAdministrativeBoundary() const;

    bool currentRelationIsCityAdministrativeLevel() const;



	void handleStartElement(const XML_Char* name, const XML_Char** attrs);
	void handleEndElement(const XML_Char* name);

    friend void startElement(void* userData, const XML_Char* name, const XML_Char** attrs);
    friend void endElement(void* userData, const XML_Char* name);
    friend void characters(void* userData, const XML_Char* txt, int txtlen);
};

void OSMParser::handleStartElement(const XML_Char* name, const XML_Char** attrs)
{
	if (!std::strcmp(name, "relation")) 
	{
		currentRelation.reset();
		inRelation = true;
	}
	else if (inRelation && !std::strcmp(name, "tag")) 
	{
        AttributesMap attributes = makeAttributesMap(attrs);
		currentRelation.addTag(attributes["k"], attributes["v"]);
	}
	else if (!std::strcmp(name, "node")) 
	{
        AttributesMap attributes = makeAttributesMap(attrs);
		OSMNode node;
		node.latitude = 3;
		node.longitude = 3;
		_nodes.insert(std::make_pair(boost::lexical_cast<long>(attributes["id"]), node));
	}

}

void OSMParser::handleEndElement(const XML_Char* name)
{
	if (!std::strcmp(name, "relation") &&
		currentRelationIsAdministrativeBoundary() && 
		currentRelationIsCityAdministrativeLevel())
	{
		fakeEntityHandler.handleCity(currentRelationName(), currentRelationCode(), "");
		inRelation = false;
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


OSMParser::OSMParser(const std::string& cityCodeTag)
: _cityCodeTag(cityCodeTag)
, inRelation(false)
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
OSMParser::parse(std::istream& osmInput)
{
   long long int count;
   int done, n = 0;
   char buf[1024*1024*4];

   XML_Parser p = XML_ParserCreate(NULL);
   XML_SetUserData(p, this);

   if (!p) {
      throw std::runtime_error("error creating expat parser");
   }
   XML_SetElementHandler(p, startElement, endElement);
   XML_SetCharacterDataHandler(p, characters);

   do {
      osmInput.read(buf, 1024 * 1024 * 4);
      n = osmInput.gcount();
      done = (n != 1024 * 1024 * 4);
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
      std::cout << "Read " << (count / (1024 * 1204)) << " MB " << std::endl;
   } while (!done);

   XML_ParserFree(p);
}

std::string 
OSMParser::currentRelationName() const
{
	return currentRelation.getValueOrEmpty("name");
}


std::string 
OSMParser::currentRelationCode() const
{
	return currentRelation.getValueOrEmpty(_cityCodeTag);
}


bool 
OSMParser::currentRelationIsAdministrativeBoundary() const
{
	return "administrative" == currentRelation.getValueOrEmpty("boundary");
}


bool 
OSMParser::currentRelationIsCityAdministrativeLevel() const
{
	return "8" == currentRelation.getValueOrEmpty("admin_level");
}



void OSMRelation::reset()
{
   tags.clear();
}


void OSMRelation::addTag(const std::string& key, const std::string& value)
{
	tags[key] = value;
}


std::string OSMRelation::getValueOrEmpty(const std::string& tag) const
{
	TagsMap::const_iterator it = tags.find(tag);
	return (tags.end() == it) ? "" : it->second;
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

