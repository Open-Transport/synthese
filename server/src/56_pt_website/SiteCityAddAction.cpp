
//////////////////////////////////////////////////////////////////////////
/// SiteCityAddAction class implementation.
/// @file SiteCityAddAction.cpp
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

#include "SiteCityAddAction.hpp"

#include "ActionException.h"
#include "City.h"
#include "ParametersMap.h"
#include "GeographyModule.h"
#include "ObjectSiteLinkTableSync.h"
#include "Profile.h"
#include "PTServiceConfig.hpp"
#include "PTServiceConfigTableSync.hpp"
#include "Request.h"
#include "Session.h"
#include "TransportWebsiteRight.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt_website::SiteCityAddAction>::FACTORY_KEY("SiteCityAddAction");
	}

	namespace pt_website
	{
		const string SiteCityAddAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "id";
		const string SiteCityAddAction::PARAMETER_CITY_NAME = Action_PARAMETER_PREFIX + "cn";



		ParametersMap SiteCityAddAction::getParametersMap() const
		{
			ParametersMap map;
			if(_config.get())
			{
				map.insert(PARAMETER_SITE_ID, _config->getKey());
			}
			if(_city)
			{

			}
			return map;
		}



		void SiteCityAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_config = PTServiceConfigTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_SITE_ID), *_env);
			}
			catch(ObjectNotFoundException<PTServiceConfig>&)
			{
				throw ActionException("No such site");
			}

			GeographyModule::CityList list(
				GeographyModule::GuessCity(
					map.get<string>(PARAMETER_CITY_NAME),
					1
			)	);
			if(list.empty())
			{
				throw ActionException("No such city");
			}
			_city = list.front().get();
		}



		void SiteCityAddAction::run(
			Request& request
		){
			ObjectSiteLink link;
			link.set<Site>(*_config);
			link.set<ObjectId>(_city->getKey());
			ObjectSiteLinkTableSync::Save(&link);

//			::AddCreationEntry(object, request.getUser().get());
			request.setActionCreatedId(link.getKey());
		}



		bool SiteCityAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(WRITE);
		}
	}
}
