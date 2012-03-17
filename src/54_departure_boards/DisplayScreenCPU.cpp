////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPU class implementation.
///	@file DisplayScreenCPU.cpp
///	@author Hugues Romain
///	@date 2008-12-26 17:40
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

#include "DisplayScreenCPU.h"
#include "StopArea.hpp"
#include "DisplayMonitoringStatus.h"

#include <sstream>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	namespace util
	{
		template<> const string Registry<departure_boards::DisplayScreenCPU>::KEY("DisplayScreenCPU");
	}

	namespace departure_boards
	{
		using namespace util;

		DisplayScreenCPU::DisplayScreenCPU(
			RegistryKeyType id
		):	Registrable(id),
			_place(NULL),
			_mac_address(),
			_is_online(true),
			_maintenance_message()
		{}



		DisplayScreenCPU::~DisplayScreenCPU(
		){
			_place = NULL;
		}



		const std::string& DisplayScreenCPU::getMacAddress(
		) const {
			return _mac_address;
		}



		time_duration DisplayScreenCPU::getMonitoringDelay(
		) const {
			return _monitoring_delay;
		}



		bool DisplayScreenCPU::getIsOnline(
		) const {
			return _is_online;
		}



		const std::string& DisplayScreenCPU::getMaintenanceMessage(
		) const {
			return _maintenance_message;
		}



		void DisplayScreenCPU::setMacAddress(
			const std::string& value
		){
			_mac_address = value;
		}



		void DisplayScreenCPU::setMonitoringDelay(
			const time_duration value
		){
			_monitoring_delay = value;
		}



		void DisplayScreenCPU::setIsOnline(
			const bool value
		){
			_is_online  =value;
		}



		void DisplayScreenCPU::setMaintenanceMessage(
			const std::string& value
		){
			_maintenance_message = value;
		}



		void DisplayScreenCPU::copy( const DisplayScreenCPU& e )
		{
			_mac_address = e._mac_address;
			setName(e.getName());
			_place = e._place;
			_monitoring_delay = e._monitoring_delay;
		}



		std::string DisplayScreenCPU::getFullName() const
		{
			if (!_place)
			{
				return getName() + " (not localized)";
			}
			else
			{
				stringstream s;
				s << _place->getFullName();
				if (!getName().empty())
					s << "/" << getName();
				return s.str();
			}
		}



		bool DisplayScreenCPU::isMonitored() const
		{
			return _is_online && (_monitoring_delay.minutes() > 0);
		}



		bool DisplayScreenCPU::isDown(
			const ptime& lastContact
		) const	{
			if(	!isMonitored()
			){
				return false;
			}

			ptime now(second_clock::local_time());

			if(now - lastContact <= _monitoring_delay)
			{
				return false;
			}
			return true;
		}
}	}
