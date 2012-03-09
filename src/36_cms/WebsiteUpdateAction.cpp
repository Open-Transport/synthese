
/** WebsiteUpdateAction class implementation.
	@file WebsiteUpdateAction.cpp
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

#include "WebsiteUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "TransportWebsite.h"
#include "WebsiteTableSync.hpp"
#include "WebPageTableSync.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace server;
	using namespace util;
	using namespace cms;

	namespace util
	{
		template<> const string FactorableTemplate<Action, WebsiteUpdateAction>::FACTORY_KEY("WebsiteUpdateAction");
	}

	namespace cms
	{
		const string WebsiteUpdateAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "si";
		const string WebsiteUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string WebsiteUpdateAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sd";
		const string WebsiteUpdateAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "ed";
		const string WebsiteUpdateAction::PARAMETER_CLIENT_URL = Action_PARAMETER_PREFIX + "cu";
		const string WebsiteUpdateAction::PARAMETER_DEFAULT_PAGE_TEMPLATE_ID = Action_PARAMETER_PREFIX + "ti";



		WebsiteUpdateAction::WebsiteUpdateAction()
			: util::FactorableTemplate<Action, WebsiteUpdateAction>()
			, _startDate(not_a_date_time)
			, _endDate(not_a_date_time)
		{
		}



		ParametersMap WebsiteUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if (_site.get())
			{
				map.insert(PARAMETER_SITE_ID, _site->getKey());
				map.insert(PARAMETER_CLIENT_URL, _clientURL);
				map.insert(PARAMETER_DEFAULT_PAGE_TEMPLATE_ID, _defaultPageTemplate.get() ? _defaultPageTemplate->getKey() : RegistryKeyType(0));
			}
			return map;
		}



		void WebsiteUpdateAction::_setFromParametersMap(const ParametersMap& map)
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



		void WebsiteUpdateAction::run(Request& request)
		{
			_site->set<Name>(_name);
			_site->set<StartDate>(_startDate);
			_site->set<EndDate>(_endDate);
			_site->set<ClientURL>(_clientURL);
			_site->set<DefaultTemplate>(_defaultPageTemplate.get() ? optional<Webpage&>(*_defaultPageTemplate) : optional<Webpage&>());

			WebsiteTableSync::Save(_site.get());
		}



		void WebsiteUpdateAction::setSiteId( RegistryKeyType id )
		{
			try
			{
				_site = WebsiteTableSync::GetEditable(id, *_env);
			}
			catch(...)
			{
				throw ActionException("No such site");
			}
		}



		bool WebsiteUpdateAction::isAuthorized(const Session* session
		) const {
			return true;
		}
	}
}
