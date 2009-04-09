
/** Crossing class header.
	@file Crossing.h

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

#ifndef SYNTHESE_ENV_CROSSING_H
#define SYNTHESE_ENV_CROSSING_H

#include "Hub.h"
#include "Registry.h"
#include "Registrable.h"

namespace synthese
{
	namespace road
	{
		class Address;
		
		/** Crossing class.
			Special kind of ConnectionPlace between roads, with a zero transfer delay between them.

			Score getter is not overloaded : all the crossing are considered interesting in routing.
			@todo See later if this assertion is the best choice.

			@ingroup m35
		*/
		class Crossing
		:	public graph::Hub,
			public virtual util::Registrable
		{
		public:
			  /// Chosen registry class.
			  typedef util::Registry<Crossing>	Registry;

		protected:
			Address* _address;

		public:
			//! @name Virtual queries
			//@{
				virtual bool isConnectionAllowed(
					const graph::Vertex* fromVertex
					, const graph::Vertex* toVertex
				) const;

				virtual int getTransferDelay(
					const graph::Vertex* fromVertex
					, const graph::Vertex* toVertex
				) const;
				

				virtual int	getMinTransferDelay() const;

				/** Links the crossing with the address.
					@param address Address to link
				*/
				void setAddress(Address* address);
				Address* getAddress() const;

				virtual const geometry::Point2D& getPoint() const;

			//@}

			Crossing(
				util::RegistryKeyType key = UNKNOWN_VALUE
			);
			
			~Crossing ();
		};
	}
}

#endif
