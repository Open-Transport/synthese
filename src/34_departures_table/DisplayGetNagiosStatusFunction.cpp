////////////////////////////////////////////////////////////////////////////////
/// DisplayGetNagiosStatusFunction class implementation.
///	@file DisplayGetNagiosStatusFunction.cpp
///	@author Hugues Romain
///	@date 2008-12-15 0:14
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////


#include "Conversion.h"
#include "RequestException.h"
#include "RequestMissingParameterException.h"
#include "DisplayGetNagiosStatusFunction.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "DisplayScreenTableSync.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "DateTime.h"

#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace time;

	template<> const string util::FactorableTemplate<Function,departurestable::DisplayGetNagiosStatusFunction>::FACTORY_KEY("DisplayGetNagiosStatusFunction");
	
	namespace departurestable
	{
		const string DisplayGetNagiosStatusFunction::PARAMETER_DISPLAY_SCREEN_ID("id");

		
		ParametersMap DisplayGetNagiosStatusFunction::_getParametersMap() const
		{
			ParametersMap map;
			if (_screen.get())
			{
				map.insert(PARAMETER_DISPLAY_SCREEN_ID, _screen->getKey());
			}
			return map;
		}


		void DisplayGetNagiosStatusFunction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_DISPLAY_SCREEN_ID, true, FACTORY_KEY));
			try
			{
				_screen = DisplayScreenTableSync::Get(id, &_env, UP_LINKS_LOAD_LEVEL);
				DisplayMonitoringStatusTableSync::Search(_env, _screen->getKey(), 0, 0, true, true, UP_LINKS_LOAD_LEVEL);
			}
			catch (...)
			{
				throw RequestException("Display screen " + Conversion::ToString(id) + " not found");
			}
		}


		void DisplayGetNagiosStatusFunction::_run( std::ostream& stream ) const
		{
			if (!_screen->getIsOnline())
			{
				stream << "3\nCheck deactivated.|-1\n";
			}
			else if (_env.getRegistry<DisplayMonitoringStatus>().empty())
			{
				stream << "1\nNever successfully checked.|-1\n";
			}
			else
			{
				shared_ptr<DisplayMonitoringStatus> status(_env.getEditableRegistry<DisplayMonitoringStatus>().front());
				DateTime now(TIME_CURRENT);
				if (now - status->getTime() > _screen->getType()->getTimeBetweenChecks())
				{
					stream << "2\nContact lost.|-1\n";
				}
				else
				{
					string returnCode("2");
					if (status->getGlobalStatus() == DisplayMonitoringStatus::DISPLAY_MONITORING_OK) returnCode = "0";
					if (status->getGlobalStatus() == DisplayMonitoringStatus::DISPLAY_MONITORING_WARNING) returnCode = "1";
					stream << returnCode << "\n";
					stream << status->getDetail() << "|" << status->getTemperatureValue() << "\n";
				}
			}
		}
	}
}
