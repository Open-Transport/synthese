
/** PTObjectsCMSExporters class header.
	@file PTObjectsCMSExporters.hpp

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

#ifndef SYNTHESE_pt_PTObjectsCMSExporters_hpp__
#define SYNTHESE_pt_PTObjectsCMSExporters_hpp__

#include <string>

namespace synthese
{
	namespace server
	{
		class ParametersMap;
	}

	namespace pt
	{
		class CommercialLine;
		class StopArea;

		/** PTObjectsCMSExporters class.
			@ingroup m35
		*/
		class PTObjectsCMSExporters
		{
		private:
			static const std::string DATA_LINE_SHORT_NAME;
			static const std::string DATA_LINE_LONG_NAME;
			static const std::string DATA_LINE_COLOR;
			static const std::string DATA_LINE_STYLE;
			static const std::string DATA_LINE_IMAGE;
			static const std::string DATA_LINE_ID;

			static const std::string DATA_STOP_ID;
			static const std::string DATA_STOP_NAME;
			static const std::string DATA_CITY_ID;
			static const std::string DATA_CITY_NAME;
			static const std::string DATA_STOP_NAME_13;
			static const std::string DATA_STOP_NAME_26;
			static const std::string DATA_STOP_NAME_FOR_TIMETABLES;


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Line CMS exporter.
			/// Values sent to the CMS template :
			/// <dl>
			/// <dt>line_id</dt><dd>line id</dd>
			///	<dt>line_short_name</dt><dd>line short name (in most of cases the line number)</dd>
			///	<dt>line_long_name</dt><dd>line long name</dd>
			///	<dt>line_color</dt><dd>line RGB color</dd>
			///	<dt>line_style</dt><dd>line CSS class</dd>
			/// <dt>line_image</dt><dd>line image url</dd>
			/// </dl>
			//////////////////////////////////////////////////////////////////////////
			/// @param pm parameters map to populate
			/// @param line line to export
			/// @param prefix prefix to add to the field names
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			static void ExportLine(
				server::ParametersMap& pm,
				const CommercialLine& line,
				std::string prefix = std::string()
			);


			//////////////////////////////////////////////////////////////////////////
			/// Line CMS exporter.
			/// Values sent to the CMS template :
			/// <dl>
			/// <dt>stop_id</dt><dd>stop area id</dd>
			///	<dt>stop_name</dt><dd>stop area name in the city</dd>
			///	<dt>city_id</dt><dd>id of the city where the stop area is located</dd>
			///	<dt>city name</dt><dd>name of the city where the stop area is located</dd>
			///	<dt>stop_name_13</dt><dd>stop area short alias (13 characters max) designed for departure boards</dd>
			///	<dt>stop_name_26</dt><dd>stop area middle sized alias (26 characters max) designed for departure boards</dd>
			///	<dt>stop_name_for_timetables</dt><dd>stop area alias designed for timetables</dd>
			/// </dl>
			//////////////////////////////////////////////////////////////////////////
			/// @param pm parameters map to populate
			/// @param stopArea stop area to export
			/// @param prefix prefix to add to the field names
			/// @author Hugues Romain
			/// @since 3.2.0
			/// @date 2010
			static void ExportStopArea(
				server::ParametersMap& pm,
				const StopArea& stopArea,
				std::string prefix = std::string()
			);
		};
	}
}

#endif // SYNTHESE_pt_PTObjectsCMSExporters_hpp__
