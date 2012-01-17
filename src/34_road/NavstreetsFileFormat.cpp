
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
#include "VirtualDBFVirtualTable.hpp"
#include "AdminFunctionRequest.hpp"
#include "DataSourceAdmin.h"
#include "ImportFunction.h"
#include "PropertiesHTMLTable.h"
#include "RoadFileFormat.hpp"

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
				VirtualDBFVirtualTable table(filePath, _dataSource.getCharset());

				map<
					pair<string,string>, // Region,Departement
					string
				> departementCodes;
				typedef map<
					pair<string, string>,
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

						shared_ptr<City> city(CityTableSync::GetEditableFromCode(code.str(), _env));

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
								os << "WARN : City " << rows->getText(NavstreetsFileFormat::_FIELD_AREA_NAME) << " (" << cityID << ") not found.<br />";
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
						int cityID(rows->getInt(NavstreetsFileFormat::_FIELD_AREA_ID));
						City* city = NULL;
						CityCodes::const_iterator it(cityCodes.find(
								make_pair(rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_3), rows->getText(NavstreetsFileFormat::_FIELD_AREACODE_4))
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
								os << "WARN : City " << rows->getText(NavstreetsFileFormat::_FIELD_AREA_NAME) << " (" << cityID << ") not found.<br />";
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
				VirtualShapeVirtualTable table(filePath, _dataSource.getCharset(), _dataSource.getCoordinatesSystem()->getSRID());

				typedef map<string, shared_ptr<Crossing> > _CrossingsMap;
				_CrossingsMap _navteqCrossings;

				typedef map<
					string,
					City*
				> CreatedCities;
				CreatedCities createdCities;

				const GeometryFactory& geometryFactory(_dataSource.getCoordinatesSystem()->getGeometryFactory());

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
										shared_ptr<City> newCity = boost::shared_ptr<City>(new City);
										newCity->setName(itc->second);
										newCity->setKey(CityTableSync::getId());
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
										os << "AUTOCREATION " << itc->second << " (" << lAreaId << ").<br />";
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
							os << "ERR : City " << lAreaId << " not found.<br />";
							continue;
						}

						// Name
						string roadName(rows->getText(NavstreetsFileFormat::_FIELD_ST_NAME));

						// Code
						string roadCode;
						roadCode = city->getName();
						roadCode += string(" ") + roadName;

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

						// RoadPlace
						RoadPlace* roadPlace = RoadFileFormat::CreateOrUpdateRoadPlace(
							_roadPlaces,
							roadCode,
							roadName,
							*city,
							_dataSource,
							_env,
							os
						);

						// Chunk insertion
						RoadFileFormat::AddRoadChunk(
							*roadPlace,
							*leftNode,
							*rightNode,
							geometry,
							rightHouseNumberingPolicy,
							leftHouseNumberingPolicy,
							rightHouseNumberBounds,
							leftHouseNumberBounds,
							_env
						);
					}
//				}
			}

			os << "<b>SUCCESS : Data loaded</b><br />";

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
			stream << t.cell("Auto création des communes", t.getForm().getOuiNonRadioInput(PARAMETER_CITIES_AUTO_CREATION, false));
			stream << t.close();
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
}	}
