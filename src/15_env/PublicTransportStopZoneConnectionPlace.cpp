
/** PublicTransportStopZoneConnectionPlace class implementation.
	@file PublicTransportStopZoneConnectionPlace.cpp

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

#include "PublicTransportStopZoneConnectionPlace.h"
#include "Registry.h"

#include "PhysicalStop.h"
#include "Edge.h"
#include "CommercialLine.h"
#include "Line.h"
#include "VertexAccessMap.h"
#include "Address.h"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const std::string Registry<env::PublicTransportStopZoneConnectionPlace>::KEY("PublicTransportStopZoneConnectionPlace");
	}

	namespace env
	{
		PublicTransportStopZoneConnectionPlace::PublicTransportStopZoneConnectionPlace(
			util::RegistryKeyType id /*= UNKNOWN_VALUE */
			, std::string name /*= std::string() */
			, const City* city /*= NULL */
			, bool allowedConnection/*= CONNECTION_TYPE_FORBIDDEN */
			, MinutesDuration defaultTransferDelay /*= FORBIDDEN_TRANSFER_DELAY  */
		):	AddressablePlace(name, city),
			Registrable(id)
			, _defaultTransferDelay (defaultTransferDelay)
			, _minTransferDelay (UNKNOWN_VALUE)
			, _score(UNKNOWN_VALUE),
			_allowedConnection(allowedConnection)
		{

		}

		const PhysicalStops& PublicTransportStopZoneConnectionPlace::getPhysicalStops() const
		{
			return _physicalStops;
		}

		MinutesDuration PublicTransportStopZoneConnectionPlace::getDefaultTransferDelay(
		) const {
			return _defaultTransferDelay;
		}

		MinutesDuration PublicTransportStopZoneConnectionPlace::getMinTransferDelay() const
		{
			if (_minTransferDelay == UNKNOWN_VALUE)
			{
				_minTransferDelay = _defaultTransferDelay;
				for (TransferDelaysMap::const_iterator it(_transferDelays.begin()); it != _transferDelays.end(); ++it)
					if (it->second < _minTransferDelay)
						_minTransferDelay = it->second;
			}
			return _minTransferDelay;
		}



		void PublicTransportStopZoneConnectionPlace::setDefaultTransferDelay(
			MinutesDuration defaultTransferDelay
		){
			assert(defaultTransferDelay >= 0 && defaultTransferDelay <= FORBIDDEN_TRANSFER_DELAY);

			_defaultTransferDelay = defaultTransferDelay;
			_minTransferDelay = UNKNOWN_VALUE;
		}

		void PublicTransportStopZoneConnectionPlace::setAllowedConnection(
			bool value
		){
			_allowedConnection = value;
		}

		void PublicTransportStopZoneConnectionPlace::addPhysicalStop( const PhysicalStop* physicalStop )
		{
			_isoBarycentreToUpdate = true;
			_physicalStops.insert(make_pair(physicalStop->getKey(),physicalStop));
		}

		void PublicTransportStopZoneConnectionPlace::addTransferDelay(
			uid departureId,
			uid arrivalId,
			MinutesDuration transferDelay
		){
			assert(transferDelay >= 0 && transferDelay <= FORBIDDEN_TRANSFER_DELAY);

			_transferDelays[std::make_pair (departureId, arrivalId)] = transferDelay;
			_minTransferDelay = UNKNOWN_VALUE;
		}



		void PublicTransportStopZoneConnectionPlace::clearTransferDelays()
		{
			_transferDelays.clear ();
			_defaultTransferDelay = Hub::FORBIDDEN_TRANSFER_DELAY;
			_minTransferDelay = UNKNOWN_VALUE;
		}

		bool PublicTransportStopZoneConnectionPlace::isConnectionAllowed(
			const Vertex* fromVertex,
			const Vertex* toVertex
		) const {
			if(!_allowedConnection) return false;

			return getTransferDelay(fromVertex,	toVertex) != Hub::FORBIDDEN_TRANSFER_DELAY;
		}

		MinutesDuration PublicTransportStopZoneConnectionPlace::getTransferDelay(
			const Vertex* fromVertex,
			const Vertex* toVertex
		) const {
			TransferDelaysMap::const_iterator it(_transferDelays.find(make_pair(fromVertex->getKey(),toVertex->getKey())));

			// If not defined in map, return default transfer delay
			return (it == _transferDelays.end ()) ? _defaultTransferDelay : it->second;
		}


		PublicTransportStopZoneConnectionPlace::PhysicalStopsLabels PublicTransportStopZoneConnectionPlace::getPhysicalStopLabels( bool withAll /*= false*/ ) const
		{
			PhysicalStopsLabels m;
			if (withAll)
				m.push_back(make_pair(0, "(tous)"));
			for (PhysicalStops::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
				m.push_back(make_pair(it->first, it->second->getOperatorCode() + " / " + it->second->getName()));
			return m;
		}

		PublicTransportStopZoneConnectionPlace::PhysicalStopsLabels PublicTransportStopZoneConnectionPlace::getPhysicalStopLabels( const PhysicalStops& noDisplay ) const
		{
			PhysicalStopsLabels m;
			for (PhysicalStops::const_iterator it = _physicalStops.begin(); it != _physicalStops.end(); ++it)
				if (noDisplay.find(it->first) == noDisplay.end())
					m.push_back(make_pair(it->first, it->second->getOperatorCode() + " / " + it->second->getName()));
			return m;
		}

		HubScore PublicTransportStopZoneConnectionPlace::getScore() const
		{
			if (_score == UNKNOWN_VALUE)
			{
				if(!_allowedConnection)
				{
					_score = 0;
				} else {
					map<const CommercialLine*, int> scores;
					BOOST_FOREACH(PhysicalStops::value_type its, _physicalStops)
					{
						BOOST_FOREACH(const Edge* edge, its.second->getDepartureEdges())
						{
							const Line* route(static_cast<const Line*>(edge->getParentPath()));
							map<const CommercialLine*, int>::iterator itl(
								scores.find(route->getCommercialLine())
							);
							if (itl == scores.end())
							{
								scores.insert(make_pair(route->getCommercialLine(), route->getServices().size()));
							} else {
								itl->second += route->getServices().size();
							}
						}
	
						for (map<const CommercialLine*, int>::const_iterator itc(scores.begin()); itc != scores.end(); ++itc)
						{
							if (itc->second <= 10)
								_score += 2;
							else if (itc->second <= 50)
								_score += 3;
							else if (itc->second <= 100)
								_score += 4;
							else
								_score += 5;
							if (_score > MAX_HUB_SCORE)
							{
								_score = MAX_HUB_SCORE;
								break;
							}
						}
					}
				}
			}
			return _score;
		}

		void PublicTransportStopZoneConnectionPlace::getImmediateVertices(
			VertexAccessMap& result,
			const AccessDirection& accessDirection,
			const AccessParameters& accessParameters,
			SearchAddresses returnAddresses,
			SearchPhysicalStops returnPhysicalStops,
			const Vertex* origin /*= NULL */
		) const {
			AddressablePlace::getImmediateVertices(
				result, accessDirection, accessParameters
				, returnAddresses, returnPhysicalStops
				, origin
			);
		    
			if (returnPhysicalStops == SEARCH_PHYSICALSTOPS)
			{
				for(PhysicalStops::const_iterator it(_physicalStops.begin());
					it != _physicalStops.end();
					++it
				){
					result.insert(
						it->second
						, getVertexAccess(
							accessDirection
							, accessParameters
							, it->second
							, origin
						)
					);
				}
			}
		}

		const geometry::Point2D& PublicTransportStopZoneConnectionPlace::getPoint() const
		{
			if (_isoBarycentreToUpdate)
			{
				_isoBarycentre.clear();
				for (PhysicalStops::const_iterator it(_physicalStops.begin()); it != _physicalStops.end(); ++it)
					_isoBarycentre.add(*it->second);
				for (Addresses::const_iterator it(_addresses.begin()); it != _addresses.end(); ++it)
					_isoBarycentre.add(**it);
				_isoBarycentreToUpdate = false;
			}
			return _isoBarycentre;
		}

		bool PublicTransportStopZoneConnectionPlace::hasPhysicalStops() const
		{
			return !_physicalStops.empty();
		}
		
				
		
		VertexAccess PublicTransportStopZoneConnectionPlace::getVertexAccess(
			const AccessDirection& accessDirection,
			const AccessParameters& accessParameters,
			const Vertex* destination,
			const Vertex* origin
		) const	{
			VertexAccess access(accessDirection);

			if (origin != 0)
			{
				access.approachDistance = 0;
				if (accessDirection == ARRIVAL_TO_DEPARTURE)
				{
					access.approachTime = getTransferDelay (origin, destination);
				} 
				else
				{
					access.approachTime = getTransferDelay (destination, origin);
				}
			}
			else
			{
				access.approachDistance = 0;
				access.approachTime = 0;
			}

			return access;
		}

	}
}
