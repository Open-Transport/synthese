
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

#include "01_util/Constants.h"
#include "Vertex.h"
#include "Registry.h"
#include "Importable.h"

#include <string>


namespace synthese
{
	namespace env
	{
		class LineStop;
		class PublicTransportStopZoneConnectionPlace;
		 
		//////////////////////////////////////////////////////////////////////////
		/// Physical stop (bus stop, etc.).
		///	A physical stop is an entry point to the transport network.
		///	RoutePoint
		///	@ingroup m35
		//////////////////////////////////////////////////////////////////////////
		class PhysicalStop
		:	public graph::Vertex,
			public impex::Importable
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<PhysicalStop>	Registry;

		private:

			std::string _name; //! Physical stop name
			
		public:

			PhysicalStop(
				util::RegistryKeyType id = UNKNOWN_VALUE,
				std::string name = std::string(), 
				const PublicTransportStopZoneConnectionPlace* place = NULL,
				double x = UNKNOWN_VALUE,
				double y = UNKNOWN_VALUE);

			~PhysicalStop ();
		    

			//! @name Getters/Setters
			//@{
				const PublicTransportStopZoneConnectionPlace* getConnectionPlace() const;

				const std::string& getName () const;
				void setName (const std::string& name);
			//@}


			//! @name Query methods
			//@{
				virtual graph::GraphIdType getGraphType() const;
			//@}

		};

	}
}

#endif
