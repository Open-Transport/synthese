
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

#include "RequestException.h"
#include "DisplayScreenSupervisionRequest.h"
#include "ParseDisplayReturnInterfacePage.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "DisplayScreenTableSync.h"
#include "DisplayMaintenanceLog.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "Interface.h"

#include <assert.h>
#include <sstream>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace dblog;
	using namespace interfaces;

	template<> const string util::FactorableTemplate<Function,departurestable::DisplayScreenSupervisionRequest>::FACTORY_KEY("tds");

	namespace departurestable
	{
		const std::string DisplayScreenSupervisionRequest::PARAMETER_DISPLAY_SCREEN_ID = "tb";
		const std::string DisplayScreenSupervisionRequest::PARAMETER_STATUS = "status";
		
		ParametersMap DisplayScreenSupervisionRequest::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_DISPLAY_SCREEN_ID, _displayScreen->getKey());
			map.insert(PARAMETER_STATUS, _text);
			return map;
		}

		void DisplayScreenSupervisionRequest::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_displayScreen = DisplayScreenTableSync::Get(map.getUid(PARAMETER_DISPLAY_SCREEN_ID, true, "dssr"), Env::GetOfficialEnv());
				_text = map.getString(PARAMETER_STATUS, true, "dssr");
			}
			catch(ObjectNotFoundException<DisplayScreen>& e)
			{
				throw RequestException("Display screen not found / " + e.getMessage());
			}
			catch (...)
			{
				throw RequestException("Unknown error");
			}

			if (_displayScreen->getType()->getMonitoringInterface() == NULL)
			{
				throw RequestException("This screen cannot be monitored because its type do not have monitoring interface");
			}
			if (_displayScreen->getType()->getMonitoringInterface()->getPage<ParseDisplayReturnInterfacePage>() == NULL)
			{
				throw RequestException("This screen cannot be monitored because its monitoring interface does not contain the parsing rules");
			}
		}

		void DisplayScreenSupervisionRequest::_run( std::ostream& stream ) const
		{
			// Assertions
			assert(_displayScreen.get() != NULL);
			assert(_displayScreen->getType() != NULL);
			assert(_displayScreen->getType()->getMonitoringInterface() != NULL);
			assert(_displayScreen->getType()->getMonitoringInterface()->getPage<ParseDisplayReturnInterfacePage>() != NULL);
			
			
			// Last monitoring status
			DisplayMonitoringStatusTableSync::SearchResult entries(
				DisplayMonitoringStatusTableSync::Search(*_env, _displayScreen->getKey(), 0, 1, true, false)
			);

			// Parsing
			stringstream s;
			const ParseDisplayReturnInterfacePage* page(_displayScreen->getType()->getMonitoringInterface()->getPage<ParseDisplayReturnInterfacePage>());
			VariablesMap v;
			page->display(s, _text, v, _request);

			// Standard status object creation
			DisplayMonitoringStatus status(s.str(), _displayScreen.get());

			// Last monitoring message
			if (entries.empty())
			{
				// First contact
				DisplayMaintenanceLog::AddMonitoringFirstEntry(*_displayScreen, status);
			}
			else
			{
				boost::shared_ptr<DisplayMonitoringStatus> lastStatus(entries.front());
				status.setKey(lastStatus->getKey());

				// Up contact?
				if (status.getTime() - lastStatus->getTime() > _displayScreen->getType()->getTimeBetweenChecks())
				{
					DisplayMaintenanceLog::AddMonitoringUpEntry(*_displayScreen, lastStatus->getTime());
				}

				// Status change ?
				if (status.getGlobalStatus() != lastStatus->getGlobalStatus())
				{
					DisplayMaintenanceLog::AddStatusChangeEntry(*_displayScreen, *lastStatus, status);
				}
			}

			// Saving
			DisplayMonitoringStatusTableSync::Save(&status);
		}



		bool DisplayScreenSupervisionRequest::_isAuthorized(
		) const {
			return true;
		}

		std::string DisplayScreenSupervisionRequest::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
