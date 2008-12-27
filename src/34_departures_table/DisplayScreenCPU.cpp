////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPU class implementation.
///	@file DisplayScreenCPU.cpp
///	@author Hugues Romain
///	@date 2008-12-26 17:40
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

// departurestable
#include "DisplayScreenCPU.h"

using namespace std;

namespace synthese
{
	namespace departurestable
	{
		using namespace util;

		DisplayScreenCPU::DisplayScreenCPU(
			RegistryKeyType id
		):	Registrable(id),
			_name(),
			_place(NULL),
			_mac_address(),
			_monitoring_delay(0),
			_is_online(true),
			_maintenance_message()
		{

		}



		DisplayScreenCPU::~DisplayScreenCPU(
		) {
			_place = NULL;
		}



		const std::string& DisplayScreenCPU::getName(

			) const {
			return _name;
		}



		const env::PublicTransportStopZoneConnectionPlace* DisplayScreenCPU::getPlace(
		) const {
			return _place;
		}



		const std::string& DisplayScreenCPU::getMacAddress(

			) const {
			return _mac_address;
		}



		int DisplayScreenCPU::getMonitoringDelay(

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



		void DisplayScreenCPU::setName(
			const std::string& value
			) {
			_name = value;
		}



		void DisplayScreenCPU::setPlace(
			const env::PublicTransportStopZoneConnectionPlace* const value
		) {
			_place = value;
		}



		void DisplayScreenCPU::setMacAddress(
			const std::string& value
		) {
			_mac_address = value;
		}



		void DisplayScreenCPU::setMonitoringDelay(
			const int value
		) {
			_monitoring_delay = value;
		}



		void DisplayScreenCPU::setIsOnline(
			const bool value
		) {
			_is_online  =value;
		}



		void DisplayScreenCPU::setMaintenanceMessage(
			const std::string& value
			) {
			_maintenance_message = value;
		}
	}
}
