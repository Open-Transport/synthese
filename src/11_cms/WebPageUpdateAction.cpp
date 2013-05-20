
//////////////////////////////////////////////////////////////////////////
/// WebPageUpdateAction class implementation.
/// @file WebPageUpdateAction.cpp
/// @author Hugues
/// @date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "WebPageUpdateAction.h"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "WebPageTableSync.h"
#include "DBTransaction.hpp"
#include "WebsiteTableSync.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, cms::WebPageUpdateAction>::FACTORY_KEY("WebPageUpdateAction");
	}

	namespace cms
	{
		const string WebPageUpdateAction::PARAMETER_WEB_PAGE_ID = Action_PARAMETER_PREFIX + "wp";
		const string WebPageUpdateAction::PARAMETER_UP_ID = Action_PARAMETER_PREFIX + "ui";
		const string WebPageUpdateAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "_site_id";
		const string WebPageUpdateAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "te";
		const string WebPageUpdateAction::PARAMETER_START_DATE = Action_PARAMETER_PREFIX + "sd";
		const string WebPageUpdateAction::PARAMETER_END_DATE = Action_PARAMETER_PREFIX + "ed";
		const string WebPageUpdateAction::PARAMETER_MAX_AGE = Action_PARAMETER_PREFIX + "ma";
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
		const string WebPageUpdateAction::PARAMETER_DO_NOT_EVALUATE = Action_PARAMETER_PREFIX + "_do_not_evaluate";



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
			if(_site)
			{
				map.insert(PARAMETER_SITE_ID, _site->get() ? (*_site)->getKey() : RegistryKeyType(0));
			}
			if(_template)
			{
				map.insert(PARAMETER_TEMPLATE_ID, *_template ? (*_template)->getKey() : RegistryKeyType(0));
			}
			if(_mimeType)
			{
				map.insert(PARAMETER_MIME_TYPE, string(*_mimeType));
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
			if(map.isDefined(PARAMETER_WEB_PAGE_ID))
			{
				try
				{
					_page = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_WEB_PAGE_ID), *_env);
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw ActionException("No such page");
				}
			}
			else
			{
				_page.reset(new Webpage);
			}

			// Up page
			if(map.isDefined(PARAMETER_UP_ID))
			{
				_up = boost::shared_ptr<Webpage>();
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

			// Site
			if(map.isDefined(PARAMETER_SITE_ID))
			{
				_site = boost::shared_ptr<Website>();
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_SITE_ID));
				if(id > 0)
					try
				{
					_site = WebsiteTableSync::GetEditable(id, *_env);
				}
				catch(ObjectNotFoundException<Website>&)
				{
					throw ActionException("No such site");
				}
			}

			// Template id
			if(map.isDefined(PARAMETER_TEMPLATE_ID))
			{
				_template = SpecificTemplate::Type();
				RegistryKeyType templateId(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE_ID, 0));
				if(templateId > 0)
				try
				{
					_template = SpecificTemplate::Type(*WebPageTableSync::GetEditable(templateId, *_env));
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

			// Max Age
			if(map.isDefined(PARAMETER_MAX_AGE))
			{
				if(map.getDefault<string>(PARAMETER_MAX_AGE).empty())
				{
					_maxAge = time_duration(not_a_date_time);
				}
				else
				{
					_maxAge = time_duration(0, 
											atoi(map.get<string>(PARAMETER_MAX_AGE).c_str()),
											0,
											0);
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

			// Mime type
			if(map.isDefined(PARAMETER_MIME_TYPE))
			{
				string value(map.get<string>(PARAMETER_MIME_TYPE));
				try
				{
					_mimeType = MimeTypes::GetMimeTypeByString(value);
				}
				catch(Exception&)
				{
					vector<string> parts;
					split(parts, value, is_any_of("/"));
					if(parts.size() >= 2)
					{
						_mimeType = MimeType(parts[0], parts[1], "");
					}
					else
					{
						throw ActionException("No such mime type");
					}
				}
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
				_content1 = map.get<string>(PARAMETER_CONTENT1, false);
			}
			if(map.isDefined(PARAMETER_ABSTRACT))
			{
				_abstract = map.get<string>(PARAMETER_ABSTRACT, false);
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

			// Do not use template
			if(map.isDefined(PARAMETER_DO_NOT_EVALUATE))
			{
				_doNotEvaluate = map.get<bool>(PARAMETER_DO_NOT_EVALUATE);
			}
		}



		void WebPageUpdateAction::_updateSite(
			Webpage& page,
			db::DBTransaction& transaction
		) const {

			// The update
			page.setRoot(_site->get());

			// Recursive calls (update only)
			if(page.getKey())
			{
				WebPageTableSync::SearchResult subPages(
					WebPageTableSync::Search(
					*_env,
					optional<RegistryKeyType>(),
					page.getKey()
					)	);
				BOOST_FOREACH(boost::shared_ptr<Webpage> subPage, subPages)
				{
					_updateSite(*subPage, transaction);
					WebPageTableSync::Save(subPage.get(), transaction);
				}
			}
		}



		void WebPageUpdateAction::run(
			Request& request
		){
			DBTransaction transaction;

			stringstream text;
			//::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			if(_site)
			{
				// Deleting the old position of the tree
				WebPageTableSync::ShiftRank(
					_page->getRoot()->getKey(),
					_page->getParent() ? _page->getParent()->getKey() : RegistryKeyType(0),
					_page->getRank() + 1,
					false,
					transaction
				);
			
				// Giving the highest rank into the new branch
				WebPageTableSync::SearchResult lastRankPage(
					WebPageTableSync::Search(
						*_env,
						(*_site)->getKey(),
						RegistryKeyType(0),
						optional<size_t>(),
						0,
						1,
						true,
						false,
						false
				)	);
				_page->setRank(lastRankPage.empty() ? 0 : lastRankPage.front()->getRank() + 1);

				// Changing the site of the page
				_updateSite(*_page, transaction);

				// Putting the page under the root of the new site
				_page->setParent(NULL);
			}

			// Moving the page into an other node of the tree
			else if(_up && _up->get() != _page->getParent(true))
			{
				// Deleting the old position of the tree
				WebPageTableSync::ShiftRank(
					_page->getRoot()->getKey(),
					_page->getParent() ? _page->getParent()->getKey() : RegistryKeyType(0),
					_page->getRank() + 1,
					false,
					transaction
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

				// Should never happen
				if(!_page->getRoot() && _up->get())
				{
					_page->setRoot((*_up)->getRoot());
				}
			}

			if(_content1 || _mimeType || _ignoreWhiteChars || _doNotEvaluate)
			{
				bool ignoreWhiteChars(
					_ignoreWhiteChars ?
					*_ignoreWhiteChars :
					_page->get<WebpageContent>().getCMSScript().getIgnoreWhiteChars()
				);

				MimeType mimeType(
					_mimeType ?
					*_mimeType :
					_page->get<WebpageContent>().getMimeType()
				);

				string content(
					_content1 ?
					*_content1 :
					_page->get<WebpageContent>().getCMSScript().getCode()
				);

				bool doNotEvaluate(
					_doNotEvaluate ?
					*_doNotEvaluate :
					_page->get<WebpageContent>().getCMSScript().getDoNotEvaluate()
				);

				if(_content1 && _decodeXMLEntitiesInContent)
				{
					boost::algorithm::replace_all(content,"&lt;?", "<?");
					boost::algorithm::replace_all(content,"?&gt;", "?>");
				}
				_page->set<WebpageContent>(WebpageContent(content, ignoreWhiteChars, mimeType,doNotEvaluate));
			}
			if(_abstract)
			{
				_page->set<Abstract>(*_abstract);
			}
			if(_image)
			{
				_page->set<ImageURL>(*_image);
			}
			if(_title)
			{
				_page->set<Title>(*_title);
			}
			if(_startDate)
			{
				_page->set<StartTime>(*_startDate);
			}
			if(_endDate)
			{
				_page->set<EndTime>(*_endDate);
			}
			if(_maxAge)
			{
				_page->set<MaxAge>( !(*_maxAge).is_not_a_date_time()
									? *_maxAge 
									: time_duration(0,0,0,0) );
			}
			if(_template)
			{
				_page->set<SpecificTemplate>(*_template);
			}
			if(_doNotUseTemplate)
			{
				_page->set<DoNotUseTemplate>(*_doNotUseTemplate);
			}
			if(_hasForum)
			{
				_page->set<HasForum>(*_hasForum);
			}
			if(_smartURLPath)
			{
				_page->set<SmartURLPath>(*_smartURLPath);
			}
			if(_smartURLDefaultParameterName)
			{
				_page->set<SmartURLDefaultParameterName>(*_smartURLDefaultParameterName);
			}
			if(_rawEditor)
			{
				_page->set<RawEditor>(*_rawEditor);
			}

			WebPageTableSync::Save(_page.get(), transaction);
			transaction.run();



			//::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool WebPageUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		void WebPageUpdateAction::setWebPage( boost::shared_ptr<Webpage> value )
		{
			_page = value;
		}



		WebPageUpdateAction::WebPageUpdateAction():
			_decodeXMLEntitiesInContent(false)
		{}
}	}
