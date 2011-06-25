
/** PTObjectsCMSExporters class implementation.
	@file PTObjectsCMSExporters.cpp

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

#include "PTObjectsCMSExporters.hpp"
#include "ParametersMap.h"
#include "CommercialLine.h"
#include "RGBColor.h"
#include "StopArea.hpp"
#include "City.h"
#include "Service.h"

#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	namespace pt
	{
		const string PTObjectsCMSExporters::DATA_LINE_SHORT_NAME("line_short_name");
		const string PTObjectsCMSExporters::DATA_LINE_LONG_NAME("line_long_name");
		const string PTObjectsCMSExporters::DATA_LINE_COLOR("line_color");
		const string PTObjectsCMSExporters::DATA_LINE_STYLE("line_style");
		const string PTObjectsCMSExporters::DATA_LINE_IMAGE("line_image");
		const string PTObjectsCMSExporters::DATA_LINE_ID("line_id");

		const std::string PTObjectsCMSExporters::DATA_STOP_ID("stop_id");
		const std::string PTObjectsCMSExporters::DATA_STOP_NAME("stop_name");
		const std::string PTObjectsCMSExporters::DATA_CITY_ID("city_id");
		const std::string PTObjectsCMSExporters::DATA_CITY_NAME("city_name");
		const std::string PTObjectsCMSExporters::DATA_STOP_NAME_13("stop_name_13");
		const std::string PTObjectsCMSExporters::DATA_STOP_NAME_26("stop_name_26");
		const std::string PTObjectsCMSExporters::DATA_STOP_NAME_FOR_TIMETABLES("stop_name_for_timetables");
		const std::string PTObjectsCMSExporters::DATA_STOP_RANK("stop_rank");
		const std::string PTObjectsCMSExporters::DATA_X("x");
		const std::string PTObjectsCMSExporters::DATA_Y("y");

		const std::string PTObjectsCMSExporters::DATA_SERVICE_ID("service_id");
		const std::string PTObjectsCMSExporters::DATA_SERVICE_NUMBER("service_number");


		void PTObjectsCMSExporters::ExportLine(
			server::ParametersMap& pm,
			const CommercialLine& line,
			string prefix
		){
			pm.insert(prefix + DATA_LINE_ID, line.getKey());
			pm.insert(prefix + DATA_LINE_SHORT_NAME, line.getShortName());
			pm.insert(prefix + DATA_LINE_LONG_NAME, line.getLongName());
			pm.insert(prefix + DATA_LINE_STYLE, line.getStyle());
			if(line.getColor())
			{
				pm.insert(prefix + DATA_LINE_COLOR, line.getColor()->toString());
			}
			pm.insert(prefix + DATA_LINE_IMAGE, line.getImage());
		}



		void PTObjectsCMSExporters::ExportStopArea(
			server::ParametersMap& pm,
			const StopArea& stopArea,
			const CoordinatesSystem* coordinatesSystem,
			string prefix,
			optional<size_t> rank
		){
			pm.insert(prefix + DATA_STOP_ID, stopArea.getKey());
			pm.insert(prefix + DATA_STOP_NAME, stopArea.getName());
			pm.insert(prefix + DATA_STOP_NAME_13, stopArea.getName13());
			pm.insert(prefix + DATA_STOP_NAME_26, stopArea.getName26());
			pm.insert(prefix + DATA_STOP_NAME_FOR_TIMETABLES, stopArea.getTimetableName());
			if(rank)
			{
				pm.insert(prefix + DATA_STOP_RANK, *rank);
			}
			if(stopArea.getCity())
			{
				pm.insert(prefix + DATA_CITY_ID, stopArea.getCity()->getKey());
				pm.insert(prefix + DATA_CITY_NAME, stopArea.getCity()->getName());
			}
			if(coordinatesSystem && stopArea.getPoint())
			{
				shared_ptr<Point> pg(
					coordinatesSystem->convertPoint(*stopArea.getPoint())
				);
				{
					stringstream s;
					s << std::fixed << pg->getX();
					pm.insert(prefix + DATA_X, s.str());
				}
				{
					stringstream s;
					s << std::fixed << pg->getY();
					pm.insert(prefix + DATA_Y, s.str());
				}
			}
		}



		void PTObjectsCMSExporters::ExportStopArea(
			std::ostream& stream,
			const StopArea& stopArea,
			const CoordinatesSystem* coordinatesSystem,
			std::string tag /*= "stopArea" */
		){
			stream << "<" << tag <<
				" " << DATA_STOP_ID << "=\"" << stopArea.getKey() << "\"" <<
				" " << DATA_STOP_NAME << "=\"" << stopArea.getName() << "\"" <<
				" " << DATA_STOP_NAME_13 << "=\"" << stopArea.getName13() << "\"" <<
				" " << DATA_STOP_NAME_26 << "=\"" << stopArea.getName26() << "\"" <<
				" " << DATA_STOP_NAME_FOR_TIMETABLES << "=\"" << stopArea.getTimetableName() << "\""
			;
			if(stopArea.getCity())
			{
				stream << " " << DATA_CITY_ID << "=\"" << stopArea.getCity()->getKey() << "\"" <<
					" " << DATA_CITY_NAME << "=\"" << stopArea.getCity()->getName() << "\""
				;
			}
			if(coordinatesSystem && stopArea.getPoint())
			{
				shared_ptr<Point> pg(
					coordinatesSystem->convertPoint(*stopArea.getPoint())
				);
				stream << " " << DATA_X << "=\"" << std::fixed << pg->getX() << "\"" <<
					" " << DATA_Y << "=\"" << std::fixed << pg->getY() << "\""
				;
			}
			stream << " />";
		}



		void PTObjectsCMSExporters::ExportService( server::ParametersMap& pm, const graph::Service& service, std::string prefix /*= std::string() */ )
		{
			pm.insert(prefix + DATA_SERVICE_ID, service.getKey());
			pm.insert(prefix + DATA_SERVICE_NUMBER, service.getServiceNumber());
		}
}	}
