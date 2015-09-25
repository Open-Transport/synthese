
/** HTMLMap class header.
	@file HTMLMap.hpp

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

#ifndef SYNTHESE_html_HTMLMap_hpp__
#define SYNTHESE_html_HTMLMap_hpp__

#include <vector>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace geography
	{
		class MapSource;

		//////////////////////////////////////////////////////////////////////////
		/// Draws map using OpenLayers library.
		///	@ingroup m05
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.18
		//////////////////////////////////////////////////////////////////////////
		/// OpenLayers API documentation : http://dev.openlayers.org/apidocs/
		///
		/// Usage :
		/// <ol>
		///		<li>Create the object with its constructor</li>
		///		<li>Add objects to it with the update methods</li>
		///		<li>Generate the HTML code and flush it onto a stream with the draw() method</li>
		///	</ol>
		///
		/// If _editFieldName is undefined, the the map is considered as uneditable.
		class HTMLMap
		{
		public:
			struct MapPoint
			{
				boost::shared_ptr<geos::geom::Point> point;
				std::string icon;
				std::string editionIcon;
				std::string waitingIcon;
				std::string updateRequest;
				std::string htmlPopup;
				std::size_t width;
				std::size_t height;


				MapPoint(
					const geos::geom::Point& _point,
					const std::string& _icon,
					const std::string& _editionIcon,
					const std::string& _waitingIcon,
					const std::string& _updateRequest,
					const std::string& _htmlPopup,
					std::size_t _width,
					std::size_t _height
				):	point(dynamic_cast<geos::geom::Point*>(_point.clone())),
					icon(_icon),
					editionIcon(_editionIcon),
					waitingIcon(_waitingIcon),
					updateRequest(_updateRequest),
					htmlPopup(_htmlPopup),
					width(_width),
					height(_height)
				{}
			};
			typedef std::vector<MapPoint> Points;

			struct MapLineString
			{
				boost::shared_ptr<geos::geom::LineString> lineString;
				std::string updateRequest;

				MapLineString(
					const geos::geom::LineString& _lineString,
					const std::string& _updateRequest
				):	lineString(dynamic_cast<geos::geom::LineString*>(_lineString.clone())),
					updateRequest(_updateRequest)
				{}
			};
			typedef std::vector<MapLineString> LineStrings;

			class Control
			{
			public:
				enum ControlType
				{
					DRAW_POINT,
					DRAG,
					HIGHLIGHT,
					MOUSE_POSITION
				};

			private:
				ControlType _controlType;
				std::string _name;
				bool _activated;

			public:
				Control(
					ControlType controlType,
					const std::string& name,
					bool activated
				):	_controlType(controlType),
					_name(name),
					_activated(activated)
				{}

				std::string getInitializationString(const std::string& layerName) const;

				const std::string& getName() const { return _name; }
				bool getActivated() const { return _activated; }
			};

			typedef std::vector<Control> Controls;

		private:
			boost::shared_ptr<geos::geom::Point> _center;
			const double _horizontalDistance;
			const std::string _id;
			Points _points;
			LineStrings _lineStrings;
			Controls _controls;
			const boost::optional<std::string> _editFieldName;
			const boost::optional<std::string> _addFieldName;
			const bool _highlight;
			const bool _mousePosition;
			const MapSource* _mapSource;
			bool _withMapSourcesMenu;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Map constructor.
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.18
			/// @param center point in the middle of the map at opening (instance coordinates system)
			/// @param horizontalDistance distance between left and right bounds at map opening (unit : instance coordinates system unit)
			/// @param editFieldName name of the geometry field to send to the action when the icon is moved
			///	       by the user by drag and drop (if undefined, then the map is considered as read only).
			/// @param addable allow the user to add a point in the map
			/// @param highlight activates the highlight control
			/// @param mousePosition activates the mouse position window
			/// @param id id of the div in the DOM
			HTMLMap(
				const geos::geom::Point& center,
				double horizontalDistance,
				boost::optional<const std::string&> editFieldName,
				boost::optional<const std::string&> addFieldName,
				bool highlight,
				bool mousePosition,
				const std::string id = "map"
			);


			//! @name Update methods
			//@{
				void addPoint(MapPoint value){ _points.push_back(value); }
				void addLineString(MapLineString value){ _lineStrings.push_back(value); }
				void setMapSource(const MapSource* value){ _mapSource = value; }
				void setWithMapSourcesMenu(bool value){ _withMapSourcesMenu = value; }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Generates the HTML code and flush it onto a stream.
				/// @param stream stream to write the code on
				/// @param request the current request
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.18
				/// @param stream stream to write the code on
				void draw(
					std::ostream& stream,
					const server::Request& request
				) const;



				std::string getAddPointLink(
					const std::string& requestURL,
					const std::string& content
				) const;
			//@}
		};
	}
}

#endif // SYNTHESE_html_HTMLMap_hpp__
