////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPU class header.
///	@file DisplayScreenCPU.h
///	@author Hugues Romain
///	@date 2008-12-26 11:48
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

#ifndef SYNTHESE_departurestable_DisplayScreenCPU_h__
#define SYNTHESE_departurestable_DisplayScreenCPU_h__

#include "Object.hpp"

#include "StringField.hpp"
#include "TimeField.hpp"
#include "TreeAlphabeticalOrderingPolicy.hpp"
#include "TreeRoot.hpp"

#include <string>
#include <set>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace synthese
{
	namespace geography
	{
		class NamedPlace;
	}

	namespace departure_boards
	{
		class DisplayScreen;

		FIELD_ID(PlaceId)
		FIELD_STRING(MacAddress)
		FIELD_TIME(MonitoringDelay)
		FIELD_BOOL(IsOnline)
		FIELD_STRING(MaintenanceMessage)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(PlaceId),
			FIELD(MacAddress),
			FIELD(MonitoringDelay),
			FIELD(IsOnline),
			FIELD(MaintenanceMessage)
		> DisplayScreenCPUSchema;

		////////////////////////////////////////////////////////////////////////
		///	Display screen CPU class.
		///
		///	Represents a CPU which can connect some display screens by serial
		///	port.
		///
		///	@ingroup m54
		////////////////////////////////////////////////////////////////////////
		class DisplayScreenCPU:
			public Object<DisplayScreenCPU, DisplayScreenCPUSchema>,
			public tree::TreeRoot<DisplayScreen, tree::TreeAlphabeticalOrderingPolicy>
		{
		protected:
			// Attributes
			const geography::NamedPlace*		_place;

		public:
			////////////////////////////////////////////////////////////////////
			///	DisplayScreenCPU Constructor.
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			explicit DisplayScreenCPU(
				util::RegistryKeyType id = 0
			);

			////////////////////////////////////////////////////////////////////
			///	DisplayScreenCPU Destructor.
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			~DisplayScreenCPU();

			//! @name Getters
			//@{
				const geography::NamedPlace*	getPlace()				const { return _place; }
				const std::string&									getMacAddress()			const;
				boost::posix_time::time_duration					getMonitoringDelay()	const;
				bool												getIsOnline()			const;
				const std::string&									getMaintenanceMessage()	const;
				virtual std::string getName() const { return get<Name>(); }
			//@}

			//! @name Setters
			//@{
				void setPlace(const geography::NamedPlace* value);
				void setMacAddress(const std::string& value);
				void setMonitoringDelay(const boost::posix_time::time_duration value);
				void setIsOnline(const bool value);
				void setMaintenanceMessage(const std::string& value);
				void setName(const std::string& value){ set<Name>(value); }
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

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_departurestable_DisplayScreenCPU_h__
