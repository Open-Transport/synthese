
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

#include "PTRoutePlannerInputInterfaceElement.h"
#include "RoutePlannerFunction.h"
#include "ValueElementList.h"
#include "HourPeriod.h"
#include "Site.h"
#include "DynamicRequest.h"
#include "DateTimeInterfacePage.h"
#include "ParametersMap.h"
#include "FunctionWithSite.h"
#include "Interface.h"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace interfaces;
	using namespace server;
	using namespace transportwebsite;
	using namespace routeplanner;

	template<> const string util::FactorableTemplate<LibraryInterfaceElement,ptrouteplanner::PTRoutePlannerInputInterfaceElement>::FACTORY_KEY("route_planner_input");

	namespace ptrouteplanner
	{
		const std::string PTRoutePlannerInputInterfaceElement::FIELD_ORIGIN_CITY("origin_city");
		const std::string PTRoutePlannerInputInterfaceElement::FIELD_DESTINATION_CITY("destination_city");
		const std::string PTRoutePlannerInputInterfaceElement::FIELD_ORIGIN_PLACE("origin_place");
		const std::string PTRoutePlannerInputInterfaceElement::FIELD_DESTINATION_PLACE("destination_place");
		const std::string PTRoutePlannerInputInterfaceElement::FIELD_DATE_LIST("date_list");
		const std::string PTRoutePlannerInputInterfaceElement::FIELD_PERIOD("period");

		void PTRoutePlannerInputInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_field = vel.front();
			_html = vel.front();
			_value = vel.front();
			_parameter1 = vel.front();
		}

		string PTRoutePlannerInputInterfaceElement::display(
			std::ostream& stream
			, const interfaces::ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {
			
			string field(_field.get() ? _field->getValue(parameters, variables, object, request) : string());
			string value(_value.get() ? _value->getValue(parameters, variables, object, request) : string());
			string html(_html.get() ? _html->getValue(parameters, variables, object, request) : string());
			
			ParametersMap requestParametersMap(
				dynamic_cast<const DynamicRequest*>(request) ?
				dynamic_cast<const DynamicRequest*>(request)->getParametersMap() :
				ParametersMap()
			);

			if(field == FIELD_PERIOD)
			{
				shared_ptr<const FunctionWithSite> function(
					dynamic_pointer_cast<const FunctionWithSite>(
						request->getFunction()
				)	);
				assert(function.get());
				if(!function.get())
				{
					return string();
				}

				const Site::Periods& periods(function->getSite()->getPeriods());

				int current(
					value.empty() ?
					requestParametersMap.getDefault<int>(RoutePlannerFunction::PARAMETER_PERIOD_ID) :
					lexical_cast<int>(value)
				);

				stream << "<select name=\"" << RoutePlannerFunction::PARAMETER_PERIOD_ID << "\">";
				for(size_t i(0); i<periods.size(); ++i)
				{
					stream << "<option value=\"" << i << "\"";
					if (i == current)
						stream << " selected=\"selected\"";
					stream << ">" << periods.at(i).getCaption() << "</option>";
				}
				stream << "</select>";
			}
			else if(field == FIELD_DATE_LIST)
			{
				date dateDefaut(day_clock::local_day());
				if(!value.empty())
				{
					dateDefaut = from_string(value);
				}
				else if(!requestParametersMap.getDefault<string>(RoutePlannerFunction::PARAMETER_DAY).empty())
				{
					dateDefaut = from_string(requestParametersMap.get<string>(RoutePlannerFunction::PARAMETER_DAY));
				}

				string style(_parameter1.get() ? _parameter1->getValue(parameters, variables, object, request) : string());

				shared_ptr<const FunctionWithSite> function(
						dynamic_pointer_cast<const FunctionWithSite>(
						request->getFunction()
				)	);
				assert(function.get());
				if(!function.get())
				{
					return string();
				}

				date minDate(function->getSite()->getMinUseDate());
				date maxDate(function->getSite()->getMaxUseDate());

				const DateTimeInterfacePage* datePage(_page->getInterface()->getPage<DateTimeInterfacePage>(style));

				// Construction de l'objet HTML
				stream << "<select name=\"" << RoutePlannerFunction::PARAMETER_DAY << "\">";
				for (date iDate(minDate); iDate <= maxDate; iDate += days(1))
				{
					stream << "<option ";
					if ( iDate == dateDefaut )
						stream << "selected=\"selected\" ";
					stream << "value=\"" << to_iso_extended_string(iDate) << "\">";
					datePage->display(stream, variables, iDate, request);
					stream << "</option>";
				}
				stream << "</select>";
			}
			else
			{

				string inputName;
				if (field == FIELD_ORIGIN_CITY)
				{
					inputName = RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT;
				}
				else if (field == FIELD_DESTINATION_CITY)
				{
					inputName = RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT;
				}
				else if (field == FIELD_ORIGIN_PLACE)
				{
					inputName = RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT;
				}
				else if (field == FIELD_DESTINATION_PLACE)
				{
					inputName = RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT;
				}

				if(value.empty())
				{
					value = requestParametersMap.getDefault<string>(inputName);
				}

				stream <<
					"<input type=\"text\" name=\"" << inputName <<
					"\" value=\"" << value << "\" id=\"" << field << "_txt\" " << html << " />"
				;
			}

			return string();
		}

		PTRoutePlannerInputInterfaceElement::~PTRoutePlannerInputInterfaceElement()
		{
		}
	}
}
