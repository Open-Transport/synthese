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
	using namespace time;

	template<> const string util::FactorableTemplate<Function,departurestable::CPUGetWiredScreensFunction>::FACTORY_KEY("CPUGetWiredScreensFunction");

	namespace departurestable
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
			uid id(map.getUid(PARAMETER_CPU_ID, false, FACTORY_KEY));
			if(id > 0)
			{
				setCPU(id);
			}
			else
			{
				string macAddress(map.getString(PARAMETER_CPU_MAC_ADDRESS, true, FACTORY_KEY));
				setCPU(macAddress);
			}

			// Last monitoring status
			DisplayMonitoringStatusTableSync::Search(_env, _cpu->getKey(), 0, 1, true, false);
		}


		void CPUGetWiredScreensFunction::_run( std::ostream& stream ) const
		{
			// Standard status object creation
			DisplayMonitoringStatus status(_cpu.get());

			// Last monitoring message
			if (_env.getRegistry<DisplayMonitoringStatus>().empty())
			{
				// First contact
				DisplayMaintenanceLog::AddMonitoringFirstEntry(*_cpu, status);
			}
			else
			{
				boost::shared_ptr<DisplayMonitoringStatus> lastStatus(_env.getEditableRegistry<DisplayMonitoringStatus>().front());
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
			stream <<
				"<?xml version='1.0' encoding='ISO-8859-15'?>\n" <<
				"<" << DISPLAY_SCREENS_XML_TAG << ">\n";

			BOOST_FOREACH(const DisplayScreen* screen, _cpu->getWiredScreens())
			{
				stream <<
					"<" << DISPLAY_SCREEN_XML_TAG << " " <<
					DISPLAY_SCREEN_ID_XML_FIELD << "=\"" << screen->getKey() << "\" " <<
					DISPLAY_SCREEN_COMPORT_XML_FIELD << "=\"" << screen->getComPort() << "\" " <<
					" />\n";
			}
			stream << "</" << DISPLAY_SCREENS_XML_TAG << ">\n";
		}



		bool CPUGetWiredScreensFunction::_isAuthorized(
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
				throw RequestException("Display screen CPU " + Conversion::ToString(id) + " not found");
			}
		}

		void CPUGetWiredScreensFunction::setCPU( const std::string& macAddress )
		{
			DisplayScreenCPUTableSync::Search(_env, optional<RegistryKeyType>(), macAddress);
			if(_env.getRegistry<DisplayScreenCPU>().empty())
			{
				throw RequestException("Display screen CPU MAC address "+ macAddress +" invalid.");
			}
			_cpu = Env::GetOfficialEnv().getRegistry<DisplayScreenCPU>().get(_env.getRegistry<DisplayScreenCPU>().getOrderedVector().front()->getKey());
		}
	}
}
