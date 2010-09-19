
/** Crossing class implementation.
	@file Crossing.cpp

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

#include "Crossing.h"
#include "RoadModule.h"
#include "VertexAccessMap.h"

using namespace std;
using namespace boost;
using namespace geos::geom;


namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace geography;
	using namespace impex;
	
	

	namespace util
	{
		template<> const string Registry<road::Crossing>::KEY("Crossing");
	}

	namespace road
	{
		Crossing::Crossing(
			util::RegistryKeyType key,
			shared_ptr<Point> geometry,
			std::string codeBySource,
			const impex::DataSource* source
		):	Hub(),
			Vertex(this, geometry),
			Registrable(key),
			Importable(codeBySource, source)
		{}
		
		
		
		Crossing::~Crossing ()
		{}



		bool Crossing::isConnectionAllowed(
			const Vertex& fromVertex,
			const Vertex& toVertex
		) const	{
			return true;
		}



		boost::shared_ptr<geos::geom::Point> Crossing::getPoint() const
		{
			return WithGeometry<Point>::getGeometry();
		}



		posix_time::time_duration Crossing::getMinTransferDelay() const
		{
			return posix_time::minutes(0);
		}



		bool Crossing::containsAnyVertex( graph::GraphIdType graphType ) const
		{
			return graphType == RoadModule::GRAPH_ID;
		}



		void Crossing::getVertexAccessMap(
			graph::VertexAccessMap& result,
			graph::GraphIdType whatToSearch,
			const graph::Vertex& origin,
			bool vertexIsOrigin
		) const	{
			if(whatToSearch == RoadModule::GRAPH_ID)
			{
				result.insert(
					this,
					VertexAccess()
				);
			}
		}



		synthese::graph::HubScore Crossing::getScore() const
		{
			return 1;
		}



		posix_time::time_duration Crossing::getTransferDelay(
			const graph::Vertex& fromVertex,
			const graph::Vertex& toVertex
		) const	{
			return posix_time::minutes(0);
		}



		graph::GraphIdType Crossing::getGraphType() const
		{
			return RoadModule::GRAPH_ID;
		}
	}
}
