
//////////////////////////////////////////////////////////////////////////
/// WebPageUpdateAction class implementation.
/// @file WebPageUpdateAction.cpp
/// @author Hugues
/// @date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "ActionException.h"
#include "ParametersMap.h"
#include "WebPageUpdateAction.h"
#include "TransportWebsiteRight.h"
#include "Request.h"
#include "WebPageTableSync.h"

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, transportwebsite::WebPageUpdateAction>::FACTORY_KEY("WebPageUpdateAction");
	}

	namespace transportwebsite
	{
		const string WebPageUpdateAction::PARAMETER_WEB_PAGE_ID = Action_PARAMETER_PREFIX + "wp";
		const string WebPageUpdateAction::PARAMETER_UP_ID = Action_PARAMETER_PREFIX + "ui";
		const string WebPageUpdateAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "ti";
		const string WebPageUpdateAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sd";
		const string WebPageUpdateAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "ed";

		
		
		ParametersMap WebPageUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_WEB_PAGE_ID, _page->getKey());
			}
			return map;
		}
		
		
		
		void WebPageUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_WEB_PAGE_ID), *_env);
			}
			catch(ObjectNotFoundException<WebPage>& e)
			{
				throw ActionException("No such page");
			}

			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_UP_ID));
			if(id > 0)
			try
			{
				_up = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_WEB_PAGE_ID), *_env);
			}
			catch(ObjectNotFoundException<WebPage>& e)
			{
				throw ActionException("No such up page");
			}

			if(!map.getDefault<string>(PARAMETER_START_DATE).empty())
			{
				_startDate = from_iso_string(map.get<string>(PARAMETER_START_DATE));
			}
			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				_endDate = from_iso_string(map.get<string>(PARAMETER_END_DATE));
			}

			_title = map.getDefault<string>(PARAMETER_TITLE);
		}
		
		
		
		void WebPageUpdateAction::run(
			Request& request
		){
			stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			_page->setName(_title);
			_page->setParent(_up.get());
			_page->setStartDate(_startDate);
			_page->setEndDate(_endDate);

			WebPageTableSync::Save(_page.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool WebPageUpdateAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(WRITE);
		}



		void WebPageUpdateAction::setWebPage( boost::shared_ptr<WebPage> value )
		{
			_page = value;
		}



		WebPageUpdateAction::WebPageUpdateAction():
			_startDate(not_a_date_time),
			_endDate(not_a_date_time)
		{

		}
	}
}
