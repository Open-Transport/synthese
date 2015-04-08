
//////////////////////////////////////////////////////////////////////////
/// WebpageContentUploadAction class implementation.
/// @file WebpageContentUploadAction.cpp
/// @author Hugues Romain
/// @date 2012
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

#include "WebpageContentUploadAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "WebPageTableSync.h"
#include "WebsiteTableSync.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, cms::WebpageContentUploadAction>::FACTORY_KEY = "WebpageContentUploadAction";

	namespace cms
	{
		const string WebpageContentUploadAction::PARAMETER_WEBPAGE_ID = Action_PARAMETER_PREFIX + "_webpage_id";
		const string WebpageContentUploadAction::PARAMETER_CONTENT = Action_PARAMETER_PREFIX + "_content";
		const string WebpageContentUploadAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "_site_id";
		const string WebpageContentUploadAction::PARAMETER_UP_ID = Action_PARAMETER_PREFIX + "_up_id";
		const string WebpageContentUploadAction::PARAMETER_SMART_URL_FROM_TREE = Action_PARAMETER_PREFIX + "_smart_url_from_tree";
		

        WebpageContentUploadAction::WebpageContentUploadAction()
            : _smartUrlFromTree(false)
        {
            
        }

		
		ParametersMap WebpageContentUploadAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_WEBPAGE_ID, _page->getKey());
			}
			if(_site.get())
			{
				map.insert(PARAMETER_SITE_ID, _site->getKey());
			}
			if(_up.get())
			{
				map.insert(PARAMETER_UP_ID, _up->getKey());
			}
			if(_smartUrlFromTree)
			{
				map.insert(PARAMETER_SMART_URL_FROM_TREE, true);
			}
			return map;
		}
		
		
		
		void WebpageContentUploadAction::_setFromParametersMap(const ParametersMap& map)
		{
			// The page to update
			if(map.getOptional<RegistryKeyType>(PARAMETER_WEBPAGE_ID))
			{
				try
				{
					_page = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_WEBPAGE_ID), *_env);
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw ActionException("Page not found");
				}
			}
			else if(map.getOptional<RegistryKeyType>(PARAMETER_UP_ID))
			{
				try
				{
					_up = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_UP_ID), *_env);
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw ActionException("Up page not found");
				}
			}
			else if(map.getOptional<RegistryKeyType>(PARAMETER_SITE_ID))
			{
				try
				{
					_site = WebsiteTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_SITE_ID), *_env);
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw ActionException("Site not found");
				}
			}

            if (map.getOptional<bool>(PARAMETER_SMART_URL_FROM_TREE))
            {
                _smartUrlFromTree = map.get<bool>(PARAMETER_SMART_URL_FROM_TREE);
            }
            
			if(!_page.get())
			{
				if(!_site.get() && !_up.get())
				{
					throw ActionException("Undefined page");
				}
				else
				{
					_page.reset(new Webpage);
					WebPageTableSync::SearchResult result(
						WebPageTableSync::Search(
							Env::GetOfficialEnv(),
							_up.get() ? _up->getRoot()->getKey() : _site->getKey(),
							_up.get() ? _up->getKey() : RegistryKeyType(0),
							optional<size_t>(),
							0,
							1,
							true,
							false,
							false
					)	);
					_page->setRank(result.empty() ? 0 : (*result.begin())->getRank() + 1);
					if(_up.get())
					{
						_page->setParent(_up.get());
						_page->setRoot(_up->getRoot());
					}
					else
					{
						_page->setRoot(_site.get());
					}
				}
			}

			// The file to catch
			try
			{
				_file = map.getFile(PARAMETER_CONTENT);
			}
			catch(Record::MissingParameterException&)
			{
				throw ActionException("No content was uploaded");
			}
		}
		
		
		
		void WebpageContentUploadAction::run(
			Request& request
		){
			// Update of the content
			WebpageContent c(_file.content, false, _file.mimeType, true);
			_page->set<WebpageContent>(c);

			// Update of the smart URL
			if(_page->get<SmartURLPath>().empty())
			{
				string smartURL;
                if (_smartUrlFromTree)
                {
                    smartURL += "/" + _file.filename;
                    Webpage* webPage(_page->getParent());
                    while (webPage)
                    {
                        smartURL = "/" + webPage->getName() + smartURL;
                        webPage = webPage->getParent();
                    }
                }
                else
                {
                    if(_page->getParent() && !_page->getParent()->get<SmartURLPath>().empty())
                    {
                        smartURL = _page->getParent()->get<SmartURLPath>();
                    }
                    smartURL += "/" + _file.filename;
                }
                _page->set<SmartURLPath>(smartURL);
			}

			// Update of the name if necessary
			if(_page->get<Title>().empty())
			{
				_page->set<Title>(_file.filename);
			}

			// Do not use template to display uploaded files
			_page->set<DoNotUseTemplate>(true);

			// Saving
			WebPageTableSync::Save(_page.get());
		}
		
		
		
		bool WebpageContentUploadAction::isAuthorized(
			const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}
}	}

