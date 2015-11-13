
/** NavstreetsFileFormat class implementation.
	@file NavstreetsFileFormat.cpp

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

#include "NavstreetsFileFormat.hpp"

#include "CityTableSync.h"
#include "CoordinatesSystem.hpp"
#include "CrossingTableSync.hpp"
#include "DataSource.h"
#include "DBTransaction.hpp"
#include "EdgeProjector.hpp"
#include "Import.hpp"
#include "PublicPlaceTableSync.h"
#include "PublicPlaceEntranceTableSync.hpp"
#include "RoadChunkTableSync.h"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"
#include "VirtualShapeVirtualTable.hpp"
#include "VirtualDBFVirtualTable.hpp"

#include <boost/algorithm/string/trim.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
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
	using namespace algorithm;
	using namespace data_exchange;
	using namespace db;
	using namespace geography;
	using namespace graph;
	using namespace impex;
	using namespace road;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, NavstreetsFileFormat>::FACTORY_KEY("Navstreets");
	}

	namespace data_exchange
	{
		const string NavstreetsFileFormat::Importer_::FILE_MTDAREA("1mtdarea");
		const string NavstreetsFileFormat::Importer_::FILE_STREETS("2streets");
		const string NavstreetsFileFormat::Importer_::FILE_PUBLIC_PLACES("3places");

		const string NavstreetsFileFormat::Importer_::PARAMETER_CITIES_AUTO_CREATION("cities_auto_creation");

		const string NavstreetsFileFormat::_FIELD_OBJECTID("OBJECTID");
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
		const string NavstreetsFileFormat::_FIELD_AREACODE_2("AREACODE_2");
		const string NavstreetsFileFormat::_FIELD_AREACODE_3("AREACODE_3");
		const string NavstreetsFileFormat::_FIELD_AREACODE_4("AREACODE_4");
		const string NavstreetsFileFormat::_FIELD_GOVT_CODE("GOVT_CODE");
		const string NavstreetsFileFormat::_FIELD_ADMIN_LVL("ADMIN_LVL");
		const string NavstreetsFileFormat::_FIELD_AREA_NAME("AREA_NAME");

		const string NavstreetsFileFormat::_FIELD_POI_ID("POI_ID");
		const string NavstreetsFileFormat::_FIELD_POI_NAME("POI_NAME");
		const string NavstreetsFileFormat::_FIELD_POI_ST_NUM("POI_ST_NUM");
		const string NavstreetsFileFormat::_FIELD_POI_ST_NAME("ST_NAME");

		const std::map<int, RoadType> highwayTypes = boost::assign::map_list_of
			(1, ROAD_TYPE_PRINCIPLEAXIS)
			(2, ROAD_TYPE_PRINCIPLEAXIS)
			(3, ROAD_TYPE_SECONDARYAXIS)
			(4, ROAD_TYPE_SECONDARYAXIS)
			(5, ROAD_TYPE_ACCESSROAD);
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<NavstreetsFileFormat>::Files MultipleFileTypesImporter<NavstreetsFileFormat>::FILES(
			NavstreetsFileFormat::Importer_::FILE_MTDAREA.c_str(),
			NavstreetsFileFormat::Importer_::FILE_STREETS.c_str(),
			NavstreetsFileFormat::Importer_::FILE_PUBLIC_PLACES.c_str(),
		"");
	}

	namespace data_exchange
	{
		bool NavstreetsFileFormat::Importer_::_checkPathsMap() const
		{
			// MTDAREA
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_MTDAREA));
			if(it == _pathsMap.end() || it->second.empty() || !exists(it->second))
			{
				return false;
			}

			// STREETS and PUBLIC_PLACES
			it = _pathsMap.find(FILE_STREETS);
			FilePathsMap::const_iterator it2(_pathsMap.find(FILE_PUBLIC_PLACES));
			if((it == _pathsMap.end() || it->second.empty() || !exists(it->second))
				&& (it2 == _pathsMap.end() || it2->second.empty() || !exists(it2->second)))
			{
				return false;
			}

			// OK
			return true;
		}



		bool NavstreetsFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			const std::string& key
		) const {

			DataSource& dataSource(*_import.get<DataSource>());

			// 1 : Administrative areas
			if(key == FILE_MTDAREA)
			{
				// Loading the file into SQLite as virtual table
				VirtualDBFVirtualTable table(filePath, dataSource.get<Charset>());

				map<
					pair<string,string>, // Region,Departement
					string
				> departementCodes;
				typedef map<
					string,
					City*
				> CityCodes;
				CityCodes cityCodes;

				{	// 1.1 Departements
					stringstream query;
					query << "SELECT * FROM " << table.getName() << " WHERE " << NavstreetsFileFormat::_FIELD_ADMIN_LVL << "=3";
					DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
					while(rows->next())
					{
						pair<string,string> item(
							make_pair(
								rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_2),
								rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3)
						)	);
						if(departementCodes.find(item) != departementCodes.end())
						{
							continue;
						}
						stringstream code;
						code << setw(2) << setfill('0') << rows->getInt(NavstreetsFileFormat::_FIELD_GOVT_CODE);

						departementCodes.insert(
							make_pair(item, code.str())
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
						code << departementCodes[make_pair(
							rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_2),
							rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3)
						) ];
						code << setw(3) << setfill('0') << rows->getInt(NavstreetsFileFormat::_FIELD_GOVT_CODE);
						if(_citiesMap.find(cityID) != _citiesMap.end())
						{
							continue;
						}

						boost::shared_ptr<City> city(CityTableSync::GetEditableFromCode(code.str(), _env));

						if(!city.get())
						{
							CityTableSync::SearchResult cities = CityTableSync::Search(
								_env,
								boost::optional<std::string>(), // exactname
								boost::optional<std::string>(rows->getText(NavstreetsFileFormat::_FIELD_AREA_NAME)), // likeName
								boost::optional<std::string>(),
								0, 0, true, true,
								util::UP_LINKS_LOAD_LEVEL
							);

							if(cities.empty())
							{
								_logWarning(
									"City "+ rows->getText(NavstreetsFileFormat::_FIELD_AREA_NAME) +
										" ("+ lexical_cast<string>(cityID) +") not found."
								);
								_missingCities.insert(make_pair(cityID, rows->getText(NavstreetsFileFormat::_FIELD_AREA_NAME)));
								continue;
							}
							else
							{
								city = cities.front();
							}
						}

						_citiesMap.insert(make_pair(
								cityID,
								city.get()
						)	);
						cityCodes.insert(make_pair(
								string(rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_2) + "_" + rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3 )+ "_" + rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_4)),
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
						int cityID(rows->getInt(NavstreetsFileFormat::_FIELD_AREA_ID));
						City* city = NULL;
						CityCodes::const_iterator it(cityCodes.find(
								string(rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_2) + "_" + rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3) + "_" + rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_4))
						)	);

						if(it == cityCodes.end())
						{
							CityTableSync::SearchResult cities = CityTableSync::Search(
								_env,
								boost::optional<std::string>(), // exactname
								boost::optional<std::string>(rows->getText(NavstreetsFileFormat::_FIELD_AREA_NAME)), // likeName
								boost::optional<std::string>(),
								0, 0, true, true,
								util::UP_LINKS_LOAD_LEVEL
							);

							if(cities.empty())
							{
								_logWarning(
									"City "+ rows->getText(NavstreetsFileFormat::_FIELD_AREA_NAME) +
										" ("+ lexical_cast<string>(cityID) +") not found."
								);
								_missingCities.insert(make_pair(cityID, rows->getText(NavstreetsFileFormat::_FIELD_AREA_NAME)));
								continue;
							}
							else
							{
								city = cities.front().get();
							}
						}
						else
						{
							city = it->second;
						}

						_citiesMap.insert(make_pair(
								cityID,
								city
						)	);
				}	}

			} // 2 : Streets and nodes
			else if(key == FILE_STREETS)
			{
				// Loading the file into SQLite as virtual table
				VirtualShapeVirtualTable table(
					filePath,
					dataSource.get<Charset>(),
					dataSource.get<CoordinatesSystem>()->getSRID()
				);

				typedef map<string, boost::shared_ptr<Crossing> > _CrossingsMap;
				_CrossingsMap _navteqCrossings;

				typedef map<
					string,
					City*
				> CreatedCities;
				CreatedCities createdCities;

				const GeometryFactory& geometryFactory(
					dataSource.get<CoordinatesSystem>()->getGeometryFactory()
				);

				stringstream query;
				query << "SELECT *, AsText(" << NavstreetsFileFormat::_FIELD_GEOMETRY << ") AS " << NavstreetsFileFormat::_FIELD_GEOMETRY << "_ASTEXT" << " FROM " << table.getName();
				DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
				while(rows->next())
				{
					// Fields to test if the record can be imported
					string leftId(rows->getText(NavstreetsFileFormat::_FIELD_REF_IN_ID));
					string rightId(rows->getText(NavstreetsFileFormat::_FIELD_NREF_IN_ID));
					int lAreaId(rows->getInt(NavstreetsFileFormat::_FIELD_L_AREA_ID));
					string leftRefHouseNumber(rows->getText(NavstreetsFileFormat::_FIELD_L_REFADDR));
					string leftNRefHouseNumber(rows->getText(NavstreetsFileFormat::_FIELD_L_NREFADDR));
					string rightRefHouseNumber(rows->getText(NavstreetsFileFormat::_FIELD_R_REFADDR));
					string rightNRefHouseNumber(rows->getText(NavstreetsFileFormat::_FIELD_R_NREFADDR));
					string leftAddressSchema(rows->getText(NavstreetsFileFormat::_FIELD_L_ADDRSCH));
					string rightAddressSchema(rows->getText(NavstreetsFileFormat::_FIELD_R_ADDRSCH));
					boost::shared_ptr<LineString> geometry(
						dynamic_pointer_cast<LineString, Geometry>(
						rows->getGeometryFromWKT(NavstreetsFileFormat::_FIELD_GEOMETRY+"_ASTEXT", geometryFactory)
					)	);

					if(!geometry.get())
					{
						_logWarning("ERR : Empty geometry.");
						continue;
					}

///	@todo Handle this case with aliases
//					for(size_t area(0); area< (lAreaId == rAreaId ? size_t(1) : size_t(2)); ++area)
//					{
						// Test if the record can be imported
						// City
						City* city = NULL;
						_CitiesMap::const_iterator itc(_citiesMap.find(/*area ?*/ lAreaId /*: rAreaId*/));
						if((itc == _citiesMap.end()) || !itc->second)
						{
							// Auto creation of the city
							if(_citiesAutoCreation)
							{
								// Try to find the city in _missingCities map
								_MissingCitiesMap::const_iterator itc(_missingCities.find(lAreaId));
								if(itc != _missingCities.end())
								{
									// If a city with that name has already been created
									CreatedCities::const_iterator itcc(createdCities.find(itc->second));
									if(itcc != createdCities.end())
									{
										city = itcc->second;
									}
									else
									{
										boost::shared_ptr<City> newCity = boost::shared_ptr<City>(new City);
										newCity->set<Name>(itc->second);
										newCity->set<Key>(CityTableSync::getId());
										_env.getEditableRegistry<City>().add(newCity);
										city = newCity.get();
										_citiesMap.insert(make_pair(
												lAreaId,
												city
										)	);
										createdCities.insert(make_pair(
												itc->second,
												city
										)	);
										_logCreation(
											"AUTOCREATION "+ lexical_cast<string>(itc->second) +" ("+ lexical_cast<string>(lAreaId) +")."
										);
									}
								}
							}
						}
						else
						{
							city = itc->second;
						}

						if(city == NULL)
						{
							_logWarning("City "+ lexical_cast<string>(lAreaId) +" not found.");
							continue;
						}

						// Name
						string roadName(rows->getText(NavstreetsFileFormat::_FIELD_ST_NAME));

						// Code
						string roadCode;
						roadCode = city->getName();
						roadCode += string(" ") + roadName;

						// House number bounds
						HouseNumberBounds rightHouseNumberBounds(_getHouseNumberBoundsFromAddresses(rightRefHouseNumber, rightNRefHouseNumber));
						HouseNumberBounds leftHouseNumberBounds(_getHouseNumberBoundsFromAddresses(leftRefHouseNumber, leftNRefHouseNumber));

						// House numbering policy
						HouseNumberingPolicy rightHouseNumberingPolicy(_getHouseNumberingPolicyFromAddressSchema(rightAddressSchema));
						HouseNumberingPolicy leftHouseNumberingPolicy(_getHouseNumberingPolicyFromAddressSchema(leftAddressSchema));

						// Left node
						_CrossingsMap::const_iterator ita1(_navteqCrossings.find(leftId));
						boost::shared_ptr<Crossing> leftNode;
						if(ita1 == _navteqCrossings.end())
						{
							leftNode.reset(
								new Crossing(
									CrossingTableSync::getId(),
									boost::shared_ptr<Point>(geometry->getStartPoint())
							)	);
							Importable::DataSourceLinks links;
							links.insert(make_pair(&dataSource, leftId));
							leftNode->setDataSourceLinksWithoutRegistration(links);

							_navteqCrossings.insert(make_pair(leftId, leftNode));
							_env.getEditableRegistry<Crossing>().add(leftNode);
						}
						else
						{
							leftNode = ita1->second;
						}

						// Right node
						_CrossingsMap::const_iterator ita2(_navteqCrossings.find(rightId));
						boost::shared_ptr<Crossing> rightNode;
						if(ita2 == _navteqCrossings.end())
						{
							rightNode.reset(
								new Crossing(
									CrossingTableSync::getId(),
									boost::shared_ptr<Point>(geometry->getEndPoint())
							)	);
							Importable::DataSourceLinks links;
							links.insert(make_pair(&dataSource, rightId));
							rightNode->setDataSourceLinksWithoutRegistration(links);

							_navteqCrossings.insert(make_pair(rightId, rightNode));
							_env.getEditableRegistry<Crossing>().add(rightNode);
						}
						else
						{
							rightNode = ita2->second;
						}

						// RoadPlace
						RoadPlace* roadPlace = _createOrUpdateRoadPlace(
							_roadPlaces,
							roadCode,
							roadName,
							*city
						);

						// Chunk insertion
						_addRoadChunk(
							*roadPlace,
							*leftNode,
							*rightNode,
							geometry,
							rightHouseNumberingPolicy,
							leftHouseNumberingPolicy,
							rightHouseNumberBounds,
							leftHouseNumberBounds,
							ROAD_TYPE_UNKNOWN
						);
					}
//				}
			} // 3 : Public places
			else if(key == FILE_PUBLIC_PLACES)
			{
				// Loading the file into SQLite as virtual table
				VirtualShapeVirtualTable table(
					filePath,
					dataSource.get<Charset>(),
					dataSource.get<CoordinatesSystem>()->getSRID()
				);

				const GeometryFactory& geometryFactory(dataSource.get<CoordinatesSystem>()->getGeometryFactory());

				// Public places
				ImportableTableSync::ObjectBySource<PublicPlaceTableSync> publicPlaces(dataSource, _env);
				ImportableTableSync::ObjectBySource<PublicPlaceEntranceTableSync> publicPlaceEntrances(dataSource, _env);

				stringstream query;
				query << "SELECT *, AsText(" << NavstreetsFileFormat::_FIELD_GEOMETRY << ") AS " << NavstreetsFileFormat::_FIELD_GEOMETRY << "_ASTEXT" << " FROM " << table.getName();
				DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
				while(rows->next())
				{
					// Code field
					string poiId(rows->getText(NavstreetsFileFormat::_FIELD_POI_ID));

					// Name field
					string poiName(rows->getText(NavstreetsFileFormat::_FIELD_POI_NAME));

					// House number
					optional<HouseNumber> houseNumber;
					if(rows->getInt(NavstreetsFileFormat::_FIELD_POI_ST_NUM) > 0)
					{
						houseNumber = rows->getInt(NavstreetsFileFormat::_FIELD_POI_ST_NUM);
					}

					// Street name field
					string streetName(rows->getText(NavstreetsFileFormat::_FIELD_ST_NAME));

					// Geometry
					boost::shared_ptr<Point> geometry(
						dynamic_pointer_cast<Point, Geometry>(
							rows->getGeometryFromWKT(NavstreetsFileFormat::_FIELD_GEOMETRY+"_ASTEXT", geometryFactory)
					)	);
					if(!geometry.get())
					{
						_logWarning("Empty geometry in the "+ poiId +" public place.");
						continue;
					}

					EdgeProjector<boost::shared_ptr<RoadChunk> >::From paths(
						RoadChunkTableSync::SearchByMaxDistance(
							*geometry.get(), _maxDistance,
							Env::GetOfficialEnv(), UP_LINKS_LOAD_LEVEL
					)	);

					if(!paths.empty())
					{
						EdgeProjector<boost::shared_ptr<RoadChunk> > projector(paths, _maxDistance);
						try
						{
							EdgeProjector<boost::shared_ptr<RoadChunk> >::PathNearby projection(projector.projectEdge(*geometry.get()->getCoordinate()));

							Address projectedAddress(
								*(projection.get<1>().get()),
								projection.get<2>()
							);

							const City* city = projectedAddress.getRoadChunk()->getRoad()->getAnyRoadPlace()->getCity();

							// PublicPlace
							PublicPlace* publicPlace(
								_createOrUpdatePublicPlace(
									publicPlaces,
									poiId,
									poiName,
									geometry,
									*city
							)	);

							// PublicPlaceEntrance
							RoadChunk* roadChunk = projectedAddress.getRoadChunk();

							// Metric offset + check of house number consistency
							MetricOffset metricOffset(
								houseNumber	?
								roadChunk->getHouseNumberMetricOffset(*houseNumber) :
								roadChunk->getMetricOffset()
							);

							_createOrUpdatePublicPlaceEntrance(
								publicPlaceEntrances,
								poiId,
								optional<const string&>(),
								metricOffset,
								houseNumber,
								*roadChunk,
								*publicPlace
							);
						}
						catch(EdgeProjector<boost::shared_ptr<RoadChunk> >::NotFoundException)
						{
						}
					}
				}
			}

			_logDebug("<b>SUCCESS : Data loaded</b>");

			return true;
		}



		DBTransaction NavstreetsFileFormat::Importer_::_save() const
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
			BOOST_FOREACH(const Registry<PublicPlace>::value_type& publicPlace, _env.getEditableRegistry<PublicPlace>())
			{
				PublicPlaceTableSync::Save(publicPlace.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<PublicPlaceEntrance>::value_type& publicPlaceEntrance, _env.getEditableRegistry<PublicPlaceEntrance>())
			{
				PublicPlaceEntranceTableSync::Save(publicPlaceEntrance.second.get(), transaction);
			}
			return transaction;
		}



		HouseNumberingPolicy NavstreetsFileFormat::Importer_::_getHouseNumberingPolicyFromAddressSchema(
			const std::string& addressSchema
		){
			if(addressSchema == "E") return EVEN_NUMBERS;
			if(addressSchema == "O") return ODD_NUMBERS;
			return ALL_NUMBERS;
		}



		HouseNumberBounds NavstreetsFileFormat::Importer_::_getHouseNumberBoundsFromAddresses(
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

					return HouseNumberBounds(
						pair<HouseNumber, HouseNumber>(
							lexical_cast<HouseNumber>(minAddress),
							lexical_cast<HouseNumber>(maxAddress)
					)	);
				}
				catch(bad_lexical_cast)
				{
 				}
			}
			return HouseNumberBounds();
		}



		util::ParametersMap NavstreetsFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result;
			result.insert(PARAMETER_CITIES_AUTO_CREATION, _citiesAutoCreation);
			return result;
		}



		void NavstreetsFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			_citiesAutoCreation = map.getDefault<bool>(PARAMETER_CITIES_AUTO_CREATION, false);
		}



		NavstreetsFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	impex::Importer(env, import, minLogLevel, logPath, outputStream, pm),
			impex::MultipleFileTypesImporter<NavstreetsFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			RoadFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_roadPlaces(*import.get<DataSource>(), env)
		{}
}	}
