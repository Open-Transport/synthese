
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

#include "CoordinatesSystem.hpp"

#include <boost/optional.hpp>
#include <string>

namespace synthese
{
	namespace server
	{
		class ParametersMap;
	}

	namespace graph
	{
		class Service;
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
			static const std::string DATA_STOP_RANK;
			static const std::string DATA_X;
			static const std::string DATA_Y;
			static const std::string DATA_SERVICE_ID;
			static const std::string DATA_SERVICE_NUMBER;


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Line CMS exporter.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Lines_in_CMS
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


			static void ExportService(
				server::ParametersMap& pm,
				const graph::Service& service,
				std::string prefix = std::string()
			);


			//////////////////////////////////////////////////////////////////////////
			/// Line CMS exporter.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Stop_Areas_in_CMS
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
				const CoordinatesSystem* coordinatesSystem = &CoordinatesSystem::GetInstanceCoordinatesSystem(),
				std::string prefix = std::string(),
				boost::optional<std::size_t> rank = boost::optional<std::size_t>()
			);



			/// @param coordinatesSystem if NULL no geometry is exported
			static void ExportStopArea(
				std::ostream& stream,
				const StopArea& stopArea,
				const CoordinatesSystem* coordinatesSystem = &CoordinatesSystem::GetInstanceCoordinatesSystem(),
				std::string tag = "stopArea"
			);
		};
	}
}

#endif // SYNTHESE_pt_PTObjectsCMSExporters_hpp__
