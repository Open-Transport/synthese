
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
		
		
		
		ParametersMap WebpageContentUploadAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_WEBPAGE_ID, _page->getKey());
			}
			return map;
		}
		
		
		
		void WebpageContentUploadAction::_setFromParametersMap(const ParametersMap& map)
		{
			// The page to update
			try
			{
				_page = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_WEBPAGE_ID), *_env);
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw ActionException("Page not found");
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
			WebpageContent c(_file.content, false, _file.mimeType, false);
			_page->set<WebpageContent>(c);

			// Update of the smart URL
			if(_page->get<SmartURLPath>().empty())
			{
				string smartURL;
				if(_page->getParent() && !_page->getParent()->get<SmartURLPath>().empty())
				{
					smartURL = _page->getParent()->get<SmartURLPath>().empty() + "/";
				}
				smartURL = smartURL + _file.filename;
				_page->set<SmartURLPath>(smartURL);
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

