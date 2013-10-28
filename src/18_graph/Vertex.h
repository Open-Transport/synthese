
/** Vertex class header.
	@file Vertex.h

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

#ifndef SYNTHESE_ENV_VERTEX_H
#define SYNTHESE_ENV_VERTEX_H

#include "Registrable.h"
#include "GraphTypes.h"
#include "FactoryBase.h"
#include "WithGeometry.hpp"
#include "RuleUser.h"

#include <map>
#include <geos/geom/Point.h>

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	namespace graph
	{
		class Hub;
		class Edge;
		class Path;

		//////////////////////////////////////////////////////////////////////////
		/// Vertex base class.
		///	@ingroup m18
		class Vertex :
			public virtual util::Registrable,
			public WithGeometry<geos::geom::Point>,
			public RuleUser
		{
		public:
			typedef std::multimap<const Path*, Edge*> Edges;

		protected:
			const Hub*	_hub;

			Edges _departureEdges; //!< Departure edges from this physical stop
			Edges _arrivalEdges; //!< Arrival edges to this physical stop

			std::size_t _index;

		private:
			static std::size_t _maxIndex;

		protected:

			Vertex(
				const Hub* hub,
				boost::shared_ptr<geos::geom::Point> geometry,
				bool withIndexation = true
			);

		public:

			virtual ~Vertex ();


			//! @name Getters
			//@{
				const Hub*		getHub()				const;
				const Edges&	getDepartureEdges ()	const;
				const Edges&	getArrivalEdges ()		const;
				std::size_t getIndex() const { return _index; }
			//@}

			//! @name Setters
			//@{
				void			setHub(const Hub* place);
			//@}


			//! @name Update methods
			//@{
				void addDepartureEdge ( Edge* edge );
				void addArrivalEdge ( Edge* edge );
				void removeArrivalEdge(Edge* edge);
				void removeDepartureEdge(Edge* edge);
			//@}


			//! @name Query methods
			//@{
				virtual const RuleUser* _getParentRuleUser() const;
				virtual GraphIdType getGraphType() const = 0;

				static std::size_t GetMaxIndex(){ return _maxIndex; }
			//@}
		};
	}
}

#endif
