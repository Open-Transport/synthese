
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

#include "15_env/Place.h"
#include "15_env/Types.h"

namespace synthese
{
	namespace env
	{
		/** Addressable place base class.

			AddressablePlace is the base for any place which can provide
			addresses. 

			@ingroup m15
		*/
		class AddressablePlace : public Place
		{
		public:

			typedef enum { 
				CONNECTION_TYPE_FORBIDDEN = 0,         //!< neither road connection nor line connection
				CONNECTION_TYPE_ROADROAD = 1,          //!< only road to road connection
				CONNECTION_TYPE_ROADLINE = 2,          //!< only road to line, or line to road, or road to road
				CONNECTION_TYPE_LINELINE = 3,          //!< any connection possible
				CONNECTION_TYPE_RECOMMENDED_SHORT = 4, //!< any connection possible, recommended if short journey
				CONNECTION_TYPE_RECOMMENDED = 5        //!< any connection possible, recommended for any journey
			} ConnectionType;

		protected:

			Addresses _addresses; 


			AddressablePlace (const std::string& name,
					  const City* city);


		public:

			virtual ~AddressablePlace ();

			//! @name Getters/Setters
			//@{

			/** Gets addresses of this place.
			 */
			const Addresses& getAddresses () const;

			virtual const ConnectionType getConnectionType () const;

			//@}


			//! @name Query methods
			//@{

			virtual void getImmediateVertices (
				VertexAccessMap& result, 
				const AccessDirection& accessDirection,
				const AccessParameters& accessParameters
				, SearchAddresses returnAddresses
				, SearchPhysicalStops returnPhysicalStops
				, const Vertex* origin = NULL
			) const;

			//@}




			//! @name Update methods.
			//@{

			/** Adds an address to this place.
			 */
				void addAddress (const Address* address);
			//@}


		};


	}
}

#endif 	    

