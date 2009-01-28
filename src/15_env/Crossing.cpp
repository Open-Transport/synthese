
/** Crossing class implementation.
	@file Crossing.cpp

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

#include "Crossing.h"

#include "Address.h"
#include "Road.h"
#include "Registry.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<env::Crossing>::KEY("Crossing");
	}

	namespace env
	{
		Crossing::Crossing(
			util::RegistryKeyType key
			, const City* city
		):	AddressablePlace("X", city),
			Registrable(key)
	    {
	    }
	    

	    
	    Crossing::~Crossing ()
	    {
		
	    }

		bool Crossing::isConnectionAllowed( const Vertex* fromVertex  , const Vertex* toVertex ) const
		{
			return true;
		}

		int Crossing::getTransferDelay( const Vertex* fromVertex  , const Vertex* toVertex ) const
		{
			return 0;
		}




		int Crossing::getMinTransferDelay() const
		{
			return 0;
		}

		void Crossing::addAddress( const Address* address )
		{
			Addresses::const_iterator it;
			for (it = _addresses.begin(); it != _addresses.end(); ++it)
				if ((*it)->getRoad()->getName() == address->getRoad()->getName())
					break;
			if (it == _addresses.end())
				setName(getName() + "/" + address->getRoad()->getName());

			AddressablePlace::addAddress(address);
		}
	}
}
