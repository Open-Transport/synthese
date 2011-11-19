
/** Road class implementation.
	@file Road.cpp

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

#include "Road.h"
#include "RoadPlace.h"
#include "Address.h"
#include "RoadChunk.h"
#include "PermanentService.h"
#include "Registry.h"

#include <geos/geom/LineString.h>

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
			RoadType type
		):	Registrable(id),
			_type (type)
		{
			// Creation of the permanent service
			PermanentService* service(new PermanentService(id, this));
			addService(*service, false);
			service->setPath(this);
		}



		Road::~Road()
		{
			BOOST_FOREACH(ServiceSet::value_type service, _services)
			{
				delete service;
			}
		}



		void Road::setType(
			const RoadType& type
		){
			_type = type;
		}



		bool Road::isPedestrianMode() const
		{
			return true;
		}



		RoadPlace* Road::getRoadPlace(
		) const {
			return static_cast<RoadPlace*>(_pathGroup);
		}



		bool Road::isRoad() const
		{
			return true;
		}



		std::string Road::getRuleUserName() const
		{
			return "Route " + getRoadPlace()->getFullName();
		}



		bool Road::isActive( const boost::gregorian::date& date ) const
		{
			return true;
		}



		void Road::_setRoadPlace( RoadPlace& value )
		{
			// Break of existing link if exists
			if(_pathGroup)
			{
				value.removeRoad(*this);
			}

			// New links
			_pathGroup = &value;
			value.addRoad(*this);
		}



		void Road::_insertRoadChunk(
			RoadChunk& chunk, double length, std::size_t rankShift
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
		}
}	}
