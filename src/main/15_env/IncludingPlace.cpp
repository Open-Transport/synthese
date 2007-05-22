
/** IncludingPlace class implementation.
	@file IncludingPlace.cpp

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

#include "IncludingPlace.h"



namespace synthese
{
	namespace env
	{

		IncludingPlace::IncludingPlace (const std::string& name,
						const City* city)
			: Place (name, city)
		{
		}



		IncludingPlace::~IncludingPlace ()
		{
		}



		const std::vector<const Place*>& 
		IncludingPlace::getIncludedPlaces () const
		{
			return _includedPlaces;
		}
		    


		void 
		IncludingPlace::addIncludedPlace (const Place* place)
		{
			_includedPlaces.push_back (place);
		}






		void
		IncludingPlace::getImmediateVertices (VertexAccessMap& result, 
							  const AccessDirection& accessDirection,
							  const AccessParameters& accessParameters,
							  const Vertex* origin,
							  bool returnAddresses,
							  bool returnPhysicalStops) const
		{

			for (std::vector<const Place*>::const_iterator it = _includedPlaces.begin ();
			 it != _includedPlaces.end (); ++it
			){
				(*it)->getImmediateVertices (result, accessDirection, accessParameters, 
							 origin, returnAddresses, returnPhysicalStops);
			}

		}

	}
}
