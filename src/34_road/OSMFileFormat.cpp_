
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

#include "OSMFileFormat.hpp"
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
#include "SQLiteTransaction.h"
#include "Log.h"
#include "LineStopProjector.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/fstream.hpp>
#include <map>

#include <iostream>
#include <fstream>
#include "OSMElements.h"
#include "OSMExpatParser.h"
#include <geos/geom/Geometry.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <iconv.h>

#include "shapefil.h"

namespace synthese {

using namespace util;
using namespace impex;
using namespace road;
using namespace env;
using namespace pt;
using namespace geography;
using namespace db;
using namespace geometry;
using namespace graph;
using boost::shared_ptr;
using std::list;
using std::pair;
using geos::geom::Geometry;

namespace util
{
	template<> const std::string FactorableTemplate<synthese::impex::FileFormat,synthese::osm::OSMFileFormat>::FACTORY_KEY("OpenStreetMap");
}

namespace impex
{
	template<> const FileFormat::Files FileFormatTemplate<synthese::osm::OSMFileFormat>::FILES(
			""
	);
}

namespace osm {
	OSMFileFormat::OSMFileFormat( util::Env* env /* = NULL */ )
	{
		_env = env;
		dstProjection = pj_init_plus("+proj=lcc +lat_1=46.8 +lat_0=46.8 +lon_0=0 +k_0=0.99987742 +x_0=600000 +y_0=2200000 +a=6378249.2 +b=6356515 +towgs84=-168,-60,320,0,0,0,0 +pm=paris +units=m +no_defs");
	}

	OSMFileFormat::~OSMFileFormat()
	{}

	std::string OSMFileFormat::utf8ToLatin1(std::string &text) {
		static iconv_t utf8tolatin1 = iconv_open("latin1","utf8");
		size_t textLength = text.size();
		char *isoName = new char[textLength+1];
		char *isoNamePtr = isoName;
		size_t isoLength = textLength+1;
		char *utf8Name = const_cast<char*>(text.c_str());
		size_t utf8Length = textLength+1;
		iconv(utf8tolatin1, &utf8Name, &utf8Length, &isoName, &isoLength);
		std::string ret(isoNamePtr);
		delete isoNamePtr;
		return ret;
	}

	void OSMFileFormat::build(std::ostream& os)
	{}

	void OSMFileFormat::save(std::ostream& os
		) const {
		SQLiteTransaction transaction;
		BOOST_FOREACH(Registry<City>::value_type city, _env->getEditableRegistry<City>())
		{
			CityTableSync::Save(city.second.get(), transaction);
		}
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

		SHPHandle   hSHP = SHPCreate( "/tmp/osmimport", SHPT_ARC );
		BOOST_FOREACH(Registry<Road>::value_type road, _env->getEditableRegistry<Road>())
		{
			const geos::geom::LineString* geom = (geos::geom::LineString*)(road.second->getGeometry().get());
			double *x,*y;
			int nVertices = geom->getNumPoints();
			x=(double*)malloc(nVertices*sizeof(double));
			y=(double*)malloc(nVertices*sizeof(double));
			for(int i=0;i<nVertices;i++) {
				geos::geom::Point* vertice = geom->getPointN(i);
				x[i] = vertice->getX();
				y[i] = vertice->getY();
			}
			SHPObject *ll = SHPCreateSimpleObject( SHPT_ARC, nVertices, x,y, NULL);
			SHPWriteObject( hSHP, -1, ll );
			SHPDestroyObject( ll );
			free(x);
			free(y);

			RoadTableSync::Save(road.second.get(),transaction);
		}
		SHPClose(hSHP);
		BOOST_FOREACH(Registry<RoadChunk>::value_type roadChunk, _env->getEditableRegistry<RoadChunk>())
		{
			RoadChunkTableSync::Save(roadChunk.second.get(),transaction);
		}
		transaction.run();
	}

	void OSMFileFormat::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& os,
			std::string key ){

		NetworkPtr network;
		boost::filesystem::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);
		boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
		if(!file.good()) {
			throw std::runtime_error("unable to open file");
		}
		std::string ext = boost::filesystem::extension(filePath);
		ExpatParser *parser = new ExpatParser();
		if(ext == ".bz2") {
			in.push(boost::iostreams::bzip2_decompressor());
			in.push(file);
			std::istream data(&in);
			network = parser->parse(data);
		} else {
			network = parser->parse(file);
		}

		network->consolidate(true);

		util::Log::GetInstance().info("finished parsing osm xml");

		std::map<int,std::pair<RelationPtr,std::map<int, WayPtr> > > waysByBoundaries =
				network->getWalkableWaysByAdminBoundary(8);

		util::Log::GetInstance().info("extracted ways by boundary");


		//cities, places and roads
		typedef pair<int,std::pair<RelationPtr,std::map<int, WayPtr> > > boundaryType;


		BOOST_FOREACH(boundaryType boundary_ways, waysByBoundaries) {
			//insert city
			RelationPtr boundary = boundary_ways.second.first;

			int cityId = boundary->getId();
			std::string cityCode = boost::lexical_cast<std::string>(cityId);
			std::string cityName = boundary->getTag(Element::TAG_NAME);
			util::Log::GetInstance().info("treating ways of boundary "+cityName);
			CityTableSync::SearchResult cities = CityTableSync::Search(
					*_env,
					boost::optional<std::string>(), //exactname
					boost::optional<std::string>(utf8ToLatin1(cityName)), //likeName
					boost::optional<std::string>(),
					0,0,true,true,
					util::UP_LINKS_LOAD_LEVEL//code
					);
			//shared_ptr<City> city = CityTableSync::GetEditableFromName(utf8ToLatin1(cityName),*_env);
			shared_ptr<City> city;

			if(!cities.size()) {
				city = boost::shared_ptr<City>(new City);
				city->setName(utf8ToLatin1(cityName));
				city->setCode(cityCode);
				city->setKey(CityTableSync::getId());
				_env->getEditableRegistry<City>().add(city);
			} else {
				city = cities.front();
			}

			typedef std::map<std::string, boost::shared_ptr<RoadPlace> > RecentlyCreatedRoadPlaces;
			RecentlyCreatedRoadPlaces recentlyCreatedRoadPlaces;

			//insert ways of city
			typedef std::pair<int, WayPtr> wayType;
			BOOST_FOREACH(wayType w, boundary_ways.second.second) {
				WayPtr way = w.second;
				shared_ptr<RoadPlace> roadPlace;
				std::string roadName = "";
				// Search for a recently created road place
				if(way->hasTag(Element::TAG_NAME)) {
					roadName = way->getTag(Element::TAG_NAME);
					roadName = utf8ToLatin1(roadName);
				}
				if(roadName != "") {
					RecentlyCreatedRoadPlaces::iterator it(recentlyCreatedRoadPlaces.find(roadName));
					if (it != recentlyCreatedRoadPlaces.end()) {
						roadPlace = it->second;
					}
				}

				if(!roadPlace) {
					roadPlace = boost::shared_ptr<RoadPlace>(new RoadPlace);
					roadPlace->setCity(city.get());
					roadPlace->setName(roadName);
					roadPlace->setKey(RoadPlaceTableSync::getId());
					_env->getEditableRegistry<RoadPlace>().add(roadPlace);
					if(roadName != "")
						recentlyCreatedRoadPlaces[roadName] = roadPlace;
				}

				const std::list<std::pair<int,NodePtr> > *nodes = way->getNodes();
				//if(nodes->front()==nodes->back()) continue;
				std::list<std::pair<int,NodePtr> >::const_iterator itn = nodes->begin(), last = --(nodes->end());
				double chunkLength(0.0);
				NodePtr prevNode;
				Point2D prevPnt;
				std::list<Point2D> viaPoints;
				shared_ptr<Address> start, end;
				while(itn != nodes->end()) {
					NodePtr node = itn->second;
					double lon = node->getLon()*DEG_TO_RAD, lat = node->getLat()*DEG_TO_RAD;
					pj_transform(Element::originalProjection, dstProjection,1,1,&lon,&lat, NULL);
					Point2D pnt(lon,lat);
					if(!prevNode) {
						start = lookupOrCreateAddress(node);
					} else {
						chunkLength += pnt.getDistanceTo(prevPnt);
						if(node->isStop() || node->numConnectedWay() > 1 || itn == last) {
							end = lookupOrCreateAddress(node);
							insertRoadChunk(start, end, chunkLength, roadPlace, viaPoints, way->getRoadType());
							start = end;
							viaPoints.clear();
							chunkLength = 0.0;
						} else {
							viaPoints.push_back(pnt);
						}
					}
					prevNode = node;
					prevPnt.setXY(lon,lat);
					++itn;
				}
			}

#if 0
			// project physical stops onto created network
			//TODO only extract stops for current city
			PhysicalStopTableSync::SearchResult stops = PhysicalStopTableSync::Search(*_env);
			LineStopProjector lsp(_env);
			BOOST_FOREACH(boost::shared_ptr<PhysicalStop> ps, stops) {

				//TODO: see above
				if(!boost::iequals(
						ps->getConnectionPlace()->getCity()->getName(),
						city->getName())) continue;

				if(ps->getKey()==3377699722035359) {
					boost::shared_ptr<PhysicalStop> pscopy = ps;
					Log::GetInstance().info(pscopy->getName());
				}
				lsp.projectLineStop(ps);
			}
#endif

		}

		util::Log::GetInstance().info("finished inserting road network");

		BOOST_FOREACH(Registry<Road>::value_type road, _env->getEditableRegistry<Road>())
		{
			road.second->validateGeometry();

		}

		util::Log::GetInstance().info("finished validating road geometries");

		// project physical stops onto created network
		PhysicalStopTableSync::SearchResult stops = PhysicalStopTableSync::Search(*_env, boost::optional<RegistryKeyType>(),boost::optional<std::string>(),0,boost::optional<std::size_t>(), UP_LINKS_LOAD_LEVEL);

		LineStopProjector lsp(_env);

		BOOST_FOREACH(boost::shared_ptr<PhysicalStop> ps, stops) {
			Log::GetInstance().info("projecting stop "+ps->getName());
			lsp.projectLineStop(ps);
		}



	}

	void OSMFileFormat::insertRoadChunk(shared_ptr<Address> &start, shared_ptr<Address> &end,
			double length, shared_ptr<RoadPlace> &roadPlace, std::list<Point2D> &via_points, Road::RoadType roadType ) {


		Address *leftNode = start.get(), *rightNode=end.get();
		// Search for an existing road which ends at the left node
		Road* road(NULL);
		double startMetricOffset(0);
		BOOST_FOREACH(Path* croad, roadPlace->getPaths())
		{
			if(croad->getLastEdge()->getFromVertex() == leftNode)
			{
				road = static_cast<Road*>(croad);
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
			assert(road->getLastEdge()->getViaPoints().empty());
			BOOST_FOREACH(Point2D pt, via_points) {
				road->getLastEdge()->addViaPoint(pt);
			}
			secondRoadChunk->setRankInPath((*(road->getEdges().end()-1))->getRankInPath() + 1);
			secondRoadChunk->setMetricOffset(startMetricOffset + length);
			secondRoadChunk->setKey(RoadChunkTableSync::getId());
			road->addRoadChunk(secondRoadChunk.get());
			_env->getEditableRegistry<RoadChunk>().add(secondRoadChunk);

			// Search for a second existing road which starts at the right node
			Road* road2 = NULL;
			BOOST_FOREACH(Path* croad, roadPlace->getPaths())
			{
				if(croad != road && croad->getEdge(0)->getFromVertex() == rightNode)
				{
				  /*
					//loop through the edges to see if rightNode is already included
					const std::vector<Edge*> edges = croad->getEdges();
					unsigned int i;
					for(i=1;i<edges.size();i++) {
						if(croad->getEdge(i)->getFromVertex() == rightNode)
							break;
					}
					if(i != edges.size()) //rightNode is already included, skip
						continue;
*/
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
				if(croad->getEdge(0)->getFromVertex() == rightNode)
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
				BOOST_FOREACH(Point2D pt, via_points) {
					firstRoadChunk->addViaPoint(pt);
				}
				firstRoadChunk->setFromAddress(leftNode);
				firstRoadChunk->setRankInPath(0);
				firstRoadChunk->setMetricOffset(0);
				firstRoadChunk->setKey(RoadChunkTableSync::getId());
				road->addRoadChunk(firstRoadChunk.get(), length);
				_env->getEditableRegistry<RoadChunk>().add(firstRoadChunk);
			}
			else
			{
				shared_ptr<Road> road(new Road(UNKNOWN_VALUE, roadType, false));
				road->setRoadPlace(roadPlace.get());
				roadPlace->addRoad(*road);
				road->setKey(RoadTableSync::getId());
				_env->getEditableRegistry<Road>().add(road);

				// First road chunk
				shared_ptr<RoadChunk> firstRoadChunk(new RoadChunk);
				firstRoadChunk->setRoad(road.get());
				BOOST_FOREACH(Point2D pt, via_points) {
					firstRoadChunk->addViaPoint(pt);
				}
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

	/*
	 * creates or retrieves an existing address for a node
	 */
	boost::shared_ptr<Address> OSMFileFormat::lookupOrCreateAddress(NodePtr &node) {

		_AddressesMap::iterator it = _addressesMap.find(node->getId());
		if(it != _addressesMap.end()) {
			return it->second;
		}

		shared_ptr<Address>  address = shared_ptr<Address>(new Address);

		address->setCodeBySource(boost::lexical_cast<std::string>(node->getId()));

		double lon = node->getLon()*DEG_TO_RAD, lat = node->getLat()*DEG_TO_RAD;
		pj_transform(Element::originalProjection, dstProjection,1,1,&lon,&lat, NULL);
		address->setXY(lon, lat);
		address->setDataSource(_dataSource);
		address->setKey(AddressTableSync::getId());
		//if (node->isStop()) {
//
		//} else {
			//crossing
			shared_ptr<Crossing> crossing(new Crossing);
			crossing->setKey(util::encodeUId(43, 0, 0, decodeObjectId(address->getKey())));
			crossing->setAddress(address.get());
			address->setHub(crossing.get());
			_env->getEditableRegistry<Crossing> ().add(crossing);
		//}
		_addressesMap[node->getId()] = address;
		_env->getEditableRegistry<Address> ().add(address);
		return address;
	}

}}
