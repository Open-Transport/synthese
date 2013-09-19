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


#include "RequestException.h"
#include "CPUGetWiredScreensFunction.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayScreen.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "DisplayMaintenanceLog.h"

#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	template<> const string util::FactorableTemplate<Function,departure_boards::CPUGetWiredScreensFunction>::FACTORY_KEY("CPUGetWiredScreensFunction");

	namespace departure_boards
	{
		const string CPUGetWiredScreensFunction::PARAMETER_CPU_ID("id");
		const string CPUGetWiredScreensFunction::PARAMETER_CPU_MAC_ADDRESS("ma");

		const std::string CPUGetWiredScreensFunction::DISPLAY_SCREENS_XML_TAG("displayScreens");
		const std::string CPUGetWiredScreensFunction::DISPLAY_SCREEN_XML_TAG("displayScreen");
		const std::string CPUGetWiredScreensFunction::DISPLAY_SCREEN_ID_XML_FIELD("id");
		const std::string CPUGetWiredScreensFunction::DISPLAY_SCREEN_COMPORT_XML_FIELD("comPort");

		ParametersMap CPUGetWiredScreensFunction::_getParametersMap() const
		{
			ParametersMap map;
			if (_cpu.get())
			{
				map.insert(PARAMETER_CPU_ID, _cpu->getKey());
			}
			return map;
		}


		void CPUGetWiredScreensFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_CPU_ID, 0));
			if(id > 0)
			{
				setCPU(id);
			}
			else
			{
				setCPU(map.get<string>(PARAMETER_CPU_MAC_ADDRESS));
			}
		}



		util::ParametersMap CPUGetWiredScreensFunction::run( std::ostream& stream, const Request& request ) const
		{
			// Last monitoring status
			DisplayMonitoringStatusTableSync::SearchResult entries(
				DisplayMonitoringStatusTableSync::Search(*_env, _cpu->getKey(), 0, 1, false, true, false)
			);

			// Standard status object creation
			DisplayMonitoringStatus status(_cpu.get());

			// Last monitoring message
			if (entries.empty())
			{
				// First contact
				DisplayMaintenanceLog::AddMonitoringFirstEntry(*_cpu, status);
			}
			else
			{
				boost::shared_ptr<DisplayMonitoringStatus> lastStatus(entries.front());
				status.setKey(lastStatus->getKey());

				// Up contact?
				if (status.getTime() - lastStatus->getTime() > _cpu->getMonitoringDelay())
				{
					DisplayMaintenanceLog::AddMonitoringUpEntry(*_cpu, lastStatus->getTime());
				}
			}

			// Saving
			DisplayMonitoringStatusTableSync::Save(&status);

			// Output generation
			util::ParametersMap pm;
			BOOST_FOREACH(const DisplayScreenCPU::ChildrenType::value_type& item, _cpu->getChildren())
			{
				const DisplayScreen& screen(*item.second);

				if(!screen.getIsOnline())
				{
					continue;
				}

				boost::shared_ptr<util::ParametersMap> screenPM(new ParametersMap);
				screenPM->insert(DISPLAY_SCREEN_ID_XML_FIELD, screen.getKey());
				screenPM->insert(DISPLAY_SCREEN_COMPORT_XML_FIELD, screen.getComPort());

				pm.insert(DISPLAY_SCREEN_XML_TAG, screenPM);
			}

			pm.outputXML(stream, DISPLAY_SCREENS_XML_TAG, true);

			return pm;
		}



		bool CPUGetWiredScreensFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string CPUGetWiredScreensFunction::getOutputMimeType() const
		{
			return "text/xml";
		}

		void CPUGetWiredScreensFunction::setCPU( util::RegistryKeyType id )
		{
			try
			{
				_cpu = Env::GetOfficialEnv().getRegistry<DisplayScreenCPU>().get(id);
			}
			catch (...)
			{
				throw RequestException("Display screen CPU " + lexical_cast<string>(id) + " not found");
			}
		}

		void CPUGetWiredScreensFunction::setCPU( const std::string& macAddress )
		{
			DisplayScreenCPUTableSync::SearchResult cpus(
				DisplayScreenCPUTableSync::Search(Env::GetOfficialEnv(), optional<RegistryKeyType>(), macAddress)
			);
			if(cpus.empty())
			{
				throw RequestException("Display screen CPU MAC address "+ macAddress +" invalid.");
			}
			_cpu = cpus.front();
		}
	}
}
