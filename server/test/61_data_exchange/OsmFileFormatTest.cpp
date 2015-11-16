
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

typedef std::map<std::string, std::string> AttributesMap;
typedef std::map<std::string, std::string> TagsMap;

struct OSMRelation
{

   private :
 
     TagsMap tags;

     std::string getValueOrEmpty(const std::string& tag) const;

   public :

     void reset();

     void addTag(const std::string& key, const std::string& value);

     std::string getName() const;

     std::string getCode() const;

     bool isAdministrativeBoundary() const;

     bool isCityAdministrativeLevel() const;

};


bool inRelation(false);
int createdCitiesCount = 0;
OSMRelation currentRelation;



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


std::string OSMRelation::getName() const
{
	return getValueOrEmpty("name");
}


std::string OSMRelation::getCode() const
{
	return getValueOrEmpty("ref:INSEE");
}


bool OSMRelation::isAdministrativeBoundary() const
{
	return "administrative" == getValueOrEmpty("boundary");
}


bool OSMRelation::isCityAdministrativeLevel() const
{
	return "8" == getValueOrEmpty("admin_level");
}


AttributesMap 
makeAttributesMap(const XML_Char **attrs) {
   int count = 0;
   AttributesMap attributesMap;
   while (attrs[count]) {
      attributesMap[attrs[count]] = attrs[count+1];
      //std::cerr << attributesMap[attrs[count]] << attrs[count+1] << std::endl;
      count += 2;
   }
   return attributesMap;
}


void createCity(const std::string& cityName, const std::string& cityCode, std::string cityBoundaries)
{
	std::cerr << "Create city " << cityName << ", code = " << cityCode << ", boundaries = " << cityBoundaries << std::endl;
	++createdCitiesCount;
}


void startElement(void* userData, const XML_Char* name, const XML_Char** attrs)
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

}

void endElement(void* userData, const XML_Char* name)
{
	if (!std::strcmp(name, "relation") &&
		currentRelation.isAdministrativeBoundary() && 
		currentRelation.isCityAdministrativeLevel())
	{
		createCity(currentRelation.getName(), currentRelation.getCode(), "");
		inRelation = false;
	}
}

void characters(void* userData, const XML_Char* txt, int txtlen)
{

}


BOOST_AUTO_TEST_CASE (should_parse_five_cities_from_osm_file)
{
	//std::ifstream osmStream("/home/mjambert/workspace/rcsmobility/gitlab/switzerland-tests/robot/resources/data/swiss.osm");
	std::ifstream osmStream("/home/mjambert/workspace/rcsmobility/git/synthese3/server/build/test/61_data_exchange/5_cities.osm");

	long long int count;
   int done, n = 0;
   char buf[1024*1024*4];

   XML_Parser p = XML_ParserCreate(NULL);
   if (!p) {
      throw std::runtime_error("error creating expat parser");
   }
   XML_SetElementHandler(p, startElement, endElement);
   XML_SetCharacterDataHandler(p, characters);

   do {
      osmStream.read(buf, 1024 * 1024 * 4);
      n = osmStream.gcount();
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
   osmStream.close();

   BOOST_CHECK_EQUAL(5, createdCitiesCount);

}

