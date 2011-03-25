
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
		const string WebPageUpdateAction::PARAMETER_SMART_URL_PATH(Action_PARAMETER_PREFIX + "sp");
		const string WebPageUpdateAction::PARAMETER_SMART_URL_DEFAULT_PARAMETER_NAME(Action_PARAMETER_PREFIX + "sn");
		const string WebPageUpdateAction::PARAMETER_CONTENT1 = Action_PARAMETER_PREFIX + "c1";
		const string WebPageUpdateAction::PARAMETER_ABSTRACT = Action_PARAMETER_PREFIX + "ab";
		const string WebPageUpdateAction::PARAMETER_IMAGE = Action_PARAMETER_PREFIX + "im";
		const string WebPageUpdateAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "ti";
		const string WebPageUpdateAction::PARAMETER_IGNORE_WHITE_CHARS = Action_PARAMETER_PREFIX + "iw";
		const string WebPageUpdateAction::PARAMETER_DECODE_XML_ENTITIES_IN_CONTENT(Action_PARAMETER_PREFIX + "dx");
		const string WebPageUpdateAction::PARAMETER_RAW_EDITOR(Action_PARAMETER_PREFIX + "re");

		
		
		ParametersMap WebPageUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_WEB_PAGE_ID, _page->getKey());
			}
			if(_up)
			{
				map.insert(PARAMETER_UP_ID, _up->get() ? (*_up)->getKey() : RegistryKeyType(0));
			}
			if(_template)
			{
				map.insert(PARAMETER_TEMPLATE_ID, _template->get() ? (*_template)->getKey() : RegistryKeyType(0));
			}
			if(_mimeType)
			{
				map.insert(PARAMETER_MIME_TYPE, *_mimeType);
			}
			if(_doNotUseTemplate)
			{
				map.insert(PARAMETER_DO_NOT_USE_TEMPLATE, *_doNotUseTemplate);
			}
			if(_hasForum)
			{
				map.insert(PARAMETER_HAS_FORUM, *_hasForum);
			}
			if(_smartURLPath)
			{
				map.insert(PARAMETER_SMART_URL_PATH, *_smartURLPath);
			}
			if(_smartURLDefaultParameterName)
			{
				map.insert(PARAMETER_SMART_URL_DEFAULT_PARAMETER_NAME, *_smartURLDefaultParameterName);
			}
			if(_content1)
			{
				map.insert(PARAMETER_CONTENT1, *_content1);
			}
			if(_abstract)
			{
				map.insert(PARAMETER_ABSTRACT, *_abstract);
			}
			if(_image)
			{
				map.insert(PARAMETER_IMAGE, *_image);
			}
			if(_title)
			{
				map.insert(PARAMETER_TITLE, *_title);
			}
			if(_ignoreWhiteChars)
			{
				map.insert(PARAMETER_IGNORE_WHITE_CHARS, *_ignoreWhiteChars);
			}
			if(_rawEditor)
			{
				map.insert(PARAMETER_RAW_EDITOR, *_rawEditor);
			}
			map.insert(PARAMETER_DECODE_XML_ENTITIES_IN_CONTENT, _decodeXMLEntitiesInContent);
			return map;
		}
		
		
		
		void WebPageUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_WEB_PAGE_ID), *_env);
			}
			catch(ObjectNotFoundException<Webpage>& e)
			{
				throw ActionException("No such page");
			}

			// Up page
			if(map.isDefined(PARAMETER_UP_ID))
			{
				_up = shared_ptr<Webpage>();
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_UP_ID));
				if(id > 0)
				try
				{
					_up = WebPageTableSync::GetEditable(id, *_env);
					for(Webpage* page(_up->get()); page != NULL; page = page->getParent())
					{
						if(page == _page.get())
						{
							throw ActionException("A page cannot be moved into a subpage node");
						}
					}
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw ActionException("No such up page");
				}
			}

			// Template id
			if(map.isDefined(PARAMETER_TEMPLATE_ID))
			{
				_template = shared_ptr<Webpage>();
				RegistryKeyType templateId(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE_ID, 0));
				if(templateId > 0)
				try
				{
					_template = WebPageTableSync::GetEditable(templateId, *_env);
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw ActionException("No such template page");
				}
			}

			// Start date
			if(map.isDefined(PARAMETER_START_DATE))
			{
				if(map.getDefault<string>(PARAMETER_START_DATE).empty())
				{
					_startDate = ptime(not_a_date_time);
				}
				else
				{
					_startDate = time_from_string(map.get<string>(PARAMETER_START_DATE));
				}
			}

			// End time
			if(map.isDefined(PARAMETER_END_DATE))
			{
				if(map.getDefault<string>(PARAMETER_END_DATE).empty())
				{
					_endDate = ptime(not_a_date_time);
				}
				else
				{
					_endDate = time_from_string(map.get<string>(PARAMETER_END_DATE));
				}
			}

			if(map.isDefined(PARAMETER_DO_NOT_USE_TEMPLATE))
			{
				_doNotUseTemplate = map.getDefault<bool>(PARAMETER_DO_NOT_USE_TEMPLATE, false);
			}

			if(map.isDefined(PARAMETER_HAS_FORUM))
			{
				_hasForum = map.getDefault<bool>(PARAMETER_HAS_FORUM, false);
			}

			if(map.isDefined(PARAMETER_MIME_TYPE))
			{
				_mimeType = map.get<string>(PARAMETER_MIME_TYPE);
			}

			if(map.isDefined(PARAMETER_SMART_URL_PATH))
			{
				_smartURLPath = map.get<string>(PARAMETER_SMART_URL_PATH);
			}

			if(map.isDefined(PARAMETER_SMART_URL_DEFAULT_PARAMETER_NAME))
			{
				_smartURLDefaultParameterName = map.get<string>(PARAMETER_SMART_URL_DEFAULT_PARAMETER_NAME);
			}

			if(map.isDefined(PARAMETER_CONTENT1))
			{
				_content1 = map.get<string>(PARAMETER_CONTENT1);
			}
			if(map.isDefined(PARAMETER_ABSTRACT))
			{
				_abstract = map.get<string>(PARAMETER_ABSTRACT);
			}
			if(map.isDefined(PARAMETER_IMAGE))
			{
				_image = map.get<string>(PARAMETER_IMAGE);
			}
			if(map.isDefined(PARAMETER_TITLE))
			{
				_title = map.get<string>(PARAMETER_TITLE);
			}
			if(map.isDefined(PARAMETER_IGNORE_WHITE_CHARS))
			{
				_ignoreWhiteChars = map.getDefault<bool>(PARAMETER_IGNORE_WHITE_CHARS, false);
			}
			if(map.isDefined(PARAMETER_RAW_EDITOR))
			{
				_rawEditor = map.getDefault<bool>(PARAMETER_RAW_EDITOR, false);
			}
			_decodeXMLEntitiesInContent = map.getDefault<bool>(PARAMETER_DECODE_XML_ENTITIES_IN_CONTENT, false);
		}
		
		
		
		void WebPageUpdateAction::run(
			Request& request
		){
			stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			
			// Moving the page into an other node of the tree
			if(_up && _up->get() != _page->getParent())
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
						_up->get() ? (*_up)->getKey() : RegistryKeyType(0),
						optional<size_t>(),
						0,
						1,
						true,
						false,
						false
				)	);
				_page->setRank(lastRankPage.empty() ? 0 : lastRankPage.front()->getRank() + 1);
				_page->setParent(_up->get());
			}

			if(_content1)
			{
				string content(*_content1);
				if(_decodeXMLEntitiesInContent)
				{
					boost::algorithm::replace_all(content,"&lt;?", "<?");
					boost::algorithm::replace_all(content,"?&gt;", "?>");
				}
				_page->setContent(content);
			}
			if(_abstract)
			{
				_page->setAbstract(*_abstract);
			}
			if(_image)
			{
				_page->setImage(*_image);
			}
			if(_title)
			{
				_page->setName(*_title);
			}
			if(_ignoreWhiteChars)
			{
				_page->setIgnoreWhiteChars(*_ignoreWhiteChars);
			}
			if(_startDate)
			{
				_page->setStartDate(*_startDate);
			}
			if(_endDate)
			{
				_page->setEndDate(*_endDate);
			}
			if(_mimeType)
			{
				_page->setMimeType(*_mimeType);
			}
			if(_template)
			{
				_page->setTemplate(_template->get());
			}
			if(_doNotUseTemplate)
			{
				_page->setDoNotUseTemplate(*_doNotUseTemplate);
			}
			if(_hasForum)
			{
				_page->setHasForum(*_hasForum);
			}
			if(_smartURLPath)
			{
				_page->setSmartURLPath(*_smartURLPath);
			}
			if(_smartURLDefaultParameterName)
			{
				_page->setSmartURLDefaultParameterName(*_smartURLDefaultParameterName);
			}
			if(_rawEditor)
			{
				_page->setRawEditor(*_rawEditor);
			}

			WebPageTableSync::Save(_page.get());

			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool WebPageUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
			//return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(WRITE);
		}



		void WebPageUpdateAction::setWebPage( boost::shared_ptr<Webpage> value )
		{
			_page = value;
		}



		WebPageUpdateAction::WebPageUpdateAction():
		_decodeXMLEntitiesInContent(false)
		{}
}	}
