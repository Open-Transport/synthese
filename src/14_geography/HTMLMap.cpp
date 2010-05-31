
/** HTMLMap class implementation.
	@file HTMLMap.cpp

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

#include "HTMLMap.hpp"
#include "HTMLModule.h"
#include "Projection.h"
#include "GeoPoint.h"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace geometry;
	using namespace html;

	namespace geography
	{
		HTMLMap::HTMLMap(
			const geometry::Point2D& center,
			int zoom,
			const std::string id /*= "map" */
		):	_center(center.getX(), center.getY()),
			_zoom(zoom),
			_id(id)
		{

		}



		void HTMLMap::draw( std::ostream& stream ) const
		{
			stream << "<div id=\"" << _id << "\"></div>";
			stream << HTMLModule::GetHTMLJavascriptOpen("http://www.openlayers.org/api/OpenLayers.js");
			stream << HTMLModule::GetHTMLJavascriptOpen();
			
			stream << "var map, vectors, controls;";
			stream << "function init(){";
			stream << "map = new OpenLayers.Map('map');";
			stream << "var mapnik = new OpenLayers.Layer.OSM();";
			stream << "var vectorLayer = new OpenLayers.Layer.Vector(\"Vector\");";
			stream << "map.addLayers([mapnik, vectorLayer]);";

			BOOST_FOREACH(const Point& point, _points)
			{
				GeoPoint geoPoint(WGS84FromLambert(point.point));
				stream << "vectorLayer.addFeatures(new OpenLayers.Feature.Vector(" <<
				"new OpenLayers.Geometry.Point(" << geoPoint.getLongitude() << "," << geoPoint.getLatitude() << ").transform(" <<
				"new OpenLayers.Projection(\"EPSG:4326\")," << // transform from WGS 1984
				"new OpenLayers.Projection(\"EPSG:900913\")" << // to Spherical Mercator Projection
				"),"
				"{editionGraphic:'" << point.editionIcon << "'}," <<
				"{externalGraphic:'" << point.icon << "', graphicHeight: 25, graphicWidth: 21}));";
			}

/*
				var modifyFeature = new OpenLayers.Control.ModifyFeature(vectorLayer, {
mode: OpenLayers.Control.ModifyFeature.DRAG,
				});
				map.addControl(modifyFeature);
				modifyFeature.activate();
				vectorLayer.events.on({
					'beforefeaturemodified': function(evt) {
						evt.feature.style.externalGraphic = 'marker.png';
					},
						'afterfeaturemodified': function(evt) {
							evt.feature.style.externalGraphic = 'aqua.png';
							vectorLayer.redraw();
							var newpoint = evt.feature.geometry.clone();
							newpoint.transform(
								new OpenLayers.Projection("EPSG:900913"), // to Spherical Mercator Projection
								new OpenLayers.Projection("EPSG:4326") // transform from WGS 1984

								);
							alert("Save "+ newpoint.x + "/"+ newpoint.y);
					}
				});
*/

			GeoPoint center(WGS84FromLambert(_center));
			stream <<
				"map.setCenter(new OpenLayers.LonLat(" << center.getLongitude() << "," << center.getLatitude() << ")" <<
				".transform(" <<
					"new OpenLayers.Projection(\"EPSG:4326\"), new OpenLayers.Projection(\"EPSG:900913\")" << // WGS84 to Spherical Mercator Projection
				")," << _zoom << // Zoom level
			");" <<
			"}" <<
			"init();";
			
			stream << HTMLModule::GetHTMLJavascriptClose();
		}
	}
}
