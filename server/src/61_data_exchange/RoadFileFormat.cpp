
/** RoadFileFormat class implementation.
	@file RoadFileFormat.cpp

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

#include "RoadFileFormat.hpp"

#include "CityTableSync.h"
#include "CrossingTableSync.hpp"
#include "Import.hpp"
#include "PublicPlaceEntranceTableSync.hpp"
#include "PublicPlaceTableSync.h"
#include "RoadChunkEdge.hpp"
#include "RoadChunkTableSync.h"
#include "RoadPath.hpp"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"

#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace impex;
	using namespace geography;
	using namespace graph;
	using namespace road;
	using namespace util;
	

	namespace data_exchange
	{
		RoadFileFormat::RoadFileFormat(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm)
		{}



		RoadPlace* RoadFileFormat::_createOrUpdateRoadPlace(
			impex::ImportableTableSync::ObjectBySource<RoadPlaceTableSync>& roadPlaces,
			const std::string& code,
			const std::string& name,
			const geography::City& city
		) const {
			RoadPlace* roadPlace(NULL);

			// Search for a road place linked with the datasource
			if(roadPlaces.contains(code))
			{
				set<RoadPlace*> loadedRoadPlaces(roadPlaces.get(code));
				if(loadedRoadPlaces.size() > 1)
				{
					_logWarning("More than one road place with key "+ code);
				}
				roadPlace = *loadedRoadPlaces.begin();
				_logLoad(
					"Use of existing road place "+ lexical_cast<string>(roadPlace->getKey()) +
						" ("+ roadPlace->getFullName() +")"
				);
			}
			else
			{
				roadPlace = new RoadPlace(
					RoadPlaceTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&*_import.get<DataSource>(), code));
				roadPlace->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<RoadPlace>().add(boost::shared_ptr<RoadPlace>(roadPlace));
				roadPlaces.add(*roadPlace);
				_logCreation(
					"Creation of the road place with key "+ code +" ("+ city.getName() +" "+ name +")"
				);
			}

			// Properties update
			roadPlace->setName(name);
			roadPlace->setCity(const_cast<City*>(&city));

			return roadPlace;
		}



		PublicPlace* RoadFileFormat::_createOrUpdatePublicPlace(
			impex::ImportableTableSync::ObjectBySource<PublicPlaceTableSync>& publicPlaces,
			const std::string& code,
			const std::string& name,
			boost::optional<boost::shared_ptr<geos::geom::Point> > geometry,
			const geography::City& city
		) const {
			PublicPlace* publicPlace(NULL);

			// Search for a public place linked with the datasource
			if(publicPlaces.contains(code))
			{
				set<PublicPlace*> loadedPublicPlaces(publicPlaces.get(code));
				if(loadedPublicPlaces.size() > 1)
				{
					_logWarning(
						"More than one public place with key "+ code
					);
				}
				publicPlace = *loadedPublicPlaces.begin();
				_logLoad(
					"Use of existing public place "+ lexical_cast<string>(publicPlace->getKey()) +" ("+ publicPlace->getFullName() +")"
				);
			}
			else
			{
				publicPlace = new PublicPlace(
					PublicPlaceTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&*_import.get<DataSource>(), code));
				publicPlace->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<PublicPlace>().add(boost::shared_ptr<PublicPlace>(publicPlace));
				publicPlaces.add(*publicPlace);
				_logCreation(
					"Creation of the public place with key "+ code +" ("+ city.getName() +" "+ name +")"
				);
			}

			// Properties update
			publicPlace->setName(name);
			publicPlace->setCity(const_cast<City*>(&city));
			if(geometry)
			{
				publicPlace->set<PointGeometry>(*geometry);
			}

			return publicPlace;
		}



		RoadPlace* RoadFileFormat::_getRoadPlace(
			impex::ImportableTableSync::ObjectBySource<RoadPlaceTableSync>& roadPlaces,
			const std::string& code
		) const {
			// Search for a road place linked with the datasource
			if(roadPlaces.contains(code))
			{
				set<RoadPlace*> loadedRoadPlaces(roadPlaces.get(code));
				if(loadedRoadPlaces.size() > 1)
				{
					_logWarning("More than one road place with key "+ code);
				}
				return *loadedRoadPlaces.begin();
			}
			else
			{
				_logWarning("No road place with key "+ code);
				return NULL;
			}
		}



		Crossing* RoadFileFormat::_createOrUpdateCrossing(
			impex::ImportableTableSync::ObjectBySource<CrossingTableSync>& crossings,
			const std::string& code,
			boost::shared_ptr<geos::geom::Point> geometry
		) const {
			Crossing* crossing(NULL);

			// Search for a road place linked with the datasource
			if(crossings.contains(code))
			{
				set<Crossing*> loadedCrossings(crossings.get(code));
				if(loadedCrossings.size() > 1)
				{
					_logWarning(
						"More than one crossing with key "+ code
					);
				}
				crossing = *loadedCrossings.begin();
				_logLoad(
					"Use of existing crossing "+ lexical_cast<string>(crossing->getKey())
				);
			}
			else
			{
				crossing = new Crossing(
					CrossingTableSync::getId(),
					boost::shared_ptr<Point>()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&*_import.get<DataSource>(), code));
				crossing->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<Crossing>().add(boost::shared_ptr<Crossing>(crossing));
				crossings.add(*crossing);
				_logCreation(
					"Creation of the crossing with key "+ code
				);
			}

			// Properties update
			crossing->setGeometry(geometry);

			return crossing;
		}



		RoadChunk* RoadFileFormat::_addRoadChunk(
			RoadPlace& roadPlace,
			Crossing& startNode,
			Crossing& endNode,
			boost::shared_ptr<geos::geom::LineString> geometry,
			HouseNumberingPolicy rightHouseNumberingPolicy,
			HouseNumberingPolicy leftHouseNumberingPolicy,
			HouseNumberBounds rightHouseNumberBounds,
			HouseNumberBounds leftHouseNumberBounds,
			RoadType roadType
		) const {
			// Declarations
			RoadChunk* result(NULL);

			// Length
			double length(geometry->getLength());

			// Search for an existing road which ends at the left node
			Road* road(NULL);
			double startMetricOffset(0);
			BOOST_FOREACH(Road* croad, roadPlace.getRoads())
			{
				if(croad->getForwardPath().getLastEdge()->getFromVertex() == &startNode)
				{
					road = croad;
					startMetricOffset = croad->getForwardPath().getLastEdge()->getMetricOffset();
					break;
				}
			}
			if(road)
			{
				result = static_cast<RoadChunkEdge*>(
					road->getForwardPath().getLastEdge()
				)->getRoadChunk();

				// Adding geometry to the last chunk
				_setGeometryAndHouses(
					*result,
					geometry,
					rightHouseNumberingPolicy,
					leftHouseNumberingPolicy,
					rightHouseNumberBounds,
					leftHouseNumberBounds
				);

				// Second road chunk creation
				boost::shared_ptr<RoadChunk> secondRoadChunk(new RoadChunk);
				secondRoadChunk->setRoad(road);
				secondRoadChunk->setFromCrossing(&endNode);
				secondRoadChunk->setRankInPath((*(road->getForwardPath().getEdges().end()-1))->getRankInPath() + 1);
				secondRoadChunk->setMetricOffset(startMetricOffset + length);
				secondRoadChunk->setKey(RoadChunkTableSync::getId());
				secondRoadChunk->link(_env);
				_env.getEditableRegistry<RoadChunk>().add(secondRoadChunk);

				// Search for a second existing road which starts at the right node
				Road* road2 = NULL;
				BOOST_FOREACH(Road* croad, roadPlace.getRoads())
				{
					if(	croad->getForwardPath().getEdge(0)->getFromVertex() == &endNode &&
						croad != road
					){
						road2 = croad;
						break;
					}
				}
				// If found, merge the two roads
				if(road2)
				{
					RegistryKeyType lastEdgeId(road->getForwardPath().getLastEdge()->getKey());
					road->merge(*road2);
					_env.getEditableRegistry<RoadChunk>().remove(lastEdgeId);
					_env.getEditableRegistry<Road>().remove(road2->getKey());
				}
			}
			else
			{
				// If not found search for an existing road which begins at the right node
				BOOST_FOREACH(Road* croad, roadPlace.getRoads())
				{
					if(croad->getForwardPath().getEdge(0)->getFromVertex() == &endNode)
					{
						road = croad;
						break;
					}
				}

				if(road)
				{
					// First road chunk creation
					boost::shared_ptr<RoadChunk> firstRoadChunk(new RoadChunk);
					firstRoadChunk->setRoad(road);
					firstRoadChunk->setFromCrossing(&startNode);
					firstRoadChunk->setRankInPath(0);
					firstRoadChunk->setMetricOffset(0);
					firstRoadChunk->setKey(RoadChunkTableSync::getId());
					road->insertRoadChunk(*firstRoadChunk, length, 1);
					_setGeometryAndHouses(
						*firstRoadChunk,
						geometry,
						rightHouseNumberingPolicy,
						leftHouseNumberingPolicy,
						rightHouseNumberBounds,
						leftHouseNumberBounds
					);
					result = firstRoadChunk.get();

					_env.getEditableRegistry<RoadChunk>().add(firstRoadChunk);
				}
				else
				{
					boost::shared_ptr<Road> roadsp(new Road(0, ROAD_TYPE_UNKNOWN));
					roadsp->get<RoadPlace::Vector>().push_back(&roadPlace);
					roadsp->set<Key>(RoadTableSync::getId());
					_env.getEditableRegistry<Road>().add(roadsp);

					// First road chunk
					boost::shared_ptr<RoadChunk> firstRoadChunk(new RoadChunk);
					firstRoadChunk->setRoad(roadsp.get());
					firstRoadChunk->setFromCrossing(&startNode);
					firstRoadChunk->setRankInPath(0);
					firstRoadChunk->setMetricOffset(0);
					firstRoadChunk->setKey(RoadChunkTableSync::getId());
					firstRoadChunk->link(_env);
					_setGeometryAndHouses(
						*firstRoadChunk,
						geometry,
						rightHouseNumberingPolicy,
						leftHouseNumberingPolicy,
						rightHouseNumberBounds,
						leftHouseNumberBounds
					);
					result = firstRoadChunk.get();

					_env.getEditableRegistry<RoadChunk>().add(firstRoadChunk);

					// Second road chunk
					boost::shared_ptr<RoadChunk> secondRoadChunk(new RoadChunk);
					secondRoadChunk->setRoad(roadsp.get());
					secondRoadChunk->setFromCrossing(&endNode);
					secondRoadChunk->setRankInPath(1);
					secondRoadChunk->setMetricOffset(length);
					secondRoadChunk->setKey(RoadChunkTableSync::getId());
					secondRoadChunk->link(_env);
					_env.getEditableRegistry<RoadChunk>().add(secondRoadChunk);
				}
			}

			return result;
		}



		void RoadFileFormat::_setGeometryAndHouses(
			RoadChunk& chunk,
			boost::shared_ptr<geos::geom::LineString> geometry,
			HouseNumberingPolicy rightHouseNumberingPolicy,
			HouseNumberingPolicy leftHouseNumberingPolicy,
			HouseNumberBounds rightHouseNumberBounds,
			HouseNumberBounds leftHouseNumberBounds
		){
			chunk.setGeometry(geometry);
			chunk.setRightHouseNumberBounds(rightHouseNumberBounds);
			chunk.setRightHouseNumberingPolicy(rightHouseNumberingPolicy);
			chunk.setLeftHouseNumberBounds(leftHouseNumberBounds);
			chunk.setLeftHouseNumberingPolicy(leftHouseNumberingPolicy);
		}



		PublicPlaceEntrance* RoadFileFormat::_createOrUpdatePublicPlaceEntrance(
			ImportableTableSync::ObjectBySource<PublicPlaceEntranceTableSync>& publicPlaceEntrances,
			const std::string& code,
			boost::optional<const std::string&> name,
			MetricOffset metricOffset,
			boost::optional<HouseNumber> number,
			RoadChunk& roadChunk,
			PublicPlace& publicPlace
		) const {
			PublicPlaceEntrance* publicPlaceEntrance(NULL);

			// Search for a public place linked with the datasource
			if(publicPlaceEntrances.contains(code))
			{
				set<PublicPlaceEntrance*> loadedPublicPlaceEntrances(publicPlaceEntrances.get(code));
				if(loadedPublicPlaceEntrances.size() > 1)
				{
					_logWarning(
						"More than one public place entrance with key "+ code
					);
				}
				publicPlaceEntrance = *loadedPublicPlaceEntrances.begin();
				_logLoad(
					"Use of existing public place entrance "+ lexical_cast<string>(publicPlaceEntrance->getKey()) +
						" ("+ publicPlaceEntrance->getName() +")"
				);
			}
			else
			{
				publicPlaceEntrance = new PublicPlaceEntrance(
					PublicPlaceEntranceTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&*_import.get<DataSource>(), code));
				publicPlaceEntrance->setDataSourceLinksWithoutRegistration(links);
				_env.getEditableRegistry<PublicPlaceEntrance>().add(boost::shared_ptr<PublicPlaceEntrance>(publicPlaceEntrance));
				publicPlaceEntrances.add(*publicPlaceEntrance);
				stringstream logStream;
				logStream << "Creation of the public place entrance with key " << code << " (" << publicPlace.getFullName();
				if(name)
				{
					logStream << " " << *name;
				}
				logStream << ")";
				_logCreation(logStream.str());
			}

			// Properties update
			if(name)
			{
				publicPlaceEntrance->set<Name>(*name);
			}
			publicPlaceEntrance->set<PublicPlace>(publicPlace);
			publicPlaceEntrance->setRoadChunk(&roadChunk);
			publicPlaceEntrance->setHouseNumber(number);
			publicPlaceEntrance->setMetricOffset(metricOffset);

			return publicPlaceEntrance;
		}
}	}
