
/** CoordinatesSystem class implementation.
	@file CoordinatesSystem.cpp

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

#include "CoordinatesSystem.hpp"
#include "DBModule.h"

using namespace std;

namespace synthese
{
	using namespace db;

	namespace geography
	{
		CoordinatesSystem::Map CoordinatesSystem::_coordinates_systems;
		CoordinatesSystem::Map::const_iterator CoordinatesSystem::_defaultCoordinatesSystem(CoordinatesSystem::_coordinates_systems.end());


		void CoordinatesSystem::AddCoordinatesSystem(
			SRID srid,
			const string& name,
			const string& projSequence
		){
			_coordinates_systems[srid] = CoordinatesSystem(srid, name, projSequence);
		}



		const CoordinatesSystem& CoordinatesSystem::GetCoordinatesSystem(
			SRID srid
		){
			Map::const_iterator it(_coordinates_systems.find(srid));
			if(it == _coordinates_systems.end())
			{
				throw NotFoundException(srid);
			}
			return it->second;
		}



		const CoordinatesSystem& CoordinatesSystem::GetInstanceCoordinateSystem()
		{
			if(_defaultCoordinatesSystem == _coordinates_systems.end())
			{
				_defaultCoordinatesSystem = _coordinates_systems.find(DBModule::GetInstanceSRID());
			}
			return _defaultCoordinatesSystem->second;
		}
}	}
