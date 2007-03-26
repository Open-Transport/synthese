
/** DisplayScreenSupervisionRequest class implementation.
	@file DisplayScreenSupervisionRequest.cpp

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

#include "01_util/Conversion.h"

#include "30_server/RequestException.h"

#include "34_departures_table/DisplayScreenSupervisionRequest.h"
#include "34_departures_table/DeparturesTableModule.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace dblog;

	namespace departurestable
	{
		const std::string DisplayScreenSupervisionRequest::PARAMETER_DISPLAY_SCREEN_ID = "tb";
		const std::string DisplayScreenSupervisionRequest::PARAMETER_TEXT = "sv";
		const std::string DisplayScreenSupervisionRequest::PARAMETER_LEVEL = "ss";
		
		DisplayScreenSupervisionRequest::DisplayScreenSupervisionRequest()
			: Request()
			, _displayScreen(NULL)
		{}

		Request::ParametersMap DisplayScreenSupervisionRequest::getParametersMap() const
		{
			Request::ParametersMap map;
			map.insert(make_pair(PARAMETER_DISPLAY_SCREEN_ID, Conversion::ToString(_displayScreen->getKey())));
			map.insert(make_pair(PARAMETER_TEXT, _text));
			map.insert(make_pair(PARAMETER_LEVEL, Conversion::ToString((int) _level)));
			return map;
		}

		void DisplayScreenSupervisionRequest::setFromParametersMap(const Request::ParametersMap& map)
		{
			try
			{
				Request::ParametersMap::const_iterator it;

				it = map.find(PARAMETER_DISPLAY_SCREEN_ID);
				if (it == map.end())
					throw RequestException("Display screen not specified");
				if (!DeparturesTableModule::getDisplayScreens().contains(Conversion::ToLongLong(it->second)))
					throw RequestException("Display screen " + it->second + " not found");
				_displayScreen = DeparturesTableModule::getDisplayScreens().get(Conversion::ToLongLong(it->second));
			
				it = map.find(PARAMETER_TEXT);
				if (it == map.end())
					throw RequestException("Log text not specified");
				_text = it->second;

				it = map.find(PARAMETER_LEVEL);
				if (it == map.end())
					throw RequestException("Log level not specified");
				_level = (DBLogEntry::Level) Conversion::ToInt(it->second);
			}
			catch (...)
			{
				throw RequestException("Unknown error");
			}
		}

		void DisplayScreenSupervisionRequest::run( std::ostream& stream ) const
		{
			if (_displayScreen != NULL)
				_displayScreen->recordSupervision(_level, _text);
		}

		DisplayScreenSupervisionRequest::~DisplayScreenSupervisionRequest()
		{
		}
	}
}
