
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
#include "ImportLogger.hpp"
#include "MainRoadPart.hpp"
#include "PublicPlaceEntranceTableSync.hpp"
#include "PublicPlaceTableSync.h"
#include "RoadChunkTableSync.h"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"

#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace impex;
	using namespace graph;
	using namespace util;

	namespace road
	{
		RoadPlace* RoadFileFormat::CreateOrUpdateRoadPlace(
			impex::ImportableTableSync::ObjectBySource<RoadPlaceTableSync>& roadPlaces,
			const std::string& code,
			const std::string& name,
			const geography::City& city,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& logger
		){
			RoadPlace* roadPlace(NULL);

			// Search for a road place linked with the datasource
			if(roadPlaces.contains(code))
			{
				set<RoadPlace*> loadedRoadPlaces(roadPlaces.get(code));
				if(loadedRoadPlaces.size() > 1)
				{
					logger.logWarning("More than one road place with key "+ code);
				}
				roadPlace = *loadedRoadPlaces.begin();
				logger.logLoad(
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
				links.insert(make_pair(&source, code));
				roadPlace->setDataSourceLinksWithoutRegistration(links);
				env.getEditableRegistry<RoadPlace>().add(boost::shared_ptr<RoadPlace>(roadPlace));
				roadPlaces.add(*roadPlace);
				logger.logCreation(
					"Creation of the road place with key "+ code +" ("+ city.getName() +" "+ name +")"
				);
			}

			// Properties update
			roadPlace->setName(name);
			roadPlace->setCity(&city);

			return roadPlace;
		}



		PublicPlace* RoadFileFormat::CreateOrUpdatePublicPlace(
			impex::ImportableTableSync::ObjectBySource<PublicPlaceTableSync>& publicPlaces,
			const std::string& code,
			const std::string& name,
			boost::optional<boost::shared_ptr<geos::geom::Point> > geometry,
			const geography::City& city,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& logger
		){
			PublicPlace* publicPlace(NULL);

			// Search for a public place linked with the datasource
			if(publicPlaces.contains(code))
			{
				set<PublicPlace*> loadedPublicPlaces(publicPlaces.get(code));
				if(loadedPublicPlaces.size() > 1)
				{
					logger.logWarning(
						"More than one public place with key "+ code
					);
				}
				publicPlace = *loadedPublicPlaces.begin();
				logger.logLoad(
					"Use of existing public place "+ lexical_cast<string>(publicPlace->getKey()) +" ("+ publicPlace->getFullName() +")"
				);
			}
			else
			{
				publicPlace = new PublicPlace(
					PublicPlaceTableSync::getId()
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, code));
				publicPlace->setDataSourceLinksWithoutRegistration(links);
				env.getEditableRegistry<PublicPlace>().add(boost::shared_ptr<PublicPlace>(publicPlace));
				publicPlaces.add(*publicPlace);
				logger.logCreation(
					"Creation of the public place with key "+ code +" ("+ city.getName() +" "+ name +")"
				);
			}

			// Properties update
			publicPlace->setName(name);
			publicPlace->setCity(&city);
			if(geometry)
			{
				publicPlace->set<PointGeometry>(*geometry);
			}

			return publicPlace;
		}



		RoadPlace* RoadFileFormat::GetRoadPlace(
			impex::ImportableTableSync::ObjectBySource<RoadPlaceTableSync>& roadPlaces,
			const std::string& code,
			const impex::ImportLogger& logger
		){
			// Search for a road place linked with the datasource
			if(roadPlaces.contains(code))
			{
				set<RoadPlace*> loadedRoadPlaces(roadPlaces.get(code));
				if(loadedRoadPlaces.size() > 1)
				{
					logger.logWarning("More than one road place with key "+ code);
				}
				return *loadedRoadPlaces.begin();
			}
			else
			{
				logger.logWarning(
					"WARN : no road place with key "+ code
				);
				return NULL;
			}
		}



		Crossing* RoadFileFormat::CreateOrUpdateCrossing(
			impex::ImportableTableSync::ObjectBySource<CrossingTableSync>& crossings,
			const std::string& code,
			boost::shared_ptr<geos::geom::Point> geometry,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& logger
		){
			Crossing* crossing(NULL);

			// Search for a road place linked with the datasource
			if(crossings.contains(code))
			{
				set<Crossing*> loadedCrossings(crossings.get(code));
				if(loadedCrossings.size() > 1)
				{
					logger.logWarning(
						"More than one crossing with key "+ code
					);
				}
				crossing = *loadedCrossings.begin();
				logger.logLoad(
					"Use of existing crossing "+ lexical_cast<string>(crossing->getKey())
				);
			}
			else
			{
				crossing = new Crossing(
					CrossingTableSync::getId(),
					boost::shared_ptr<Point>(),
					string(),
					NULL,
					false
				);
				Importable::DataSourceLinks links;
				links.insert(make_pair(&source, code));
				crossing->setDataSourceLinksWithoutRegistration(links);
				env.getEditableRegistry<Crossing>().add(boost::shared_ptr<Crossing>(crossing));
				crossings.add(*crossing);
				logger.logCreation(
					"Creation of the crossing with key "+ code
				);
			}

			// Properties update
			crossing->setGeometry(geometry);

			return crossing;
		}



		MainRoadChunk* RoadFileFormat::AddRoadChunk(
			RoadPlace& roadPlace,
			Crossing& startNode,
			Crossing& endNode,
			boost::shared_ptr<geos::geom::LineString> geometry,
			MainRoadChunk::HouseNumberingPolicy rightHouseNumberingPolicy,
			MainRoadChunk::HouseNumberingPolicy leftHouseNumberingPolicy,
			MainRoadChunk::HouseNumberBounds rightHouseNumberBounds,
			MainRoadChunk::HouseNumberBounds leftHouseNumberBounds,
			util::Env& env,
			Road::RoadType roadType
		){
			// Declarations
			MainRoadChunk* result(NULL);

			// Length
			double length(geometry->getLength());

			// Search for an existing road which ends at the left node
			MainRoadPart* road(NULL);
			double startMetricOffset(0);
			BOOST_FOREACH(Path* croad, roadPlace.getPaths())
			{
				if(!dynamic_cast<MainRoadPart*>(croad))
				{
					continue;
				}
				if(croad->getLastEdge()->getFromVertex() == &startNode)
				{
					road = static_cast<MainRoadPart*>(croad);
					startMetricOffset = croad->getLastEdge()->getMetricOffset();
					break;
				}
			}
			if(road)
			{
				result = static_cast<MainRoadChunk*>(
					road->getLastEdge()
				);

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
				boost::shared_ptr<MainRoadChunk> secondRoadChunk(new MainRoadChunk);
				secondRoadChunk->setRoad(road);
				secondRoadChunk->setFromCrossing(&endNode);
				secondRoadChunk->setRankInPath((*(road->getEdges().end()-1))->getRankInPath() + 1);
				secondRoadChunk->setMetricOffset(startMetricOffset + length);
				secondRoadChunk->setKey(RoadChunkTableSync::getId());
				road->addRoadChunk(*secondRoadChunk);
				env.getEditableRegistry<MainRoadChunk>().add(secondRoadChunk);

				// Search for a second existing road which starts at the right node
				MainRoadPart* road2 = NULL;
				BOOST_FOREACH(Path* croad, roadPlace.getPaths())
				{
					if(!dynamic_cast<MainRoadPart*>(croad))
					{
						continue;
					}
					if(	croad->getEdge(0)->getFromVertex() == &endNode &&
						croad != road
					){
						road2 = static_cast<MainRoadPart*>(croad);
						break;
					}
				}
				// If found, merge the two roads
				if(road2)
				{
					RegistryKeyType lastEdgeId(road->getLastEdge()->getKey());
					road->merge(*road2);
					env.getEditableRegistry<MainRoadChunk>().remove(lastEdgeId);
					env.getEditableRegistry<MainRoadPart>().remove(road2->getKey());
				}
			}
			else
			{
				// If not found search for an existing road which begins at the right node
				BOOST_FOREACH(Path* croad, roadPlace.getPaths())
				{
					if(!dynamic_cast<MainRoadPart*>(croad))
					{
						continue;
					}
					if(croad->getEdge(0)->getFromVertex() == &endNode)
					{
						road = static_cast<MainRoadPart*>(croad);
						break;
					}
				}

				if(road)
				{
					// First road chunk creation
					boost::shared_ptr<MainRoadChunk> firstRoadChunk(new MainRoadChunk);
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

					env.getEditableRegistry<MainRoadChunk>().add(firstRoadChunk);
				}
				else
				{
					boost::shared_ptr<MainRoadPart> road(new MainRoadPart(0, Road::ROAD_TYPE_UNKNOWN));
					road->setRoadPlace(roadPlace);
					road->setKey(RoadTableSync::getId());
					env.getEditableRegistry<MainRoadPart>().add(road);

					// First road chunk
					boost::shared_ptr<MainRoadChunk> firstRoadChunk(new MainRoadChunk);
					firstRoadChunk->setRoad(road.get());
					firstRoadChunk->setFromCrossing(&startNode);
					firstRoadChunk->setRankInPath(0);
					firstRoadChunk->setMetricOffset(0);
					firstRoadChunk->setKey(RoadChunkTableSync::getId());
					road->addRoadChunk(*firstRoadChunk);
					_setGeometryAndHouses(
						*firstRoadChunk,
						geometry,
						rightHouseNumberingPolicy,
						leftHouseNumberingPolicy,
						rightHouseNumberBounds,
						leftHouseNumberBounds
					);
					result = firstRoadChunk.get();

					env.getEditableRegistry<MainRoadChunk>().add(firstRoadChunk);

					// Second road chunk
					boost::shared_ptr<MainRoadChunk> secondRoadChunk(new MainRoadChunk);
					secondRoadChunk->setRoad(road.get());
					secondRoadChunk->setFromCrossing(&endNode);
					secondRoadChunk->setRankInPath(1);
					secondRoadChunk->setMetricOffset(length);
					secondRoadChunk->setKey(RoadChunkTableSync::getId());
					road->addRoadChunk(*secondRoadChunk);
					env.getEditableRegistry<MainRoadChunk>().add(secondRoadChunk);
				}
			}

			return result;
		}



		void RoadFileFormat::_setGeometryAndHouses(
			MainRoadChunk& chunk,
			boost::shared_ptr<geos::geom::LineString> geometry,
			MainRoadChunk::HouseNumberingPolicy rightHouseNumberingPolicy,
			MainRoadChunk::HouseNumberingPolicy leftHouseNumberingPolicy,
			MainRoadChunk::HouseNumberBounds rightHouseNumberBounds,
			MainRoadChunk::HouseNumberBounds leftHouseNumberBounds
		){
			chunk.setGeometry(geometry);
			chunk.setRightHouseNumberBounds(rightHouseNumberBounds);
			chunk.setRightHouseNumberingPolicy(rightHouseNumberingPolicy);
			chunk.setLeftHouseNumberBounds(leftHouseNumberBounds);
			chunk.setLeftHouseNumberingPolicy(leftHouseNumberingPolicy);
		}



		PublicPlaceEntrance* RoadFileFormat::CreateOrUpdatePublicPlaceEntrance(
			ImportableTableSync::ObjectBySource<PublicPlaceEntranceTableSync>& publicPlaceEntrances,
			const std::string& code,
			boost::optional<const std::string&> name,
			MetricOffset metricOffset,
			boost::optional<MainRoadChunk::HouseNumber> number,
			MainRoadChunk& roadChunk,
			PublicPlace& publicPlace,
			const impex::DataSource& source,
			util::Env& env,
			const impex::ImportLogger& logger
		){
			PublicPlaceEntrance* publicPlaceEntrance(NULL);

			// Search for a public place linked with the datasource
			if(publicPlaceEntrances.contains(code))
			{
				set<PublicPlaceEntrance*> loadedPublicPlaceEntrances(publicPlaceEntrances.get(code));
				if(loadedPublicPlaceEntrances.size() > 1)
				{
					logger.logWarning(
						"More than one public place entrance with key "+ code
					);
				}
				publicPlaceEntrance = *loadedPublicPlaceEntrances.begin();
				logger.logLoad(
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
				links.insert(make_pair(&source, code));
				publicPlaceEntrance->setDataSourceLinksWithoutRegistration(links);
				env.getEditableRegistry<PublicPlaceEntrance>().add(boost::shared_ptr<PublicPlaceEntrance>(publicPlaceEntrance));
				publicPlaceEntrances.add(*publicPlaceEntrance);
				stringstream logStream;
				logStream << "Creation of the public place entrance with key " << code << " (" << publicPlace.getFullName();
				if(name)
				{
					logStream << " " << *name;
				}
				logStream << ")";
				logger.logCreation(logStream.str());
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
