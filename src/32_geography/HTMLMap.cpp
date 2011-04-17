
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
		const std::string HTMLMap::PARAMETER_ACTION_WKT("actionParamwkt");

		HTMLMap::HTMLMap(
			const Point& center,
			int zoom,
			bool editable,
			bool addable,
			bool highlight,
			const std::string id /*= "map" */
		):	_center(static_cast<Point*>(center.clone())),
			_zoom(zoom),
			_editable(editable),
			_highlight(highlight),
			_id(id)
		{
			if(highlight)
			{
				_controls.push_back(Control(Control::HIGHLIGHT, "highlight", true));
			}
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

			if(!_lineStrings.empty())
			{
				bool first(true);
				stream << "vectorLayer.addFeatures(Array(";
				BOOST_FOREACH(const LineStrings::value_type& lineString, _lineStrings)
				{
					if(first)
					{
						first = false;
					}
					else
					{
						stream << ",";
					}
					stream << "new OpenLayers.Feature.Vector(new OpenLayers.Geometry.LineString(Array(";
					for(size_t i=0; i<lineString.lineString->getNumPoints(); ++i)
					{
						shared_ptr<Point> wgs84Point(
							CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
								*lineString.lineString->getPointN(i)
						)	);

						if(i>0)
						{
							stream << ",";
						}
						stream <<
							"new OpenLayers.Geometry.Point(" <<
								fixed << wgs84Point->getX() << "," << fixed << wgs84Point->getY() <<
							").transform(" <<
								"new OpenLayers.Projection(\"EPSG:4326\")," << // transform from WGS 1984
								"new OpenLayers.Projection(\"EPSG:900913\")" << // to Spherical Mercator Projection
							")"
						;
					}
					stream <<
						")),{" <<
							"'editionStrokeColor':'#000080'," <<
							"'waitingStrokeColor':'#00ffff'," <<
							"'strokeColor':'#ff0000'," <<
							"'requestURL':\"" << HTMLModule::EscapeDoubleQuotes(lineString.updateRequest) << "\"" <<
						"},{" <<
							"'strokeWidth': 5," <<
							"'strokeColor': '#ff0000'" <<
						"}" <<
					")";
				}
				stream << "));";
			}

			if(!_points.empty())
			{
				bool first(true);
				stream << "vectorLayer.addFeatures(Array(";
				BOOST_FOREACH(const Points::value_type& point, _points)
				{
					if(first)
					{
						first = false;
					}
					else
					{
						stream << ",";
					}
					shared_ptr<Point> wgs84Point(
						CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*point.point
					)	);
					stream <<
						"new OpenLayers.Feature.Vector(" <<
							"new OpenLayers.Geometry.Point(" << fixed << wgs84Point->getX() << "," << fixed << wgs84Point->getY() <<
							").transform(" <<
								"new OpenLayers.Projection(\"EPSG:4326\")," << // transform from WGS 1984
								"new OpenLayers.Projection(\"EPSG:900913\")" << // to Spherical Mercator Projection
							"),{graphic:\"" << HTMLModule::EscapeDoubleQuotes(point.icon) << "\"," <<
								"waitingGraphic:\"" << HTMLModule::EscapeDoubleQuotes(point.waitingIcon) << "\"," <<
								"editionGraphic:\"" << HTMLModule::EscapeDoubleQuotes(point.editionIcon) << "\"," <<
								"requestURL:\"" << HTMLModule::EscapeDoubleQuotes(point.updateRequest) << "\",";
					if(_highlight)
					{
						stream <<
								"htmlPopup:\"" << HTMLModule::EscapeDoubleQuotes(point.htmlPopup) << "\"";
					}
					stream <<
							"},{externalGraphic:\"" << HTMLModule::EscapeDoubleQuotes(point.icon) << "\"," <<
							"graphicWidth:" << point.width << "," <<
							"graphicHeight:" << point.height;
					if(!_highlight)
					{
						stream << ",label:\"" << HTMLModule::EscapeDoubleQuotes(point.htmlPopup) << "\"" <<
								",fontSize: '10px'" <<
								",fontWeight: 'bold'" <<
								",labelAlign: 'lt'" <<
								",labelXOffset: '5'" <<
								",labelYOffset: '-5'";
					}
					stream <<
							"}" <<
						")"
					;
				}
				stream << "));";
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
							"if(controls['highlight']) controls['highlight'].deactivate();" <<
							"if(evt.feature.data.editionGraphic) evt.feature.style.externalGraphic = evt.feature.data.editionGraphic;" <<
							"if(evt.feature.data.editionStrokeColor) evt.feature.style.strokeColor = evt.feature.data.editionStrokeColor;" <<
							"vectorLayer.redraw();" <<
						"}," <<
						"'afterfeaturemodified': function(evt) {" <<
							"if(controls['highlight']) controls['highlight'].activate();" <<
							"if(evt.feature.data.waitingGraphic) evt.feature.style.externalGraphic = evt.feature.data.waitingGraphic;\n" <<
							"if(evt.feature.data.waitingStrokeColor) evt.feature.style.strokeColor = evt.feature.data.waitingStrokeColor;" <<
							"vectorLayer.redraw();" <<
							"var feat = evt.feature.clone();" <<
							"feat.geometry.transform(" <<
								"new OpenLayers.Projection(\"EPSG:900913\")," << // to Spherical Mercator Projection
								"new OpenLayers.Projection(\"EPSG:4326\")" << // transform from WGS 1984
							");" <<
							"var writer = new OpenLayers.Format.WKT();" <<
							"var wkt = writer.write(feat);" <<
							"new OpenLayers.Ajax.Request(evt.feature.data.requestURL + '&" << PARAMETER_ACTION_WKT << "='+ wkt," <<
							"{	method: 'get'," <<
								"onComplete: function(transport) {" <<
									"if(evt.feature.data.graphic) evt.feature.style.externalGraphic = evt.feature.data.graphic;" <<
									"if(evt.feature.data.strokeColor) evt.feature.style.strokeColor = evt.feature.data.strokeColor;" <<
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
				"map.setCenter(new OpenLayers.LonLat(" << fixed << wgs84Center->getX() << "," << fixed << wgs84Center->getY() << ")" <<
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
				stream << "ModifyFeature(" << layerName << ", { mode: OpenLayers.Control.ModifyFeature.RESHAPE })";
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
