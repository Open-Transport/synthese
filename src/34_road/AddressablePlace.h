
/** AddressablePlace class header.
	@file AddressablePlace.h

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

#ifndef SYNTHESE_ENV_ADDRESSABLEPLACE_H
#define SYNTHESE_ENV_ADDRESSABLEPLACE_H


#include <vector>

#include "Place.h"
#include "Hub.h"
#include "15_env/Types.h"

namespace synthese
{
	namespace road
	{
		class Address;
		
		/** Addressable place base class.

			AddressablePlace is the base for any place which can provide
			addresses, i.e. it contains vertices which could be found at a path
			traversing.

			@ingroup m34
		*/
		class AddressablePlace
		:	public env::Place,
			public graph::Hub
		{
		public:
			typedef std::vector<const Address*> Addresses;

		protected:
			Addresses _addresses; 

			AddressablePlace(
				const std::string& name,
				const env::City* city
			);

		public:

			virtual ~AddressablePlace ();

			//! @name Getters/Setters
			//@{
				/** Gets addresses of this place.
				 */
				const Addresses& getAddresses () const;
			//@}


			//! @name Query methods
			//@{
				virtual void getImmediateVertices (
					graph::VertexAccessMap& result, 
					const graph::AccessDirection& accessDirection,
					const env::AccessParameters& accessParameters
					, SearchAddresses returnAddresses
					, SearchPhysicalStops returnPhysicalStops
					, const graph::Vertex* origin = NULL
				) const;

				virtual const geometry::Point2D& getPoint() const;

				bool hasAddresses() const;
				virtual bool hasPhysicalStops()	const;
			//@}



			//! @name Update methods.
			//@{
				/** Adds an address to this place.
					@param address Address to add
					This methods cancels the caching of the isobarycentre
				 */
				virtual void addAddress (const Address* address);
			//@}
			
			static const AddressablePlace* GetPlace(const graph::Hub* hub);
		};
	}
}

#endif 	    
