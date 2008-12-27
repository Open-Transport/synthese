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

#include <string>

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace departurestable
	{
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
			typedef Registry<DisplayScreenCPU> Registry;
			
		protected:
			// Attributes
			std::string											_name;
			const env::PublicTransportStopZoneConnectionPlace*	_place;
			std::string											_mac_address;
			int													_monitoring_delay;
			bool												_is_online;
			std::string											_maintenance_message;
			
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
				const env::PublicTransportStopZoneConnectionPlace*	getPlace()				const;
				const std::string&									getMacAddress()			const;
				int													getMonitoringDelay()	const;
				bool												getIsOnline()			const;
				const std::string&									getMaintenanceMessage()	const;
			//@}
			
			//! @name Setters
			//@{
				void setName(const std::string& value);
				void setPlace(const env::PublicTransportStopZoneConnectionPlace* const value);
				void setMacAddress(const std::string& value);
				void setMonitoringDelay(const int value);
				void setIsOnline(const bool value);
				void setMaintenanceMessage(const std::string& value);
			//@}
			
			//! @name Queries
			//@{
			//@}
			
			//! @name Others
			//@{
			//@}
		};
	}
}

#endif // SYNTHESE_departurestable_DisplayScreenCPU_h__
