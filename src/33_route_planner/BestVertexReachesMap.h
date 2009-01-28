
/** BestVertexReachesMap class header.
	@file BestVertexReachesMap.h

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

#ifndef SYNTHESE_ROUTEPLANNER_BESTVERTEXREACHESMAP_H
#define SYNTHESE_ROUTEPLANNER_BESTVERTEXREACHESMAP_H

#include "GraphTypes.h"

#include "DateTime.h"

#include <map>

namespace synthese
{

	namespace graph
	{
		class Vertex;
		class ServiceUse;
	}


	namespace routeplanner
	{
		/** Best vertex reaches map class.
			@ingroup m53
		*/
		class BestVertexReachesMap
		{
		 private:


			typedef std::map<const graph::Vertex*, time::DateTime> TimeMap;
		    
			const graph::AccessDirection _accessDirection;
			TimeMap _bestTimeMap;

			time::DateTime::ComparisonOperator	_comparison;
			time::DateTime::ComparisonOperator	_cleanUpUselessComparison;
			time::DateTime::ComparisonOperator	_strictWeakCTimeComparison;

		 public:


			BestVertexReachesMap(
				graph::AccessDirection accessDirection,
				bool optim
			);
			~BestVertexReachesMap();


			//! @name Getters/Setters
			//@{
			//@}


			//! @name Query methods
			//@{
				bool contains (const graph::Vertex* vertex) const;

				const time::DateTime& getBestTime(
					const graph::Vertex* vertex
					, const time::DateTime& defaultValue
				) const;

				bool isUseless(
					const graph::Vertex* vertex
					, const time::DateTime& dateTime
				) const;
				bool mustBeCleared(
					const graph::Vertex* vertex
					, const time::DateTime& dateTime
					, const time::DateTime& bestEndTime
				) const;


			//@}


			//! @name Update methods
			//@{
				void insert(const graph::ServiceUse& journeyLeg);
			    
				void insert(
					const graph::Vertex* vertex
					, const time::DateTime& dateTime
					, bool propagateInConnectionPlace = true
				);
			//@}

		};
	}
}

#endif
