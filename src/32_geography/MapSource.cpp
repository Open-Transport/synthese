
/** MapSource class implementation.
	@file MapSource.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
		const int MapSource::MAX_TYPE_INT(2);
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
				return "map = new OpenLayers.Map('map'); map.addLayer(new OpenLayers.Layer.OSM());";

			case WMS:
				return "map = new OpenLayers.Map('map'); map.addLayer(new OpenLayers.Layer.WMS(\""+ getName() +" WMS\",\""+ _url +"\", {},{projection:'EPSG:"+ lexical_cast<string>(_coordinatesSystem->getSRID()) +"', minScale:500, maxScale:30000}));";

			case IGN:
				stringstream str;
				str <<

				"var epsg4258= new OpenLayers.Projection('EPSG:4258');" <<
				"map= new OpenLayers.Map('map', OpenLayers.Util.extend({" <<
				"maxResolution: 1.40625," <<
				"numZoomLevels: 21," <<
				"projection: epsg4258," <<
				"units: epsg4258.getUnits()," <<
				"maxExtent: new OpenLayers.Bounds(-180, -90, 180, 90)" <<
				"}," <<
				"gGEOPORTALRIGHTSMANAGEMENT));" <<

				"var cat= new Geoportal.Catalogue(map,gGEOPORTALRIGHTSMANAGEMENT);" <<
				"var zon= cat.getTerritory('EUE');" <<

				"map.addLayers([" <<
				"new OpenLayers.Layer(" <<
				"'__PlateCarre__'," <<
				"{" <<
				"isBaseLayer: true," <<
				"projection: new OpenLayers.Projection('EPSG:4326')," <<
				"units: 'degrees'," <<
				"maxResolution: 1.40625," <<
				"numZoomLevels: 21," <<
				"maxExtent: new OpenLayers.Bounds(-180, -90, 180, 90)," <<
				"minZoomLevel:5," <<
				"maxZoomLevel:18," <<
				"territory:'EUE'" <<
				"})]);" <<


				"var europeanMapOpts= cat.getLayerParameters(zon, 'GEOGRAPHICALGRIDSYSTEMS.MAPS');" <<
				"europeanMapOpts.options.opacity= 1.0;" <<
				"europeanMapOpts.options['GeoRM']= setGeoRM();" <<
				"var europeanMap= new europeanMapOpts.classLayer(" <<
					"OpenLayers.i18n(europeanMapOpts.options.name)," <<
					"europeanMapOpts.url," <<
					"europeanMapOpts.params," <<
					"europeanMapOpts.options);" <<
				"europeanMapOpts.options.maxExtent.transform(europeanMapOpts.options.projection, map.getProjection(), true);" <<

				"map.addLayers([europeanMap]);";

				return str.str();
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
			case IGN: return "IGN Geoportail";
			}
			return string();
		}



		MapSource::TypesMap MapSource::GetTypesMap()
		{
			TypesMap result;
			result.insert(make_pair(optional<Type>(OSM), GetTypeString(OSM)));
			result.insert(make_pair(optional<Type>(WMS), GetTypeString(WMS)));
			result.insert(make_pair(optional<Type>(IGN), GetTypeString(IGN)));
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
