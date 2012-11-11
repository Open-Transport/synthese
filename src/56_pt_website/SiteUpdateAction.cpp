
/** SiteUpdateAction class implementation.
	@file SiteUpdateAction.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "SiteUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "PTServiceConfigTableSync.hpp"
#include "Session.h"
#include "TransportWebsiteRight.h"
#include "User.h"

using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace security;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt_website::SiteUpdateAction>::FACTORY_KEY("SiteUpdateAction");
	}

	namespace pt_website
	{
		const string SiteUpdateAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "si";
		const string SiteUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string SiteUpdateAction::PARAMETER_ONLINE_BOOKING = Action_PARAMETER_PREFIX + "ob";
		const string SiteUpdateAction::PARAMETER_USE_OLD_DATA = Action_PARAMETER_PREFIX + "uo";
		const string SiteUpdateAction::PARAMETER_MAX_CONNECTIONS = Action_PARAMETER_PREFIX + "mc";
		const string SiteUpdateAction::PARAMETER_USE_DATES_RANGE = Action_PARAMETER_PREFIX + "dr";
		const string SiteUpdateAction::PARAMETER_DISPLAY_ROAD_APPROACH_DETAIL = Action_PARAMETER_PREFIX + "da";



		SiteUpdateAction::SiteUpdateAction()
			: util::FactorableTemplate<Action, SiteUpdateAction>(),
			_displayRoadApproachDetail(true)
		{
		}



		ParametersMap SiteUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_site.get())
			{
				map.insert(PARAMETER_SITE_ID, _site->getKey());
				map.insert(PARAMETER_DISPLAY_ROAD_APPROACH_DETAIL, _displayRoadApproachDetail);
			}
			return map;
		}



		void SiteUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Site
			try
			{
				_site = PTServiceConfigTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_SITE_ID),
					*_env
				);
			}
			catch(ObjectNotFoundException<PTServiceConfig>&)
			{
				throw ActionException("No such site");
			}

			_name = map.get<string>(PARAMETER_NAME);
			_onlineBooking = map.get<bool>(PARAMETER_ONLINE_BOOKING);
			_useOldData = map.get<bool>(PARAMETER_USE_OLD_DATA);
			_useDatesRange = days(map.get<int>(PARAMETER_USE_DATES_RANGE));
			_maxConnections = map.get<size_t>(PARAMETER_MAX_CONNECTIONS);
			_displayRoadApproachDetail = map.get<bool>(PARAMETER_DISPLAY_ROAD_APPROACH_DETAIL);
		}



		void SiteUpdateAction::run(Request& request)
		{
			_site->set<Name>(_name);
			_site->set<OnlineBookingActivated>(_onlineBooking);
			_site->set<UseOldData>(_useOldData);
			_site->set<UseDatesRange>(_useDatesRange);
			_site->set<MaxConnections>(_maxConnections);
			_site->set<DisplayRoadApproachDetails>(_displayRoadApproachDetail);

			PTServiceConfigTableSync::Save(_site.get());
		}



		bool SiteUpdateAction::isAuthorized(const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(WRITE);
		}
}	}
