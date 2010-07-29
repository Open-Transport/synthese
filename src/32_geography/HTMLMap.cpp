
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
			bool editable,
			const std::string id /*= "map" */
		):	_center(center.getX(), center.getY()),
			_zoom(zoom),
			_editable(editable),
			_id(id)
		{
		}



		void HTMLMap::draw( std::ostream& stream ) const
		{
			stream << "<div id=\"" << _id << "\"></div>";
			stream << HTMLModule::GetHTMLJavascriptOpen("http://www.openlayers.org/api/OpenLayers.js");
			stream << HTMLModule::GetHTMLJavascriptOpen();
			
			stream << "var map, vectors, controls;";
			//support functions
			stream <<
				"var lastFeature = null;" <<
				"var tooltipPopup = null;" <<

				"function init(){" <<
					"map = new OpenLayers.Map('map');" <<
					"var mapnik = new OpenLayers.Layer.OSM();" <<
					"var vectorLayer = new OpenLayers.Layer.Vector(\"Vector\");" <<
					"map.addLayers([mapnik, vectorLayer]);"
			;

			BOOST_FOREACH(const Point& point, _points)
			{
				GeoPoint geoPoint(WGS84FromLambert(point.point));
				stream <<
					"vectorLayer.addFeatures(" <<
						"new OpenLayers.Feature.Vector(" <<
							"new OpenLayers.Geometry.Point(" << geoPoint.getLongitude() << "," << geoPoint.getLatitude() << ").transform(" <<
								"new OpenLayers.Projection(\"EPSG:4326\")," << // transform from WGS 1984
								"new OpenLayers.Projection(\"EPSG:900913\")" << // to Spherical Mercator Projection
							"),{ graphic:\"" << HTMLModule::EscapeDoubleQuotes(point.icon) << "\"," <<
								"waitingGraphic:\"" << HTMLModule::EscapeDoubleQuotes(point.waitingIcon) << "\"," <<
								"editionGraphic:\"" << HTMLModule::EscapeDoubleQuotes(point.editionIcon) << "\"," <<
								"requestURL:\"" << HTMLModule::EscapeDoubleQuotes(point.updateRequest) << "\"," <<
								"htmlPopup:\"" << HTMLModule::EscapeDoubleQuotes(point.htmlPopup) << "\"" <<
							"},{ externalGraphic:\"" << HTMLModule::EscapeDoubleQuotes(point.icon) << "\"," <<
								"graphicHeight: 25," <<
								"graphicWidth: 21" <<
							"}" <<
					")	);"
				;
			}

			// Popup
			stream <<
				"var highlightCtrl = new OpenLayers.Control.SelectFeature(vectorLayer, {" <<
					"hover: true, highlightOnly: true, renderIntent: \"temporary\"," <<
					"eventListeners: {" <<
						"featurehighlighted:	function(event){" <<
							"var feature = event.feature;" <<
							"var selectedFeature = feature;" <<
							//if there is already an opened details window, don\'t draw the    tooltip
							"if(feature.popup != null){" <<
								"return;" <<
							"}" <<
							//if there are other tooltips active, destroy them
							"if(tooltipPopup != null){" <<
								"map.removePopup(tooltipPopup);" <<
								"tooltipPopup.destroy();" <<
								"if(lastFeature != null){" <<
									"delete lastFeature.popup;" <<
									"tooltipPopup = null;" <<
								"}" <<
							"}" <<
							"lastFeature = feature;" <<
							"var tooltipPopup = new OpenLayers.Popup('activetooltip'," <<
								"feature.geometry.getBounds().getCenterLonLat()," <<
								"new OpenLayers.Size(200,40)," <<
								"feature.data.htmlPopup," <<
								"false);"
							//this is messy, but I'm not a CSS guru
							"tooltipPopup.contentDiv.style.backgroundColor='ffffcb';" <<
							"tooltipPopup.contentDiv.style.overflow='hidden';" <<
							"tooltipPopup.contentDiv.style.padding='3px';" <<
							"tooltipPopup.contentDiv.style.margin='0';" <<
							"tooltipPopup.closeOnMove = true;" <<
							"tooltipPopup.autoSize = true;" <<
							"feature.popup = tooltipPopup;" <<
							"map.addPopup(tooltipPopup);" <<
						"}," <<
						"featureunhighlighted: function (event){" <<
							"var feature = event.feature;" <<
							"if(feature != null && feature.popup != null){" <<
								"map.removePopup(feature.popup);" <<
								"feature.popup = null;" <<
								"tooltipPopup = null;" <<
								"lastFeature = null;" <<
							"}" <<
						"}" <<
				" }	});" <<

				"map.addControl(highlightCtrl);" <<
				"highlightCtrl.activate();"
			;


			// Moving by drag and drop
			if(_editable)
			{
				stream <<
					"var modifyFeature = new OpenLayers.Control.ModifyFeature(vectorLayer, {" <<
					"mode: OpenLayers.Control.ModifyFeature.DRAG," <<
					"});" <<
					"map.addControl(modifyFeature);" <<
					"modifyFeature.activate();" <<
					"vectorLayer.events.on({" <<
						"'beforefeaturemodified': function(evt) {" <<
							"evt.feature.style.externalGraphic = evt.feature.data.editionGraphic;" <<
							"vectorLayer.redraw();" <<
						"}," <<
						"'afterfeaturemodified': function(evt) {" <<
							"evt.feature.style.externalGraphic = evt.feature.data.waitingGraphic;" <<
							"vectorLayer.redraw();" <<
							"var newpoint = evt.feature.geometry.clone();" <<
							"newpoint.transform(" <<
								"new OpenLayers.Projection(\"EPSG:900913\")," << // to Spherical Mercator Projection
								"new OpenLayers.Projection(\"EPSG:4326\")" << // transform from WGS 1984
							");" <<
							"new OpenLayers.Ajax.Request(evt.feature.data.requestURL + '&actionParamlon='+ newpoint.x +'&actionParamlat=' + newpoint.y," <<
							"{   method: 'get'," <<
								"onComplete: function(transport) {" <<
									"evt.feature.style.externalGraphic = evt.feature.data.graphic;" <<
									"vectorLayer.redraw();" <<
								"}" <<
							"}" <<
						");" <<
					"}" <<
				"});";
			}

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
