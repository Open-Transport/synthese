
/** OSMFileFormat class implementation.
	@file OSMFileFormat.cpp

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

#include "OSMFileFormat.hpp"
#include "City.h"
#include "CityTableSync.h"
#include "Crossing.h"
#include "CrossingTableSync.hpp"
#include "Road.h"
#include "RoadTableSync.h"
#include "RoadChunk.h"
#include "RoadChunkTableSync.h"
#include "RoadPlace.h"
#include "RoadPlaceTableSync.h"
#include "RoadPath.hpp"
#include "DataSource.h"
#include "DataSourceTableSync.h"

#include "FrenchPhoneticString.h"
#include "Import.hpp"
#include "OSMParser.hpp"
#include "OSMLocale.hpp"
#include "OSMEntityHandler.hpp"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/util/TopologyException.h>


namespace synthese 
{
	using namespace geography;

	namespace util
	{
		template<>
		const string FactorableTemplate<impex::FileFormat, data_exchange::OSMFileFormat>::FACTORY_KEY("OSM");
	}

	namespace data_exchange
	{

		const string OSMFileFormat::Importer_::PARAMETER_COUNTRY_CODE("country_code");

		const string OSMFileFormat::Importer_::PARAMETER_PROJECT_HOUSES("project_houses");

		const string OSMFileFormat::Importer_::PARAMETER_DEFAULT_ROAD_PLACE_ID("default_road_place_id");

class OSMFileFormatEntityHandler : public OSMEntityHandler
{
	private:
		typedef std::map<std::string, boost::shared_ptr<road::RoadPlace> > _RecentlyCreatedRoadPlaces;
		typedef std::map<unsigned long long int, boost::shared_ptr<road::RoadPlace> > _LinkBetweenWayAndRoadPlaces;
		typedef std::map<unsigned long long int, boost::shared_ptr<road::Crossing> > _CrossingsMap;
		typedef std::map<util::RegistryKeyType, std::vector<road::HouseNumber> > _ChunksAssociatedHouseNumbers;

		const impex::Importer& _importer;

		util::Env& _env;

		bool _projectHouses;
		util::RegistryKeyType _defaultRoadPlaceId;

		boost::shared_ptr<road::RoadPlace> _getOrCreateRoadPlace(const OSMId& roadSourceId,
										const std::string& roadName, 
										boost::shared_ptr<City> city) const;

		std::vector<boost::shared_ptr<road::RoadPlace> > _collectRoadPlaces(const std::string& roadName);
		void _updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(road::RoadChunk* chunk) const;
		void _projectHouseAndUpdateChunkHouseNumberBounds(const HouseNumber& houseNumber,
														  const std::vector<road::RoadChunk*>& roadChunks,
															boost::shared_ptr<geos::geom::Point> houseCoord,
															const bool autoUpdatePolicy) const;

		std::string _toAlphanumericString(const std::string& input) const;

		mutable _CrossingsMap _crossingsMap;
		mutable _RecentlyCreatedRoadPlaces _recentlyCreatedRoadPlaces;
		mutable _LinkBetweenWayAndRoadPlaces _linkBetweenWayAndRoadPlaces;
		mutable _ChunksAssociatedHouseNumbers _chunksAssociatedHouseNumbers;

		boost::shared_ptr<road::Crossing> _currentCrossing;
		boost::shared_ptr<road::Road> _currentRoad;

	public:

		OSMFileFormatEntityHandler(const impex::Importer& importer, util::Env& env, bool projectHouses, util::RegistryKeyType defaultRoadPlaceId):
			_importer(importer),
			_env(env),
			_projectHouses(projectHouses),
			_defaultRoadPlaceId(defaultRoadPlaceId)
		{ }

		void handleCity(const std::string& cityName, const std::string& cityCode, boost::shared_ptr<geos::geom::Geometry> boundary);

		void handleRoad(const OSMId& roadSourceId, 
						const std::string& name,
						const road::RoadType& roadType, boost::shared_ptr<geos::geom::Geometry> path);

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
						 boost::shared_ptr<geos::geom::Point> point);		

		void handleHouse(const HouseNumber& houseNumber,
						 const OSMId& roadSourceId,
						 boost::shared_ptr<geos::geom::Point> point);

};




void
OSMFileFormatEntityHandler::handleCity(
	const std::string&    cityName,
	const std::string&    cityCode,
	boost::shared_ptr<geos::geom::Geometry> boundary)
{
	_importer._logDebug("Processing city " + cityName);

	boost::shared_ptr<City> city;
	std::string normalizedCityName = boost::to_upper_copy(lexical_matcher::FrenchPhoneticString::to_plain_lower_copy(cityName));
	geos::geom::Polygon* polygonBoundary = NULL;

	if(boundary)
	{
		geos::geom::MultiPolygon* multiPolygonBoundary = dynamic_cast<geos::geom::MultiPolygon*>(boundary.get());

		if(NULL != multiPolygonBoundary)
		{
			size_t polygonCount = multiPolygonBoundary->getNumGeometries();
			_importer._logDebug("City geometry is a multipolygon with " + boost::lexical_cast<std::string>(polygonCount) + " polygons");

			if(1 == polygonCount)
			{
				const geos::geom::Geometry* geometry = multiPolygonBoundary->getGeometryN(0);

				if(NULL != dynamic_cast<const geos::geom::Polygon*>(geometry))
				{
					polygonBoundary = dynamic_cast<geos::geom::Polygon*> (geometry->clone());
				}
			}
		}

		if(NULL == polygonBoundary)
		{
			// This block converts a geometry object into a polygon
			// Note : this should be a multi-polygon instead, because some cities consist of multiple disjoint areas
			geos::geom::CoordinateSequence *cs = boundary->getCoordinates();

			if(0 < cs->size())
			{
				if(!cs->back().equals(cs->front()))
				{
					cs->add(cs->front());
				}
				geos::geom::LinearRing *lr = CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory().createLinearRing(cs);
				polygonBoundary = CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory().createPolygon(lr, NULL);
			}
		}
	}

	bool cityCodeIsSet = !cityCode.empty() && ("0" != cityCode);
	CityTableSync::SearchResult cities = CityTableSync::Search(
		_env,
		boost::optional<std::string>(), // exactname
		(cityCodeIsSet ? boost::optional<std::string>() : boost::optional<std::string>(normalizedCityName)), // likeName
		(cityCodeIsSet ? boost::optional<std::string>(cityCode) : boost::optional<std::string>()),
		0, 0, true, true,
		util::UP_LINKS_LOAD_LEVEL
	);

	if(cities.empty() && cityCodeIsSet)
	{
		// No matching city was found by code, try to find one by name
		cities = CityTableSync::Search(
			_env,
			boost::optional<std::string>(), // exactname
			boost::optional<std::string>(normalizedCityName), // likeName
			boost::optional<std::string>(),
			0, 0, true, true,
			util::UP_LINKS_LOAD_LEVEL
		);
	}

	if(cities.empty())
	{
		// No matching city was found, create a new one
		_importer._logCreation("New city " + normalizedCityName + " (" + cityCode + ")");

		city = boost::shared_ptr<City>(new City);
		city->set<Name>(normalizedCityName);
		city->set<Code>(cityCode);
		city->set<Key>(CityTableSync::getId());

		if(NULL != polygonBoundary)
		{
			city->set<PolygonGeometry>(boost::shared_ptr<geos::geom::Polygon>(polygonBoundary));
		}
		else
		{
			_importer._logWarning("City " + normalizedCityName + " has no geometry");
		}

		// Add the new city to the registry
		_env.getEditableRegistry<City>().add(city);
	}

	else
	{
		// At least one matching city found, update the first one
		city = cities.front();

		// Throw a warning if more than one city found
		if (cities.size() > 1)
		{
			_importer._logWarning("More than one existing city " + city->get<Name>());
		}

		_importer._logLoad("Updating city " + city->get<Name>());


		if(NULL != polygonBoundary)
		{
			city->set<PolygonGeometry>(boost::shared_ptr<geos::geom::Polygon>(polygonBoundary));
		}
		else
		{
			_importer._logWarning("City " + normalizedCityName + " has no geometry");
		}
	}
}



boost::shared_ptr<road::RoadPlace>
OSMFileFormatEntityHandler::_getOrCreateRoadPlace(const OSMId& roadSourceId,
										const std::string& roadName, 
										boost::shared_ptr<City> city) const {
	string plainRoadName = _toAlphanumericString(roadName);

	// Search for a recently created road place
	if(!roadName.empty())
	{
		_RecentlyCreatedRoadPlaces::iterator it(
			_recentlyCreatedRoadPlaces.find(city->getName() + string(" ") + plainRoadName));
		if(it != _recentlyCreatedRoadPlaces.end())
		{
			_importer._logInfo("Found matching road place " + city->getName() + " " + roadName + " for OSM road #" + boost::lexical_cast<std::string>(roadSourceId));
			_linkBetweenWayAndRoadPlaces[roadSourceId] = it->second;
			return it->second;
		}
	}

	_importer._logCreation("Creating new road place " + city->getName() + "/" + roadName + " for OSM road #" + boost::lexical_cast<std::string>(roadSourceId));

	boost::shared_ptr<road::RoadPlace> roadPlace;
	roadPlace = boost::shared_ptr<road::RoadPlace>(new road::RoadPlace);
	roadPlace->setCity(city.get());
	roadPlace->setName(roadName);
	roadPlace->setKey(road::RoadPlaceTableSync::getId());
	_env.getEditableRegistry<road::RoadPlace>().add(roadPlace);
	if (!roadName.empty())
	{
		_recentlyCreatedRoadPlaces[city->getName() + string(" ") + plainRoadName] = roadPlace;
	}
	_linkBetweenWayAndRoadPlaces[roadSourceId] = roadPlace;
	return roadPlace;
}


std::vector<boost::shared_ptr<road::RoadPlace> >
OSMFileFormatEntityHandler::_collectRoadPlaces(const std::string& roadName)
{
	std::vector<boost::shared_ptr<road::RoadPlace> > roadPlaces;
	std::string plainRoadName = _toAlphanumericString(roadName);
	BOOST_FOREACH(const road::RoadPlace::Registry::value_type& roadPlace, _env.getEditableRegistry<road::RoadPlace>())
	{
		std::string plainRoadPlaceName = _toAlphanumericString(roadPlace.second->getName());
		if (plainRoadPlaceName == plainRoadName)
		{
			roadPlaces.push_back(roadPlace.second);
		}
	}

	_importer._logDebug("Found " + boost::lexical_cast<std::string>(roadPlaces.size()) + " road places matching name " + roadName);

	return roadPlaces;
}


void
OSMFileFormatEntityHandler::handleRoad(const OSMId& roadSourceId,
							 const std::string& name,
							 const road::RoadType& roadType, 
							 boost::shared_ptr<geos::geom::Geometry> path)
{
	boost::shared_ptr<road::Road> road(new road::Road(0, roadType));
	road->set<Key>(road::RoadTableSync::getId());

	bool intersectsAnyCity = false;
	BOOST_FOREACH(const City::Registry::value_type& city, _env.getEditableRegistry<City>())
	{
		// Skip cities without geometry
		if(!city.second->get<PolygonGeometry>()) continue;

		bool roadIntersectsCity = false;

		try
		{
			roadIntersectsCity = city.second->get<PolygonGeometry>()->intersects(path.get());
		}

		catch(const geos::util::TopologyException& te)
		{
			_importer._logWarning("Failed to check intersection between road " + name + " and city " + city.second->getName() + ", reason : " + te.what());
		}

		if (roadIntersectsCity)
		{
			intersectsAnyCity = true;
			boost::shared_ptr<road::RoadPlace> roadPlace = _getOrCreateRoadPlace(roadSourceId, name, city.second);
			road->get<road::RoadPlace::Vector>().push_back(roadPlace.get());
		}
	}
	// do not consider orphan roads (ie without any road place)
	if (!intersectsAnyCity && (_defaultRoadPlaceId == 0)) 
	{
		_importer._logCreation("Road " + boost::lexical_cast<std::string>(road->getKey())
						   + " is not imported because it does not intersect any city boundary");
		_currentRoad.reset();
		return;
	}

	if (!intersectsAnyCity)
	{
		boost::shared_ptr<road::RoadPlace> defaultRoadPlace(
			util::Env::GetOfficialEnv().getEditable<road::RoadPlace>(_defaultRoadPlaceId));
		road->get<road::RoadPlace::Vector>().push_back(defaultRoadPlace.get());
	}

	_importer._logCreation("Creating new road " + boost::lexical_cast<std::string>(road->getKey())
						   + " for OSM road #" + boost::lexical_cast<std::string>(roadSourceId));

	_env.getEditableRegistry<road::Road>().add(road);
	road->link(_env);
	_currentRoad = road;
}



void
OSMFileFormatEntityHandler::handleCrossing(const OSMId& crossingSourceId, boost::shared_ptr<geos::geom::Point> point)
{
	_CrossingsMap::const_iterator it = _crossingsMap.find(crossingSourceId);
	if(it != _crossingsMap.end()) {
		_currentCrossing = it->second;
		return;
	}

	boost::shared_ptr<road::Crossing> crossing(
		new road::Crossing(
			road::CrossingTableSync::getId(),
			point
	)	);
	impex::Importable::DataSourceLinks links;
	links.insert(make_pair(&(*_importer.getImport().get<impex::DataSource>()), 
		boost::lexical_cast<string>(crossingSourceId)));
	crossing->setDataSourceLinksWithoutRegistration(links);

	_crossingsMap[crossingSourceId] = crossing;
	_env.getEditableRegistry<road::Crossing>().add(crossing);
	_currentCrossing = crossing;

	_importer._logCreation("Creating new crossing " + boost::lexical_cast<std::string>(crossing->getKey())
						   + " for OSM crossing #" + boost::lexical_cast<std::string>(crossingSourceId));
}



void
OSMFileFormatEntityHandler::handleRoadChunk(size_t rank,
								  graph::MetricOffset metricOffset,
								  TrafficDirection trafficDirection,
								  double maxSpeed,
			                      bool isDrivable,
			                      bool isBikable,
			                      bool isWalkable,
								  boost::shared_ptr<geos::geom::LineString> path)
{
	if (!_currentRoad.get()) return;

	boost::shared_ptr<road::RoadChunk> roadChunk(new road::RoadChunk);
	roadChunk->setRoad(_currentRoad.get());
	roadChunk->setFromCrossing(_currentCrossing.get());
	roadChunk->setRankInPath(rank);
	roadChunk->setMetricOffset(metricOffset);
	roadChunk->setKey(road::RoadChunkTableSync::getId());

	if(path.get() != NULL)
	{
		roadChunk->set<LineStringGeometry>(path);
	}

	roadChunk->setNonDrivable(!isDrivable);
	roadChunk->setNonBikable(!isBikable);
	roadChunk->setNonWalkable(!isWalkable);

	if (trafficDirection == ONE_WAY)
	{
		roadChunk->setCarOneWay(1);
	}
	else if (trafficDirection == REVERSED_ONE_WAY)
	{
		roadChunk->setCarOneWay(-1);
	}
	roadChunk->setCarSpeed(maxSpeed / 3.6);
	roadChunk->link(_env);
	_env.getEditableRegistry<road::RoadChunk>().add(roadChunk);
}


void 
OSMFileFormatEntityHandler::_updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(road::RoadChunk* chunk) const {
	_ChunksAssociatedHouseNumbers::iterator itChunk = _chunksAssociatedHouseNumbers.find(chunk->getKey());
	if(itChunk == _chunksAssociatedHouseNumbers.end()) return;

	road::HouseNumberingPolicy policy(road::ALL_NUMBERS);
	if(itChunk->second.size() > 2)
	{
		unsigned int curMod(*(itChunk->second.begin()) % 2);
		bool multipleMod(false);

		BOOST_FOREACH(road::HouseNumber n, itChunk->second)
		{
			if(curMod != (n % 2))
			{
				multipleMod = true;
				break;
			}
		}

		if(!multipleMod && curMod)
		{
			policy = road::ODD_NUMBERS;
		}
		else if(!multipleMod)
		{
			policy = road::EVEN_NUMBERS;
		}
	}

	chunk->setLeftHouseNumberingPolicy(policy);
	chunk->setRightHouseNumberingPolicy(policy);
}


void 
OSMFileFormatEntityHandler::_projectHouseAndUpdateChunkHouseNumberBounds(
	const HouseNumber& houseNumber,
	const std::vector<road::RoadChunk*>& roadChunks,
	boost::shared_ptr<geos::geom::Point> houseCoord,
	const bool autoUpdatePolicy) const {
	road::HouseNumber num(0);

	if(roadChunks.empty())
	{
		_importer._logWarning("No road chunk found to project OSM house number " + houseNumber);
		return;
	}

	try
	{
		num = boost::lexical_cast<road::HouseNumber>(houseNumber);
	}
	catch(boost::bad_lexical_cast)
	{
		_importer._logWarning("Cannot convert OSM house number " + houseNumber + " into a valid house number");
		return;
	}

	_importer._logDebug("Projecting OSM house number " + houseNumber + " on " + boost::lexical_cast<std::string>(roadChunks.size()) + " road chunks");

	try
	{
		// Use Projector to get the closest road chunk according to the geometry 
		algorithm::EdgeProjector<road::RoadChunk*> projector(roadChunks, 200);
		algorithm::EdgeProjector<road::RoadChunk*>::PathNearby projection(
			projector.projectEdge(*houseCoord->getCoordinate()));

		road::RoadChunk* linkedRoadChunk(projection.get<1>());
		_chunksAssociatedHouseNumbers[linkedRoadChunk->getKey()].push_back(num);
		road::HouseNumberBounds leftBounds = linkedRoadChunk->getLeftHouseNumberBounds();

		// If we haven't set any bounds, we set a default one
		if(!leftBounds)
		{
			road::HouseNumberBounds bounds(std::make_pair(num, num));
			linkedRoadChunk->setLeftHouseNumberBounds(bounds);
			linkedRoadChunk->setRightHouseNumberBounds(bounds);
		}
		else
		{
			// If there is one and the lower bounds is higher than the current house number, update
			if(num < leftBounds->first)
			{
				road::HouseNumberBounds bounds(std::make_pair(num, leftBounds->second));
				linkedRoadChunk->setLeftHouseNumberBounds(bounds);
				linkedRoadChunk->setRightHouseNumberBounds(bounds);
			}
			// Or the upper bounds is lower than the current house number, update
			else if(num > leftBounds->second)
			{
				road::HouseNumberBounds bounds(std::make_pair(leftBounds->first, num));
				linkedRoadChunk->setLeftHouseNumberBounds(bounds);
				linkedRoadChunk->setRightHouseNumberBounds(bounds);
			}
		}

		if (autoUpdatePolicy) 
		{
			_updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(linkedRoadChunk);
		}
	}
	catch(const algorithm::EdgeProjector<road::RoadChunk*>::NotFoundException& nfe)
	{
		_importer._logWarning("Projecting OSM house number " + houseNumber + " failed, reason : " + nfe.what());
	}
}


void
OSMFileFormatEntityHandler::handleHouse(const HouseNumber& houseNumber,
							 const std::string& streetName,
							 boost::shared_ptr<geos::geom::Point> point)
{
	if(false == _projectHouses) return;

	std::vector<boost::shared_ptr<road::RoadPlace> > roadPlaces = _collectRoadPlaces(streetName);
	std::vector<road::RoadChunk*> roadChunks;
	BOOST_FOREACH(boost::shared_ptr<road::RoadPlace> roadPlace, roadPlaces)
	{
		BOOST_FOREACH(road::Road* path, roadPlace->getRoads())
		{
			BOOST_FOREACH(graph::Edge* edge, path->getForwardPath().getEdges())
			{
				roadChunks.push_back(static_cast<road::RoadChunkEdge*>(edge)->getRoadChunk());
			}
		}
	}

	_projectHouseAndUpdateChunkHouseNumberBounds(houseNumber, roadChunks, point, true);
}


void
OSMFileFormatEntityHandler::handleHouse(const HouseNumber& houseNumber,
							 const OSMId& roadSourceId,
							 boost::shared_ptr<geos::geom::Point> point)
{
	if(false == _projectHouses) return;

	_LinkBetweenWayAndRoadPlaces::iterator itRoadPlace = _linkBetweenWayAndRoadPlaces.find(roadSourceId);
	if (itRoadPlace == _linkBetweenWayAndRoadPlaces.end())
	{
		_importer._logWarning("House " + houseNumber + " is located on OSM road #"
							  + boost::lexical_cast<std::string>(roadSourceId) + " which does not belong to a road place");
		return;
	}

	std::vector<road::RoadChunk*> roadChunks;
	BOOST_FOREACH(road::Road* path, itRoadPlace->second->getRoads())
	{
		BOOST_FOREACH(graph::Edge* edge, path->getForwardPath().getEdges())
		{
			roadChunks.push_back(static_cast<road::RoadChunkEdge*>(edge)->getRoadChunk());
		}
	}

	_projectHouseAndUpdateChunkHouseNumberBounds(houseNumber, roadChunks, point, true);
}


std::string 
OSMFileFormatEntityHandler::_toAlphanumericString(
	const std::string& input
) const {
	string lowerInput(lexical_matcher::FrenchPhoneticString::to_plain_lower_copy(input));
	std::stringstream output;

	boost::char_separator<char> sep(" :,;.-_|/\\¦'°");
	boost::tokenizer<boost::char_separator<char> > words(lowerInput, sep);
	BOOST_FOREACH(string source, words)
	{
		std::string curWord(source);

		if(source == "10")
			curWord = "dix";
		else if(source == "11")
			curWord = "onze";
		else if(source == "12")
			curWord = "douze";
		else if(source == "13")
			curWord = "treize";
		else if(source == "14")
			curWord = "quatorze";
		else if(source == "15")
			curWord = "quinze";
		else if(source == "16")
			curWord = "seize";
		else if(source == "17")
			curWord = "dix sept";
		else if(source == "18")
			curWord = "dix huit";
		else if(source == "19")
			curWord = "dix neuf";
		else if(source == "20")
			curWord = "vingt";
		else if(source == "st")
			curWord = "saint";
		else if(source == "ste")
			curWord = "sainte";
		else if(source == "pl")
			curWord = "place";
		else if(source == "av")
			curWord = "avenue";
		else if(source == "imp")
			curWord = "impasse";
		else if(source == "bd")
			curWord = "boulevard";
		else if(source == "fg")
			curWord = "faubourg";
		else if(source == "che")
			curWord = "chemin";
		else if(source == "rte")
			curWord = "route";
		else if(source == "rpt")
			curWord = "rond point";
		else if(source == "dr")
			curWord = "docteur";
		else if(source == "pr")
			curWord = "professeur";
		else if(source == "cdt" || source == "cmdt")
			curWord = "commandant";
		else if(source == "chu" || source == "chr")
			curWord = "hopital";
		else if(source == "fac" || source == "faculte")
			curWord = "universite";
		else if(
			source == "a" ||
			source == "au" ||
			source == "d" ||
			source == "de" ||
			source == "des" ||
			source == "du" ||
			source == "en" ||
			source == "et" ||
			source == "l" ||
			source == "la" ||
			source == "le" ||
			source == "les" ||
			source == "un"
		)
			curWord = string();

		if(curWord.empty())
			continue;
		else
		{
			if(!output.str().empty())
				output << " ";
			output << curWord;
		}
	}

	return output.str();
}


bool OSMFileFormat::Importer_::_parse(
	const boost::filesystem::path& filePath
) const {
	impex::DataSource& dataSource(*_import.get<impex::DataSource>());
	boost::filesystem::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	if(!file.good())
	{
		_logError("Unable to open file");
		throw std::runtime_error("Unable to open file");
	}
	std::string ext = boost::filesystem::extension(filePath);
	OSMFileFormatEntityHandler handler(*this, _env, _projectHouses, _defaultRoadPlaceId);

	OSMParser parser(
		*_fileStream,
		dataSource.getActualCoordinateSystem(),
		handler,
		OSMLocale::getInstance(*_countryCode)
	);

	if(ext == ".bz2")
	{
		in.push(boost::iostreams::bzip2_decompressor());
		in.push(file);
		std::istream data(&in);
		parser.parse(data);
	}
	else
	{
		parser.parse(file);
	}

	_logDebug("Finished work on city boundaries");

	return true;
}



OSMFileFormat::Importer_::Importer_(
	util::Env& env,
	const impex::Import& import,
	impex::ImportLogLevel minLogLevel,
	const std::string& logPath,
	boost::optional<std::ostream&> outputStream,
	util::ParametersMap& pm
):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
	OneFileTypeImporter<OSMFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
	_projectHouses(false),
	_defaultRoadPlaceId(0)
{}



util::ParametersMap OSMFileFormat::Importer_::_getParametersMap() const
{
	util::ParametersMap result;

	if(_countryCode)
	{
		result.insert(PARAMETER_COUNTRY_CODE, *_countryCode);
	}

	result.insert(PARAMETER_PROJECT_HOUSES, _projectHouses);

	return result;
}



void OSMFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
{
	_countryCode = map.getDefault<std::string>(PARAMETER_COUNTRY_CODE, "FR");
	_projectHouses = map.isTrue(PARAMETER_PROJECT_HOUSES);
	_defaultRoadPlaceId = map.getDefault<util::RegistryKeyType>(PARAMETER_DEFAULT_ROAD_PLACE_ID, 0);
}



db::DBTransaction OSMFileFormat::Importer_::_save() const
{
	db::DBTransaction transaction;
	BOOST_FOREACH(const util::Registry<geography::City>::value_type& city, _env.getEditableRegistry<geography::City>())
	{
		geography::CityTableSync::Save(city.second.get(), transaction);
	}
	BOOST_FOREACH(const util::Registry<road::Road>::value_type& road, _env.getEditableRegistry<road::Road>())
	{
		road::RoadTableSync::Save(road.second.get(), transaction);
	}
	BOOST_FOREACH(const util::Registry<road::RoadPlace>::value_type& roadPlace, _env.getEditableRegistry<road::RoadPlace>())
	{
		road::RoadPlaceTableSync::Save(roadPlace.second.get(), transaction);
	}
	BOOST_FOREACH(const util::Registry<road::RoadChunk>::value_type& roadChunk, _env.getEditableRegistry<road::RoadChunk>())
	{
		road::RoadChunkTableSync::Save(roadChunk.second.get(), transaction);
	}
	BOOST_FOREACH(const util::Registry<road::Crossing>::value_type& crossing, _env.getEditableRegistry<road::Crossing>())
	{
		road::CrossingTableSync::Save(crossing.second.get(), transaction);
	}

	_env.getEditableRegistry<geography::City>().clear();
	_env.getEditableRegistry<road::Road>().clear();
	_env.getEditableRegistry<road::RoadPlace>().clear();
	_env.getEditableRegistry<road::RoadChunk>().clear();
	_env.getEditableRegistry<road::Crossing>().clear();

	return transaction;
}



}
}
