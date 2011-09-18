
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
#include "CoordinatesSystem.hpp"
#include "DBTransaction.hpp"
#include "VirtualShapeVirtualTable.hpp"
#include "AdminFunctionRequest.hpp"
#include "DataSourceAdmin.h"
#include "ImportFunction.h"
#include "PropertiesHTMLTable.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem.hpp>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <fstream>

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
	using namespace admin;
	using namespace server;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,NavstreetsFileFormat>::FACTORY_KEY("Navstreets");
	}

	namespace road
	{
		const string NavstreetsFileFormat::Importer_::FILE_MTDAREA("1mtdarea");
		const string NavstreetsFileFormat::Importer_::FILE_STREETS("2streets");

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
		const string NavstreetsFileFormat::_FIELD_GEOMETRY("Geometry");

		const string NavstreetsFileFormat::_FIELD_AREA_ID("AREA_ID");
		const string NavstreetsFileFormat::_FIELD_AREACODE_3("AREACODE_3");
		const string NavstreetsFileFormat::_FIELD_AREACODE_4("AREACODE_4");
		const string NavstreetsFileFormat::_FIELD_GOVT_CODE("GOVT_CODE");
		const string NavstreetsFileFormat::_FIELD_ADMIN_LVL("ADMIN_LVL");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<NavstreetsFileFormat>::Files MultipleFileTypesImporter<NavstreetsFileFormat>::FILES(
			NavstreetsFileFormat::Importer_::FILE_MTDAREA.c_str(),
			NavstreetsFileFormat::Importer_::FILE_STREETS.c_str(),
		"");
	}

	namespace road
	{
		bool NavstreetsFileFormat::Importer_::_checkPathsMap() const
		{
			// MTDAREA
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_MTDAREA));
			if(it == _pathsMap.end() || it->second.empty() || !exists(it->second))
			{
				return false;
			}

			// STREETS
			it = _pathsMap.find(FILE_STREETS);
			if(it == _pathsMap.end() || it->second.empty() || !exists(it->second))
			{
				return false;
			}

			// OK
			return true;
		}



		bool NavstreetsFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& os,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> adminRequest
		) const {
			// 1 : Administrative areas
			if(key == FILE_MTDAREA)
			{
				// Loading the file into SQLite as virtual table
				VirtualShapeVirtualTable table(filePath, _dataSource.getCharset(), _dataSource.getCoordinatesSystem()->getSRID());

				map<string, string> departementCodes;
				typedef map<pair<string, string>, City*> CityCodes;
				CityCodes cityCodes;

				{	// 1.1 Departements
					stringstream query;
					query << "SELECT * FROM " << table.getName() << " WHERE " << NavstreetsFileFormat::_FIELD_ADMIN_LVL << "=3";
					DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
					while(rows->next())
					{
						string item(rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3));

						if(departementCodes.find(item) != departementCodes.end())
						{
							continue;
						}
						departementCodes.insert(
							make_pair(item, rows->getText(NavstreetsFileFormat::_FIELD_GOVT_CODE))
						);
				}	}

				{	// 1.2 Cities
					stringstream query;
					query << "SELECT * FROM " << table.getName() << " WHERE " << NavstreetsFileFormat::_FIELD_ADMIN_LVL << "=4";
					DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
					while(rows->next())
					{
						stringstream code;
						int cityID(rows->getInt(NavstreetsFileFormat::_FIELD_AREA_ID));
						code << departementCodes[rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3)];
						code << setw(3) << setfill('0') << rows->getInt(NavstreetsFileFormat::_FIELD_GOVT_CODE);
						if(_citiesMap.find(cityID) != _citiesMap.end())
						{
							continue;
						}

						shared_ptr<City> city(CityTableSync::GetEditableFromCode(code.str(), _env));

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
								make_pair(rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3), rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_4)),
								city.get()
						)	);
				}	}

				{
					// 1.3 Settlements
					stringstream query;
					query << "SELECT * FROM " << table.getName() << " WHERE " << NavstreetsFileFormat::_FIELD_ADMIN_LVL << "=5";
					DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
					while(rows->next())
					{
						CityCodes::const_iterator it(cityCodes.find(
								make_pair(rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3), rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_4))
						)	);

						if(it == cityCodes.end())
						{
							os << "WARN : City " << rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3) << "/" << rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_4) << " not found.<br />";
							continue;
						}

						_citiesMap.insert(make_pair(
								rows->getInt(NavstreetsFileFormat::_FIELD_AREA_ID),
								it->second
						)	);
				}	}

			} // 2 : Streets and nodes
			else if(key == FILE_STREETS)
			{
				// Loading the file into SQLite as virtual table
				VirtualShapeVirtualTable table(filePath, _dataSource.getCharset(), LAMBERT_II_SRID);

				typedef map<string, shared_ptr<Crossing> > _CrossingsMap;
				_CrossingsMap _navteqCrossings;

				const GeometryFactory& geometryFactory(CoordinatesSystem::GetCoordinatesSystem(LAMBERT_II_SRID).getGeometryFactory());

				// Recently added road places
				typedef map<pair<RegistryKeyType, string>, shared_ptr<RoadPlace> > RecentlyCreatedRoadPlaces;
				RecentlyCreatedRoadPlaces recentlyCreatedRoadPlaces;

				stringstream query;
				query << "SELECT *, AsText(" << NavstreetsFileFormat::_FIELD_GEOMETRY << ") AS " << NavstreetsFileFormat::_FIELD_GEOMETRY << "_ASTEXT" << " FROM " << table.getName();
				DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
				while(rows->next())
				{
					// Fields to test if the record can be imported
					string leftId(rows->getText(NavstreetsFileFormat::_FIELD_REF_IN_ID));
					string rightId(rows->getText(NavstreetsFileFormat::_FIELD_NREF_IN_ID));
					int lAreaId(rows->getInt(NavstreetsFileFormat::_FIELD_L_AREA_ID));
					int rAreaId(rows->getInt(NavstreetsFileFormat::_FIELD_R_AREA_ID));
					string leftRefHouseNumber(rows->getText(NavstreetsFileFormat::_FIELD_L_REFADDR));
					string leftNRefHouseNumber(rows->getText(NavstreetsFileFormat::_FIELD_L_NREFADDR));
					string rightRefHouseNumber(rows->getText(NavstreetsFileFormat::_FIELD_R_REFADDR));
					string rightNRefHouseNumber(rows->getText(NavstreetsFileFormat::_FIELD_R_NREFADDR));
					string leftAddressSchema(rows->getText(NavstreetsFileFormat::_FIELD_L_ADDRSCH));
					string rightAddressSchema(rows->getText(NavstreetsFileFormat::_FIELD_R_ADDRSCH));
					shared_ptr<LineString> geometry(
						dynamic_pointer_cast<LineString, Geometry>(
						rows->getGeometryFromWKT(NavstreetsFileFormat::_FIELD_GEOMETRY+"_ASTEXT", geometryFactory)
					)	);

					if(!geometry.get())
					{
						os << "ERR : Empty geometry.<br />";
						continue;
					}



///	@todo Handle this case with aliases
//					for(size_t area(0); area< (lAreaId == rAreaId ? size_t(1) : size_t(2)); ++area)
//					{
						// Test if the record can be imported
						_CitiesMap::const_iterator itc(_citiesMap.find(/*area ?*/ lAreaId /*: rAreaId*/));
						if(	itc == _citiesMap.end()
						){
							continue;
						}

						// Chunk length
						double length(geometry->getLength());

						// Name
						string roadName(rows->getText(NavstreetsFileFormat::_FIELD_ST_NAME));

						// City
						City* city(itc->second);

						// House number bounds
						MainRoadChunk::HouseNumberBounds rightHouseNumberBounds(_getHouseNumberBoundsFromAddresses(rightRefHouseNumber, rightNRefHouseNumber));
						MainRoadChunk::HouseNumberBounds leftHouseNumberBounds(_getHouseNumberBoundsFromAddresses(leftRefHouseNumber, leftNRefHouseNumber));

						// House numbering policy
						MainRoadChunk::HouseNumberingPolicy rightHouseNumberingPolicy(_getHouseNumberingPolicyFromAddressSchema(rightAddressSchema));
						MainRoadChunk::HouseNumberingPolicy leftHouseNumberingPolicy(_getHouseNumberingPolicyFromAddressSchema(leftAddressSchema));


						// Left node
						_CrossingsMap::const_iterator ita1(_navteqCrossings.find(leftId));
						shared_ptr<Crossing> leftNode;
						if(ita1 == _navteqCrossings.end())
						{
							leftNode.reset(
								new Crossing(
									CrossingTableSync::getId(),
									shared_ptr<Point>(geometry->getStartPoint()),
									leftId,
									&_dataSource
							)	);

							_navteqCrossings.insert(make_pair(leftId, leftNode));
							_env.getEditableRegistry<Crossing>().add(leftNode);
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
							rightNode.reset(
								new Crossing(
									CrossingTableSync::getId(),
									shared_ptr<Point>(geometry->getEndPoint()),
									rightId,
									&_dataSource
							)	);

							_navteqCrossings.insert(make_pair(rightId, rightNode));
							_env.getEditableRegistry<Crossing>().add(rightNode);
						}
						else
						{
							rightNode = ita2->second;
						}


						// Search for an existing road place
						shared_ptr<RoadPlace> roadPlace(RoadPlaceTableSync::GetEditableFromCityAndName(
								city->getKey(),
								roadName,
								_env
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
							_env.getEditableRegistry<RoadPlace>().add(roadPlace);
							recentlyCreatedRoadPlaces.insert(
								make_pair(
									make_pair(
										city->getKey(),
										roadName
									), roadPlace
							)	);
						}

						// Search for an existing road which ends at the left node
						MainRoadPart* road(NULL);
						double startMetricOffset(0);
						BOOST_FOREACH(Path* croad, roadPlace->getPaths())
						{
							if(!dynamic_cast<MainRoadPart*>(croad))
							{
								continue;
							}
							if(croad->getLastEdge()->getFromVertex() == leftNode.get())
							{
								road = static_cast<MainRoadPart*>(croad);
								startMetricOffset = croad->getLastEdge()->getMetricOffset();
								break;
							}
						}
						if(road)
						{
							// Adding geometry to the last chunk
							_setGeometryAndHouses(
								static_cast<MainRoadChunk&>(*road->getLastEdge()),
								geometry,
								rightHouseNumberingPolicy,
								leftHouseNumberingPolicy,
								rightHouseNumberBounds,
								leftHouseNumberBounds
								);

							// Second road chunk creation
							shared_ptr<MainRoadChunk> secondRoadChunk(new MainRoadChunk);
							secondRoadChunk->setRoad(road);
							secondRoadChunk->setFromCrossing(rightNode.get());
							secondRoadChunk->setRankInPath((*(road->getEdges().end()-1))->getRankInPath() + 1);
							secondRoadChunk->setMetricOffset(startMetricOffset + length);
							secondRoadChunk->setKey(RoadChunkTableSync::getId());
							road->addRoadChunk(*secondRoadChunk);
							_env.getEditableRegistry<MainRoadChunk>().add(secondRoadChunk);

							// Search for a second existing road which starts at the right node
							MainRoadPart* road2 = NULL;
							BOOST_FOREACH(Path* croad, roadPlace->getPaths())
							{
								if(!dynamic_cast<MainRoadPart*>(croad))
								{
									continue;
								}
								if(croad->getEdge(0)->getFromVertex() == rightNode.get())
								{
									road2 = static_cast<MainRoadPart*>(croad);
									break;
								}
							}
							// If found, merge the two roads
							if(road2)
							{
								RegistryKeyType lastEdgeId(road->getLastEdge()->getKey());
								road->merge(*road2);
								_env.getEditableRegistry<MainRoadChunk>().remove(lastEdgeId);
								_env.getEditableRegistry<MainRoadPart>().remove(road2->getKey());
							}

						}
						else
						{
							// If not found search for an existing road which begins at the right node
							BOOST_FOREACH(Path* croad, roadPlace->getPaths())
							{
								if(!dynamic_cast<MainRoadPart*>(croad))
								{
									continue;
								}
								if(croad->getEdge(0)->getFromVertex() == rightNode.get())
								{
									road = static_cast<MainRoadPart*>(croad);
									break;
								}
							}

							if(road)
							{
								// First road chunk creation
								shared_ptr<MainRoadChunk> firstRoadChunk(new MainRoadChunk);
								firstRoadChunk->setRoad(road);
								firstRoadChunk->setFromCrossing(leftNode.get());
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

								_env.getEditableRegistry<MainRoadChunk>().add(firstRoadChunk);
							}
							else
							{
								shared_ptr<MainRoadPart> road(new MainRoadPart(0, Road::ROAD_TYPE_UNKNOWN));
								road->setRoadPlace(*roadPlace);
								road->setKey(RoadTableSync::getId());
								_env.getEditableRegistry<MainRoadPart>().add(road);

								// First road chunk
								shared_ptr<MainRoadChunk> firstRoadChunk(new MainRoadChunk);
								firstRoadChunk->setRoad(road.get());
								firstRoadChunk->setFromCrossing(leftNode.get());
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

								_env.getEditableRegistry<MainRoadChunk>().add(firstRoadChunk);

								// Second road chunk
								shared_ptr<MainRoadChunk> secondRoadChunk(new MainRoadChunk);
								secondRoadChunk->setRoad(road.get());
								secondRoadChunk->setFromCrossing(rightNode.get());
								secondRoadChunk->setRankInPath(1);
								secondRoadChunk->setMetricOffset(length);
								secondRoadChunk->setKey(RoadChunkTableSync::getId());
								road->addRoadChunk(*secondRoadChunk);
								_env.getEditableRegistry<MainRoadChunk>().add(secondRoadChunk);
							}
						}
//					}
				}
			}

			os << "<b>SUCCESS : Data loaded</b><br />";

			return true;
		}



		DBTransaction NavstreetsFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;
			BOOST_FOREACH(const Registry<Crossing>::value_type& crossing, _env.getEditableRegistry<Crossing>())
			{
				CrossingTableSync::Save(crossing.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<RoadPlace>::value_type& roadplace, _env.getEditableRegistry<RoadPlace>())
			{
				RoadPlaceTableSync::Save(roadplace.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<MainRoadPart>::value_type& road, _env.getEditableRegistry<MainRoadPart>())
			{
				RoadTableSync::Save(road.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<MainRoadChunk>::value_type& roadChunk, _env.getEditableRegistry<MainRoadChunk>())
			{
				RoadChunkTableSync::Save(roadChunk.second.get(), transaction);
			}
			return transaction;
		}



		MainRoadChunk::HouseNumberingPolicy NavstreetsFileFormat::Importer_::_getHouseNumberingPolicyFromAddressSchema(
			const std::string& addressSchema
		){
			if(addressSchema == "E") return MainRoadChunk::EVEN;
			if(addressSchema == "O") return MainRoadChunk::ODD;
			return MainRoadChunk::ALL;
		}



		MainRoadChunk::HouseNumberBounds NavstreetsFileFormat::Importer_::_getHouseNumberBoundsFromAddresses(
			const std::string& minAddressConst,
			const std::string& maxAddressConst
		){
			if(!minAddressConst.empty() && !maxAddressConst.empty())
			{
				try
				{
					// FIXME(sylvain): why isn't this code using boost trim?

					// Remove whitespace because of a bad_lexical_cast on "2 " !!!
					string minAddress = minAddressConst;
					string maxAddress = maxAddressConst;

					string::size_type k = 0;
					while((k = minAddress.find(' ', k)) != minAddress.npos) {
						minAddress.erase(k, 1);
					}
					k = 0;
					while((k = maxAddress.find(' ', k)) != maxAddress.npos) {
						maxAddress.erase(k, 1);
					}

					return MainRoadChunk::HouseNumberBounds(
						pair<MainRoadChunk::HouseNumber, MainRoadChunk::HouseNumber>(
							lexical_cast<MainRoadChunk::HouseNumber>(minAddress),
							lexical_cast<MainRoadChunk::HouseNumber>(maxAddress)
					)	);
				}
				catch(bad_lexical_cast)
				{
 				}
			}
			return MainRoadChunk::HouseNumberBounds();
		}



		void NavstreetsFileFormat::Importer_::_setGeometryAndHouses(
			MainRoadChunk& chunk,
			shared_ptr<geos::geom::LineString> geometry,
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



		void NavstreetsFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			AdminFunctionRequest<DataSourceAdmin> importRequest(request);
			PropertiesHTMLTable t(importRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Propriétés");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Données");
			stream << t.cell("Rues (streets)", t.getForm().getTextInput(_getFileParameterName(FILE_STREETS), _pathsMap[FILE_STREETS].file_string()));
			stream << t.cell("Zones administratives (mtdarea)", t.getForm().getTextInput(_getFileParameterName(FILE_MTDAREA), _pathsMap[FILE_MTDAREA].file_string()));
			stream << t.close();
		}



		util::ParametersMap NavstreetsFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result;
			return result;
		}



		void NavstreetsFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
		}
}	}
