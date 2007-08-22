
/** TransportNetwork class implementation.
	@file TransportNetwork.cpp

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

#include "15_env/TransportNetwork.h"

namespace synthese
{
	namespace util
	{
		template<> typename Registrable<uid,env::TransportNetwork>::Registry Registrable<uid,env::TransportNetwork>::_registry;
	}

	namespace env
	{
		TransportNetwork::TransportNetwork (uid id, std::string name)
			: synthese::util::Registrable<uid,TransportNetwork> (id)
			, _name (name)
		{
		}


		TransportNetwork::~TransportNetwork()
		{
		}



		const std::string& 
		TransportNetwork::getName () const
		{
			return _name;
		}



		void 
		TransportNetwork::setName( const std::string& name)
		{
			_name = name;
		}



	}
}
