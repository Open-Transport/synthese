
/** IncludingPlace class header.
	@file IncludingPlace.h

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

#ifndef SYNTHESE_ENV_INCLUDINGPLACE_H
#define SYNTHESE_ENV_INCLUDINGPLACE_H


#include <vector>

#include "Place.h"

namespace synthese
{
	namespace env
	{

		/** Base class for a place including other places.

			@ingroup m35
		*/
		class IncludingPlace : public Place
		{
		public:
			typedef std::vector<const Place*> IncludedPlaces;

		protected:
			IncludedPlaces _includedPlaces; 

		public:

			virtual ~IncludingPlace ();


			//! @name Getters/Setters
			//@{

				/** Gets included places.
				 */
				const IncludedPlaces& getIncludedPlaces () const;
			    
			//@}


			//! @name Query methods
			//@{

				void getImmediateVertices (VertexAccessMap& result, 
						   const AccessDirection& accessDirection,
						   const AccessParameters& accessParameters,
						   SearchAddresses returnAddresses
						   , SearchPhysicalStops returnPhysicalStops
						   , const Vertex* origin = 0
						   ) const;

				virtual const geometry::Point2D& getPoint() const;

				virtual bool includes(const Place* place) const;

			//@}


			//! @name Update methods.
			//@{

				/** Adds an included place to this place.
					@param place Place to include
					This methods cancels the caching of the isobarycentre.
				 */
				void addIncludedPlace (const Place* place);

			//@}


		protected:

			IncludingPlace (const std::string& name,
					  const City* city);

		};
	}
}

#endif 	    
