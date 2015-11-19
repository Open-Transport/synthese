
/** OSMCityBoundariesFileFormat class implementation.
	@file OSMCityBoundariesFileFormat.cpp

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

#include "OSMCityBoundariesFileFormat.hpp"
#include "CityTableSync.h"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"
#include "RoadChunkTableSync.h"
#include "CrossingTableSync.hpp"
#include "Road.h"
#include "RoadChunk.h"
#include "RoadPath.hpp"
#include "RoadPlace.h"
#include "Crossing.h"

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
#include <geos/geom/LinearRing.h>
#include <geos/geom/CoordinateSequence.h>


namespace synthese 
{
	using namespace geography;

	namespace util
	{
		template<>
		const string FactorableTemplate<impex::FileFormat, data_exchange::OSMCityBoundariesFileFormat>::FACTORY_KEY("OpenStreetMapCityBoundaries");
	}

	namespace data_exchange
	{

const string OSMCityBoundariesFileFormat::Importer_::PARAMETER_COUNTRY_CODE("country_code");


class OSMCitiesHandler : public OSMEntityHandler
{
	private:
		typedef std::map<std::string, boost::shared_ptr<road::RoadPlace> > _RecentlyCreatedRoadPlaces;
		typedef std::map<unsigned long long int, boost::shared_ptr<road::RoadPlace> > _LinkBetweenWayAndRoadPlaces;
		typedef std::map<unsigned long long int, boost::shared_ptr<road::Crossing> > _CrossingsMap;
		typedef std::map<util::RegistryKeyType, std::vector<road::HouseNumber> > _ChunksAssociatedHouseNumbers;

		const impex::Importer& _importer;

		util::Env& _env;

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

		OSMCitiesHandler(const impex::Importer& importer, util::Env& env):
			_importer(importer),
			_env(env)
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
OSMCitiesHandler::handleCity(
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

	bool cityCodeIsSet = !cityCode.empty() && ("0" != cityCode);
	CityTableSync::SearchResult cities = CityTableSync::Search(
		_env,
		boost::optional<std::string>(), // exactname
		(cityCodeIsSet ? boost::optional<std::string>() : boost::optional<std::string>(normalizedCityName)), // likeName
		(cityCodeIsSet ? boost::optional<std::string>(cityCode) : boost::optional<std::string>()),
		0, 0, true, true,
		util::UP_LINKS_LOAD_LEVEL // code
	);

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
OSMCitiesHandler::_getOrCreateRoadPlace(const OSMId& roadSourceId,
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
			_linkBetweenWayAndRoadPlaces[roadSourceId] = it->second;
			return it->second;
		}
	}

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
OSMCitiesHandler::_collectRoadPlaces(const std::string& roadName)
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
	return roadPlaces;
}


void
OSMCitiesHandler::handleRoad(const OSMId& roadSourceId, 
							 const std::string& name,
							 const road::RoadType& roadType, 
							 boost::shared_ptr<geos::geom::Geometry> path)
{
	boost::shared_ptr<road::Road> road(new road::Road(0, roadType));
	road->set<Key>(road::RoadTableSync::getId());
	_env.getEditableRegistry<road::Road>().add(road);
	road->link(_env);

	BOOST_FOREACH(const City::Registry::value_type& city, _env.getEditableRegistry<City>())
	{
		if (city.second->get<PolygonGeometry>())
		{
			if (city.second->get<PolygonGeometry>()->intersects(path.get()))
			{
				boost::shared_ptr<road::RoadPlace> roadPlace = _getOrCreateRoadPlace(roadSourceId, name, city.second);
				road->get<road::RoadPlace::Vector>().push_back(roadPlace.get());
			}
		}
	}
	_currentRoad = road;
}



void
OSMCitiesHandler::handleCrossing(const OSMId& crossingSourceId, boost::shared_ptr<geos::geom::Point> point)
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
}



void
OSMCitiesHandler::handleRoadChunk(size_t rank, 
								  graph::MetricOffset metricOffset,
								  TrafficDirection trafficDirection,
								  double maxSpeed,
			                      bool isDrivable,
			                      bool isBikable,
			                      bool isWalkable,
								  boost::shared_ptr<geos::geom::LineString> path)
{
	boost::shared_ptr<road::RoadChunk> roadChunk(new road::RoadChunk);
	roadChunk->setRoad(_currentRoad.get());
	roadChunk->setFromCrossing(_currentCrossing.get());
	roadChunk->setRankInPath(rank);
	roadChunk->setMetricOffset(metricOffset);
	roadChunk->setKey(road::RoadChunkTableSync::getId());
	if(path)
	{
		roadChunk->setGeometry(path);
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
OSMCitiesHandler::_updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(road::RoadChunk* chunk) const {
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
OSMCitiesHandler::_projectHouseAndUpdateChunkHouseNumberBounds(
	const HouseNumber& houseNumber,
	const std::vector<road::RoadChunk*>& roadChunks,
	boost::shared_ptr<geos::geom::Point> houseCoord,
	const bool autoUpdatePolicy) const {

	road::HouseNumber num;
	try
	{
		num = boost::lexical_cast<road::HouseNumber>(houseNumber);
	}
	catch(boost::bad_lexical_cast)
	{
		return;
	}

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
	catch(algorithm::EdgeProjector<road::RoadChunk*>::NotFoundException)
	{
	}
}




void
OSMCitiesHandler::handleHouse(const HouseNumber& houseNumber,
							 const std::string& streetName,
							 boost::shared_ptr<geos::geom::Point> point)
{
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
OSMCitiesHandler::handleHouse(const HouseNumber& houseNumber,
							 const OSMId& roadSourceId,
							 boost::shared_ptr<geos::geom::Point> point)
{
	_LinkBetweenWayAndRoadPlaces::iterator itRoadPlace = _linkBetweenWayAndRoadPlaces.find(roadSourceId);
	if (itRoadPlace == _linkBetweenWayAndRoadPlaces.end()) return;
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
OSMCitiesHandler::_toAlphanumericString(
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

















bool OSMCityBoundariesFileFormat::Importer_::_parse(
	const boost::filesystem::path& filePath
) const {
	boost::filesystem::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);
	boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
	if(!file.good())
	{
		_logError("Unable to open file");
		throw std::runtime_error("Unable to open file");
	}
	std::string ext = boost::filesystem::extension(filePath);
	OSMCitiesHandler handler(*this, _env);

	OSMParser parser(*_fileStream, handler, OSMLocale::OSMLocale_CH);
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



OSMCityBoundariesFileFormat::Importer_::Importer_(
	util::Env& env,
	const impex::Import& import,
	impex::ImportLogLevel minLogLevel,
	const std::string& logPath,
	boost::optional<std::ostream&> outputStream,
	util::ParametersMap& pm
):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
	OneFileTypeImporter<OSMCityBoundariesFileFormat>(env, import, minLogLevel, logPath, outputStream, pm)
{}



util::ParametersMap OSMCityBoundariesFileFormat::Importer_::_getParametersMap() const
{
	util::ParametersMap result;

	if(_countryCode)
	{
		result.insert(PARAMETER_COUNTRY_CODE, *_countryCode);
	}

	return result;
}



void OSMCityBoundariesFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
{
	_countryCode = map.getDefault<std::string>(PARAMETER_COUNTRY_CODE, "FR");
}



db::DBTransaction OSMCityBoundariesFileFormat::Importer_::_save() const
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

	return transaction;
}



}
}
