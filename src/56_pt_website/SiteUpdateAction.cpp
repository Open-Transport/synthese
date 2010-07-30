
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "SiteUpdateAction.h"
#include "TransportWebsite.h"
#include "TransportWebsiteTableSync.h"
#include "WebPageTableSync.h"

using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace cms;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt_website::SiteUpdateAction>::FACTORY_KEY("SiteUpdateAction");
	}

	namespace pt_website
	{
		const string SiteUpdateAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "si";
		const string SiteUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string SiteUpdateAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sd";
		const string SiteUpdateAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "ed";
		const string SiteUpdateAction::PARAMETER_ONLINE_BOOKING = Action_PARAMETER_PREFIX + "ob";
		const string SiteUpdateAction::PARAMETER_USE_OLD_DATA = Action_PARAMETER_PREFIX + "uo";
		const string SiteUpdateAction::PARAMETER_MAX_CONNECTIONS = Action_PARAMETER_PREFIX + "mc";
		const string SiteUpdateAction::PARAMETER_USE_DATES_RANGE = Action_PARAMETER_PREFIX + "dr";
		const string SiteUpdateAction::PARAMETER_DISPLAY_ROAD_APPROACH_DETAIL = Action_PARAMETER_PREFIX + "da";
		const string SiteUpdateAction::PARAMETER_CLIENT_URL = Action_PARAMETER_PREFIX + "cu";
		const string SiteUpdateAction::PARAMETER_DEFAULT_PAGE_TEMPLATE_ID = Action_PARAMETER_PREFIX + "ti";
		
		
		
		SiteUpdateAction::SiteUpdateAction()
			: util::FactorableTemplate<Action, SiteUpdateAction>()
			, _startDate(not_a_date_time)
			, _endDate(not_a_date_time),
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
				map.insert(PARAMETER_CLIENT_URL, _clientURL);
				map.insert(PARAMETER_DEFAULT_PAGE_TEMPLATE_ID, _defaultPageTemplate.get() ? _defaultPageTemplate->getKey() : RegistryKeyType(0));
			}
			return map;
		}
		
		
		
		void SiteUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			setSiteId(map.get<RegistryKeyType>(PARAMETER_SITE_ID));
			_name = map.get<string>(PARAMETER_NAME);
			if(!map.getDefault<string>(PARAMETER_START_DATE).empty())
			{
				_startDate = from_string(map.get<string>(PARAMETER_START_DATE));
			}
			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				_endDate = from_string(map.get<string>(PARAMETER_END_DATE));
			}
			_onlineBooking = map.get<bool>(PARAMETER_ONLINE_BOOKING);
			_useOldData = map.get<bool>(PARAMETER_USE_OLD_DATA);
			_useDatesRange = days(map.get<int>(PARAMETER_USE_DATES_RANGE));
			_maxConnections = map.get<int>(PARAMETER_MAX_CONNECTIONS);
			_displayRoadApproachDetail = map.get<bool>(PARAMETER_DISPLAY_ROAD_APPROACH_DETAIL);
			_clientURL = map.get<string>(PARAMETER_CLIENT_URL);
			RegistryKeyType pageTemplateId(map.get<RegistryKeyType>(PARAMETER_DEFAULT_PAGE_TEMPLATE_ID));
			if(pageTemplateId > 0) try
			{
				_defaultPageTemplate = WebPageTableSync::GetEditable(pageTemplateId, *_env);
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw ActionException("No such page template");
			}
		}
		
		
		
		void SiteUpdateAction::run(Request& request)
		{
			_site->setName(_name);
			_site->setStartDate(_startDate);
			_site->setEndDate(_endDate);
			_site->setOnlineBookingAllowed(_onlineBooking);
			_site->setPastSolutionsDisplayed(_useOldData);
			_site->setUseDateRange(_useDatesRange);
			_site->setMaxTransportConnectionsCount(_maxConnections);
			_site->setDisplayRoadApproachDetail(_displayRoadApproachDetail);
			_site->setClientURL(_clientURL);
			_site->setDefaultTemplate(_defaultPageTemplate.get());

			TransportWebsiteTableSync::Save(_site.get());
		}



		void SiteUpdateAction::setSiteId( RegistryKeyType id )
		{
			try
			{
				_site = TransportWebsiteTableSync::GetEditable(id, *_env);
			}
			catch(...)
			{
				throw ActionException("No such site");	
			}
		}



		bool SiteUpdateAction::isAuthorized(const Session* session
		) const {
			return true;
		}
	}
}
