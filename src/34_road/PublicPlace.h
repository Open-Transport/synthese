
/** PublicPlace class header.
	@file PublicPlace.h

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

#ifndef SYNTHESE_road_PUBLICPLACE_H
#define SYNTHESE_road_PUBLICPLACE_H

#include "Registry.h"
#include "NamedPlaceTemplate.h"

#include <string>

namespace synthese
{
	namespace road
	{
		//////////////////////////////////////////////////////////////////////////
		/// Public place.
		///	@ingroup m34
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Public place model :
		/// @image html uml_public_place.png
		///
		class PublicPlace:
			public geography::NamedPlaceTemplate<PublicPlace>
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<PublicPlace>	Registry;

			PublicPlace (
				util::RegistryKeyType id = 0
			);

			virtual ~PublicPlace ();

			virtual std::string getNameForAllPlacesMatcher(
				std::string text = std::string()
			) const;

			//! @name Virtual queries for geography::Place interface
			//@{
				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const geography::Place::GraphTypes& whatToSearch
				) const;

				virtual boost::shared_ptr<geos::geom::Point> getPoint() const;
			//@}
		};
	}
}

#endif 	    
