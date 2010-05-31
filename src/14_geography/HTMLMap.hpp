
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

#include "Point2D.h"

#include <vector>

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
			struct Point
			{
				geometry::Point2D point;
				std::string icon;
				std::string editionIcon;
				std::string updateRequest;
				Point(geometry::Point2D _point, const std::string& _icon, const std::string _editionIcon, const std::string _updateRequest):
				point(_point.getX(), _point.getY()), icon(_icon), editionIcon(_editionIcon), updateRequest(_updateRequest)
				{}
			};
			typedef std::vector<Point> Points;

		private:
			const geometry::Point2D _center;
			const int _zoom;
			const std::string _id;
			Points _points;

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
				const geometry::Point2D& center,
				int zoom,
				const std::string id = "map"
			);


			//! @name Update methods
			//@{
				void addPoint(Point value){ _points.push_back(value); }
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
			//@}
		};
	}
}

#endif // SYNTHESE_html_HTMLMap_hpp__
