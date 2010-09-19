
/** StopArea class implementation.
	@file StopArea.cpp

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

#include "StopArea.hpp"
#include "Registry.h"
#include "PTModule.h"
#include "StopPoint.hpp"
#include "Edge.h"
#include "CommercialLine.h"
#include "JourneyPattern.hpp"
#include "VertexAccessMap.h"
#include "RoadModule.h"

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

	namespace util
	{
		template<> const string Registry<pt::StopArea>::KEY("StopArea");
		template<> const string FactorableTemplate<geography::NamedPlace,pt::StopArea>::FACTORY_KEY("StopArea");
	}

	namespace pt
	{
		StopArea::StopArea(
			util::RegistryKeyType id
			, bool allowedConnection/*= CONNECTION_TYPE_FORBIDDEN */
			, posix_time::time_duration defaultTransferDelay /*= FORBIDDEN_TRANSFER_DELAY  */
		):	Registrable(id),
			NamedPlaceTemplate<StopArea>(),
			_allowedConnection(allowedConnection),
			_defaultTransferDelay(defaultTransferDelay),
			_score(UNKNOWN_VALUE)
		{
		}



		void StopArea::addPhysicalStop(
			const StopPoint& physicalStop
		){
			_isoBarycentre.reset();
			_physicalStops.insert(make_pair(physicalStop.getKey(), &physicalStop));
		}



		StopArea::PhysicalStopsLabels StopArea::getPhysicalStopLabels( bool withAll /*= false*/ ) const
		{
			PhysicalStopsLabels m;
			if (withAll)
				m.push_back(make_pair(0, "(tous)"));
			for (PhysicalStops::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
				m.push_back(make_pair(it->first, it->second->getCodeBySource() + " / " + it->second->getName()));
			return m;
		}



		StopArea::PhysicalStopsLabels StopArea::getPhysicalStopLabels( const PhysicalStops& noDisplay ) const
		{
			PhysicalStopsLabels m;
			for (PhysicalStops::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
				if (noDisplay.find(it->first) == noDisplay.end())
					m.push_back(make_pair(it->first, it->second->getCodeBySource() + " / " + it->second->getName()));
			return m;
		}



		HubScore StopArea::getScore() const
		{
			typedef map<const CommercialLine*, int> ScoresMap;

			if (_score == UNKNOWN_VALUE)
			{
				if(!_allowedConnection)
				{
					_score = NO_TRANSFER_HUB_SCORE;
				} else {
					ScoresMap scores;
					BOOST_FOREACH(PhysicalStops::value_type its, _physicalStops)
					{
						BOOST_FOREACH(const Vertex::Edges::value_type& edge, its.second->getDepartureEdges())
						{
							const JourneyPattern* route(static_cast<const JourneyPattern*>(edge.first));
							ScoresMap::iterator itl(
								scores.find(route->getCommercialLine())
							);
							if (itl == scores.end())
							{
								scores.insert(make_pair(route->getCommercialLine(), route->getServices().size()));
							} else {
								itl->second += route->getServices().size();
							}
						}
					}
	
					BOOST_FOREACH(ScoresMap::value_type itc, scores)
					{
						if (itc.second <= 10)
							_score += 1;
						else if (itc.second <= 50)
							_score += 2;
						else if (itc.second <= 100)
							_score += 3;
						else
							_score += 4;
						if (_score > MAX_HUB_SCORE)
						{
							_score = MAX_HUB_SCORE;
							break;
						}
					}
					if(_score < NO_TRANSFER_HUB_SCORE)
					{
						_score = NO_TRANSFER_HUB_SCORE;
					}
				}
			}
			return _score;
		}



		void StopArea::getVertexAccessMap(
			VertexAccessMap& result,
			GraphIdType whatToSearch,
			const Vertex& origin,
			bool vertexIsOrigin
		) const {
			if(whatToSearch == RoadModule::GRAPH_ID)
			{
				/*
				if(vertexIsOrigin)
				{
					BOOST_FOREACH(const Address* address, _addresses)
					{
						if(!isConnectionAllowed(origin, *address)) continue;

						result.insert(
							address,
							VertexAccess(getTransferDelay(origin, *address))
						);
					}
				} else {
					BOOST_FOREACH(const Address* address, _addresses)
					{
						if(!isConnectionAllowed(*address, origin)) continue;

						result.insert(
							address,
							VertexAccess(getTransferDelay(*address, origin))
						);
					}
				}
				*/
			}
		    
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
			if(whatToSearch.find(RoadModule::GRAPH_ID) != whatToSearch.end())
			{
				/*
				BOOST_FOREACH(const Address* address, _addresses)
				{
					result.insert(
						address,
						VertexAccess()
					);
				}
				*/
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



		shared_ptr<Point> StopArea::getPoint() const
		{
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
	}
}
