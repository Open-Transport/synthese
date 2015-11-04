////////////////////////////////////////////////////////////////////////////////
/// DisplayGetNagiosStatusFunction class implementation.
///	@file DisplayGetNagiosStatusFunction.cpp
///	@author Hugues Romain
///	@date 2008-12-15 0:14
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "DisplayGetNagiosStatusFunction.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayMonitoringStatusTableSync.h"

#include <boost/shared_ptr.hpp>
#include <boost/optional/optional_io.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	template<> const string util::FactorableTemplate<Function,departure_boards::DisplayGetNagiosStatusFunction>::FACTORY_KEY("DisplayGetNagiosStatusFunction");

	namespace departure_boards
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
			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_DISPLAY_SCREEN_ID));
			try
			{
				_screen = Env::GetOfficialEnv().get<DisplayScreen>(id);
			}
			catch (ObjectNotFoundException<DisplayScreen>&)
			{
				throw RequestException("Display screen " + lexical_cast<string>(id) + " not found");
			}
		}



		ParametersMap DisplayGetNagiosStatusFunction::run( std::ostream& stream, const Request& request ) const
		{
			if(	!_screen->get<MaintenanceIsOnline>()
			){
				stream << "3\nCheck deactivated.|temp=-1\n";
			} else if(	!_screen->isMonitored()
			){
				stream << "3\nUnmonitored.|temp=-1\n";
			} else {
				boost::shared_ptr<DisplayMonitoringStatus> status(DisplayMonitoringStatusTableSync::GetStatus(_screen->getKey()));

				if(	status.get() == NULL
				){
					stream << "1\nNever successfully checked.|temp=-1\n";
				} else if(	_screen->isDown(*status)
				){
					stream << "2\nContact lost.|temp=-1\n";
				} else {
					string returnCode("2");
					if (status->getGlobalStatus() == DISPLAY_MONITORING_OK) returnCode = "0";
					if (status->getGlobalStatus() == DISPLAY_MONITORING_WARNING) returnCode = "1";
					stream << returnCode << "\n";
					stream << status->getDetail() << "|temp=" << status->getTemperatureValue() << "\n";
				}
			}

			return ParametersMap();
		}



		bool DisplayGetNagiosStatusFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string DisplayGetNagiosStatusFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
