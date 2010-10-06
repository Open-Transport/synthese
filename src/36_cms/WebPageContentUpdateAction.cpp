
//////////////////////////////////////////////////////////////////////////
/// WebPageContentUpdateAction class implementation.
/// @file WebPageContentUpdateAction.cpp
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
#include "WebPageContentUpdateAction.hpp"
#include "Request.h"
#include "Webpage.h"
#include "WebPageTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, cms::WebPageContentUpdateAction>::FACTORY_KEY("WebPageContentUpdateAction");
	}

	namespace cms
	{
		const string WebPageContentUpdateAction::PARAMETER_WEB_PAGE_ID = Action_PARAMETER_PREFIX + "wp";
		const string WebPageContentUpdateAction::PARAMETER_CONTENT1 = Action_PARAMETER_PREFIX + "c1";
		const string WebPageContentUpdateAction::PARAMETER_ABSTRACT = Action_PARAMETER_PREFIX + "ab";
		const string WebPageContentUpdateAction::PARAMETER_IMAGE = Action_PARAMETER_PREFIX + "im";
		const string WebPageContentUpdateAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "ti";
		
		
		
		ParametersMap WebPageContentUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_WEB_PAGE_ID, _page->getKey());
			}
			map.insert(PARAMETER_CONTENT1, _content1);
			map.insert(PARAMETER_ABSTRACT, _abstract);
			map.insert(PARAMETER_IMAGE, _image);
			map.insert(PARAMETER_TITLE, _title);
			return map;
		}
		
		
		
		void WebPageContentUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_WEB_PAGE_ID), *_env);
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw ActionException("No such page");
			}

			_content1 = map.getDefault<string>(PARAMETER_CONTENT1);
			_abstract = map.getDefault<string>(PARAMETER_ABSTRACT);
			_image = map.getDefault<string>(PARAMETER_IMAGE);
			_title = map.getDefault<string>(PARAMETER_TITLE);
		}
		
		
		
		void WebPageContentUpdateAction::run(
			Request& request
		){
			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);
			_page->setContent(_content1);
			_page->setAbstract(_abstract);
			_page->setImage(_image);
			_page->setName(_title);

			WebPageTableSync::Save(_page.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}
		
		
		
		bool WebPageContentUpdateAction::isAuthorized(
			const Session* session
		) const {
			return true;
			//return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(WRITE);
		}


		void WebPageContentUpdateAction::setWebPage( boost::shared_ptr<Webpage> value )
		{
			_page = value;
		}
	}
}
