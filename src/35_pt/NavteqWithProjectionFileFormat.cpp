
/** NavteqWithProjectionFileFormat class implementation.
	@file NavteqWithProjectionFileFormat.cpp

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

#include "NavteqWithProjectionFileFormat.h"
#include "cdbfile.h"
#include "DataSource.h"
#include "Address.h"
#include "AddressTableSync.h"
#include "PhysicalStop.h"
#include "PhysicalStopTableSync.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"
#include "Road.h"
#include "RoadTableSync.h"
#include "RoadChunk.h"
#include "RoadChunkTableSync.h"
#include "RoadPlace.h"
#include "RoadPlaceTableSync.h"
#include "City.h"
#include "CityTableSync.h"
#include "Crossing.h"
#include "shapefil.h"
#include "Point2D.h"
#include "SQLiteTransaction.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <map>

using namespace std;
using namespace boost;
using namespace boost::filesystem;

namespace synthese
{
	using namespace util;
	using namespace impex;
	using namespace pt;
	using namespace road;
	using namespace env;
	using namespace geography;
	using namespace db;
	using namespace geometry;
	
	

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,NavteqWithProjectionFileFormat>::FACTORY_KEY("NavteqWithProjection");
	}

	namespace pt
	{
		const std::string NavteqWithProjectionFileFormat::FILE_NODES("2nodes"); 
		const std::string NavteqWithProjectionFileFormat::FILE_STREETS("3streets");
		const std::string NavteqWithProjectionFileFormat::FILE_MTDAREA("1mtdarea");

		const string NavteqWithProjectionFileFormat::_FIELD_ID("ID");
		const string NavteqWithProjectionFileFormat::_FIELD_X("X");
		const string NavteqWithProjectionFileFormat::_FIELD_Y("Y");
		const string NavteqWithProjectionFileFormat::_FIELD_LINK_ID("LINK_ID");
		const string NavteqWithProjectionFileFormat::_FIELD_L_AREA_ID("L_AREA_ID");
		const string NavteqWithProjectionFileFormat::_FIELD_R_AREA_ID("R_AREA_ID");
		const string NavteqWithProjectionFileFormat::_FIELD_REF_IN_ID("REF_IN_ID");
		const string NavteqWithProjectionFileFormat::_FIELD_NREF_IN_ID("NREF_IN_ID");
		const string NavteqWithProjectionFileFormat::_FIELD_ST_NAME("ST_NAME");
		const string NavteqWithProjectionFileFormat::_FIELD_AREA_ID("AREA_ID");
		const string NavteqWithProjectionFileFormat::_FIELD_AREACODE_3("AREACODE_3");
		const string NavteqWithProjectionFileFormat::_FIELD_AREACODE_4("AREACODE_4");
		const string NavteqWithProjectionFileFormat::_FIELD_GOVT_CODE("GOVT_CODE");
		const string NavteqWithProjectionFileFormat::_FIELD_ADMIN_LVL("ADMIN_LVL");
	}

	namespace impex
	{
		template<> const FileFormat::Files FileFormatTemplate<NavteqWithProjectionFileFormat>::FILES(
			NavteqWithProjectionFileFormat::FILE_MTDAREA.c_str(),
			NavteqWithProjectionFileFormat::FILE_NODES.c_str(),
			NavteqWithProjectionFileFormat::FILE_STREETS.c_str(),
		"");
	}

	namespace pt
	{
		NavteqWithProjectionFileFormat::NavteqWithProjectionFileFormat( util::Env* env /* = NULL */)
		{
			_env = env;
		}

		bool NavteqWithProjectionFileFormat::_controlPathsMap( const FilePathsMap& paths )
		{
			FilePathsMap::const_iterator it(paths.find(FILE_MTDAREA));
			if(it == paths.end() || it->second.empty()) return false;
			it = paths.find(FILE_NODES);
			if(it == paths.end() || it->second.empty()) return false;
			it = paths.find(FILE_STREETS);
			if(it == paths.end() || it->second.empty()) return false;
			return true;
		}

		NavteqWithProjectionFileFormat::~NavteqWithProjectionFileFormat()
		{}

		void NavteqWithProjectionFileFormat::build(std::ostream& os)
		{}

		void NavteqWithProjectionFileFormat::save(std::ostream& os
		) const {
			SQLiteTransaction transaction;
			BOOST_FOREACH(Registry<Address>::value_type address, _env->getEditableRegistry<Address>())
			{
				AddressTableSync::Save(address.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<PublicTransportStopZoneConnectionPlace>::value_type stop, _env->getEditableRegistry<PublicTransportStopZoneConnectionPlace>())
			{
				ConnectionPlaceTableSync::Save(stop.second.get(),transaction);
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


		void NavteqWithProjectionFileFormat::_parse(
			const path& filePath,
			std::ostream& os,
			std::string key
		){

			CDBFile dbfile;
			dbfile.OpenFile(filePath.file_string().c_str());
			if(!dbfile.IsOpen())
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}

			// 1 : Administrative areas
			if(key == FILE_MTDAREA)
			{
				map<string, string> departementCodes;
				typedef map<pair<string, string>, City*> CityCodes;
				CityCodes cityCodes;
				
				// Departements
				for(unsigned long i(1); i <= dbfile.GetRecordCount(); ++i)
				{
					shared_ptr<Record> record(dbfile.ReadRecord(i));

					if(dbfile.getText(*record, _FIELD_ADMIN_LVL) != "3") continue;
					string item(algorithm::trim_copy(dbfile.getText(*record, _FIELD_AREACODE_3)));
					if(departementCodes.find(item) != departementCodes.end()) continue;
					departementCodes.insert(make_pair(item, algorithm::trim_copy(dbfile.getText(*record, _FIELD_GOVT_CODE))));
				}

				// Cities
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

				// Places
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

			} // 2 : Nodes
			else if(key == FILE_NODES)
			{
				for(unsigned long i(1); i <= dbfile.GetRecordCount(); ++i)
				{
					shared_ptr<Record> record(dbfile.ReadRecord(i));
					shared_ptr<Address> address(new Address);

					address->setCodeBySource(algorithm::trim_copy(dbfile.getText(*record, _FIELD_ID)));
					address->setXY(
						lexical_cast<double>(algorithm::trim_copy(dbfile.getText(*record, _FIELD_X))),
						lexical_cast<double>(algorithm::trim_copy(dbfile.getText(*record, _FIELD_Y)))
					);
					address->setDataSource(_dataSource);
					address->setKey(AddressTableSync::getId());

					RegistryKeyType stopId(0); 
					try
					{
						stopId = lexical_cast<RegistryKeyType>(algorithm::trim_copy(dbfile.getText(*record, _FIELD_LINK_ID)));
					}
					catch(...)
					{

					}

					if(stopId > 0)
					{ // PT Connection place
						try
						{
							shared_ptr<PhysicalStop> stop(PhysicalStopTableSync::GetEditable(stopId, *_env, UP_LINKS_LOAD_LEVEL));
							PublicTransportStopZoneConnectionPlace* place(const_cast<PublicTransportStopZoneConnectionPlace*>(stop->getConnectionPlace()));
							place->addAddress(address.get());
							BOOST_FOREACH(const Address* add2, place->getAddresses())
							{
								place->addForbiddenTransferDelay(add2->getKey(), address->getKey());
								place->addForbiddenTransferDelay(address->getKey(), add2->getKey());
							}
							BOOST_FOREACH(const PublicTransportStopZoneConnectionPlace::PhysicalStops::value_type& ps2, place->getPhysicalStops())
							{
								if(ps2.second == stop.get())
								{
									place->addTransferDelay(
										ps2.second->getKey(),
										address->getKey(),
										posix_time::minutes(0)
									);
									place->addTransferDelay(
										address->getKey(),
										ps2.second->getKey(),
										posix_time::minutes(0)
									);
								}
								else
								{
									place->addForbiddenTransferDelay(ps2.second->getKey(), address->getKey());
									place->addForbiddenTransferDelay(address->getKey(), ps2.second->getKey());
								}
							}
							address->setHub(place);
						}
						catch(...)
						{
							shared_ptr<Crossing> crossing(new Crossing);
							crossing->setKey(util::encodeUId(43,0,0,decodeObjectId(address->getKey())));
							crossing->setAddress(address.get());
							address->setHub(crossing.get());
							_env->getEditableRegistry<Crossing>().add(crossing);
						}
					}
					else
					{ // Crossing
						shared_ptr<Crossing> crossing(new Crossing);
						crossing->setKey(util::encodeUId(43,0,0,decodeObjectId(address->getKey())));
						crossing->setAddress(address.get());
						address->setHub(crossing.get());
						_env->getEditableRegistry<Crossing>().add(crossing);
					}

					_navteqAddressses.insert(make_pair(address->getCodeBySource(), address.get()));
					_env->getEditableRegistry<Address>().add(address);
				}
			} // 3 : Streets
			else if (key == FILE_STREETS)
			{
				path shapeFilePath(filePath);
				shapeFilePath.replace_extension("shp");
				SHPHandle shapeFile(SHPOpen(shapeFilePath.file_string().c_str(), "rb"));

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

					for(size_t area(0); area< (lAreaId == rAreaId ? 1 : 2); ++area)
					{
						// Test if the record can be imported
						_CitiesMap::const_iterator itc(_citiesMap.find(area ? lAreaId : rAreaId));
						_AddressesMap::const_iterator ita1(_navteqAddressses.find(leftId));
						_AddressesMap::const_iterator ita2(_navteqAddressses.find(rightId));
						if(	itc == _citiesMap.end() ||
							ita1 == _navteqAddressses.end() ||
							ita2 == _navteqAddressses.end()
						){
							continue;
						}

						// Other fields

						string roadName(algorithm::trim_copy(dbfile.getText(*record, _FIELD_ST_NAME)));
						SHPObject* shpObject(SHPReadObject(shapeFile, int(i-1)));
						double length(0);
						optional<Point2D> lastPt;
						for(int i(0); i< shpObject->nVertices; ++i)
						{
							Point2D point(shpObject->padfX[i], shpObject->padfY[i]);
							if(lastPt)
							{
								length += point.getDistanceTo(*lastPt);
							}
							lastPt = point;
						}
						SHPDestroyObject(shpObject);

						City* city(itc->second);
						Address* leftNode(ita1->second);
						Address* rightNode(ita2->second);

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
						BOOST_FOREACH(const Road* croad, roadPlace->getRoads())
						{
							if(croad->getLastEdge()->getFromVertex() == leftNode)
							{
								road = const_cast<Road*>(croad);
								startMetricOffset = croad->getLastEdge()->getMetricOffset();
								break;
							}
						}
						if(road)
						{
							// Second road chunk creation
							shared_ptr<RoadChunk> secondRoadChunk(new RoadChunk);
							secondRoadChunk->setRoad(road);
							secondRoadChunk->setFromAddress(rightNode);
							secondRoadChunk->setRankInPath((*(road->getEdges().end()-1))->getRankInPath() + 1);
							secondRoadChunk->setMetricOffset(startMetricOffset + length);
							secondRoadChunk->setKey(RoadChunkTableSync::getId());
							road->addRoadChunk(secondRoadChunk.get());
							_env->getEditableRegistry<RoadChunk>().add(secondRoadChunk);
						}
						else
						{
							// If not found search for an existing road which begins at the right node
							BOOST_FOREACH(const Road* croad, roadPlace->getRoads())
							{
								if(croad->getEdge(0)->getFromVertex() == rightNode)
								{
									road = const_cast<Road*>(croad);
									break;
								}
							}

							if(road)
							{
								// First road chunk creation
								shared_ptr<RoadChunk> firstRoadChunk(new RoadChunk);
								firstRoadChunk->setRoad(road);
								firstRoadChunk->setFromAddress(leftNode);
								firstRoadChunk->setRankInPath(0);
								firstRoadChunk->setMetricOffset(0);
								firstRoadChunk->setKey(RoadChunkTableSync::getId());
								road->addRoadChunk(firstRoadChunk.get(), length);
								_env->getEditableRegistry<RoadChunk>().add(firstRoadChunk);
							}
							else
							{
								shared_ptr<Road> road(new Road);
								road->setRoadPlace(roadPlace.get());
								roadPlace->addRoad(*road);
								road->setKey(RoadTableSync::getId());
								_env->getEditableRegistry<Road>().add(road);

								// First road chunk
								shared_ptr<RoadChunk> firstRoadChunk(new RoadChunk);
								firstRoadChunk->setRoad(road.get());
								firstRoadChunk->setFromAddress(leftNode);
								firstRoadChunk->setRankInPath(0);
								firstRoadChunk->setMetricOffset(0);
								firstRoadChunk->setKey(RoadChunkTableSync::getId());
								road->addRoadChunk(firstRoadChunk.get());
								_env->getEditableRegistry<RoadChunk>().add(firstRoadChunk);

								// Second road chunk
								shared_ptr<RoadChunk> secondRoadChunk(new RoadChunk);
								secondRoadChunk->setRoad(road.get());
								secondRoadChunk->setFromAddress(rightNode);
								secondRoadChunk->setRankInPath(1);
								secondRoadChunk->setMetricOffset(length);
								secondRoadChunk->setKey(RoadChunkTableSync::getId());
								road->addRoadChunk(secondRoadChunk.get());
								_env->getEditableRegistry<RoadChunk>().add(secondRoadChunk);
							}
						}
					}
				}
			}

			dbfile.CloseFile();
		}
	}
}

