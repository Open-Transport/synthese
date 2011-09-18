
/** OSMFileFormat class implementation.
	@file OSMFileFormat.cpp

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

#include "OSMExpatParser.h"
#include "OSMFileFormat.hpp"
#include "AdminFunctionRequest.hpp"
#include "CityTableSync.h"
#include "Crossing.h"
#include "CrossingTableSync.hpp"
#include "DataSource.h"
#include "DataSourceAdmin.h"
#include "PropertiesHTMLTable.h"
#include "RoadPlace.h"
#include "RoadPlaceTableSync.h"
#include "RoadTableSync.h"
#include "RoadChunk.h"
#include "RoadChunkTableSync.h"
#include "RoadFileFormat.hpp"

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>

#include <geos/geom/LineString.h>
#include <geos/geom/CoordinateSequenceFactory.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese {
	using namespace admin;
	using namespace db;
	using namespace geography;
	using namespace graph;
	using namespace html;
	using namespace impex;
	using namespace osm;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, road::OSMFileFormat>::FACTORY_KEY("OpenStreetMap");
	}

	namespace road
	{
		bool OSMFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			boost::optional<const admin::AdminRequest&> request
		) const {
			NetworkPtr network;
			boost::filesystem::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);
			boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
			if(!file.good())
			{
				throw std::runtime_error("unable to open file");
			}
			std::string ext = boost::filesystem::extension(filePath);
			ExpatParser *parser = new ExpatParser();
			if(ext == ".bz2")
			{
				in.push(boost::iostreams::bzip2_decompressor());
				in.push(file);
				std::istream data(&in);
				network = parser->parse(data);
			}
			else
			{
				network = parser->parse(file);
			}

			network->consolidate(true);

			util::Log::GetInstance().info("finished parsing osm xml");

			// TODO: use a typedef
			std::map<int, std::pair<RelationPtr, std::map<int, WayPtr> > > waysByBoundaries =
					network->getWalkableWaysByAdminBoundary(8);

			util::Log::GetInstance().info("extracted ways by boundary");

			// cities, places and roads
			// TODO: move to osm module.
			typedef pair<int, std::pair<RelationPtr, std::map<int, WayPtr> > > BoundaryType;

			BOOST_FOREACH(const BoundaryType& boundary_ways, waysByBoundaries)
			{
				// insert city
				// TODO: extract into a _getOrCreateCity method.
				RelationPtr boundary = boundary_ways.second.first;
				int cityId = boundary->getId();
				std::string cityCode = boost::lexical_cast<std::string>(cityId);
				std::string cityName = boundary->getTag(Element::TAG_NAME);
				util::Log::GetInstance().info("treating ways of boundary " + cityName);
				CityTableSync::SearchResult cities = CityTableSync::Search(
					_env,
					boost::optional<std::string>(), // exactname
					boost::optional<std::string>(cityName), // likeName
					boost::optional<std::string>(),
					0, 0, true, true,
					util::UP_LINKS_LOAD_LEVEL // code
				);
				shared_ptr<City> city;

				if(cities.empty())
				{
					city = boost::shared_ptr<City>(new City);
					city->setName(cityName);
					city->setCode(cityCode);
					city->setKey(CityTableSync::getId());
					_env.getEditableRegistry<City>().add(city);
				}
				else
				{
					city = cities.front();
				}

				// The Synthese <-> OSM objects mapping is done in the following way:
				// 1:n OSM ways with the same name and on the same city -> 1 RoadPlace
				// TODO: for now only the name is considered for grouping RoadPlaces.
				// 1 OSM way -> 1 Road
				// 1:n OSM nodes between start/end/intersection node -> 1 RoadChunk

				// insert ways of city
				// TODO: move to osm module
				typedef std::pair<int, WayPtr> WayType;
				BOOST_FOREACH(const WayType& w, boundary_ways.second.second)
				{
					WayPtr way = w.second;

					shared_ptr<RoadPlace> roadPlace = _getOrCreateRoadPlace(way, city);

					// Create Road
					shared_ptr<MainRoadPart> road(new MainRoadPart(0, way->getRoadType()));
					road->setRoadPlace(*roadPlace);
					road->setKey(RoadTableSync::getId());
					_env.getEditableRegistry<MainRoadPart>().add(road);

					// TODO: move to OSM module
					typedef std::list<std::pair<int, NodePtr> > NodeList;
					const NodeList *nodes = way->getNodes();
					if(nodes->front() == nodes->back())
					{
						// TODO: is this ok?
						util::Log::GetInstance().warn("Ignoring circular way");
						continue;
					}
					if(nodes->size() < 2)
					{
						util::Log::GetInstance().warn("Ignoring way with less than 2 nodes");
						continue;
					}

					const GeometryFactory& geometryFactory(
						_dataSource.getActualCoordinateSystem().getGeometryFactory()
					);
					CoordinateSequence* cs(geometryFactory.getCoordinateSequenceFactory()->create(0, 2));
					shared_ptr<Crossing> startCrossing;
					size_t rank(0);
					MetricOffset metricOffset(0);

					BOOST_FOREACH(const NodeList::value_type& idAndNode, *nodes)
					{
						NodePtr node = idAndNode.second;

						shared_ptr<Point> point(_dataSource.getActualCoordinateSystem().createPoint(
							node->getLon(),
							node->getLat()
						));

						cs->add(*point->getCoordinate());

						if(!startCrossing.get())
						{
							startCrossing = _getOrCreateCrossing(node, point);
							continue;
						}
						bool isLast = idAndNode == nodes->back();

						if(!node->isStop() && node->numConnectedWay() <= 1 && !isLast)
						{
							// Just extend the current geometry.
							continue;
						}

						// FIXME: is this useful?
						cs->removeRepeatedPoints();

						shared_ptr<LineString> roadChunkLine(geometryFactory.createLineString(cs));

						_createRoadChunk(road, startCrossing, roadChunkLine, rank, metricOffset);

						// TODO: set {right,left}HouseNumbering{Policy,Bounds}

						// FIXME: this doesn't seem to return the result in meters.
						metricOffset += roadChunkLine->getLength();
						startCrossing = _getOrCreateCrossing(node, point);

						if(!isLast)
						{
							cs = geometryFactory.getCoordinateSequenceFactory()->create(0, 2);
							cs->add(*point->getCoordinate());
						}
						++rank;
					}

					// Add last road chunk.
					_createRoadChunk(road, startCrossing, optional<shared_ptr<LineString> >(), rank, metricOffset);
				}
			}

			util::Log::GetInstance().info("finished inserting road network");

			BOOST_FOREACH(const Registry<MainRoadPart>::value_type& road, _env.getEditableRegistry<MainRoadPart>())
			{
				road.second->validateGeometry();
			}

			util::Log::GetInstance().info("finished validating road geometries");
			return true;
		}



		OSMFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	OneFileTypeImporter<OSMFileFormat>(env, dataSource),
			Importer(env, dataSource)
		{}



		void OSMFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
			) const	{
				AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
				PropertiesHTMLTable t(reloadRequest.getHTMLForm());
				stream << t.open();
				stream << t.title("Mode");
				stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
				stream << t.title("Fichier");
				stream << t.cell("Fichier", t.getForm().getTextInput(PARAMETER_PATH, _pathsSet.empty() ? string() : _pathsSet.begin()->file_string()));
				/*
				stream << t.title("Paramètres");
				stream << t.cell("Effacer données existantes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_OLD_DATA, _cleanOldData));
				stream << t.cell("Ne pas importer données anciennes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_FROM_TODAY, _fromToday));
				stream << t.cell("Calendrier", 
					t.getForm().getSelectInput(
					PTDataCleanerFileFormat::PARAMETER_CALENDAR_ID,
					CalendarTemplateTableSync::GetCalendarTemplatesList(),
					optional<RegistryKeyType>(_calendarTemplate.get() ? _calendarTemplate->getKey() : RegistryKeyType(0))
					)	);
				*/
				stream << t.close();
		}


		util::ParametersMap OSMFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result;
			return result;
		}



		void OSMFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
		}



		db::DBTransaction OSMFileFormat::Importer_::_save() const
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



		shared_ptr<RoadPlace> OSMFileFormat::Importer_::_getOrCreateRoadPlace(
			WayPtr& way,
			shared_ptr<City> city
		) const {
			string roadName;

			// Search for a recently created road place
			if(way->hasTag(Element::TAG_NAME))
			{
				roadName = way->getTag(Element::TAG_NAME);
			}
			if(!roadName.empty())
			{
				_RecentlyCreatedRoadPlaces::iterator it(_recentlyCreatedRoadPlaces.find(roadName));
				if(it != _recentlyCreatedRoadPlaces.end())
				{
					return it->second;
				}
			}

			shared_ptr<RoadPlace> roadPlace;
			roadPlace = shared_ptr<RoadPlace>(new RoadPlace);
			roadPlace->setCity(city.get());
			roadPlace->setName(roadName);
			roadPlace->setKey(RoadPlaceTableSync::getId());
			_env.getEditableRegistry<RoadPlace>().add(roadPlace);
			if(!roadName.empty())
			{
				_recentlyCreatedRoadPlaces[roadName] = roadPlace;
			}
			return roadPlace;
		};



		/*
		 * creates or retrieves an existing crossing for a node
		 */
		shared_ptr<Crossing> OSMFileFormat::Importer_::_getOrCreateCrossing(
			NodePtr &node,
			shared_ptr<Point> position
		) const {
			_CrossingsMap::const_iterator it = _crossingsMap.find(node->getId());
			if(it != _crossingsMap.end())
			{
				return it->second;
			}

			shared_ptr<Crossing> crossing(
				new Crossing(
					CrossingTableSync::getId(),
					position,
					lexical_cast<string>(node->getId()),
					&_dataSource
			)	);

			_crossingsMap[node->getId()] = crossing;
			_env.getEditableRegistry<Crossing>().add(crossing);
			return crossing;
		}



		void OSMFileFormat::Importer_::_createRoadChunk(
			const shared_ptr<MainRoadPart> road,
			const shared_ptr<Crossing> crossing,
			const optional<shared_ptr<LineString> > geometry,
			size_t rank,
			MetricOffset metricOffset
		) const {
			shared_ptr<MainRoadChunk> roadChunk(new MainRoadChunk);
			roadChunk->setRoad(road.get());
			roadChunk->setFromCrossing(crossing.get());
			roadChunk->setRankInPath(rank);
			roadChunk->setMetricOffset(metricOffset);
			roadChunk->setKey(RoadChunkTableSync::getId());
			if(geometry)
			{
				roadChunk->setGeometry(*geometry);
			}

			road->addRoadChunk(*roadChunk);
			_env.getEditableRegistry<MainRoadChunk>().add(roadChunk);
		};
	}
}
