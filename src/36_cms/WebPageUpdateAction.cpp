
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
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, cms::WebPageUpdateAction>::FACTORY_KEY("WebPageUpdateAction");
	}

	namespace cms
	{
		const string WebPageUpdateAction::PARAMETER_WEB_PAGE_ID = Action_PARAMETER_PREFIX + "wp";
		const string WebPageUpdateAction::PARAMETER_UP_ID = Action_PARAMETER_PREFIX + "ui";
		const string WebPageUpdateAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "te";
		const string WebPageUpdateAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sd";
		const string WebPageUpdateAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "ed";
		const string WebPageUpdateAction::PARAMETER_MIME_TYPE = Action_PARAMETER_PREFIX + "mt";
		const string WebPageUpdateAction::PARAMETER_DO_NOT_USE_TEMPLATE = Action_PARAMETER_PREFIX + "du";
		const string WebPageUpdateAction::PARAMETER_HAS_FORUM = Action_PARAMETER_PREFIX + "fo";

		
		
		ParametersMap WebPageUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_WEB_PAGE_ID, _page->getKey());
			}
			map.insert(PARAMETER_UP_ID, _up.get() ?_up->getKey() : RegistryKeyType(0));
			map.insert(PARAMETER_TEMPLATE_ID, _template.get() ? _template->getKey() : RegistryKeyType(0));
			map.insert(PARAMETER_MIME_TYPE, _mimeType);
			map.insert(PARAMETER_DO_NOT_USE_TEMPLATE, _doNotUseTemplate);
			map.insert(PARAMETER_HAS_FORUM, _hasForum);
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

			// Up page
			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_UP_ID));
			if(id > 0)
			try
			{
				_up = WebPageTableSync::GetEditable(id, *_env);
				for(WebPage* page(_up.get()); page != NULL; page = page->getParent())
				{
					if(page == _page.get())
					{
						throw ActionException("A page cannot be moved into a subpage node");
					}
				}
			}
			catch(ObjectNotFoundException<WebPage>&)
			{
				throw ActionException("No such up page");
			}

			RegistryKeyType templateId(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE_ID, 0));
			if(templateId > 0)
			try
			{
				_template = WebPageTableSync::GetEditable(templateId, *_env);
			}
			catch(ObjectNotFoundException<WebPage>&)
			{
				throw ActionException("No such template page");
			}

			if(!map.getDefault<string>(PARAMETER_START_DATE).empty())
			{
				_startDate = time_from_string(map.get<string>(PARAMETER_START_DATE));
			}
			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				_endDate = time_from_string(map.get<string>(PARAMETER_END_DATE));
			}

			_doNotUseTemplate = map.getDefault<bool>(PARAMETER_DO_NOT_USE_TEMPLATE, false);
			_hasForum = map.getDefault<bool>(PARAMETER_HAS_FORUM, false);
			_mimeType = map.getDefault<string>(PARAMETER_MIME_TYPE);
		}
		
		
		
		void WebPageUpdateAction::run(
			Request& request
		){
			stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			// Moving the page into an other node of the tree
			if(_up.get() != _page->getParent())
			{
				// Deleting the old position of the tree
				WebPageTableSync::ShiftRank(
					_page->getRoot()->getKey(),
					_page->getParent() ? _page->getParent()->getKey() : RegistryKeyType(0),
					_page->getRank(),
					false
				);

				// Giving the highest rank into the new branch
				WebPageTableSync::SearchResult lastRankPage(
					WebPageTableSync::Search(
						*_env,
						_page->getRoot()->getKey(),
						_up.get() ? _up->getKey() : RegistryKeyType(0),
						optional<size_t>(),
						0,
						1,
						true,
						false,
						false
				)	);
				_page->setRank(lastRankPage.empty() ? 0 : lastRankPage.front()->getRank() + 1);
				_page->setParent(_up.get());
			}
			_page->setStartDate(_startDate);
			_page->setEndDate(_endDate);
			_page->setMimeType(_mimeType);
			_page->setTemplate(_template.get());
			_page->setDoNotUseTemplate(_doNotUseTemplate);
			_page->setHasForum(_hasForum);

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
			_endDate(not_a_date_time),
			_doNotUseTemplate(false),
			_hasForum(false)
		{

		}
	}
}
