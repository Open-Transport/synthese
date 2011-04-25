
/** Depot class header.
	@file Depot.hpp

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

#ifndef SYNTHESE_pt_operation_Depot_hpp__
#define SYNTHESE_pt_operation_Depot_hpp__

#include "Named.h"
#include "Registrable.h"
#include "Registry.h"
#include "WithGeometry.hpp"

namespace synthese
{
	namespace pt_operation
	{
		/** Depot class.
			@ingroup m37
		*/
		class Depot:
			public util::Named,
			public util::Registrable,
			public WithGeometry<geos::geom::Point>
		{
		public:
			typedef util::Registry<Depot> Registry;

			Depot(util::RegistryKeyType id = 0);
		};
	}
}

#endif // SYNTHESE_pt_operation_Depot_hpp__
