
/** MapSource class implementation.
	@file MapSource.cpp

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
#include "Env.h"
#include "MapSource.hpp"
#include "Session.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace geography;
	
	namespace util
	{
		template<> const string Registry<MapSource>::KEY("MapSource");
	}

	namespace geography
	{
		const int MapSource::MAX_TYPE_INT(1);
		const string MapSource::SESSION_VARIABLE_CURRENT_MAPSOURCE("current_mapsource");



		MapSource::MapSource(
			RegistryKeyType id
		):	Registrable(id),
			_coordinatesSystem(NULL),
			_type(WMS)
		{}



		const CoordinatesSystem& MapSource::getCoordinatesSystem() const
		{
			if(!_coordinatesSystem)
			{
				throw NoCoordinatesSystemException(getName());
			}
			return *_coordinatesSystem;
		}



		void MapSource::setCoordinatesSystem( const CoordinatesSystem& value )
		{
			_coordinatesSystem = &value;
		}



		std::string MapSource::getOpenLayersConstructor() const
		{
			switch(_type)
			{
			case OSM:
				return "new OpenLayers.Layer.OSM()";

			case WMS:
				return "new OpenLayers.Layer.WMS(\""+ getName() +" WMS\",\""+ _url +"\", {},{projection:'epsg:"+ lexical_cast<string>(_coordinatesSystem->getSRID()) +"', minScale:500, maxScale:30000})";
			}

			assert(false);
			return string();
		}



		MapSource* MapSource::GetSessionMapSource(
			const Session& session
		){
			// If no base layer is configured in SYNTHESE, no map can be drawn
			if(Env::GetOfficialEnv().getRegistry<MapSource>().empty())
			{
				return NULL;
			}

			// Current map source
			string sessionMapSourceId(session.getSessionVariable(SESSION_VARIABLE_CURRENT_MAPSOURCE));
			try
			{
				RegistryKeyType sessionMapSourceUId(lexical_cast<RegistryKeyType>(sessionMapSourceId));
				return Env::GetOfficialEnv().getEditable<MapSource>(sessionMapSourceUId).get();
			}
			catch(bad_cast&)
			{
			}
			catch(ObjectNotFoundException<MapSource>&)
			{
			}

			// Default map source
			BOOST_FOREACH(const MapSource::Registry::value_type& ms, Env::GetOfficialEnv().getEditableRegistry<MapSource>())
			{
				if(!ms.second->hasCoordinatesSystem())
				{
					continue;
				}
				return ms.second.get();
			}

			// No valid map source
			return NULL;
		}



		string MapSource::GetTypeString(Type type)
		{
			switch(type)
			{
			case OSM: return "OSM";
			case WMS: return "WMS";
			}
			return string();
		}



		MapSource::TypesMap MapSource::GetTypesMap()
		{
			TypesMap result;
			result.insert(make_pair(optional<Type>(OSM), GetTypeString(OSM)));
			result.insert(make_pair(optional<Type>(WMS), GetTypeString(WMS)));
			return result;
		}



		MapSource::NoCoordinatesSystemException::NoCoordinatesSystemException(
			const string& name
		):	synthese::Exception("The CoordinatesSystem of the "+ name +" map source is not defined.")
		{}


		bool MapSource::hasCoordinatesSystem() const
		{
			return _coordinatesSystem != NULL;
		}
}	}
