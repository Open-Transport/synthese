
/** VertexAccessMap class header.
	@file VertexAccessMap.h

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

#ifndef SYNTHESE_ENV_VERTEXACCESSMAP_H
#define SYNTHESE_ENV_VERTEXACCESSMAP_H

#include <vector>
#include <map>
#include <set>

#include "15_env/Point.h"
#include "15_env/SquareDistance.h"

namespace synthese
{
	namespace env
	{
		class Vertex;
		class Path;

		typedef struct {
			int approachTime;
			double approachDistance;
		} VertexAccess;
		 
		/** 
TRIDENT : VertexAccess => AccesPoint
		@ingroup m15
		*/
		class VertexAccessMap
		{
		public:
		    
			typedef enum { MERGE_ADDRESSES, DO_NOT_MERGE_ADDRESSES } MergeAddresses;
			typedef enum { MERGE_PHYSICALSTOPS, DO_NOT_MERGE_PHYSICALSTOPS } MergePhysicalStops;
		    

		private:

			std::map<const Vertex*, VertexAccess> _map;
			std::set<const Path*> _pathOnWhichFineSteppingForDeparture;
			std::set<const Path*> _pathOnWhichFineSteppingForArrival;
		    
			mutable bool _isobarycenterUpToDate;
			mutable bool _isobarycenterMaxSquareDistanceUpToDate;

			mutable Point _isobarycenter;   //!< Isobarycenter of all points contained in this map.
			mutable SquareDistance _isobarycenterMaxSquareDistance;   //!< Maximum square distance of one map point with the isobarycenter.

			int _minApproachTime;


		// une fonction qui verifie pour une ligne donnée si elle passe par l'un des vertex
		// de la vam qui n'est pas de correspondance transport.
		// VertexAccessMap vam, 

		public:

			VertexAccessMap ();
			~VertexAccessMap ();


			const std::map<const Vertex*, VertexAccess>& getMap () const;

			const VertexAccess& getVertexAccess (const Vertex* vertex) const;

			bool needFineSteppingForDeparture (const Path* path) const;
			bool needFineSteppingForArrival (const Path* path) const;
		    
			bool contains (const Vertex* vertex) const;
			void insert (const Vertex* vertex, const VertexAccess& vertexAccess);
		    
			void merge (const VertexAccessMap& vam,
				MergeAddresses mergeAddresses = MERGE_ADDRESSES,
				MergePhysicalStops mergePhysicalStops = MERGE_PHYSICALSTOPS);

			const Point& getIsobarycenter () const;
			const SquareDistance& getIsobarycenterMaxSquareDistance () const;
		    
			int getMinApproachTime () const;

		private:

			void updateFineSteppingVertexMap ();


		};
	}
}

#endif 	    
