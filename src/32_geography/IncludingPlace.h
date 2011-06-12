
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

#include <set>

#include "Place.h"

#pragma warning( disable : 4250 )

namespace synthese
{
	namespace geography
	{
		/** Base class for a place including other places.

			@ingroup m32
		*/
		class IncludingPlace:
			public virtual Place
		{
		public:
			typedef std::set<const Place*> IncludedPlaces;

		protected:
			IncludedPlaces _includedPlaces;

			IncludingPlace(
			);

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

				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const GraphTypes& whatToSearch
				) const;

				virtual boost::shared_ptr<geos::geom::Point> getPoint() const;

				virtual bool includes(const Place* place) const;

			//@}


			//! @name Update methods.
			//@{

				/** Adds an included place to this place.
					@param place Place to include
					This methods cancels the caching of the isobarycentre.
				 */
				void addIncludedPlace (const Place* place);



				//////////////////////////////////////////////////////////////////////////
				/// Removes an included place to the current one.
				/// @param place place to remove
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				/// This methods cancels the caching of the isobarycentre.
				/// If the place was not included, the method do nothing.
				void removeIncludedPlace(const Place* place);
			//@}
		};
	}
}

#endif
