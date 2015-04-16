
/** Depot class header.
	@file Depot.hpp

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

#ifndef SYNTHESE_pt_operation_Depot_hpp__
#define SYNTHESE_pt_operation_Depot_hpp__

#include "Registrable.h"
#include "Registry.h"
#include "Vertex.h"
#include "Hub.h"
#include "ImportableTemplate.hpp"

namespace synthese
{
	namespace pt_operation
	{
		/** Depot class.
			@ingroup m37
		*/
		class Depot:
			public graph::Vertex,
			public graph::Hub,
			public impex::ImportableTemplate<Depot>
		{
		private:
			std::string _name;

		public:
			typedef util::Registry<Depot> Registry;

			Depot(util::RegistryKeyType id = 0);

			virtual graph::GraphIdType getGraphType() const;
			virtual std::string getName() const { return _name; }

			void setName(const std::string& value){ _name = value; }

			virtual boost::posix_time::time_duration getMinTransferDelay() const;
			virtual void getVertexAccessMap(synthese::graph::VertexAccessMap &,synthese::graph::GraphIdType,const synthese::graph::Vertex &,bool) const;
			virtual bool isConnectionAllowed(const synthese::graph::Vertex &,const synthese::graph::Vertex &) const;
			virtual boost::posix_time::time_duration getTransferDelay(const synthese::graph::Vertex &,const synthese::graph::Vertex &) const;
			virtual graph::HubScore getScore(void) const;
			virtual const boost::shared_ptr<geos::geom::Point>& getPoint(void) const;
			virtual bool containsAnyVertex(synthese::graph::GraphIdType) const;

			virtual std::string getRuleUserName() const;



			//////////////////////////////////////////////////////////////////////////
			/// Lists the vertices contained in the hub.
			/// @param graphId the graph the vertices must belong to
			virtual Vertices getVertices(
				graph::GraphIdType graphId
			) const;
		};
	}
}

#endif // SYNTHESE_pt_operation_Depot_hpp__
