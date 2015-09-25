
/** HTMLMap class implementation.
	@file HTMLMap.cpp

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

#include "HTMLMap.hpp"
#include "HTMLModule.h"
#include "CoordinatesSystem.hpp"
#include "Env.h"
#include "GetMapOpenLayersConstructorService.hpp"
#include "MapSource.hpp"
#include "SetSessionVariableAction.hpp"
#include "StaticActionFunctionRequest.h"

#include <boost/foreach.hpp>
#include <sstream>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace html;
	using namespace util;
	using namespace server;

	namespace geography
	{
		HTMLMap::HTMLMap(
			const Point& center,
			double horizontalDistance,
			optional<const string&> editFieldName,
			optional<const string&> addFieldName,
			bool highlight,
			bool mousePosition,
			const string id /*= "map" */
		):	_center(dynamic_cast<Point*>(center.clone())),
			_horizontalDistance(horizontalDistance),
			_id(id),
			_editFieldName(editFieldName),
			_addFieldName(addFieldName),
			_highlight(highlight),
			_mousePosition(mousePosition),
			_mapSource(NULL),
			_withMapSourcesMenu(true)
		{
			if(highlight)
			{
				_controls.push_back(Control(Control::HIGHLIGHT, "highlight", true));
			}
			if(_editFieldName)
			{
				_controls.push_back(Control(Control::DRAG, "modify", true));
			}
			if(_addFieldName)
			{
				_controls.push_back(Control(Control::DRAW_POINT, "point", false));
			}
			if(mousePosition)
			{
				_controls.push_back(Control(Control::MOUSE_POSITION, "mouse_position", true));
			}
		}



		void HTMLMap::draw(
			std::ostream& stream,
			const Request& request
		) const {
			// If no base layer is defined, no map can be drawn
			if(!_mapSource)
			{
				return;
			}

			// Map sources menu
			if(_withMapSourcesMenu)
			{
				StaticActionFunctionRequest<SetSessionVariableAction,GetMapOpenLayersConstructorService> changeMapSourceRequest(request, false);
				changeMapSourceRequest.getAction()->setVariable(MapSource::SESSION_VARIABLE_CURRENT_MAPSOURCE);
				changeMapSourceRequest.setRedirectAfterAction(false);
				HTMLForm f(changeMapSourceRequest.getHTMLForm());

				stream << "<div id=\"" << _id << "_menu\">";
				stream << f.open("onsubmit=\"ajaxMapChange(this);return false;\"");
				BOOST_FOREACH(const MapSource::Registry::value_type& mapSource, Env::GetOfficialEnv().getRegistry<MapSource>())
				{
					// Avoid invalid map sources
					if(!mapSource.second->hasCoordinatesSystem())
					{
						continue;
					}

					stream << f.getRadioInput(
						SetSessionVariableAction::PARAMETER_VALUE,
						optional<string>(lexical_cast<string>(mapSource.first)),
						optional<string>(lexical_cast<string>(_mapSource->getKey())),
						mapSource.second->getName()
					);
				}
				stream << " " << f.getSubmitButton("Changer");
				stream << f.close();
				stream << "</div>";
				// TODO : add ajax call on click on the button (replace it by a link button)
			}
			stream << "<div id=\"" << _id << "\"></div>";
			stream << HTMLModule::GetHTMLJavascriptOpen("/lib/openlayers/OpenLayers.js");

			// For Geoportail
			BOOST_FOREACH(const MapSource::Registry::value_type& mapSource, Env::GetOfficialEnv().getRegistry<MapSource>())
			{
				if(mapSource.second->getType() == MapSource::IGN)
				{
					stringstream streamTemp;
					streamTemp << "http://api.ign.fr/geoportail/api/js/1.3.0/GeoportalMin.js";
					stream << HTMLModule::GetHTMLJavascriptOpen(streamTemp.str());
				}
			}


			stream << HTMLModule::GetHTMLJavascriptOpen();
			stream << "var map, vectors, controls;";
			//support functions
			stream <<
				"var lastFeature = null;" <<
				"var tooltipPopup = null;" <<
				"var addURL = '';";

			// Features function
			stream <<
				"function getFeaturesLayer(mapProjection){" <<
				"var vectorLayer = new OpenLayers.Layer.Vector('Vector');";

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
						boost::shared_ptr<Point> wgs84Point(
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
								"mapProjection" <<
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
					boost::shared_ptr<Point> wgs84Point(
						CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(
							*point.point
					)	);
					stream <<
						"new OpenLayers.Feature.Vector(" <<
							"new OpenLayers.Geometry.Point(" << fixed << wgs84Point->getX() << "," << fixed << wgs84Point->getY() <<
							").transform(" <<
								"new OpenLayers.Projection(\"EPSG:4326\")," << // transform from WGS 1984
								"mapProjection" << // to map projection
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

			if(_editFieldName || _addFieldName)
			{
				stream << "vectorLayer.events.on({";
			}

			// Moving by drag and drop
			if(_editFieldName)
			{
				stream <<
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
								"mapProjection," << // from map projection
								"new OpenLayers.Projection(\"EPSG:4326\")" << // to WGS 1984
							");" <<
							"var writer = new OpenLayers.Format.WKT();" <<
							"var wkt = writer.write(feat);" <<
							"new OpenLayers.Ajax.Request(evt.feature.data.requestURL + '&" << *_editFieldName << "='+ wkt," <<
							"{	method: 'get'," <<
								"onComplete: function(transport) {" <<
									"if(evt.feature.data.graphic) evt.feature.style.externalGraphic = evt.feature.data.graphic;" <<
									"if(evt.feature.data.strokeColor) evt.feature.style.strokeColor = evt.feature.data.strokeColor;" <<
									"vectorLayer.redraw();" <<
								"}" <<
							"});" <<
						"}"
					;
			}
			if(_addFieldName)
			{
				if(_editFieldName)
				{
					stream << ",";
				}
				stream <<
						"'featureadded': function(evt) {" <<
							"var newpoint = evt.feature.clone();" <<
							"newpoint.geometry.transform(" <<
								"mapProjection," << // from map projection
								"new OpenLayers.Projection(\"EPSG:4326\")" << // to WGS 1984
							");" <<
							"var writer = new OpenLayers.Format.WKT();" <<
							"var wkt = writer.write(newpoint);" <<
							"new OpenLayers.Ajax.Request(addURL + '&" << *_addFieldName << "='+ wkt," <<
							"{   method: 'get'," <<
								"onComplete: function(transport) {" <<
//									"evt.feature.style.externalGraphic = evt.feature.data.graphic;" <<
									"vectorLayer.redraw();" <<
								"}" <<
							"});" <<
							"controls['point'].deactivate();" <<
						"}"
					;
			}
			if(_addFieldName || _editFieldName)
			{
				stream << "});";
			}
			stream <<
				"return vectorLayer;" <<
				"}"
			;

			// First initialization
			boost::shared_ptr<Point> firstPoint(
				CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
					_center->getX() - _horizontalDistance / 2,
					_center->getY() - _horizontalDistance / 2
			)	);
			boost::shared_ptr<Point> firstPointProjected(
				_mapSource->getCoordinatesSystem().convertPoint(
					*firstPoint
			)	);
			boost::shared_ptr<Point> secondPoint(
				CoordinatesSystem::GetInstanceCoordinatesSystem().createPoint(
					_center->getX() + _horizontalDistance / 2,
					_center->getY() + _horizontalDistance / 2
			)	);
			boost::shared_ptr<Point> secondPointProjected(
				_mapSource->getCoordinatesSystem().convertPoint(
					*secondPoint
			)	);


			stream <<
            "function loadMap() {" <<
               _mapSource->getOpenLayersConstructor() <<
			   "map.addLayer(getFeaturesLayer(map.getProjectionObject()));" <<
			   "var bounds = new OpenLayers.Bounds();" <<
			   "bounds.extend(new OpenLayers.LonLat(" << fixed << firstPointProjected->getX() << "," << fixed << firstPointProjected->getY() << "));" <<
			   "bounds.extend(new OpenLayers.LonLat(" << fixed << secondPointProjected->getX() << "," << fixed << secondPointProjected->getY() << "));" <<
			   "map.zoomToExtent(bounds, true);" <<
            "}";

			// For Geoportail
			BOOST_FOREACH(const MapSource::Registry::value_type& mapSource, Env::GetOfficialEnv().getRegistry<MapSource>())
			{
				if(mapSource.second->getType() == MapSource::IGN)
				{
//					stream << "if (checkApiLoading('loadAPI();',['OpenLayers','Geoportal'])===false) {}";
					stream << "Geoportal.GeoRMHandler.getConfig(['" << mapSource.second->getURL() << "'], null,null);";
				}
			}

            stream << "window.onload = loadMap;" <<

			"function activateAddPoint_" << _id << "(requestURL)" <<
			"{" <<
				"addURL = requestURL;"
				"controls['point'].activate();" <<
			"}";

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
			else if(_controlType == Control::MOUSE_POSITION)
			{
				stream << "MousePosition()";
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
}	}
