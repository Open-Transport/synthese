
/** TextInputInterfaceElemet class implementation.
	@file TextInputInterfaceElemet.cpp

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

#include "TextInputInterfaceElemet.h"

#include "33_route_planner/RoutePlannerFunction.h"

#include "11_interfaces/ValueElementList.h"

using namespace std;

namespace synthese
{
	using namespace interfaces;

	namespace routeplanner
	{
		const std::string TextInputInterfaceElemet::FIELD_ORIGIN_CITY("origin_city");
		const std::string TextInputInterfaceElemet::FIELD_DESTINATION_CITY("destination_city");
		const std::string TextInputInterfaceElemet::FIELD_ORIGIN_PLACE("origin_place");
		const std::string TextInputInterfaceElemet::FIELD_DESTINATION_PLACE("destination_place");

		void TextInputInterfaceElemet::storeParameters(ValueElementList& vel)
		{
			_field = vel.front();
			_uid = vel.front();
			_text = vel.front();
			_html = vel.front();
		}

		string TextInputInterfaceElemet::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			
			string field(_field->getValue(parameters, variables, object, request));
			string value(_text->getValue(parameters, variables, object, request));
			string number(_uid->getValue(parameters, variables, object, request));
			string html(_html->getValue(parameters, variables, object, request));

			stream << "<input type=\"text\" name=\"";

			if (field == FIELD_ORIGIN_CITY)
				stream << RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT;
			else if (field == FIELD_DESTINATION_CITY)
				stream << RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT;
			else if (field == FIELD_ORIGIN_PLACE)
				stream << RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT;
			else if (field == FIELD_DESTINATION_PLACE)
				stream << RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT;
			stream << "\" value=\"" << value << "\" id=\"" << field << "_txt\" " << html << " />";

			stream << "<input type=\"hidden\" name=\"";
			if (field == FIELD_ORIGIN_CITY)
				stream << RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_ID;
			else if (field == FIELD_DESTINATION_CITY)
				stream << RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_ID;
			else if (field == FIELD_ORIGIN_PLACE)
				stream << RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_ID;
			else if (field == FIELD_DESTINATION_PLACE)
				stream << RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_ID;
			stream << "\" value=\"" << number << "\" id=\"" << field << "_id\" />";

			return string();
		}

		TextInputInterfaceElemet::~TextInputInterfaceElemet()
		{
		}
	}
}
