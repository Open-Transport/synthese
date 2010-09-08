
//////////////////////////////////////////////////////////////////////////
/// SiteObjectLinkRemoveAction class implementation.
/// @file SiteObjectLinkRemoveAction.cpp
/// @author Hugues Romain
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
#include "SiteObjectLinkRemoveAction.hpp"
#include "TransportWebsiteRight.h"
#include "Request.h"
#include "ObjectSiteLinkTableSync.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt_website::SiteObjectLinkRemoveAction>::FACTORY_KEY("SiteObjectLinkRemoveAction");
	}

	namespace pt_website
	{
		const string SiteObjectLinkRemoveAction::PARAMETER_LINK_ID = Action_PARAMETER_PREFIX + "id";
		
		
		
		ParametersMap SiteObjectLinkRemoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_link.get())
			{
				map.insert(PARAMETER_LINK_ID, _link->getKey());
			}
			return map;
		}
		
		
		
		void SiteObjectLinkRemoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_link = ObjectSiteLinkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_LINK_ID), *_env);
			}
			catch(ObjectNotFoundException<ObjectSiteLink>&)
			{
				throw ActionException("No such link");
			}
		}
		
		
		
		void SiteObjectLinkRemoveAction::run(
			Request& request
		){
			ObjectSiteLinkTableSync::Remove(_link->getKey());
//			::AddDeleteEntry(*_object, request.getUser().get());

		}
		
		
		
		bool SiteObjectLinkRemoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(DELETE_RIGHT);
		}
	}
}
