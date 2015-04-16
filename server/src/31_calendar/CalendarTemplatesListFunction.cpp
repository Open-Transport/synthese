
/** CalendarTemplatesListFunction class implementation.
	@file CalendarTemplatesListFunction.cpp
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
#include "CalendarTemplatesListFunction.hpp"
#include "CalendarTemplate.h"
#include "CalendarTemplateTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<> const string util::FactorableTemplate<Function,calendar::CalendarTemplatesListFunction>::FACTORY_KEY("CalendarTemplatesListFunction");

	namespace calendar
	{
		const string CalendarTemplatesListFunction::PARAMETER_ROOT_CALENDAR("root_calendar");
		const string CalendarTemplatesListFunction::PARAMETER_DEFAULT_VALUE("default_value");
		const string CalendarTemplatesListFunction::PARAMETER_FIELD_NAME("field_name");

		ParametersMap CalendarTemplatesListFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_rootCalendar.get())
			{
				map.insert(PARAMETER_ROOT_CALENDAR,  _rootCalendar->getKey());
			}
			if(_defaultValue.get())
			{
				map.insert(PARAMETER_DEFAULT_VALUE,  _defaultValue->getKey());
			}
			map.insert(PARAMETER_FIELD_NAME, _fieldName);
			return map;
		}

		void CalendarTemplatesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			_fieldName = map.getDefault<string>(PARAMETER_FIELD_NAME, "calendar_template");

			// Root calendar
			RegistryKeyType rid(map.getDefault<RegistryKeyType>(PARAMETER_ROOT_CALENDAR, 0));
			if(rid)
			{
				if(	!Env::GetOfficialEnv().getRegistry<CalendarTemplate>().contains(rid))
				{
					throw RequestException("No such root calendar");
				}
				_rootCalendar = Env::GetOfficialEnv().get<CalendarTemplate>(rid);
			}

			// Default value
			RegistryKeyType did(map.getDefault<RegistryKeyType>(PARAMETER_DEFAULT_VALUE, 0));
			if(did)
			{
				if(	!Env::GetOfficialEnv().getRegistry<CalendarTemplate>().contains(did))
				{
					throw RequestException("No such default value calendar");
				}
				_defaultValue = Env::GetOfficialEnv().get<CalendarTemplate>(did);
			}
		}



		util::ParametersMap CalendarTemplatesListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			CalendarTemplateTableSync::SearchResult s(CalendarTemplateTableSync::Search(Env::GetOfficialEnv()));

			stream << "<select name=\"" << _fieldName << "\">";
			BOOST_FOREACH(const boost::shared_ptr<const CalendarTemplate>& object, s)
			{
				stream << "<option value=\"" << object->getKey() << "\"";
				if (_defaultValue.get() == object.get())
					stream << " selected=\"selected\"";
				stream << ">" << object->getName() << "</option>";
			}
			stream << "</select>";

			return pm;
		}



		bool CalendarTemplatesListFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string CalendarTemplatesListFunction::getOutputMimeType() const
		{
			return "text/html";
		}
	}
}
