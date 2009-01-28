
/** Vertex class header.
	@file Vertex.h

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

#ifndef SYNTHESE_ENV_VERTEX_H
#define SYNTHESE_ENV_VERTEX_H

#include <set>

#include "Registrable.h"
#include "Point2D.h"

namespace synthese
{
	namespace graph
	{
		class Hub;
		class Edge;
		class Path;

		/** Vertex base class.

			A vertex is the abstract graph node used for route planning.
			A vertex is necessarily connected to its corresponding path.
			@ingroup m35
		*/
		class Vertex
		:	public geometry::Point2D,
			public virtual util::Registrable
		{
		public:
			typedef std::set<const Edge*> Edges;

		protected:
			const Hub*	_place;

			Edges _departureEdges; //!< Departure edges from this physical stop
			Edges _arrivalEdges; //!< Arrival edges to this physical stop

		protected:

			Vertex(
				const Hub* place,
				double x = UNKNOWN_VALUE,
				double y = UNKNOWN_VALUE
			);

		public:

			virtual ~Vertex ();


			//! @name Getters
			//@{
				const Hub*		getPlace ()				const;  
				const Edges&	getDepartureEdges ()	const;
				const Edges&	getArrivalEdges ()		const;
			//@}

			//! @name Setters
			//@{
				void			setPlace(const Hub* place);
			//@}


			//! @name Update methods
			//@{
				void addDepartureEdge ( const Edge* edge );
				void addArrivalEdge ( const Edge* edge );
			//@}


			//! @name Query methods
			//@{
				virtual bool isAddress () const = 0;
				virtual bool isPhysicalStop () const = 0;
			//@}
		};
	}
}

#endif
