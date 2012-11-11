
//////////////////////////////////////////////////////////////////////////
/// WebPageLinkRemoveAction class implementation.
/// @file WebPageLinkRemoveAction.cpp
/// @author Hugues Romain
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

#include "WebPageLinkRemoveAction.hpp"

#include "ActionException.h"
#include "CMSRight.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "WebPageTableSync.h"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<Action, cms::WebPageLinkRemoveAction>::FACTORY_KEY("WebPageLinkRemoveAction");
	}

	namespace cms
	{
		const string WebPageLinkRemoveAction::PARAMETER_PAGE_ID = Action_PARAMETER_PREFIX + "id";
		const string WebPageLinkRemoveAction::PARAMETER_DESTINATION_ID = Action_PARAMETER_PREFIX + "to";



		ParametersMap WebPageLinkRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			if(_destinationPage.get())
			{
				map.insert(PARAMETER_DESTINATION_ID, _destinationPage->getKey());
			}
			return map;
		}



		void WebPageLinkRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_PAGE_ID), *_env);
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw ActionException("No such web page");
			}
			try
			{
				_destinationPage = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_DESTINATION_ID), *_env);
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw ActionException("No such destination web page");
			}
		}



		void WebPageLinkRemoveAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			WebpageLinks::Type links(_page->get<WebpageLinks>());
			WebpageLinks::Type newLinks;
			BOOST_FOREACH(const WebpageLinks::Type::value_type& link, links)
			{
				if(link != _destinationPage.get())
				{
					newLinks.push_back(link);
				}
			}
			_page->set<WebpageLinks>(newLinks);

			WebPageTableSync::Save(_page.get());

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool WebPageLinkRemoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CMSRight>(WRITE);
		}
}	}
