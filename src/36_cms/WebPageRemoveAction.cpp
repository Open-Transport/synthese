
//////////////////////////////////////////////////////////////////////////
/// WebPageRemoveAction class implementation.
/// @file WebPageRemoveAction.cpp
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
#include "WebPageRemoveAction.h"
#include "Request.h"
#include "WebPageTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, cms::WebPageRemoveAction>::FACTORY_KEY("WebPageRemoveAction");
	}

	namespace cms
	{
		const string WebPageRemoveAction::PARAMETER_WEB_PAGE_ID = Action_PARAMETER_PREFIX + "wi";
		
		
		
		ParametersMap WebPageRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_WEB_PAGE_ID, _page->getKey());
			}
			return map;
		}
		
		
		
		void WebPageRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = WebPageTableSync::Get(map.get<RegistryKeyType>(PARAMETER_WEB_PAGE_ID), *_env);
			}
			catch(ObjectNotFoundException<Webpage>& e)
			{
				throw ActionException("No such page");
			}

		}
		
		
		
		void WebPageRemoveAction::run(
			Request& request
		){
			WebPageTableSync::Remove(_page->getKey());
			
			WebPageTableSync::ShiftRank(
				_page->getRoot()->getKey(),
				_page->getParent() ? _page->getParent()->getKey() : RegistryKeyType(0),
				_page->getRank(),
				false
			);

//			::AddDeleteEntry(*_object, request.getUser().get());

		}
		
		
		
		bool WebPageRemoveAction::isAuthorized(
			const Session* session
		) const {
			return true;
			//return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(DELETE_RIGHT);
		}



		void WebPageRemoveAction::setPage( boost::shared_ptr<const Webpage> value )
		{
			_page = value;
		}
	}
}
