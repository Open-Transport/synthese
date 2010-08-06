
/** Road class implementation.
	@file Road.cpp

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

#include "Road.h"
#include "RoadPlace.h"
#include "Address.h"
#include "RoadChunk.h"
#include "PermanentService.h"
#include "Registry.h"

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<road::Road>::KEY("Road");
	}

	namespace road
	{
		Road::Road (
			RegistryKeyType id,
			RoadType type,
			bool autoCreateReverseRoad
		):	Registrable(id),
			_type (type)
		{
			// Creation of the permanent service
			addService(new PermanentService(id, this), false);
			_reverseRoad = (autoCreateReverseRoad ? new Road(*this) : NULL);
		}



		Road::Road(const Road& reverseRoad
		):	Registrable(0),
			_type(reverseRoad.getType()),
			_reverseRoad(NULL)
		{
			addService(new PermanentService(0, this), false);
		}



		Road::~Road()
		{
			BOOST_FOREACH(ServiceSet::value_type service, _services)
			{
				delete service;
			}
			if(_reverseRoad)
			{
				BOOST_FOREACH(Edges::value_type edge, _reverseRoad->_edges)
				{
					delete edge;
				}
				delete _reverseRoad;
			}
		}

		
		
		void Road::setType(
			const RoadType& type
		){
			_type = type;
			if(_reverseRoad)
			{
				_reverseRoad->setType(type);
			}
		}



		const Address* 
		Road::findClosestAddressBefore (double metricOffset) const
		{
			const Address* address = 0;
/*			for (Addresses::const_iterator it = getAddresses ().begin ();
			 it != getAddresses ().end (); ++it)
			{
			if ( ((*it)->getMetricOffset () <= metricOffset) &&
				 ( (address == 0) || ((*it)->getMetricOffset () > address->getMetricOffset ()) ) )
			{
				address = (*it);
			}
			}
*/			return address;
		}


		const Address* 
		Road::findClosestAddressAfter (double metricOffset) const
		{
			const Address* address = 0;
/*			for (Addresses::const_iterator it = getAddresses ().begin ();
			 it != getAddresses ().end (); ++it)
			{
			if ( ((*it)->getMetricOffset () >= metricOffset) &&
				 ( (address == 0) || ((*it)->getMetricOffset () < address->getMetricOffset ()) ) )
			{
				address = (*it);
			}
			}
*/			return address;
		}



		void Road::addRoadChunk(
			RoadChunk& chunk
		){
			if(_reverseRoad)
			{
				if(getEdges().empty())
				{
					RoadChunk* reverseChunk(
						new RoadChunk(
							0,
							chunk.getFromAddress(),
							0,
							_reverseRoad,
							-chunk.getMetricOffset()
					)	);
					BOOST_REVERSE_FOREACH(const Coordinate*pt, chunk.getViaPoints())
					{
						reverseChunk->addViaPoint(*pt);
					}
					_reverseRoad->addEdge(*reverseChunk);
				}
				else
				{
					const Edge& lastEdge(**(getEdges().end() - 1));
					if(chunk.getRankInPath() > lastEdge.getRankInPath())
					{
						RoadChunk* reverseChunk(
							new RoadChunk(
								0,
								chunk.getFromAddress(),
								0,
								_reverseRoad,
								-chunk.getMetricOffset()
						)	);
						_reverseRoad->insertRoadChunk(
							*reverseChunk,
							0,
							chunk.getRankInPath() - lastEdge.getRankInPath()
						);
					}
					else
					{
						RoadChunk* reverseChunk(
							new RoadChunk(
								0,
								chunk.getFromAddress(),
								lastEdge.getRankInPath() - chunk.getRankInPath(),
								_reverseRoad,
								-chunk.getMetricOffset()
						)	);
						_reverseRoad->addEdge(
							*reverseChunk							
						);
					}
				}
			}

			addEdge(static_cast<Edge&>(chunk));
		}



		void Road::insertRoadChunk(
			RoadChunk& chunk,
			double length,
			size_t rankShift
		){
			assert(!_edges.empty());
			assert(chunk.getMetricOffset() <= (*_edges.begin())->getMetricOffset() + length);
			assert(chunk.getMetricOffset() <= (*_edges.begin())->getRankInPath() + rankShift);

			// Shifting the whole road to right
			for(Edges::const_iterator it(_edges.begin()); it != _edges.end(); ++it)
			{
				(*it)->setRankInPath((*it)->getRankInPath() + rankShift);
				(*it)->setMetricOffset((*it)->getMetricOffset() + length);
			}

			// Insertion of the chunk
			addEdge(chunk);

			if(_reverseRoad)
			{
				// Shifting the whole reverse road to left
				for(Edges::const_iterator it(_reverseRoad->_edges.begin()); it != _reverseRoad->_edges.end(); ++it)
				{
					(*it)->setMetricOffset((*it)->getMetricOffset() - length);
				}

				// Insertion of the chunk
				const Edge& lastEdge(**(_reverseRoad->getEdges().end() - 1));

				RoadChunk* reverseChunk(
					new RoadChunk(
						0,
						chunk.getFromAddress(),
						lastEdge.getRankInPath() + rankShift,
						_reverseRoad,
						0
				)	);
				_reverseRoad->addEdge(
					*reverseChunk							
				);
			}
		}



		bool Road::isPedestrianMode() const
		{
			return true;
		}
		
		

		RoadPlace* Road::getRoadPlace(
		) const {
			return static_cast<RoadPlace*>(_pathGroup);
		}
		
		

		void Road::setRoadPlace(
			RoadPlace& value
		){
			// Break of existing link if exists
			if(_pathGroup)
			{
				value.removeRoad(*this);
			}
			
			// New links
			_pathGroup = &value;
			value.addRoad(*this);
			
			// Links the reverse road too
			if(_reverseRoad)
			{
				_reverseRoad->setRoadPlace(value);
			}
		}



		void Road::merge( Road& other )
		{
			Path::merge(static_cast<Path&>(other));
			if(_reverseRoad && other._reverseRoad)
			{
				other._reverseRoad->merge(*_reverseRoad);
			}
		}



		bool Road::isRoad() const
		{
			return true;
		}



		std::string Road::getRuleUserName() const
		{
			return "Route " + getRoadPlace()->getFullName() + " sens "+ (_reverseRoad ? "direct" : "opposé");
		}



		bool Road::isActive( const boost::gregorian::date& date ) const
		{
			return true;
		}

		bool Road::isReversed() const
		{
			return (_reverseRoad == NULL);
		}
}	}
