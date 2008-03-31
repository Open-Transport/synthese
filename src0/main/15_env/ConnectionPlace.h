
/** ConnectionPlace class header.
	@file ConnectionPlace.h

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

#ifndef SYNTHESE_ENV_CONNECTIONPLACE_H
#define SYNTHESE_ENV_CONNECTIONPLACE_H

#include <string>

#include "15_env/AddressablePlace.h"
#include "15_env/Types.h"

namespace synthese
{
	namespace env
	{
		class Vertex;

		/** A connection place indicates that there are possible
			connections between different network vertices.

			Each connection is associated with a type (authorized, 
			forbidden, recommended...) and a transfer delay.

			@ingroup m15
		*/
		class ConnectionPlace : 
			public AddressablePlace
		{
		public:
			typedef enum { 
				CONNECTION_TYPE_FORBIDDEN = 0,         //!< neither road connection nor line connection
				CONNECTION_TYPE_ROADROAD = 1,          //!< only road to road connection
				CONNECTION_TYPE_ROADLINE = 2,          //!< only road to line, or line to road, or road to road
				CONNECTION_TYPE_LINELINE = 3,          //!< any connection possible
				CONNECTION_TYPE_RECOMMENDED_SHORT = 4, //!< any connection possible, recommended if short journey (deprecated)
				CONNECTION_TYPE_RECOMMENDED = 5        //!< any connection possible, recommended for any journey (deprecated)
			} ConnectionType;
			static const int FORBIDDEN_TRANSFER_DELAY;

		protected:
			ConnectionType		_connectionType;

		public:

			/** Constructor.
				@param name Name of the connection place
				@param city City which belongs the connection place
			*/
			ConnectionPlace (
				std::string name = std::string()
				, const City* city = NULL
				, ConnectionType type = CONNECTION_TYPE_FORBIDDEN
			);
			virtual ~ConnectionPlace ();

			//! @name Getters
			//@{
				ConnectionType	getConnectionType () const;
			//@}

			//! @name Interface for query methods
			//@{
				virtual int						getMinTransferDelay() const = 0;
				
				virtual bool isConnectionAllowed(
					const Vertex* fromVertex
					, const Vertex* toVertex
				) const = 0;

				virtual int getTransferDelay(
					const Vertex* fromVertex
					, const Vertex* toVertex
				) const = 0;

				virtual int getScore() const = 0;
			//@}


			//! @name Query methods.
			//@{
				VertexAccess getVertexAccess(
					const AccessDirection& accessDirection
					, const AccessParameters& accessParameters
					, const Vertex* destination
					, const Vertex* origin = NULL
				) const;

			//@}

		};
	}
}

#endif 	    
