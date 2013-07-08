
/** StopArea class implementation.
	@file StopArea.cpp

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

#include "StopArea.hpp"

#include "AllowedUseRule.h"
#include "CommercialLine.h"
#include "Crossing.h"
#include "Edge.h"
#include "Env.h"
#include "ForbiddenUseRule.h"
#include "FreeDRTArea.hpp"
#include "DRTArea.hpp"
#include "JourneyPattern.hpp"
#include "ParametersMap.h"
#include "PTModule.h"
#include "Registry.h"
#include "ReverseRoadChunk.hpp"
#include "RoadModule.h"
#include "StopPoint.hpp"
#include "VertexAccessMap.h"

#include <boost/foreach.hpp>
#include <geos/geom/Envelope.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace boost::posix_time;


namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;
	using namespace road;
	using namespace geography;

	FIELD_DEFINITION_OF_OBJECT(StopArea, "stop_area_id", "stop_area_ids")

	namespace util
	{
		template<> const string Registry<pt::StopArea>::KEY("StopArea");
		template<> const string FactorableTemplate<geography::NamedPlace,pt::StopArea>::FACTORY_KEY("StopArea");
	}

	namespace pt
	{
		const std::string StopArea::DATA_STOP_ID("stop_id");
		const std::string StopArea::DATA_STOP_NAME("stop_name");
		const std::string StopArea::DATA_CITY_ID("city_id");
		const std::string StopArea::DATA_CITY_NAME("city_name");
		const std::string StopArea::DATA_STOP_NAME_13("stop_name_13");
		const std::string StopArea::DATA_STOP_NAME_26("stop_name_26");
		const std::string StopArea::DATA_STOP_NAME_FOR_TIMETABLES("stop_name_for_timetables");
		const std::string StopArea::DATA_X("x");
		const std::string StopArea::DATA_Y("y");



		StopArea::StopArea(
			util::RegistryKeyType id
			, bool allowedConnection/*= CONNECTION_TYPE_FORBIDDEN */
			, posix_time::time_duration defaultTransferDelay /*= FORBIDDEN_TRANSFER_DELAY  */
		):	Registrable(id),
			NamedPlaceTemplate<StopArea>(),
			_allowedConnection(allowedConnection),
			_defaultTransferDelay(defaultTransferDelay)
		{
			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			setRules(rules);
		}



		void StopArea::addPhysicalStop(
			const StopPoint& physicalStop
		){
			_isoBarycentre.reset();
			_physicalStops.insert(make_pair(physicalStop.getKey(), &physicalStop));
		}



		StopArea::PhysicalStopsLabels StopArea::getPhysicalStopLabels(
			bool withAll /*= false*/,
			string noStopLabel
		) const {

			PhysicalStopsLabels m;

			// All stops item
			if (withAll)
			{
				m.push_back(make_pair(0, "(tous)"));
			}

			// Stop items
			for (PhysicalStops::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
			{
				m.push_back(make_pair(it->first, it->second->getCodeBySources() + " / " + it->second->getName()));
			}

			// No stop item
			if(!noStopLabel.empty())
			{
				m.push_back(make_pair(0, noStopLabel));
			}

			return m;
		}



		StopArea::PhysicalStopsLabels StopArea::getPhysicalStopLabels( const PhysicalStops& noDisplay ) const
		{
			PhysicalStopsLabels m;
			for (PhysicalStops::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
				if (noDisplay.find(it->first) == noDisplay.end())
					m.push_back(make_pair(it->first, it->second->getCodeBySources() + " / " + it->second->getName()));
			return m;
		}



		HubScore StopArea::getScore() const
		{
			typedef map<const CommercialLine*, int> ScoresMap;

			if(	!_score)
			{
				// Default value
				_score = NO_TRANSFER_HUB_SCORE;

				if(_allowedConnection)
				{
					ScoresMap scores;
					BOOST_FOREACH(PhysicalStops::value_type its, _physicalStops)
					{
						BOOST_FOREACH(const Vertex::Edges::value_type& edge, its.second->getDepartureEdges())
						{
							// Jump over junctions
							if(!dynamic_cast<const JourneyPattern*>(edge.second))
							{
								continue;
							}

							const JourneyPattern* route(static_cast<const JourneyPattern*>(edge.first));
							ScoresMap::iterator itl(
								scores.find(route->getCommercialLine())
							);
							if (itl == scores.end())
							{
								scores.insert(make_pair(route->getCommercialLine(), route->getServices().size()));
							}
							else
							{
								itl->second += route->getServices().size();
							}
						}
					}

					BOOST_FOREACH(ScoresMap::value_type itc, scores)
					{
						if (itc.second <= 10)
						{
							*_score += 1;
						}
						else if (itc.second <= 50)
						{
							*_score += 2;
						}
						else if (itc.second <= 100)
						{
							*_score += 3;
						}
						else
						{
							*_score += 4;
						}
						if (*_score > MAX_HUB_SCORE)
						{
							_score = MAX_HUB_SCORE;
							break;
						}
					}
				}
			}
			return *_score;
		}



		void StopArea::getVertexAccessMap(
			VertexAccessMap& result,
			GraphIdType whatToSearch,
			const Vertex& origin,
			bool vertexIsOrigin
		) const {

			if (whatToSearch != PTModule::GRAPH_ID) return;

			if(vertexIsOrigin)
			{
				BOOST_FOREACH(
					const PhysicalStops::value_type& it,
					_physicalStops
				){

					if(!isConnectionAllowed(origin, *it.second)) continue;

					result.insert(
						it.second,
						VertexAccess(getTransferDelay(origin, *it.second))
					);
				}
			} else {
				BOOST_FOREACH(
					const PhysicalStops::value_type& it,
					_physicalStops
				){

					if(!isConnectionAllowed(*it.second, origin)) continue;

					result.insert(
						it.second,
						VertexAccess(getTransferDelay(*it.second, origin))
					);
				}
			}
		}



		void StopArea::getVertexAccessMap(
			VertexAccessMap& result,
			const AccessParameters& accessParameters,
			const geography::Place::GraphTypes& whatToSearch
		) const {
			/*
			 * If StopArea isn't in a DRTArea, then attempt to use crossings arround stop.
			 * Else AVOID IT : if user want to start from a stopArea (to make a reservation) we musn't change the starting stopArea without notification !!
			 */
			if(whatToSearch.find(RoadModule::GRAPH_ID) != whatToSearch.end() && !isInDRT())
			{
				BOOST_FOREACH(
					const PhysicalStops::value_type& it,
					_physicalStops
				){
					if(!it.second->getProjectedPoint().getRoadChunk())
					{
						continue;
					}
					result.insert(
						it.second->getProjectedPoint().getRoadChunk()->getFromCrossing(),
						VertexAccess(minutes(static_cast<long>(it.second->getProjectedPoint().getMetricOffset() / 50)), it.second->getProjectedPoint().getMetricOffset())
					);
					/*
					 * If next edge exist try add next crossing to vam (see issue #23315)
					 */
					if(it.second->getProjectedPoint().getRoadChunk()->getNext())
					{
						result.insert(
                            				it.second->getProjectedPoint().getRoadChunk()->getNext()->getFromVertex(),
                            				VertexAccess(
                                				minutes(static_cast<long>(ceil(((it.second->getProjectedPoint().getRoadChunk()->getEndMetricOffset() - it.second->getProjectedPoint().getRoadChunk()->getMetricOffset() - it.second->getProjectedPoint().getMetricOffset()) / 50.0)))),
                                				it.second->getProjectedPoint().getRoadChunk()->getEndMetricOffset() - it.second->getProjectedPoint().getRoadChunk()->getMetricOffset() - it.second->getProjectedPoint().getMetricOffset()
							)	);
					}
				}
			}

			if (whatToSearch.find(PTModule::GRAPH_ID) == whatToSearch.end()) return;

			BOOST_FOREACH(
				const PhysicalStops::value_type& it,
				_physicalStops
			){
				result.insert(
					it.second,
					VertexAccess()
				);
			}
		}



		const boost::shared_ptr<Point>& StopArea::getPoint() const
		{
			if (_location.get())
			{
				return _location;
			}
			if (!_isoBarycentre.get())
			{
				Envelope e;
				BOOST_FOREACH(const PhysicalStops::value_type& it, _physicalStops)
				{
					if(it.second->hasGeometry())
					{
						e.expandToInclude(*it.second->getGeometry()->getCoordinate());
					}
				}
				if(!e.isNull())
				{
					Coordinate c;
					e.centre(c);
					_isoBarycentre.reset(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(c));
				}
			}
			return _isoBarycentre;
		}



		bool StopArea::containsAnyVertex(
			GraphIdType graphType
		) const	{
			if(graphType == PTModule::GRAPH_ID)
			{
				return !_physicalStops.empty();
			}
			if(graphType == RoadModule::GRAPH_ID)
			{
//				return !_addresses.empty(); @todo Station entrances
			}
			return false;
		}



		std::string StopArea::getNameForAllPlacesMatcher(
			std::string text
		) const	{
			return (text.empty() ? getName() : text);
		}



		bool StopArea::isConnectionPossible() const
		{
			return _allowedConnection;
		}



		bool StopArea::isConnectionAllowed( const graph::Vertex& fromVertex , const graph::Vertex& toVertex ) const
		{
			return !getTransferDelay(fromVertex, toVertex).is_not_a_date_time();
		}



		boost::posix_time::time_duration StopArea::getTransferDelay(
			const graph::Vertex& fromVertex,
			const graph::Vertex& toVertex
		) const	{
			TransferDelaysMap::const_iterator it(
				_transferDelays.find(make_pair(fromVertex.getKey(), toVertex.getKey()))
			);

			// If not defined in map, return default transfer delay
			return (it == _transferDelays.end ()) ? _defaultTransferDelay : it->second;
		}



		boost::posix_time::time_duration StopArea::getMinTransferDelay() const
		{
			if (_minTransferDelay.is_not_a_date_time())
			{
				_minTransferDelay = _defaultTransferDelay;
				for (TransferDelaysMap::const_iterator it(_transferDelays.begin()); it != _transferDelays.end(); ++it)
					if (it->second < _minTransferDelay)
						_minTransferDelay = it->second;
			}
			return _minTransferDelay;
		}



		void StopArea::addTransferDelay(
			TransferDelaysMap::key_type::first_type fromVertex,
			TransferDelaysMap::key_type::second_type toVertex,
			boost::posix_time::time_duration transferDelay
		){
			assert(transferDelay >= minutes(0) && !transferDelay.is_not_a_date_time());

			_transferDelays[std::make_pair (fromVertex, toVertex)] = transferDelay;
			_minTransferDelay = posix_time::time_duration(not_a_date_time);
		}



		void StopArea::addForbiddenTransferDelay(
			TransferDelaysMap::key_type::first_type fromVertex,
			TransferDelaysMap::key_type::second_type toVertex
		){
			_transferDelays[std::make_pair (fromVertex, toVertex)] = posix_time::time_duration(not_a_date_time);
			_minTransferDelay = posix_time::time_duration(not_a_date_time);
		}



		void StopArea::clearTransferDelays()
		{
			_transferDelays.clear ();
			_defaultTransferDelay = posix_time::time_duration(not_a_date_time);
			_minTransferDelay = posix_time::time_duration(not_a_date_time);
		}



		void StopArea::setDefaultTransferDelay( boost::posix_time::time_duration defaultTransferDelay )
		{
			assert(defaultTransferDelay >= minutes(0) && !defaultTransferDelay.is_not_a_date_time());

			_defaultTransferDelay = defaultTransferDelay;
			_minTransferDelay = posix_time::time_duration(not_a_date_time);
		}



		void StopArea::removePhysicalStop( const pt::StopPoint& physicalStop )
		{
			_isoBarycentre.reset();
			_physicalStops.erase(physicalStop.getKey());
		}



		void StopArea::removeTransferDelay( TransferDelaysMap::key_type::first_type departure, TransferDelaysMap::key_type::second_type arrival )
		{
			TransferDelaysMap::iterator it(
				_transferDelays.find(std::make_pair (departure, arrival))
			);
			if(it != _transferDelays.end())
			{
				_transferDelays.erase(it);
			}
		}



		StopArea::Lines StopArea::getLines(
			bool includeArrivals
		) const {
			Lines result;
			BOOST_FOREACH(const PhysicalStops::value_type& itStop, _physicalStops)
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, itStop.second->getDepartureEdges())
				{
					if(!dynamic_cast<const JourneyPattern*>(itEdge.first))
					{
						continue;
					}
					CommercialLine* line(
						static_cast<const JourneyPattern*>(itEdge.first)->getCommercialLine()
					);
					if(line)
					{
						result.insert(line);
					}
				}
				if(includeArrivals)
				{
					BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, itStop.second->getArrivalEdges())
					{
						if(!dynamic_cast<const JourneyPattern*>(itEdge.first))
						{
							continue;
						}
						{
							result.insert(static_cast<const JourneyPattern*>(itEdge.first)->getCommercialLine());
						}
					}
				}
			}
			return result;
		}



		void StopArea::toParametersMap(
			util::ParametersMap& pm,
			const CoordinatesSystem* coordinatesSystem,
			string prefix
		) const {
			pm.insert(prefix + DATA_STOP_ID, getKey());
			pm.insert(prefix + "id", getKey()); // For StopAreasList compatibility
			pm.insert(prefix + DATA_STOP_NAME, getName());
			pm.insert(prefix + "name", getName()); // For StopAreasList compatibility
			pm.insert(prefix + DATA_STOP_NAME_13, getName13());
			pm.insert(prefix + DATA_STOP_NAME_26, getName26());
			pm.insert(prefix + "directionAlias", getName26()); // For StopAreasList compatibility
			pm.insert(prefix + DATA_STOP_NAME_FOR_TIMETABLES, getTimetableName());
			if(getCity())
			{
				pm.insert(prefix + DATA_CITY_ID, getCity()->getKey());
				pm.insert(prefix + "cityId", getCity()->getKey()); // For StopAreasList compatibility
				pm.insert(prefix + DATA_CITY_NAME, getCity()->getName());
				pm.insert(prefix + "cityName", getCity()->getName()); // For StopAreasList compatibility
			}
			if(coordinatesSystem && getPoint())
			{
				boost::shared_ptr<Point> pg(
					coordinatesSystem->convertPoint(*getPoint())
				);
				{
					stringstream s;
					s << std::fixed << pg->getX();
					pm.insert(prefix + DATA_X, s.str());
				}
				{
					stringstream s;
					s << std::fixed << pg->getY();
					pm.insert(prefix + DATA_Y, s.str());
				}
				pm.setGeometry(static_pointer_cast<Geometry,Point>(getPoint()));
			}
		}



		void StopArea::toParametersMap( util::ParametersMap& pm, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			toParametersMap(
				pm,
				&CoordinatesSystem::GetInstanceCoordinatesSystem(),
				prefix
			);
		}



		std::string StopArea::getRuleUserName() const
		{
			return "Zone d'arrêt";
		}



		StopArea::FreeDRTs StopArea::getFreeDRTs() const
		{
			FreeDRTs result;
			BOOST_FOREACH(const FreeDRTArea::Registry::value_type& item, Env::GetOfficialEnv().getRegistry<FreeDRTArea>())
			{
				if(item.second->includesPlace(*this))
				{
					result.insert(item.second.get());
				}
			}
			return result;
		}

		bool StopArea::isInDRT() const
		{
			BOOST_FOREACH(const DRTArea::Registry::value_type& item, Env::GetOfficialEnv().getRegistry<DRTArea>())
			{
				if(item.second->contains(*this))
					return true;
			}
			return false;
		}

		Hub::Vertices StopArea::getVertices(
			GraphIdType graphId
		) const	{
			Vertices result;
			if(graphId == PTModule::GRAPH_ID)
			{
				BOOST_FOREACH(const PhysicalStops::value_type& it, _physicalStops)
				{
					result.push_back(it.second);
				}
			}
			return result;
		}
}	}
