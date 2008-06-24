
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

#include "15_env/ConnectionPlace.h"

#include "01_util/Registrable.h"

namespace synthese
{
	namespace env
	{
		/** Crossing class.
			Special kind of ConnectionPlace between roads, with a zero transfer delay between them.

			@ingroup m35
		*/
		class Crossing
			: public util::Registrable<uid,Crossing>
			, public ConnectionPlace
		{
		 private:

		 public:
			//! @name Virtual queries
			//@{
				virtual bool isConnectionAllowed(
					const Vertex* fromVertex
					, const Vertex* toVertex
				) const;

				virtual int getTransferDelay(
					const Vertex* fromVertex
					, const Vertex* toVertex
				) const;

				/** Score getter.
					@return int the score of the place
					@author Hugues Romain
					@date 2007

					The vertex score is calculated by the following way :
						- each commercial line gives some points, depending of the number of services which belongs to the line :
							- 1 to 10 services lines gives 2 point
							- 10 to 50 services lines gives 3 points
							- 50 to 100 services lines gives 4 points
							- much than 100 services lines gives 5 points
						- if the score is bigger than 100 points, then the score is 100
				*/
				virtual int getScore() const;

				virtual uid getId() const;

				virtual int						getMinTransferDelay() const;

				/** Adds an address to this place.
					@param address Address to add
					This methods cancels the caching of the isobarycentre
				*/
				virtual void addAddress (const Address* address);

			//@}

			Crossing (const uid& key = UNKNOWN_VALUE,
				  const City* city = NULL);
			      
			~Crossing ();
		};
	}
}

#endif
