
/** HikingTrail class implementation.
	@file HikingTrail.cpp

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

#include "HikingTrail.h"

using namespace std;

namespace synthese
{
	using namespace pt;

	namespace util
	{
		template<> const string Registry<hiking::HikingTrail>::KEY("HikingTrail");
	}

	namespace hiking
	{
		using namespace util;




		HikingTrail::HikingTrail( util::RegistryKeyType id  ):
			Registrable(id)
		{

		}



		const std::string& HikingTrail::getMap() const
		{
			return _map;
		}



		const std::string& HikingTrail::getDuration() const
		{
			return _duration;
		}



		const std::string& HikingTrail::getProfile() const
		{
			return _profile;
		}



		const HikingTrail::Stops& HikingTrail::getStops() const
		{
			return _stops;
		}



		void HikingTrail::setMap( const std::string& value )
		{
			_map = value;
		}



		void HikingTrail::setDuration( const std::string& value )
		{
			_duration = value;
		}



		void HikingTrail::setProfile( const std::string& value )
		{
			_profile = value;
		}



		void HikingTrail::clearStops()
		{
			_stops.clear();
		}



		void HikingTrail::addStop(PublicTransportStopZoneConnectionPlace* value, size_t rank )
		{
			if(rank >= _stops.size())
			{
				_stops.insert(_stops.end(), value);
			}
			else
			{
				_stops.insert(_stops.begin() + rank, value);
			}
		}



		void HikingTrail::removeStop( size_t rank )
		{
			_stops.erase(_stops.begin() + rank);
		}
	}
}
