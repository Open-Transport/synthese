
/** Address class implementation.
	@file Address.cpp

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

#include "Address.h"
#include "ConnectionPlace.h"
#include "Edge.h"
#include "Registry.h"

using namespace std;

namespace synthese
{
	namespace util
	{
		template<> const string Registry<env::Address>::KEY("Address");
	}

	namespace env
	{


		Address::Address (const uid id,
				  const AddressablePlace* place,
				  const Road* road, 
				  double metricOffset,
				  double x,
				  double y)
		:	util::Registrable(id)
			, Vertex (place, x, y)
			, _road (road)
			, _metricOffset (metricOffset)
		{

		}



		Address::~Address()
		{
		}



		const Road* 
		Address::getRoad() const
		{
			return _road;
		}



		double 
		Address::getMetricOffset () const
		{
			return _metricOffset;
		}



		bool 
		Address::isAddress () const
		{
			return true;
		}



		bool 
		Address::isPhysicalStop () const
		{
			return false;
		}



		void Address::setRoad( const Road* road )
		{
			_road = road;
		}

		void Address::setMetricOffset( double value )
		{
			_metricOffset = value;
		}

		bool Address::isConnectionAllowed() const
		{
			return getConnectionPlace()->getConnectionType() >= ConnectionPlace::CONNECTION_TYPE_ROADROAD;
		}
	}
}


