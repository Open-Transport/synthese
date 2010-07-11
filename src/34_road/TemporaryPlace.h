
/** TemporaryPlace class header.
	@file TemporaryPlace.h

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

#ifndef SYNTHESE_ENV_TEMPORARYPLACE_H
#define SYNTHESE_ENV_TEMPORARYPLACE_H

#include "Place.h"
#include <string>

namespace synthese
{
	namespace road
	{
		class RoadPlace;
	
		/** Temporary place on the road network.
			@ingroup m34
			
			Temporary means it is not a part of the graph definition and can just
			be used as an entry point on this graph.
			
		*/
		class TemporaryPlace:
			public virtual geography::Place
		{
		
		private:
		
			const RoadPlace* _road;
			double _metricOffset;
		
		protected:
		
		public:
		
			TemporaryPlace (const RoadPlace* road, double metricOffset);
		
			virtual ~TemporaryPlace ();
		
		
			//! @name Getters/Setters
			//@{
				const RoadPlace* getRoad () const;
				double getMetricOffset () const;
			//@}
		
			//! @name Query methods
			//@{
				void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					graph::GraphIdType whatToSearch
				) const;
			//@}
			
		};
	}
}

#endif
