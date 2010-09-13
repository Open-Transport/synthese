
/** NavstreetsFileFormat class implementation.
	@file NavstreetsFileFormat.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "NavstreetsFileFormat.hpp"
#include "CrossingTableSync.hpp"
#include "RoadTableSync.h"
#include "RoadChunkTableSync.h"
#include "RoadPlaceTableSync.h"
#include "CityTableSync.h"
#include "DataSource.h"
#include "Crossing.h"
#include "cdbfile.h"
#include "CoordinatesSystem.hpp"
#include "SQLiteTransaction.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <shapefil.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace geos::geom;

namespace synthese
{
	using namespace road;
	using namespace util;
	using namespace impex;
	using namespace geography;
	using namespace graph;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,NavstreetsFileFormat>::FACTORY_KEY("Navstreets");
	}

	namespace road
	{
		const string NavstreetsFileFormat::FILE_MTDAREA("1mtdarea");
		const string NavstreetsFileFormat::FILE_STREETS("2streets");
		
		const string NavstreetsFileFormat::_FIELD_LINK_ID("LINK_ID");
		const string NavstreetsFileFormat::_FIELD_ST_NAME("ST_NAME");
		const string NavstreetsFileFormat::_FIELD_REF_IN_ID("REF_IN_ID");
		const string NavstreetsFileFormat::_FIELD_NREF_IN_ID("NREF_IN_ID");
		const string NavstreetsFileFormat::_FIELD_L_AREA_ID("L_AREA_ID");
		const string NavstreetsFileFormat::_FIELD_R_AREA_ID("R_AREA_ID");
		const string NavstreetsFileFormat::_FIELD_L_REFADDR("L_REFADDR");
		const string NavstreetsFileFormat::_FIELD_L_NREFADDR("L_NREFADDR");
		const string NavstreetsFileFormat::_FIELD_R_REFADDR("R_REFADDR");
		const string NavstreetsFileFormat::_FIELD_R_NREFADDR("R_NREFADDR");
		const string NavstreetsFileFormat::_FIELD_L_ADDRSCH("L_ADDRSCH");
		const string NavstreetsFileFormat::_FIELD_R_ADDRSCH("R_ADDRSCH");

		const string NavstreetsFileFormat::_FIELD_AREA_ID("AREA_ID");
		const string NavstreetsFileFormat::_FIELD_AREACODE_3("AREACODE_3");
		const string NavstreetsFileFormat::_FIELD_AREACODE_4("AREACODE_4");
		const string NavstreetsFileFormat::_FIELD_GOVT_CODE("GOVT_CODE");
		const string NavstreetsFileFormat::_FIELD_ADMIN_LVL("ADMIN_LVL");
	}

	namespace impex
	{
		template<> const FileFormat::Files FileFormatTemplate<NavstreetsFileFormat>::FILES(
			NavstreetsFileFormat::FILE_MTDAREA.c_str(),
			NavstreetsFileFormat::FILE_STREETS.c_str(),
		"");
	}

	namespace road
	{
		NavstreetsFileFormat::NavstreetsFileFormat(
			Env* env /* = NULL */
		){
			_env = env;
		}



		bool NavstreetsFileFormat::_controlPathsMap( const FilePathsMap& paths )
		{
			FilePathsMap::const_iterator it(paths.find(FILE_MTDAREA));
			if(it == paths.end() || it->second.empty()) return false;
			it = paths.find(FILE_STREETS);
			if(it == paths.end() || it->second.empty()) return false;
			return true;
		}



		NavstreetsFileFormat::~NavstreetsFileFormat()
		{
		}



		void NavstreetsFileFormat::build( std::ostream& os )
		{
		}



		void NavstreetsFileFormat::_parse( const boost::filesystem::path& filePath, std::ostream& os, std::string key )
		{

			CDBFile dbfile;
			dbfile.OpenFile(filePath.file_string().c_str());
			if(!dbfile.IsOpen())
			{
				throw Exception("Could not open the file " + filePath.file_string());
			}

			// 1 : Administrative areas
			if(key == FILE_MTDAREA)
			{
				map<string, string> departementCodes;
				typedef map<pair<string, string>, City*> CityCodes;
				CityCodes cityCodes;
				
				// 1.1 Departements
				for(unsigned long i(1); i <= dbfile.GetRecordCount(); ++i)
				{
					shared_ptr<Record> record(dbfile.ReadRecord(i));

					if(dbfile.getText(*record, _FIELD_ADMIN_LVL) != "3") continue;
					string item(algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREACODE_3)));

					if(departementCodes.find(item) != departementCodes.end()) continue;
					departementCodes.insert(make_pair(item, algorithm::trim_copy(dbfile.getText(*record, _FIELD_GOVT_CODE))));
				}

				// 1.2 Cities
				for(unsigned long i(1); i <= dbfile.GetRecordCount(); ++i)
				{
					shared_ptr<Record> record(dbfile.ReadRecord(i));

					if(dbfile.getText(*record, _FIELD_ADMIN_LVL) != "4") continue;
					stringstream code;
					int cityID(lexical_cast<int>(algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREA_ID))));
					code << departementCodes[algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREACODE_3))];
					code << setw(3) << setfill('0') << lexical_cast<int>(algorithm::trim_copy(dbfile.getText(*record, _FIELD_GOVT_CODE)));
					if(_citiesMap.find(cityID) != _citiesMap.end()) continue;

					shared_ptr<City> city(CityTableSync::GetEditableFromCode(code.str(), *_env));

					if(!city.get())
					{
						os << "WARN : City " << code.str() << " not found.<br />";
						continue;
					}

					_citiesMap.insert(make_pair(
							cityID,
							city.get()
					)	);
					cityCodes.insert(make_pair(
							make_pair(algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREACODE_3)), algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREACODE_4))),
							city.get()
					)	);
				}

				// 1.3 Settlements
				for(unsigned long i(1); i <= dbfile.GetRecordCount(); ++i)
				{
					shared_ptr<Record> record(dbfile.ReadRecord(i));

					if(dbfile.getText(*record, _FIELD_ADMIN_LVL) != "5") continue;

					CityCodes::const_iterator it(cityCodes.find(
							make_pair(algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREACODE_3)), algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREACODE_4)))
					)	);
					
					if(it == cityCodes.end())
					{
						os << "WARN : City " << algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREACODE_3)) << "/" << algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREACODE_4)) << " not found.<br />";
						continue;
					}

					_citiesMap.insert(make_pair(
							lexical_cast<int>(algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREA_ID))),
							it->second
					)	);
				}

			} // 2 : Streets and nodes
			else if(key == FILE_STREETS)
			{
				// Opening of corresponding shapefile to get the geometry
				path shapeFilePath(filePath);
				shapeFilePath.replace_extension("shp");
				SHPHandle shapeFile(SHPOpen(shapeFilePath.file_string().c_str(), "rb"));
				if(!shapeFile)
				{
					throw Exception("Could no open the shapefile corresponding to " + filePath.file_string());
				}

				typedef map<string, shared_ptr<Crossing> > _CrossingsMap;
				_CrossingsMap _navteqCrossings;	

				const GeometryFactory& geometryFactory(DBModule::GetDefaultGeometryFactory());

				// Recently added road places
				typedef map<pair<RegistryKeyType, string>, shared_ptr<RoadPlace> > RecentlyCreatedRoadPlaces;
				RecentlyCreatedRoadPlaces recentlyCreatedRoadPlaces;

				for(unsigned long i(1); i <= dbfile.GetRecordCount(); ++i)
				{
					// Fields to test if the record can be imported
					shared_ptr<Record> record(dbfile.ReadRecord(i));
					string leftId(algorithm::trim_copy(dbfile.getText(*record, _FIELD_REF_IN_ID)));
					string rightId(algorithm::trim_copy(dbfile.getText(*record, _FIELD_NREF_IN_ID)));
					int lAreaId(lexical_cast<int>(algorithm::trim_copy(dbfile.getText(*record, _FIELD_L_AREA_ID))));
					int rAreaId(lexical_cast<int>(algorithm::trim_copy(dbfile.getText(*record, _FIELD_R_AREA_ID))));
					string leftMinHouseNumber(algorithm::trim_copy(dbfile.getText(*record, _FIELD_L_REFADDR)));
					string leftMaxHouseNumber(algorithm::trim_copy(dbfile.getText(*record, _FIELD_L_NREFADDR)));
					string rightMinHouseNumber(algorithm::trim_copy(dbfile.getText(*record, _FIELD_R_REFADDR)));
					string rightMaxHouseNumber(algorithm::trim_copy(dbfile.getText(*record, _FIELD_R_NREFADDR)));

///	@todo Handle this case with aliases
//					for(size_t area(0); area< (lAreaId == rAreaId ? size_t(1) : size_t(2)); ++area)
//					{
						// Test if the record can be imported
						_CitiesMap::const_iterator itc(_citiesMap.find(/*area ?*/ lAreaId /*: rAreaId*/));
						if(	itc == _citiesMap.end()
						){
							continue;
						}

						// Fields to load

						// Geometry of the street and the crossings
						SHPObject* shpObject(SHPReadObject(shapeFile, int(i-1)));
						std::vector<Coordinate>* coordinates = new std::vector<geos::geom::Coordinate>();
						for(int p(0); p< shpObject->nVertices; ++p)
						{
							coordinates->push_back(
								Coordinate(shpObject->padfX[p], shpObject->padfY[p])
							);
						}
						Coordinate leftNodeCoordinate(shpObject->padfX[0], shpObject->padfY[0]);
						Coordinate rightNodeCoordinate(shpObject->padfX[shpObject->nVertices-1], shpObject->padfY[shpObject->nVertices-1]);
						SHPDestroyObject(shpObject);
						CoordinateSequence *cs = geometryFactory.getCoordinateSequenceFactory()->create(coordinates);
						//coordinates is now owned by cs

						shared_ptr<LineString> geometry(geometryFactory.createLineString(cs));
						//cs is now owned by geometry

						// Chunk length
						double length(geometry->getLength());

						// Name
						string roadName(algorithm::trim_copy(dbfile.getText(*record, _FIELD_ST_NAME)));

						// City
						City* city(itc->second);

						// Left node
						_CrossingsMap::const_iterator ita1(_navteqCrossings.find(leftId));
						shared_ptr<Crossing> leftNode;
						if(ita1 == _navteqCrossings.end())
						{
							GeoPoint gp(
								leftNodeCoordinate,
								DBModule::GetCoordinatesSystem(27572)
							);
							leftNode.reset(
								new Crossing(
									CrossingTableSync::getId(),
									gp.getLongitude(),
									gp.getLatitude(),
									leftId,
									_dataSource
							)	);

							_navteqCrossings.insert(make_pair(leftId, leftNode));
							_env->getEditableRegistry<Crossing>().add(leftNode);
						}
						else
						{
							leftNode = ita1->second;
						}


						// Right node
						_CrossingsMap::const_iterator ita2(_navteqCrossings.find(rightId));
						shared_ptr<Crossing> rightNode;
						if(ita2 == _navteqCrossings.end())
						{
							GeoPoint gp(
								rightNodeCoordinate,
								DBModule::GetCoordinatesSystem(27572)
							);
							rightNode.reset(
								new Crossing(
									CrossingTableSync::getId(),
									gp.getLongitude(),
									gp.getLatitude(),
									rightId,
									_dataSource
							)	);
							
							_navteqCrossings.insert(make_pair(rightId, rightNode));
							_env->getEditableRegistry<Crossing>().add(rightNode);
						}
						else
						{
							rightNode = ita2->second;
						}


						// Search for an existing road place
						shared_ptr<RoadPlace> roadPlace(RoadPlaceTableSync::GetEditableFromCityAndName(
								city->getKey(),
								roadName,
								*_env
						)	);

						// Search for a recently created road place
						RecentlyCreatedRoadPlaces::iterator it(
							recentlyCreatedRoadPlaces.find(
							make_pair(
								city->getKey(),
								roadName
						)	)	);
						if(it != recentlyCreatedRoadPlaces.end())
						{
							roadPlace = it->second;
						}

						// Road place creation if necessary
						if(!roadPlace.get())
						{
							roadPlace.reset(new RoadPlace);
							roadPlace->setCity(city);
							roadPlace->setKey(RoadPlaceTableSync::getId());
							roadPlace->setName(roadName);
							_env->getEditableRegistry<RoadPlace>().add(roadPlace);
							recentlyCreatedRoadPlaces.insert(
								make_pair(
									make_pair(
										city->getKey(),
										roadName
									), roadPlace
							)	);
						}

						// Search for an existing road which ends at the left node
						Road* road(NULL);
						double startMetricOffset(0);
						BOOST_FOREACH(Path* croad, roadPlace->getPaths())
						{
							if(croad->getLastEdge()->getFromVertex() == leftNode.get())
							{
								road = static_cast<Road*>(croad);
								startMetricOffset = croad->getLastEdge()->getMetricOffset();
								break;
							}
						}
						if(road)
						{
							// Adding geometry to the last chunk
							road->getLastEdge()->setGeometry(geometry);

							// Second road chunk creation
							shared_ptr<RoadChunk> secondRoadChunk(new RoadChunk);
							secondRoadChunk->setRoad(road);
							secondRoadChunk->setFromCrossing(rightNode.get());
							secondRoadChunk->setRankInPath((*(road->getEdges().end()-1))->getRankInPath() + 1);
							secondRoadChunk->setMetricOffset(startMetricOffset + length);
							secondRoadChunk->setKey(RoadChunkTableSync::getId());
							road->addRoadChunk(*secondRoadChunk);
							_env->getEditableRegistry<RoadChunk>().add(secondRoadChunk);

							// Search for a second existing road which starts at the right node
							Road* road2 = NULL;
							BOOST_FOREACH(Path* croad, roadPlace->getPaths())
							{
								if(croad->getEdge(0)->getFromVertex() == rightNode.get())
								{
									road2 = static_cast<Road*>(croad);
									break;
								}
							}
							// If found, merge the two roads
							if(road2)
							{
								RegistryKeyType lastEdgeId(road->getLastEdge()->getKey());
								road->merge(*road2);
								_env->getEditableRegistry<RoadChunk>().remove(lastEdgeId);
								_env->getEditableRegistry<Road>().remove(road2->getKey());
							}

						}
						else
						{
							// If not found search for an existing road which begins at the right node
							BOOST_FOREACH(Path* croad, roadPlace->getPaths())
							{
								if(croad->getEdge(0)->getFromVertex() == rightNode.get())
								{
									road = static_cast<Road*>(croad);
									break;
								}
							}

							if(road)
							{
								// First road chunk creation
								shared_ptr<RoadChunk> firstRoadChunk(new RoadChunk);
								firstRoadChunk->setRoad(road);
								firstRoadChunk->setFromCrossing(leftNode.get());
								firstRoadChunk->setRankInPath(0);
								firstRoadChunk->setMetricOffset(0);
								firstRoadChunk->setKey(RoadChunkTableSync::getId());
								firstRoadChunk->setGeometry(geometry);
								road->insertRoadChunk(*firstRoadChunk, length, 1);
								_env->getEditableRegistry<RoadChunk>().add(firstRoadChunk);
							}
							else
							{
								shared_ptr<Road> road(new Road(0, Road::ROAD_TYPE_UNKNOWN, false));
								road->setRoadPlace(*roadPlace);
								road->setKey(RoadTableSync::getId());
								_env->getEditableRegistry<Road>().add(road);

								// First road chunk
								shared_ptr<RoadChunk> firstRoadChunk(new RoadChunk);
								firstRoadChunk->setRoad(road.get());
								firstRoadChunk->setFromCrossing(leftNode.get());
								firstRoadChunk->setRankInPath(0);
								firstRoadChunk->setMetricOffset(0);
								firstRoadChunk->setKey(RoadChunkTableSync::getId());
								firstRoadChunk->setGeometry(geometry);
								road->addRoadChunk(*firstRoadChunk);
								_env->getEditableRegistry<RoadChunk>().add(firstRoadChunk);

								// Second road chunk
								shared_ptr<RoadChunk> secondRoadChunk(new RoadChunk);
								secondRoadChunk->setRoad(road.get());
								secondRoadChunk->setFromCrossing(rightNode.get());
								secondRoadChunk->setRankInPath(1);
								secondRoadChunk->setMetricOffset(length);
								secondRoadChunk->setKey(RoadChunkTableSync::getId());
								road->addRoadChunk(*secondRoadChunk);
								_env->getEditableRegistry<RoadChunk>().add(secondRoadChunk);
							}
						}
//					}
				}
			}

			dbfile.CloseFile();

		}



		void NavstreetsFileFormat::save( std::ostream& os ) const
		{
			SQLiteTransaction transaction;
			BOOST_FOREACH(Registry<Crossing>::value_type crossing, _env->getEditableRegistry<Crossing>())
			{
				CrossingTableSync::Save(crossing.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<RoadPlace>::value_type roadplace, _env->getEditableRegistry<RoadPlace>())
			{
				RoadPlaceTableSync::Save(roadplace.second.get(),transaction);
			}
			BOOST_FOREACH(Registry<Road>::value_type road, _env->getEditableRegistry<Road>())
			{
				RoadTableSync::Save(road.second.get(),transaction);
			}
			BOOST_FOREACH(Registry<RoadChunk>::value_type roadChunk, _env->getEditableRegistry<RoadChunk>())
			{
				RoadChunkTableSync::Save(roadChunk.second.get(),transaction);
			}
			transaction.run();
		}
	}
}
