
/** PTRoutePlannerInputFunction class implementation.
	@file PTRoutePlannerInputFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "PTRoutePlannerInputFunction.hpp"
#include "DynamicRequest.h"
#include "PTServiceConfig.hpp"
#include "RoutePlannerFunction.h"
#include "CMSModule.hpp"
#include "Webpage.h"
#include "DateTimeInterfacePage.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt_website;
	using namespace algorithm;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,pt_journey_planner::PTRoutePlannerInputFunction>::FACTORY_KEY("PTRoutePlannerInputFunction");

	namespace pt_journey_planner
	{
		const string PTRoutePlannerInputFunction::PARAMETER_FIELD("field");
		const string PTRoutePlannerInputFunction::PARAMETER_VALUE("value");
		const string PTRoutePlannerInputFunction::PARAMETER_HTML("html");
		const string PTRoutePlannerInputFunction::PARAMETER_DATE_DISPLAY_TEMPLATE("date_display_template");
		const string PTRoutePlannerInputFunction::PARAMETER_CONFIG_ID = "config_id";

		const string PTRoutePlannerInputFunction::FIELD_ORIGIN_CITY("origin_city");
		const string PTRoutePlannerInputFunction::FIELD_DESTINATION_CITY("destination_city");
		const string PTRoutePlannerInputFunction::FIELD_ORIGIN_PLACE("origin_place");
		const string PTRoutePlannerInputFunction::FIELD_DESTINATION_PLACE("destination_place");
		const string PTRoutePlannerInputFunction::FIELD_DATE_LIST("date_list");
		const string PTRoutePlannerInputFunction::FIELD_PERIOD("period");



		PTRoutePlannerInputFunction::PTRoutePlannerInputFunction():
			_config(NULL)
		{}



		ParametersMap PTRoutePlannerInputFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_FIELD, _field);
			map.insert(PARAMETER_VALUE, _value);
			map.insert(PARAMETER_HTML, _html);
			if(_dateDisplayTemplate.get())
			{
				map.insert(PARAMETER_DATE_DISPLAY_TEMPLATE, _dateDisplayTemplate->getKey());
			}
			if(_config)
			{
				map.insert(PARAMETER_CONFIG_ID, _config->getKey());
			}
			return map;
		}



		void PTRoutePlannerInputFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_field = map.get<string>(PARAMETER_FIELD);
			_value = map.getDefault<string>(PARAMETER_VALUE);
			_html = map.getDefault<string>(PARAMETER_HTML);
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_DATE_DISPLAY_TEMPLATE));
				if(id) try
				{
					_dateDisplayTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such date display template");
				}
			}

			// Config
			RegistryKeyType configId(map.getDefault<RegistryKeyType>(PARAMETER_CONFIG_ID, 0));
			if(configId) try
			{
				_config = Env::GetOfficialEnv().get<PTServiceConfig>(configId).get();
			}
			catch(ObjectNotFoundException<PTServiceConfig>&)
			{
				throw RequestException("No such PT service config");
			}
		}



		util::ParametersMap PTRoutePlannerInputFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap requestParametersMap(getTemplateParameters());

			if(!_config)
			{
				return ParametersMap();
			}

			if(_field == FIELD_PERIOD)
			{
				const HourPeriods::Type& periods(_config->get<HourPeriods>());

				size_t current(
					_value.empty() ?
					requestParametersMap.getDefault<int>(RoutePlannerFunction::PARAMETER_PERIOD_ID) :
					lexical_cast<size_t>(_value)
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
			else if(_field == FIELD_DATE_LIST)
			{
				date dateDefaut(day_clock::local_day());
				if(!_value.empty())
				{
					dateDefaut = from_string(_value);
				}
				else if(!requestParametersMap.getDefault<string>(RoutePlannerFunction::PARAMETER_DAY).empty())
				{
					dateDefaut = from_string(requestParametersMap.get<string>(RoutePlannerFunction::PARAMETER_DAY));
				}

				date minDate(_config->getMinUseDate());
				date maxDate(_config->getMaxUseDate());

				//				const DateTimeInterfacePage* datePage(_page->getInterface()->getPage<DateTimeInterfacePage>(style));

				// Construction de l'objet HTML
				stream << "<select name=\"" << RoutePlannerFunction::PARAMETER_DAY << "\">";
				for (date iDate(minDate); iDate <= maxDate; iDate += days(1))
				{
					stream << "<option ";
					if ( iDate == dateDefaut )
						stream << "selected=\"selected\" ";
					stream << "value=\"" << to_iso_extended_string(iDate) << "\">";
					if(_dateDisplayTemplate.get())
					{
						DateTimeInterfacePage::Display(stream, _dateDisplayTemplate, request, iDate);
					}
					else
					{
						stream << to_simple_string(iDate);
					}
					stream << "</option>";
				}
				stream << "</select>";
			}
			else
			{

				string inputName;
				if (_field == FIELD_ORIGIN_CITY)
				{
					inputName = RoutePlannerFunction::PARAMETER_DEPARTURE_CITY_TEXT;
				}
				else if (_field == FIELD_DESTINATION_CITY)
				{
					inputName = RoutePlannerFunction::PARAMETER_ARRIVAL_CITY_TEXT;
				}
				else if (_field == FIELD_ORIGIN_PLACE)
				{
					inputName = RoutePlannerFunction::PARAMETER_DEPARTURE_PLACE_TEXT;
				}
				else if (_field == FIELD_DESTINATION_PLACE)
				{
					inputName = RoutePlannerFunction::PARAMETER_ARRIVAL_PLACE_TEXT;
				}

				stream <<
					"<input type=\"text\" name=\"" << inputName <<
					"\" value=\"" << (_value.empty() ? requestParametersMap.getDefault<string>(inputName) : _value) << "\" id=\"" << _field << "_txt\" " << _html << " />"
				;
			}

			return util::ParametersMap();
		}



		bool PTRoutePlannerInputFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PTRoutePlannerInputFunction::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
