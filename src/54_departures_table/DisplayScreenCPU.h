////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPU class header.
///	@file DisplayScreenCPU.h
///	@author Hugues Romain
///	@date 2008-12-26 11:48
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

#ifndef SYNTHESE_departurestable_DisplayScreenCPU_h__
#define SYNTHESE_departurestable_DisplayScreenCPU_h__

#include "Registrable.h"
#include "Registry.h"

#include <string>
#include <set>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace synthese
{
	namespace pt
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace departurestable
	{
		class DisplayScreen;

		////////////////////////////////////////////////////////////////////////
		///	Display screen CPU class.
		///	
		///	Represents a CPU which can connect some display screens by serial
		///	port.
		///	
		///	@ingroup m54
		////////////////////////////////////////////////////////////////////////
		class DisplayScreenCPU
		:	public util::Registrable
		{
		public:
			typedef util::Registry<DisplayScreenCPU> Registry;
			typedef std::set<const DisplayScreen*> WiredScreens;
			
		protected:
			// Attributes
			std::string											_name;
			const pt::PublicTransportStopZoneConnectionPlace*	_place;
			std::string											_mac_address;
			boost::posix_time::time_duration					_monitoring_delay;
			bool												_is_online;
			std::string											_maintenance_message;

			// Linked objects
			WiredScreens										_wiredScreens;
			
		public:
			////////////////////////////////////////////////////////////////////
			///	DisplayScreenCPU Constructor.
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			explicit DisplayScreenCPU(
				util::RegistryKeyType id = UNKNOWN_VALUE
			);
			
			////////////////////////////////////////////////////////////////////
			///	DisplayScreenCPU Destructor.
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			~DisplayScreenCPU();
			
			//! @name Getters
			//@{
				const std::string&									getName()				const;
				const pt::PublicTransportStopZoneConnectionPlace*	getPlace()				const;
				const std::string&									getMacAddress()			const;
				boost::posix_time::time_duration					getMonitoringDelay()	const;
				bool												getIsOnline()			const;
				const std::string&									getMaintenanceMessage()	const;
				const WiredScreens&									getWiredScreens()		const;
			//@}
			
			//! @name Setters
			//@{
				void setName(const std::string& value);
				void setPlace(const pt::PublicTransportStopZoneConnectionPlace* const value);
				void setMacAddress(const std::string& value);
				void setMonitoringDelay(const boost::posix_time::time_duration value);
				void setIsOnline(const bool value);
				void setMaintenanceMessage(const std::string& value);
				void addWiredScreen(const DisplayScreen* value);
				void removeWiredScreen(const DisplayScreen* value);
			//@}
			
			//! @name Queries
			//@{
				std::string getFullName() const;
				bool isMonitored() const;

				/** Analyzes a monitoring status to determinate if the cpu is down or up.
					@param status Status to read
					@return true if the status is too old for the cpu
					If the screen is not monitored for any reason, then the method always returns false.
					@warning This method checks only if the status is too old. It does not read the status itself.
				*/
				bool isDown(const boost::posix_time::ptime& status) const;

			//@}
			
			//! @name Others
			//@{
				void copy(const DisplayScreenCPU& e);
			//@}
		};
	}
}

#endif // SYNTHESE_departurestable_DisplayScreenCPU_h__
