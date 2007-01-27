
/** PhysicalStop class header.
	@file PhysicalStop.h

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

#ifndef SYNTHESE_ENV_PHYSICALSTOP_H
#define SYNTHESE_ENV_PHYSICALSTOP_H

#include "01_util/Registrable.h"
#include "01_util/UId.h"
#include "01_util/Constants.h"

#include "15_env/Vertex.h"

#include <string>


namespace synthese
{
	namespace env
	{

		class LineStop;
		class ConnectionPlace;
		 

		/** Physical stop (bus stop, etc.).
			A physical stop is an entry point to the transport network.
		@ingroup m15
		*/
		class PhysicalStop : 
			public Vertex,
			public synthese::util::Registrable<uid, PhysicalStop>
		{

		private:

			std::string _name; //! Physical stop name
			
		public:

			PhysicalStop (const uid& id,
				const std::string& name, 
				const AddressablePlace* place,
				double x = UNKNOWN_VALUE,
				double y = UNKNOWN_VALUE);

			~PhysicalStop ();
		    

			//! @name Getters/Setters
			//@{
			const uid& getId () const;

			const std::string& getName () const;
			void setName (const std::string& name);
			//@}


			//! @name Query methods
			//@{
			bool isAddress () const;
			bool isPhysicalStop () const;
			//@}

		};



	}
}

#endif
