
/** SiteUpdateAction class implementation.
	@file SiteUpdateAction.cpp
	@author Hugues Romain
	@date 2008

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

#include "30_server/ActionException.h"
#include "30_server/ParametersMap.h"

#include "SiteUpdateAction.h"

#include "36_places_list/Site.h"
#include "36_places_list/SiteTableSync.h"

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfaceTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace interfaces;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, transportwebsite::SiteUpdateAction>::FACTORY_KEY("SiteUpdateAction");
	}

	namespace transportwebsite
	{
		const string SiteUpdateAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "si";
		const string SiteUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string SiteUpdateAction::PARAMETER_INTERFACE_ID = Action_PARAMETER_PREFIX + "in";
		const string SiteUpdateAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sd";
		const string SiteUpdateAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "ed";
		const string SiteUpdateAction::PARAMETER_ONLINE_BOOKING = Action_PARAMETER_PREFIX + "ob";
		const string SiteUpdateAction::PARAMETER_USE_OLD_DATA = Action_PARAMETER_PREFIX + "uo";
		const string SiteUpdateAction::PARAMETER_MAX_CONNECTIONS = Action_PARAMETER_PREFIX + "mc";
		const string SiteUpdateAction::PARAMETER_USE_DATES_RANGE = Action_PARAMETER_PREFIX + "dr";
		
		
		
		SiteUpdateAction::SiteUpdateAction()
			: util::FactorableTemplate<Action, SiteUpdateAction>()
			, _startDate(UNKNOWN_VALUE)
			, _endDate(UNKNOWN_VALUE)
		{
		}
		
		
		
		ParametersMap SiteUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_site.get())
				map.insert(PARAMETER_SITE_ID, _site->getKey());
			return map;
		}
		
		
		
		void SiteUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			setSiteId(map.getUid(PARAMETER_SITE_ID, true, FACTORY_KEY));
			_name = map.getString(PARAMETER_NAME, true, FACTORY_KEY);
			try
			{
				_interface = InterfaceTableSync::Get(map.getUid(PARAMETER_INTERFACE_ID, true, FACTORY_KEY), *_env);
			}
			catch (...)
			{
				throw ActionException("No such interface");
			}
			_startDate = map.getDate(PARAMETER_START_DATE, true, FACTORY_KEY);
			_endDate = map.getDate(PARAMETER_END_DATE, true, FACTORY_KEY);
			_onlineBooking = map.getBool(PARAMETER_ONLINE_BOOKING, true, true, FACTORY_KEY);
			_useOldData = map.getBool(PARAMETER_USE_OLD_DATA, true, true, FACTORY_KEY);
			_useDatesRange = map.getInt(PARAMETER_USE_DATES_RANGE, true, FACTORY_KEY);
			_maxConnections = map.getInt(PARAMETER_MAX_CONNECTIONS, true, FACTORY_KEY);
		}
		
		
		
		void SiteUpdateAction::run(Request& request)
		{
			_site->setName(_name);
			_site->setInterface(_interface.get());
			_site->setStartDate(_startDate);
			_site->setEndDate(_endDate);
			_site->setOnlineBookingAllowed(_onlineBooking);
			_site->setPastSolutionsDisplayed(_useOldData);
			_site->setUseDateRange(_useDatesRange);
			_site->setMaxTransportConnectionsCount(_maxConnections);

			SiteTableSync::Save(_site.get());
		}



		void SiteUpdateAction::setSiteId( uid id )
		{
			try
			{
				_site = SiteTableSync::GetEditable(id, *_env);
			}
			catch(...)
			{
				throw ActionException("No such site");	
			}
		}



		bool SiteUpdateAction::isAuthorized(const Profile& profile

			) const {
			return true;
		}
	}
}
