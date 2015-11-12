
/** OpenStreetMapFileFormat class implementation.
	@file OpenStreetMapFileFormat.cpp

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

#include "OSMExpatParser.h"
#include "OpenStreetMapFileFormat.hpp"

#include "AdminFunctionRequest.hpp"
#include "AllowedUseRule.h"
#include "CityTableSync.h"
#include "Crossing.h"
#include "CrossingTableSync.hpp"
#include "DataSource.h"
#include "DataSourceAdmin.h"
#include "EdgeProjector.hpp"
#include "ForbiddenUseRule.h"
#include "FrenchPhoneticString.h"
#include "Import.hpp"
#include "PropertiesHTMLTable.h"
#include "RoadPath.hpp"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"
#include "RoadChunkEdge.hpp"
#include "RoadChunkTableSync.h"
#include "StopAreaTableSync.hpp"

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/lexical_cast.hpp>

#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/prep/PreparedGeometry.h>
#include <geos/operation/distance/DistanceOp.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::operation;
using namespace geos::geom::prep;

namespace synthese 
{
	using namespace admin;
	using namespace algorithm;
	using namespace data_exchange;
	using namespace db;
	using namespace geography;
	using namespace graph;
	using namespace impex;
	using namespace osm;
	using namespace road;
	using namespace util;

	namespace util
	{
		template<>
		const string FactorableTemplate<FileFormat, OpenStreetMapFileFormat>::FACTORY_KEY("OpenStreetMap");
	}

	namespace data_exchange
	{
		const string OpenStreetMapFileFormat::Importer_::PARAMETER_ADD_CENTRAL_CHUNK_REFERENCE("add_central_chunk_reference");



		bool OpenStreetMapFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath
		) const {

			DataSource& dataSource(*_import.get<DataSource>());

			NetworkPtr network;
			boost::filesystem::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);
			boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
			if(!file.good())
			{
				_logError("Unable to open file");
				throw std::runtime_error("unable to open file");
			}
			std::string ext = boost::filesystem::extension(filePath);
			ExpatParser parser;
			if(ext == ".bz2")
			{
				in.push(boost::iostreams::bzip2_decompressor());
				in.push(file);
				std::istream data(&in);
				network = parser.parse(data);
			}
			else
			{
				network = parser.parse(file);
			}

			network->consolidate(true);

			_logDebug("finished parsing osm xml");

			// TODO: use a typedef
			// FIXME: valgrind shows a leak from here.
			std::map<unsigned long long int, std::pair<RelationPtr, std::map<unsigned long long int, WayPtr> > > waysByBoundaries = network->getWaysByAdminBoundary(8);

			_logDebug("Extracted ways by boundary");

			typedef std::map<unsigned long long int, NodePtr> NodesMap;
			typedef std::vector<std::pair<osm::NodePtr, Point*> > HousesNodesWithGeom;
			HousesNodesWithGeom housesNodesWithGeom;

			BOOST_FOREACH(const NodesMap::value_type& nodePair, *network->getNodes())
			{
				NodePtr node = nodePair.second;

				if(node->hasTag("addr:housenumber") && node->hasTag("addr:street"))
				{
					// Compute the house geometry
					boost::shared_ptr<Point> houseCoord(
						dataSource.getActualCoordinateSystem().createPoint(node->getLon(), node->getLat())
					);
					housesNodesWithGeom.push_back(make_pair(node, static_cast<Point*>(houseCoord->clone())));
				}
			}

			// cities, places and roads
			// TODO: move to osm module.
			typedef pair<unsigned long long int, std::pair<RelationPtr, std::map<unsigned long long int, WayPtr> > > BoundaryType;
			typedef pair<unsigned long long int, double> ClosestWayFromCentroid;

			BOOST_FOREACH(const BoundaryType& boundary_ways, waysByBoundaries)
			{
				// insert city
				// TODO: extract into a _getOrCreateCity method.
				ClosestWayFromCentroid closestWayFromCentroid;
				RelationPtr boundary = boundary_ways.second.first;
				string cityId("0");
				if(boundary->hasTag("ref:INSEE"))
					cityId = boundary->getTag("ref:INSEE");
				Geometry* centroid = boundary->toGeometry()->getCentroid();
				std::string cityCode = cityId;
				std::string cityName = to_upper_copy(lexical_matcher::FrenchPhoneticString::to_plain_lower_copy(boundary->getTag(Element::TAG_NAME)));
				_logDebug("treating ways of boundary " + cityName);
				CityTableSync::SearchResult cities = CityTableSync::Search(
					_env,
					boost::optional<std::string>(), // exactname
					((cityId != "0") ? boost::optional<std::string>() : boost::optional<std::string>(cityName)), // likeName
					((cityId != "0") ? boost::optional<std::string>(cityId) : boost::optional<std::string>()),
					0, 0, true, true,
					util::UP_LINKS_LOAD_LEVEL // code
				);
				boost::shared_ptr<City> city;

				if(cities.empty())
				{
					city = boost::shared_ptr<City>(new City);
					city->set<Name>(cityName);
					city->set<Code>(cityCode);
					city->set<Key>(CityTableSync::getId());
					_env.getEditableRegistry<City>().add(city);
					closestWayFromCentroid = make_pair(0, 9999.9);
				}
				else
				{
					city = cities.front();

					pt::StopAreaTableSync::SearchResult stopAreas = pt::StopAreaTableSync::Search(
						_env,
						optional<RegistryKeyType>(city->getKey()),
						logic::tribool(true),
						optional<string>(),
						optional<string>(),
						optional<string>(),
						true,
						true,
						0,
						0,
						util::UP_LINKS_LOAD_LEVEL
					);

					if(stopAreas.empty())
					{
						closestWayFromCentroid = make_pair(0, 9999.9);
					}
					else
					{
						closestWayFromCentroid = make_pair(0, 0);
					}
				}

				// The Synthese <-> OSM objects mapping is done in the following way:
				// 1:n OSM ways with the same name and on the same city (case and accents insensitive) -> 1 RoadPlace
				// OSM way -> 1 Road
				// 1:n OSM nodes between start/end/intersection node -> 1 RoadChunk

				// insert ways of city
				// TODO: move to osm module
				typedef std::pair<unsigned long long int, WayPtr> WayType;
				BOOST_FOREACH(const WayType& w, boundary_ways.second.second)
				{
					WayPtr way = w.second;
					RoadType wayType = way->getAssociatedRoadType();

					bool nonWalkableWay(!way->isWalkable());
					bool nonDrivableWay(!way->isDrivable());
					bool nonBikableWay(!way->isBikable());

					boost::shared_ptr<RoadPlace> roadPlace = _getOrCreateRoadPlace(way, city);

					// Create Road
					boost::shared_ptr<Road> road(new Road(0, wayType));

					road->get<RoadPlace::Vector>().push_back(roadPlace.get());
					road->set<Key>(RoadTableSync::getId());
					_env.getEditableRegistry<Road>().add(road);
					road->link(_env);
					_recentlyCreatedRoadParts[way->getId()] = road;

					double maxSpeed = way->getAssociatedSpeed();

					TraficDirection traficDirection = TWO_WAYS;
					if(way->hasTag("highway"))
					{
						if(way->getTag("highway") == "motorway")
							traficDirection = ONE_WAY;
						else if(way->getTag("highway") == "motorway_link")
							traficDirection = ONE_WAY;
					}

					if(way->hasTag("oneway"))
					{
						if(way->getTag("oneway") == "yes")
							traficDirection = ONE_WAY;
						else if(way->getTag("oneway") == "true")
							traficDirection = ONE_WAY;
						else if(way->getTag("oneway") == "1")
							traficDirection = ONE_WAY;
						else if(way->getTag("oneway") == "-1")
							traficDirection = REVERSED_ONE_WAY;
						else if(way->getTag("oneway") == "no")
							traficDirection = TWO_WAYS;
						else if(way->getTag("oneway") == "0")
							traficDirection = TWO_WAYS;
						else if(way->getTag("oneway") == "false")
							traficDirection = TWO_WAYS;
					}

					if(way->hasTag("junction") && (way->getTag("junction") == "roundabout"))
						traficDirection = ONE_WAY;

					// Check if the central chunk is allowed for everybody
					if(_addCentralChunkReference && !nonWalkableWay && !nonDrivableWay && !nonBikableWay)
					{
						Geometry* wayCentroid = way->toGeometry()->getCentroid();
						double distance = fabs(distance::DistanceOp::distance(*centroid, *wayCentroid));
						delete wayCentroid;
						if(closestWayFromCentroid.second > distance)
						{
							closestWayFromCentroid = make_pair(way->getId(), distance);
						}
					}

					// TODO: move to OSM module
					typedef std::list<std::pair<unsigned long long int, NodePtr> > NodeList;
					const NodeList* nodes = way->getNodes();

					if(nodes->size() < 2)
					{
						_logWarning("Ignoring way with less than 2 nodes");
						continue;
					}

					const GeometryFactory& geometryFactory(CoordinatesSystem::GetDefaultGeometryFactory());
					boost::shared_ptr<CoordinateSequence> cs(geometryFactory.getCoordinateSequenceFactory()->create(0, 2));
					boost::shared_ptr<Crossing> startCrossing;
					size_t rank(0);
					MetricOffset metricOffset(0);

					int nodeCount(nodes->size());
					int i(0);
					BOOST_FOREACH(const NodeList::value_type& idAndNode, *nodes)
					{
						NodePtr node = idAndNode.second;
						i++;

						boost::shared_ptr<Point> point(CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
							*dataSource.getActualCoordinateSystem().createPoint(
								node->getLon(),
								node->getLat()
						)	)	);

						cs->add(*point->getCoordinate());

						if(!startCrossing.get())
						{
							startCrossing = _getOrCreateCrossing(node, point);
							continue;
						}

						bool isLast = i == nodeCount;
						if(!node->isStop() && node->numConnectedWay() <= 1 && !isLast)
						{
							// Just extend the current geometry.
							continue;
						}

						boost::shared_ptr<LineString> roadChunkLine(geometryFactory.createLineString(*cs));

						_createRoadChunk(road, startCrossing, roadChunkLine, rank, metricOffset, traficDirection, maxSpeed, nonWalkableWay, nonDrivableWay, nonBikableWay);

						metricOffset += roadChunkLine->getLength();
						startCrossing = _getOrCreateCrossing(node, point);

						if(!isLast)
						{
							cs.reset(geometryFactory.getCoordinateSequenceFactory()->create(0, 2));
							cs->add(*point->getCoordinate());
						}
						++rank;
					}

					// Add last road chunk.
					_createRoadChunk(road, startCrossing, optional<boost::shared_ptr<LineString> >(), rank, metricOffset, traficDirection, maxSpeed, nonWalkableWay, nonDrivableWay, nonBikableWay);
				}

				const PreparedGeometry* cityGeom = boundary->toPreparedGeometry().get();
				BOOST_FOREACH(HousesNodesWithGeom::value_type& nodePair, housesNodesWithGeom)
				{
					NodePtr node = nodePair.first;
					if(!cityGeom->contains(nodePair.second))
					{
						continue;
					}

					_RecentlyCreatedRoadPlaces::iterator it(_recentlyCreatedRoadPlaces.find(cityName + string(" ") + _toAlphanumericString(node->getTag("addr:street"))));
					if(it != _recentlyCreatedRoadPlaces.end())
					{
						boost::shared_ptr<RoadPlace> refRoadPlace;
						refRoadPlace = it->second;
						std::vector<RoadChunk*> refRoadChunks;

						// Get every road chunk of the RoadPlace
						BOOST_FOREACH(Road* path, refRoadPlace->getRoads())
						{
							BOOST_FOREACH(Edge* edge, path->getForwardPath().getEdges())
							{
								refRoadChunks.push_back(static_cast<RoadChunkEdge*>(edge)->getRoadChunk());
							}
						}

						_projectHouseAndUpdateChunkHouseNumberBounds(node, refRoadChunks, true);
					}
				}

				if(_addCentralChunkReference && closestWayFromCentroid.first)
				{
					_RecentlyCreatedRoadParts::iterator centralRoad = _recentlyCreatedRoadParts.find(closestWayFromCentroid.first);

					if(centralRoad != _recentlyCreatedRoadParts.end())
					{
						city->addIncludedPlace(static_cast<NamedPlace&>(*centralRoad->second->getAnyRoadPlace()));
					}
				}
				delete centroid;
			}

			_logDebug("finished inserting road network");

			typedef std::map<unsigned long long int, RelationPtr> ChunkRelations;

			// Loop over relations
			BOOST_FOREACH(ChunkRelations::value_type rel, *(network->getRelations()))
			{
				// If it's a restriction and if it's having a restriction tag
				if(rel.second->hasTag("type") && (rel.second->getTag("type") == "restriction") && rel.second->hasTag("restriction"))
				{
					string tag(rel.second->getTag("restriction"));

					// Get standard from / via / to elements of the restriction
					string role("via");
					list<NodePtr> viaList = rel.second->getNodes(role);
					role = string("from");
					list<WayPtr> fromList = rel.second->getWays(role);
					role = string("to");
					list<WayPtr> toList = rel.second->getWays(role);

					// If we have all three of these
					if(!(viaList.empty() || fromList.empty() || toList.empty()))
					{
						// Trying to find SYNTHESE objects created above
						_CrossingsMap::iterator via = _crossingsMap.find(viaList.front()->getId());
						_RecentlyCreatedRoadParts::iterator from = _recentlyCreatedRoadParts.find(fromList.front()->getId());
						_RecentlyCreatedRoadParts::iterator to = _recentlyCreatedRoadParts.find(toList.front()->getId());

						// If we find them
						if(via != _crossingsMap.end() && from != _recentlyCreatedRoadParts.end() && to != _recentlyCreatedRoadParts.end())
						{
							// If it's a "simple" restriction, mark the road pair as unreachable in the crossing
							if((tag == "no_left_turn") || (tag == "no_right_turn") || (tag == "no_straight_on") || (tag == "no_u_turn"))
							{
								via->second->addNonReachableRoad(make_pair(from->second.get(), to->second.get()));
							}
							// If it's a "only" restriction, run through every ways connected to the "via" node.
							else if((tag == "only_right_turn") || (tag == "only_left_turn") || (tag == "only_straight_on"))
							{
								NodePtr intersection = network->getNode(viaList.front()->getId());

/*								BOOST_FOREACH(Way* curWay, intersection->getWays())
								{
									// If it's not the "to" way of the "only" restriction, mark the road pair as unreachable in the crossing
									if(curWay->getId() != toList.front()->getId())
									{
										_RecentlyCreatedRoadParts::iterator toRestrict = _recentlyCreatedRoadParts.find(curWay->getId());
										if(toRestrict != _recentlyCreatedRoadParts.end())
										{
											via->second->addNonReachableRoad(make_pair(from->second.get(), toRestrict->second.get()));	
										}
									}
								}
								*/
							}
						}
					}
				}
				// If it's an associatedStreet (relation between one or many ways and nodes which are house numbers)
				else if(rel.second->hasTag("type") && (rel.second->getTag("type") == "associatedStreet"))
				{
					// Get all the ways of the relation
					string role("street");
					list<WayPtr> waysList = rel.second->getWays(role);

					// If there is at least one
					if(!waysList.empty())
					{
						role = string("house");
						list<NodePtr> housesList = rel.second->getNodes(role);

						if(!housesList.empty())
						{
							boost::shared_ptr<RoadPlace> refRoadPlace;

							BOOST_FOREACH(WayPtr curWay, waysList)
							{
								// If we find a road place linked to this way
								_LinkBetweenWayAndRoadPlaces::iterator itWay(_linkBetweenWayAndRoadPlaces.find(curWay->getId()));
								if(itWay != _linkBetweenWayAndRoadPlaces.end())
								{
									refRoadPlace = itWay->second;
									break;
								}
							}

							if(refRoadPlace.get())
							{
								std::vector<RoadChunk*> refRoadChunks;

								// Get every road chunk of the RoadPlace
								BOOST_FOREACH(Road* path, refRoadPlace->getRoads())
								{
									BOOST_FOREACH(Edge* edge, path->getForwardPath().getEdges())
									{
										refRoadChunks.push_back(static_cast<RoadChunkEdge*>(edge)->getRoadChunk());
									}
								}

								// Get all the houses
								BOOST_FOREACH(NodePtr house, housesList)
								{
									if(house->hasTag("addr:housenumber"))
									{
										_projectHouseAndUpdateChunkHouseNumberBounds(house, refRoadChunks, false);
									}
								}

								BOOST_FOREACH(RoadChunk* chunk, refRoadChunks)
								{
									_updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(chunk);
								}
							}
						}

						role = string("sidewalk");
						list<WayPtr> sidewalkList = rel.second->getWays(role);

						if(!sidewalkList.empty())
						{
							boost::shared_ptr<RoadPlace> refRoadPlace;

							BOOST_FOREACH(WayPtr curWay, waysList)
							{
								// If we find a road place linked to this way with a name
								_LinkBetweenWayAndRoadPlaces::iterator itWay(_linkBetweenWayAndRoadPlaces.find(curWay->getId()));
								if(itWay != _linkBetweenWayAndRoadPlaces.end() && !itWay->second->getName().empty())
								{
									refRoadPlace = itWay->second;
									break;
								}
							}

							if(refRoadPlace.get())
							{
								BOOST_FOREACH(WayPtr curWay, sidewalkList)
								{
									if(!curWay->hasTag("name"))
									{
										_LinkBetweenWayAndRoadPlaces::iterator itWay(_linkBetweenWayAndRoadPlaces.find(curWay->getId()));
										if(itWay != _linkBetweenWayAndRoadPlaces.end())
										{
											itWay->second->setName(refRoadPlace->getName());
										}
									}
								}
							}
						}
					}
				}
			}

			_logDebug("finished parsing relation");

			BOOST_FOREACH(const Registry<RoadChunk>::value_type& chunk, _env.getEditableRegistry<RoadChunk>())
			{
				_reorderHouseNumberingBounds(chunk.second);
			}

			BOOST_FOREACH(const Registry<Road>::value_type& road, _env.getEditableRegistry<Road>())
			{
				road.second->getForwardPath().validateGeometry();
			}

			_logDebug("finished validating road geometries");

			BOOST_FOREACH(HousesNodesWithGeom::value_type& nodePair, housesNodesWithGeom)
			{
				delete nodePair.second;
			}
			housesNodesWithGeom.clear();

			return true;
		}



		OpenStreetMapFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			OneFileTypeImporter<OpenStreetMapFileFormat>(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		util::ParametersMap OpenStreetMapFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result;

			result.insert(PARAMETER_ADD_CENTRAL_CHUNK_REFERENCE, _addCentralChunkReference);

			return result;
		}



		void OpenStreetMapFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			_addCentralChunkReference = map.getDefault<bool>(PARAMETER_ADD_CENTRAL_CHUNK_REFERENCE, false);
		}



		db::DBTransaction OpenStreetMapFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(const Registry<City>::value_type& city, _env.getEditableRegistry<City>())
			{
				CityTableSync::Save(city.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<Crossing>::value_type& crossing, _env.getEditableRegistry<Crossing>())
			{
				CrossingTableSync::Save(crossing.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<RoadPlace>::value_type& roadplace, _env.getEditableRegistry<RoadPlace>())
			{
				RoadPlaceTableSync::Save(roadplace.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<Road>::value_type& road, _env.getEditableRegistry<Road>())
			{
				RoadTableSync::Save(road.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<RoadChunk>::value_type& roadChunk, _env.getEditableRegistry<RoadChunk>())
			{
				RoadChunkTableSync::Save(roadChunk.second.get(), transaction);
			}
			return transaction;
		}



		boost::shared_ptr<RoadPlace> OpenStreetMapFileFormat::Importer_::_getOrCreateRoadPlace(
			WayPtr& way,
			boost::shared_ptr<City> city
		) const {
			string roadName;
			string plainRoadName;

			roadName = way->getName();
			plainRoadName = _toAlphanumericString(roadName);

			// Search for a recently created road place
			if(way->hasTag(Element::TAG_NAME) && !roadName.empty())
			{
				_RecentlyCreatedRoadPlaces::iterator it(_recentlyCreatedRoadPlaces.find(city->getName() + string(" ") + plainRoadName));
				if(it != _recentlyCreatedRoadPlaces.end())
				{
					_linkBetweenWayAndRoadPlaces[way->getId()] = it->second;
					return it->second;
				}
			}

			boost::shared_ptr<RoadPlace> roadPlace;
			roadPlace = boost::shared_ptr<RoadPlace>(new RoadPlace);
			roadPlace->setCity(city.get());
			roadPlace->setName(roadName);
			roadPlace->setKey(RoadPlaceTableSync::getId());
			_env.getEditableRegistry<RoadPlace>().add(roadPlace);
			if(way->hasTag(Element::TAG_NAME) && !roadName.empty())
			{
				_recentlyCreatedRoadPlaces[city->getName() + string(" ") + plainRoadName] = roadPlace;
			}
			_linkBetweenWayAndRoadPlaces[way->getId()] = roadPlace;
			return roadPlace;
		}



		/*
		 * creates or retrieves an existing crossing for a node
		 */
		boost::shared_ptr<Crossing> OpenStreetMapFileFormat::Importer_::_getOrCreateCrossing(
			NodePtr &node,
			boost::shared_ptr<Point> position
		) const {
			_CrossingsMap::const_iterator it = _crossingsMap.find(node->getId());
			if(it != _crossingsMap.end())
			{
				return it->second;
			}

			boost::shared_ptr<Crossing> crossing(
				new Crossing(
					CrossingTableSync::getId(),
					position
			)	);
			Importable::DataSourceLinks links;
			links.insert(make_pair(&(*_import.get<DataSource>()), lexical_cast<string>(node->getId())));
			crossing->setDataSourceLinksWithoutRegistration(links);

			_crossingsMap[node->getId()] = crossing;
			_env.getEditableRegistry<Crossing>().add(crossing);
			return crossing;
		}



		void OpenStreetMapFileFormat::Importer_::_createRoadChunk(
			const boost::shared_ptr<Road> road,
			const boost::shared_ptr<Crossing> crossing,
			const optional<boost::shared_ptr<LineString> > geometry,
			size_t rank,
			MetricOffset metricOffset,
			TraficDirection traficDirection,
			double maxSpeed,
			bool isNonWalkable,
			bool isNonDrivable,
			bool isNonBikable
		) const {
			boost::shared_ptr<RoadChunk> roadChunk(new RoadChunk);
			roadChunk->setRoad(road.get());
			roadChunk->setFromCrossing(crossing.get());
			roadChunk->setRankInPath(rank);
			roadChunk->setMetricOffset(metricOffset);
			roadChunk->setKey(RoadChunkTableSync::getId());
			if(geometry)
			{
				roadChunk->setGeometry(*geometry);
			}

			roadChunk->setNonWalkable(isNonWalkable);
			roadChunk->setNonBikable(isNonBikable);
			roadChunk->setNonDrivable(isNonDrivable);
			if(traficDirection == ONE_WAY)
			{
				roadChunk->setCarOneWay(1);
			}
			else if(traficDirection == REVERSED_ONE_WAY)
			{
				roadChunk->setCarOneWay(-1);
			}

			roadChunk->setCarSpeed(maxSpeed);
			roadChunk->link(_env);
			_env.getEditableRegistry<RoadChunk>().add(roadChunk);
		}



		void OpenStreetMapFileFormat::Importer_::_projectHouseAndUpdateChunkHouseNumberBounds(
			const NodePtr& house,
			vector<RoadChunk*>& refRoadChunks,
			const bool autoUpdatePolicy
		) const {
			try
			{
				HouseNumber num = lexical_cast<HouseNumber>(house->getTag("addr:housenumber"));
				// Compute the house geometry
				boost::shared_ptr<Point> houseCoord(CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
					*_import.get<DataSource>()->getActualCoordinateSystem().createPoint(
						house->getLon(),
						house->getLat()
					)
				));

				try
				{
					// Use Projector to get the closest road chunk according to the geometry 
					EdgeProjector<RoadChunk*> projector(refRoadChunks, 200);
					EdgeProjector<RoadChunk*>::PathNearby projection(projector.projectEdge(*houseCoord->getCoordinate()));
					RoadChunk* linkedRoadChunk(projection.get<1>());

					_chunksAssociatedHousesList[linkedRoadChunk->getKey()].push_back(house);

					HouseNumberBounds leftBounds = linkedRoadChunk->getLeftHouseNumberBounds();

					// If we haven't set any bounds, we set a default one
					if(!leftBounds)
					{
						HouseNumberBounds bounds(make_pair(num, num));
						linkedRoadChunk->setLeftHouseNumberBounds(bounds);
						linkedRoadChunk->setRightHouseNumberBounds(bounds);
					}
					else
					{
						// If there is one and the lower bounds is higher than the current house number, update
						if(num < leftBounds->first)
						{
							HouseNumberBounds bounds(make_pair(num, leftBounds->second));
							linkedRoadChunk->setLeftHouseNumberBounds(bounds);
							linkedRoadChunk->setRightHouseNumberBounds(bounds);
						}
						// Or the upper bounds is lower than the current house number, update
						else if(num > leftBounds->second)
						{
							HouseNumberBounds bounds(make_pair(leftBounds->first, num));
							linkedRoadChunk->setLeftHouseNumberBounds(bounds);
							linkedRoadChunk->setRightHouseNumberBounds(bounds);
						}
					}

					if(autoUpdatePolicy)
					{
						_updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(linkedRoadChunk);
					}
				}
				catch(EdgeProjector<RoadChunk*>::NotFoundException)
				{
				}
			}
			catch(bad_lexical_cast)
			{
			}
		}



		void OpenStreetMapFileFormat::Importer_::_updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(
			RoadChunk* chunk
		) const {
			ChunksAssociatedHousesList::iterator itChunk = _chunksAssociatedHousesList.find(chunk->getKey());

			if(itChunk != _chunksAssociatedHousesList.end())
			{
				HouseNumberingPolicy policy(ALL_NUMBERS);

				if(itChunk->second.size() > 2)
				{
					unsigned int curMod(lexical_cast<HouseNumber>((*(itChunk->second.begin()))->getTag("addr:housenumber")) % 2);
					bool multipleMod(false);

					BOOST_FOREACH(NodePtr n, itChunk->second)
					{
						if(curMod != (lexical_cast<HouseNumber>(n->getTag("addr:housenumber")) % 2))
						{
							multipleMod = true;
							break;
						}
					}

					if(!multipleMod && curMod)
					{
						policy = ODD_NUMBERS;
					}
					else if(!multipleMod)
					{
						policy = EVEN_NUMBERS;
					}
				}

				chunk->setLeftHouseNumberingPolicy(policy);
				chunk->setRightHouseNumberingPolicy(policy);
			}
		}


		void OpenStreetMapFileFormat::Importer_::_reorderHouseNumberingBounds(
			boost::shared_ptr<RoadChunk> chunk
		) const {
			const HouseNumberBounds& leftBounds = chunk->getLeftHouseNumberBounds();
			DataSource& dataSource(*_import.get<DataSource>());

			// Continue if bounds are defined and different, and the chunk has a next chunk
			if(leftBounds && leftBounds->first != leftBounds->second && chunk->getForwardEdge().getNext())
			{
				HouseNumber startBound = leftBounds->first, endBound = leftBounds->second;
				NodePtr startHouse, endHouse;

				// Check if we have found the house number, we should always find it according to the code above
				ChunksAssociatedHousesList::iterator housesVector = _chunksAssociatedHousesList.find(chunk->getKey());
				if(housesVector != _chunksAssociatedHousesList.end())
				{
					BOOST_FOREACH(ChunksAssociatedHousesList::mapped_type::value_type& house, housesVector->second)
					{
						if(lexical_cast<HouseNumber>(house->getTag("addr:housenumber")) == startBound)
						{
							startHouse = house;
						}
						if(lexical_cast<HouseNumber>(house->getTag("addr:housenumber")) == endBound)
						{
							endHouse = house;
						}
					}

					// If we have found our 2 houses and their geometries
					if(startHouse && endHouse)
					{
						boost::shared_ptr<const Geometry> startGeom = chunk->getForwardEdge().getFromVertex()->getGeometry();
						boost::shared_ptr<const Geometry> endGeom = chunk->getForwardEdge().getNext()->getFromVertex()->getGeometry();
						boost::shared_ptr<Point> startHouseGeom(CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
								*dataSource.getActualCoordinateSystem().createPoint(startHouse->getLon(), startHouse->getLat())
						)	);
						boost::shared_ptr<Point> endHouseGeom(CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
								*dataSource.getActualCoordinateSystem().createPoint(endHouse->getLon(), endHouse->getLat())
						)	);

						if(startGeom && endGeom && startHouseGeom && endHouseGeom)
						{
							double distanceStartToStartCrossing = fabs(distance::DistanceOp::distance(*startHouseGeom, *startGeom));
							double distanceEndToStartCrossing = fabs(distance::DistanceOp::distance(*endHouseGeom, *startGeom));
							double distanceStartToEndCrossing = fabs(distance::DistanceOp::distance(*startHouseGeom, *endGeom));
							double distanceEndToEndCrossing = fabs(distance::DistanceOp::distance(*endHouseGeom, *endGeom));

							// If the first house is further away from the start than the last one and the last house is further away from the end than the first one
							if(distanceStartToStartCrossing > distanceEndToStartCrossing && distanceStartToEndCrossing < distanceEndToEndCrossing)
							{
								// Switch bounds order
								HouseNumberBounds newBounds(make_pair(leftBounds->second, leftBounds->first));
								chunk->setLeftHouseNumberBounds(newBounds);
								chunk->setRightHouseNumberBounds(newBounds);
							}
						}
					}
				}
			}
		}


		std::string OpenStreetMapFileFormat::Importer_::_toAlphanumericString(
			const std::string& input
		) const {
			string lowerInput(lexical_matcher::FrenchPhoneticString::to_plain_lower_copy(input));
			stringstream output;

			char_separator<char> sep(" :,;.-_|/\\¦'°");
			tokenizer<char_separator<char> > words(lowerInput, sep);
			BOOST_FOREACH(string source, words)
			{
				string curWord(source);

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
	}
}
