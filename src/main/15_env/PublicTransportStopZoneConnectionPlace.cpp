
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

#include "15_env/PhysicalStop.h"
#include "15_env/Edge.h"
#include "15_env/CommercialLine.h"
#include "15_env/Line.h"
#include "15_env/VertexAccessMap.h"
#include "15_env/Address.h"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> typename Registrable<uid,env::PublicTransportStopZoneConnectionPlace>::Registry Registrable<uid,env::PublicTransportStopZoneConnectionPlace>::_registry;
	}

	namespace env
	{
		PublicTransportStopZoneConnectionPlace::PublicTransportStopZoneConnectionPlace(
			uid id /*= UNKNOWN_VALUE */
			, std::string name /*= std::string() */
			, const City* city /*= NULL */
			, ConnectionType connectionType /*= CONNECTION_TYPE_FORBIDDEN */
			, int defaultTransferDelay /*= FORBIDDEN_TRANSFER_DELAY  */ 
		)	: Registrable<uid,PublicTransportStopZoneConnectionPlace>(id)
			, ConnectionPlace(name, city, connectionType)
			, _defaultTransferDelay (defaultTransferDelay)
			, _minTransferDelay (UNKNOWN_VALUE)
			, _score(UNKNOWN_VALUE)
		{

		}

		const PhysicalStops& PublicTransportStopZoneConnectionPlace::getPhysicalStops() const
		{
			return _physicalStops;
		}

		int PublicTransportStopZoneConnectionPlace::getDefaultTransferDelay() const
		{
			return _defaultTransferDelay;
		}

		int PublicTransportStopZoneConnectionPlace::getMinTransferDelay() const
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



		void PublicTransportStopZoneConnectionPlace::setDefaultTransferDelay( int defaultTransferDelay )
		{
			assert(defaultTransferDelay >= 0 && defaultTransferDelay <= FORBIDDEN_TRANSFER_DELAY);

			_defaultTransferDelay = defaultTransferDelay;
			_minTransferDelay = UNKNOWN_VALUE;
		}

		void PublicTransportStopZoneConnectionPlace::setConnectionType( const ConnectionType& connectionType )
		{
			_connectionType = connectionType;
		}

		void PublicTransportStopZoneConnectionPlace::addPhysicalStop( const PhysicalStop* physicalStop )
		{
			_isoBarycentreToUpdate = true;
			_physicalStops.insert(make_pair(physicalStop->getKey(),physicalStop));
		}

		void PublicTransportStopZoneConnectionPlace::addTransferDelay( uid departureId, uid arrivalId, int transferDelay )
		{
			assert(transferDelay >= 0 && transferDelay <= FORBIDDEN_TRANSFER_DELAY);

			_transferDelays[std::make_pair (departureId, arrivalId)] = transferDelay;
			_minTransferDelay = UNKNOWN_VALUE;
		}

		uid PublicTransportStopZoneConnectionPlace::getId() const
		{
			return getKey();
		}

		void PublicTransportStopZoneConnectionPlace::clearTransferDelays()
		{
			_transferDelays.clear ();
			_defaultTransferDelay = FORBIDDEN_TRANSFER_DELAY;
			_minTransferDelay = UNKNOWN_VALUE;
		}

		bool PublicTransportStopZoneConnectionPlace::isConnectionAllowed( const Vertex* fromVertex  , const Vertex* toVertex ) const /*= 0*/
		{
			if(_connectionType == CONNECTION_TYPE_FORBIDDEN)
				return false;

			bool fromVertexOnLine (dynamic_cast<const PhysicalStop*> (fromVertex));
			bool toVertexOnLine (dynamic_cast<const PhysicalStop*> (toVertex));

			if(	(_connectionType == CONNECTION_TYPE_ROADROAD)
			&&	(fromVertexOnLine == false)
			&&	(toVertexOnLine == false)
			)	return true;

			if(	(_connectionType == CONNECTION_TYPE_ROADLINE)
			&&	(	(fromVertexOnLine == false)
				||	(toVertexOnLine == false)
				)
			) return true;

			if (_connectionType >= CONNECTION_TYPE_LINELINE) 
			{
				return getTransferDelay(fromVertex,	toVertex) != FORBIDDEN_TRANSFER_DELAY;
			}

			return false;
		}

		int PublicTransportStopZoneConnectionPlace::getTransferDelay( const Vertex* fromVertex  , const Vertex* toVertex ) const /*= 0*/
		{
			TransferDelaysMap::const_iterator it(_transferDelays.find(make_pair(fromVertex->getId(),toVertex->getId())));

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

		int PublicTransportStopZoneConnectionPlace::getScore() const
		{
			if (_score == UNKNOWN_VALUE)
			{
				map<const CommercialLine*, int> scores;
				for (PhysicalStops::const_iterator its(_physicalStops.begin()); its != _physicalStops.end(); ++its)
					for (PhysicalStop::Edges::const_iterator ite(its->second->getDepartureEdges().begin()); ite != its->second->getDepartureEdges().end(); ++ite)
					{
						const Line* route(static_cast<const Line*>((*ite)->getParentPath()));
						map<const CommercialLine*, int>::iterator itl(scores.find(route->getCommercialLine()));
						if (itl == scores.end())
							scores.insert(make_pair(route->getCommercialLine(), route->getServices().size()));
						else
							itl->second += route->getServices().size();
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
						if (_score > 100)
						{
							_score = 100;
							break;
						}
					}
			}
			return _score;
		}

		void PublicTransportStopZoneConnectionPlace::getImmediateVertices( VertexAccessMap& result  , const AccessDirection& accessDirection  , const AccessParameters& accessParameters  , SearchAddresses returnAddresses  , SearchPhysicalStops returnPhysicalStops  , const Vertex* origin /*= NULL */ ) const
		{
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
	}
}
