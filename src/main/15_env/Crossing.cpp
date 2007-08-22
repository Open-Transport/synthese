
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

#include <assert.h>

using namespace std;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> typename Registrable<uid,env::Crossing>::Registry Registrable<uid,env::Crossing>::_registry;
	}

	namespace env
	{
		Crossing::Crossing(
			const uid& key
			, const City* city
		)	: ConnectionPlace (string(), city, CONNECTION_TYPE_ROADROAD)
			, Registrable<uid,Crossing>(key)
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

		int Crossing::getScore() const
		{
			return 50;
		}



		uid Crossing::getId() const
		{
			return getKey();
		}

		int Crossing::getMinTransferDelay() const
		{
			return 0;
		}
	}
}
