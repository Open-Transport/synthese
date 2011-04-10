
/** HTMLMap class header.
	@file HTMLMap.hpp

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

#ifndef SYNTHESE_html_HTMLMap_hpp__
#define SYNTHESE_html_HTMLMap_hpp__

#include <vector>
#include <boost/shared_ptr.hpp>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>

namespace synthese
{
	namespace geography
	{
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
		class HTMLMap
		{
		public:
			static const std::string PARAMETER_ACTION_WKT;

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
				):	point(static_cast<geos::geom::Point*>(_point.clone())),
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
				):	lineString(static_cast<geos::geom::LineString*>(_lineString.clone())),
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
					HIGHLIGHT
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
				const bool getActivated() const { return _activated; }
			};

			typedef std::vector<Control> Controls;

		private:
			boost::shared_ptr<geos::geom::Point> _center;
			const int _zoom;
			const std::string _id;
			Points _points;
			LineStrings _lineStrings;
			Controls _controls;
			bool _editable;
			bool _highlight;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Map constructor.
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.18
			/// @param center point in the middle of the map at opening (LambertII)
			/// @param zoom zoom level of the map at opening
			/// @param id id of the div in the DOM
			HTMLMap(
				const geos::geom::Point& center,
				int zoom,
				bool editable,
				bool addable,
				bool highlight,
				const std::string id = "map"
			);


			//! @name Update methods
			//@{
				void addPoint(MapPoint value){ _points.push_back(value); }
				void addLineString(MapLineString value){ _lineStrings.push_back(value); }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Generates the HTML code and flush it onto a stream.
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.18
				/// @param stream stream to write the code on
				void draw(std::ostream& stream) const;

				std::string getAddPointLink(
					const std::string& requestURL,
					const std::string& content
				) const;
			//@}
		};
	}
}

#endif // SYNTHESE_html_HTMLMap_hpp__
