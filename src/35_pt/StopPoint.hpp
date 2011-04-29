
/** StopPoint class header.
	@file StopPoint.hpp

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

#ifndef SYNTHESE_ENV_PHYSICALSTOP_H
#define SYNTHESE_ENV_PHYSICALSTOP_H

#include "UtilConstants.h"
#include "Vertex.h"
#include "Registry.h"
#include "Importable.h"
#include "Named.h"
#include "Address.h"
#include "ReachableFromCrossing.hpp"

#include <string>

namespace synthese
{
	namespace pt
	{
		class LineStop;
		class StopArea;

		//////////////////////////////////////////////////////////////////////////
		/// Physical stop (bus stop, etc.).
		///	A physical stop is an entry point to the transport network.
		///	RoutePoint
		///	@ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class StopPoint:
			public graph::Vertex,
			public impex::Importable,
			public util::Named,
			public road::ReachableFromCrossing
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<StopPoint>	Registry;

		private:
			road::Address _projectedPoint;

		public:

			StopPoint(
				util::RegistryKeyType id = 0,
				std::string name = std::string(),
				const pt::StopArea* place = NULL,
				boost::shared_ptr<geos::geom::Point> geometry = boost::shared_ptr<geos::geom::Point>()
			);

			~StopPoint();


			//! @name Getters
			//@{
				const road::Address& getProjectedPoint() const { return _projectedPoint; }
			//@}

			//! @name Setters
			//@{
				void setProjectedPoint(const road::Address& value){ _projectedPoint = value; }
			//@}

			//! @name Query methods
			//@{
				const pt::StopArea* getConnectionPlace() const;
				virtual graph::GraphIdType getGraphType() const;
				virtual graph::VertexAccess getVertexAccess(const road::Crossing& crossing) const;
			//@}
		};

	}
}

#endif
