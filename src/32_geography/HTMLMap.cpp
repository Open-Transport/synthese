
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
#include "CoordinatesSystem.hpp"

#include <boost/foreach.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace html;

	namespace geography
	{
		HTMLMap::HTMLMap(
			const Point& center,
			int zoom,
			bool editable,
			bool addable,
			const std::string id /*= "map" */
		):	_center(static_cast<Point*>(center.clone())),
			_zoom(zoom),
			_editable(editable),
			_id(id)
		{
			_controls.push_back(Control(Control::HIGHLIGHT, "highlight", true));
			if(editable)
			{
				_controls.push_back(Control(Control::DRAG, "modify", true));
			}
			if(addable)
			{
				_controls.push_back(Control(Control::DRAW_POINT, "point", false));
			}
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
				"var addURL = '';" <<

				"function init(){" <<
					"map = new OpenLayers.Map('map');" <<
					"var mapnik = new OpenLayers.Layer.OSM();" <<
					"var vectorLayer = new OpenLayers.Layer.Vector(\"Vector\");" <<
					"map.addLayers([mapnik, vectorLayer]);"
			;

			BOOST_FOREACH(const Points::value_type& point, _points)
			{
				shared_ptr<Point> wgs84Point(
					CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
						*point.point
				)	);
				stream <<
					"vectorLayer.addFeatures(" <<
						"new OpenLayers.Feature.Vector(" <<
							"new OpenLayers.Geometry.Point(" << wgs84Point->getX() << "," << wgs84Point->getY() << ").transform(" <<
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

			if(!_controls.empty())
			{
				stream << "controls = {";
				bool first(true);
				BOOST_FOREACH(const Control& control, _controls)
				{
					if(first)
					{
						first = false;
					}
					else
					{
						stream << ",";
					}
					stream << control.getInitializationString("vectorLayer");
				}
				stream << "};";
				BOOST_FOREACH(const Control& control, _controls)
				{
					stream << "map.addControl(controls['" << control.getName() << "']);";
					if(control.getActivated())
					{
						stream << "controls['" << control.getName() << "'].activate();";
					}
				}
			}


			// Moving by drag and drop
			if(_editable)
			{
				stream <<
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
							"});" <<
						"}," <<
						"'featureadded': function(evt) {" <<
							"var newpoint = evt.feature.geometry.clone();" <<
							"newpoint.transform(" <<
								"new OpenLayers.Projection(\"EPSG:900913\")," << // to Spherical Mercator Projection
								"new OpenLayers.Projection(\"EPSG:4326\")" << // transform from WGS 1984
							");" <<
							"new OpenLayers.Ajax.Request(addURL + '&actionParamlon='+ newpoint.x +'&actionParamlat=' + newpoint.y," <<
							"{   method: 'get'," <<
								"onComplete: function(transport) {" <<
//									"evt.feature.style.externalGraphic = evt.feature.data.graphic;" <<
									"vectorLayer.redraw();" <<
								"}" <<
							"});" <<
							"controls['point'].deactivate();" <<
						"}" <<
					"});"
				;
			}

			shared_ptr<Point> wgs84Center(
				CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
					*_center
			)	);
			stream <<
				"map.setCenter(new OpenLayers.LonLat(" << wgs84Center->getX() << "," << wgs84Center->getY() << ")" <<
				".transform(" <<
					"new OpenLayers.Projection(\"EPSG:4326\"), new OpenLayers.Projection(\"EPSG:900913\")" << // WGS84 to Spherical Mercator Projection
				")," << _zoom << // Zoom level
			");" <<
			"}" <<
			"init();" <<
			"function activateAddPoint_" << _id << "(requestURL)" <<
			"{" <<
				"addURL = requestURL;"
				"controls['point'].activate();" <<
			"}"
			;

			stream << HTMLModule::GetHTMLJavascriptClose();
		}



		std::string HTMLMap::getAddPointLink(
			const std::string& requestURL,
			const std::string& content
		) const {
			stringstream stream;
			stream << "<a href=\"#\" onclick=\"activateAddPoint_" << _id << "('" << requestURL << "');\">" << content << "</a>";
			return stream.str();
		}



		std::string HTMLMap::Control::getInitializationString(
			const std::string& layerName
		) const	{
			stringstream stream;
			stream << _name << ": new OpenLayers.Control.";
			if(_controlType == Control::DRAW_POINT)
			{
				stream << "DrawFeature(" << layerName << ", OpenLayers.Handler.Point)";
			}
			else if(_controlType == Control::DRAG)
			{
				stream << "ModifyFeature(" << layerName << ", { mode: OpenLayers.Control.ModifyFeature.DRAG })";
			}
			else if(_controlType == Control::HIGHLIGHT)
			{
				stream <<
					"SelectFeature(" << layerName << ", {" <<
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
					" }	})"
				;
			}
			return stream.str();
		}
	}
}
