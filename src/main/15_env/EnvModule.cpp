
/** EnvModule class implementation.
	@file EnvModule.cpp

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

#include "15_env/EnvModule.h"

namespace synthese
{
	namespace env
	{
		Environment::Registry		EnvModule::_environments;
		City::Registry				EnvModule::_cities;
		ConnectionPlace::Registry	EnvModule::_connectionPlaces;
		PhysicalStop::Registry		EnvModule::_physicalStops;

		void EnvModule::initialize()
		{
		}


		Environment::Registry& 
			EnvModule::getEnvironments ()
		{
			return _environments;
		}

		City::Registry& EnvModule::getCities()
		{
			return _cities;
		}

		ConnectionPlace::Registry& EnvModule::getConnectionPlaces()
		{
			return _connectionPlaces;
		}

		PhysicalStop::Registry& EnvModule::getPhysicalStops()
		{
			return _physicalStops;
		}

		EnvModule::~EnvModule()
		{
			_physicalStops.clear ();
			_cities.clear ();
			_connectionPlaces.clear ();
		}
	}
}
