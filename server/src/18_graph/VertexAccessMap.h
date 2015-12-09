
/** VertexAccessMap class header.
	@file VertexAccessMap.h

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

#ifndef SYNTHESE_ENV_VERTEXACCESSMAP_H
#define SYNTHESE_ENV_VERTEXACCESSMAP_H

#include "Journey.h"

#include <vector>
#include <map>
#include <set>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

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
		class Vertex;
		class Path;

		struct VertexAccess {
			boost::posix_time::time_duration approachTime;
			double approachDistance;
			Journey approachJourney;

			VertexAccess(
				boost::posix_time::time_duration __approachTime,
				double __approachDistance,
				const Journey& __approachJourney
			):	approachTime(__approachTime),
				approachDistance(__approachDistance),
				approachJourney(__approachJourney)
			{}


			VertexAccess(
				boost::posix_time::time_duration __approachTime = boost::posix_time::minutes(0),
				double __approachDistance = 0
			):	approachTime(__approachTime),
				approachDistance(__approachDistance),
				approachJourney()
			{}
		};




		/**
TRIDENT : VertexAccess => AccesPoint
		@ingroup m35
		*/
		class VertexAccessMap
		{
		public:
			typedef std::map<const Vertex*, VertexAccess> VamMap;

		private:

			VamMap _map;
			std::set<const Path*> _pathOnWhichFineSteppingForDeparture;
			std::set<const Path*> _pathOnWhichFineSteppingForArrival;

			mutable boost::shared_ptr<geos::geom::Point> _isobarycentre;   //!< Isobarycenter of all points contained in this map.

			boost::posix_time::time_duration _minApproachTime;


		// une fonction qui verifie pour une ligne donnée si elle passe par l'un des vertex
		// de la vam qui n'est pas de correspondance transport.
		// VertexAccessMap vam,

		public:

			VertexAccessMap ();
			~VertexAccessMap ();


			//! @name Getters
			//@{
				const VamMap& getMap () const;
			//@}

			const VertexAccess& getVertexAccess (const Vertex* vertex) const;

			bool needFineSteppingForDeparture (const Path* path) const;
			bool needFineSteppingForArrival (const Path* path) const;

			bool contains (const Vertex* vertex) const;



			/** Tests if the object and the tested one have at least one common vertex.
				@param other the other object
				@return bool true if the objects have at least one common vertex.
				@author Hugues Romain
				@date 2009
			*/
			bool intersercts(const VertexAccessMap& other) const;



			//////////////////////////////////////////////////////////////////////////
			/// Get the best journey between any point of the object and an other one.
			/// @param other the other object
			/// @return the best journey. If no journey is found, return an empty journey.
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.18
			Journey getBestIntersection(const VertexAccessMap& other) const;



			/** Vertex insertion.
				@param vertex the vertex to insert
				@param vertexAccess access conditions to the vertex
				@author Hugues Romain
				@date 2008

				If the vertex already belongs to the object, then only the access conditions are updated, only if they are more efficient.
			*/
			void insert (const Vertex* vertex, const VertexAccess& vertexAccess);


			const boost::shared_ptr<geos::geom::Point>& getCentroid() const;

			boost::posix_time::time_duration getMinApproachTime () const;

		private:

			void updateFineSteppingVertexMap ();


		};

		std::ostream& operator<<(std::ostream& os, const VertexAccessMap& vam);

	}
}

#endif
