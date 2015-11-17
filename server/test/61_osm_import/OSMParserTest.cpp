
/** OsmParserTest class implementation.
	@file OsmParserTest.cpp
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

#include "OSMParser.hpp"
#include "OSMEntityHandler.hpp"

#include <boost/tuple/tuple.hpp>
#include <fstream>
#include <iostream>
#include <geos/geom/Geometry.h>

namespace synthese
{
namespace data_exchange
{


class FakeOSMEntityHandler : public OSMEntityHandler
{
public:
	std::vector<boost::tuple<std::string, std::string, geos::geom::Geometry*> > handledCities;

	virtual void handleCity(const std::string& cityName, const std::string& cityCode, geos::geom::Geometry* boundary)
	{
		handledCities.push_back(boost::make_tuple(cityName, cityCode, boundary));
	}
};




BOOST_AUTO_TEST_CASE (should_parse_five_cities_from_osm_file)
{
	std::ifstream osmStream("5_cities.osm");
	FakeOSMEntityHandler fakeOSMEntityHandler;
	OSMParser parser(fakeOSMEntityHandler);
	parser.parse(osmStream);
    osmStream.close();

   BOOST_CHECK_EQUAL(5, fakeOSMEntityHandler.handledCities.size());
   int cityIndex = 0;
   BOOST_CHECK_EQUAL("Hauterive (NE)", fakeOSMEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6454", fakeOSMEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Neuchâtel", fakeOSMEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6458", fakeOSMEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Saint-Blaise", fakeOSMEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6459", fakeOSMEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Valangin", fakeOSMEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6485", fakeOSMEntityHandler.handledCities[cityIndex++].get<1>());

   BOOST_CHECK_EQUAL("Val-de-Ruz", fakeOSMEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6487", fakeOSMEntityHandler.handledCities[cityIndex++].get<1>());

}

BOOST_AUTO_TEST_CASE (should_parse_city_boundary)
{
	FakeOSMEntityHandler fakeOSMEntityHandler;
	std::ifstream osmStream("capendu.osm");
	OSMParser parser(fakeOSMEntityHandler);
	parser.parse(osmStream);
    osmStream.close();

   //BOOST_CHECK_EQUAL(5, fakeEntityHandler.handledCities.size());
   int cityIndex = 0;
   BOOST_CHECK_EQUAL("Hauterive (NE)", fakeOSMEntityHandler.handledCities[cityIndex].get<0>());
   BOOST_CHECK_EQUAL("6454", fakeOSMEntityHandler.handledCities[cityIndex++].get<1>());

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


}
}
