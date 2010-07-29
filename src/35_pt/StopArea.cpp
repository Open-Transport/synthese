
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
#include "PhysicalStop.h"
#include "Edge.h"
#include "CommercialLine.h"
#include "JourneyPattern.hpp"
#include "VertexAccessMap.h"
#include "Address.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

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
		):	AddressablePlace(allowedConnection, defaultTransferDelay),
			Registrable(id),
			NamedPlaceTemplate<StopArea>()
			, _score(UNKNOWN_VALUE)
		{

		}

		const StopArea::PhysicalStops& StopArea::getPhysicalStops() const
		{
			return _physicalStops;
		}

		void StopArea::addPhysicalStop(
			const PhysicalStop& physicalStop
		){
			_isoBarycentreToUpdate = true;
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
			AddressablePlace::getVertexAccessMap(
				result, whatToSearch, origin, vertexIsOrigin
			);
		    
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
			AddressablePlace::getVertexAccessMap(
				result, accessParameters
				, whatToSearch
			);
		    
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

		const geometry::Point2D& StopArea::getPoint() const
		{
			if (_isoBarycentreToUpdate)
			{
				_isoBarycentre.clear();
				BOOST_FOREACH(const PhysicalStops::value_type& it, _physicalStops)
				{
					_isoBarycentre.add(*it.second);
				}
				BOOST_FOREACH(const Address* address, _addresses)
				{
					_isoBarycentre.add(*address);
				}
				_isoBarycentreToUpdate = false;
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
			return AddressablePlace::containsAnyVertex(graphType);
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
	}
}
