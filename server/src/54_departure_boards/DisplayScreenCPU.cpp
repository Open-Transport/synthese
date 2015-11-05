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

#include "DisplayMaintenanceRight.h"
#include "DisplayMonitoringStatus.h"
#include "Fetcher.h"
#include "Profile.h"
#include "Session.h"
#include "StopArea.hpp"
#include "User.h"

#include <sstream>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace departure_boards;

	CLASS_DEFINITION(DisplayScreenCPU, "t058_display_screen_cpu", 58)
	FIELD_DEFINITION_OF_OBJECT(DisplayScreenCPU, "display_screen_cpu_id", "display_screen_cpu_ids")

	FIELD_DEFINITION_OF_TYPE(PlaceId, "place_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MacAddress, "mac_address", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(MonitoringDelay, "monitoring_delay", SQL_TIME)
	FIELD_DEFINITION_OF_TYPE(IsOnline, "is_online", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(MaintenanceMessage, "maintenance_message", SQL_TEXT)

	namespace departure_boards
	{
		using namespace db;
		using namespace util;

		DisplayScreenCPU::DisplayScreenCPU(
			RegistryKeyType id
		):	Registrable(id),
			Object<DisplayScreenCPU, DisplayScreenCPUSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(PlaceId),
					FIELD_DEFAULT_CONSTRUCTOR(MacAddress),
					FIELD_DEFAULT_CONSTRUCTOR(MonitoringDelay),
					FIELD_VALUE_CONSTRUCTOR(IsOnline, true),
					FIELD_DEFAULT_CONSTRUCTOR(MaintenanceMessage)
			)	),
			_place(NULL)
		{}



		DisplayScreenCPU::~DisplayScreenCPU(
		){
			_place = NULL;
		}



		const std::string& DisplayScreenCPU::getMacAddress(
		) const {
			return get<MacAddress>();
		}



		time_duration DisplayScreenCPU::getMonitoringDelay(
		) const {
			return get<MonitoringDelay>();
		}



		bool DisplayScreenCPU::getIsOnline(
		) const {
			return get<IsOnline>();
		}



		const std::string& DisplayScreenCPU::getMaintenanceMessage(
		) const {
			return get<MaintenanceMessage>();
		}



		void DisplayScreenCPU::setMacAddress(
			const std::string& value
		){
			set<MacAddress>(value);
		}



		void DisplayScreenCPU::setMonitoringDelay(
			const time_duration value
		){
			set<MonitoringDelay>(value);
		}



		void DisplayScreenCPU::setIsOnline(
			const bool value
		){
			set<IsOnline>(value);
		}



		void DisplayScreenCPU::setMaintenanceMessage(
			const std::string& value
		){
			set<MaintenanceMessage>(value);
		}



		void DisplayScreenCPU::copy( const DisplayScreenCPU& e )
		{
			setMacAddress(e.getMacAddress());
			setName(e.getName());
			setPlace(e.getPlace());
			setMonitoringDelay(e.getMonitoringDelay());
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
			return getIsOnline() && (getMonitoringDelay().minutes() > 0);
		}



		bool DisplayScreenCPU::isDown(
			const ptime& lastContact
		) const	{
			if(	!isMonitored()
			){
				return false;
			}

			ptime now(second_clock::local_time());

			if(now - lastContact <= getMonitoringDelay())
			{
				return false;
			}
			return true;
		}

		void DisplayScreenCPU::setPlace(const geography::NamedPlace* value)
		{
			_place = value;
			if (value)
			{
				set<PlaceId>(value->getKey());
			}
		}

		void DisplayScreenCPU::link(util::Env& env, bool withAlgorithmOptimizations)
		{
			setPlace(NULL);
			RegistryKeyType placeId(get<PlaceId>());
			if(placeId != 0) try
			{
				setPlace(Fetcher<geography::NamedPlace>::Fetch(placeId, env).get());
			}
			catch(ObjectNotFoundException<DisplayScreenCPU>& e)
			{
				Log::GetInstance().warn("Data corrupted in " + DisplayScreenCPU::TABLE_NAME + "/" + boost::lexical_cast<string>(getKey()) + e.getMessage());
			}
		}

		bool DisplayScreenCPU::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(security::WRITE);
		}

		bool DisplayScreenCPU::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(security::WRITE);
		}

		bool DisplayScreenCPU::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(security::DELETE_RIGHT);
		}
}	}
