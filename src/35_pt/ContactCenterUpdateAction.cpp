
//////////////////////////////////////////////////////////////////////////
/// ContactCenterUpdateAction class implementation.
/// @file ContactCenterUpdateAction.cpp
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

#include "ContactCenterUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Request.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "TransportNetworkRight.h"

using namespace std;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::ContactCenterUpdateAction>::FACTORY_KEY = "ContactCenterUpdate";
	}

	namespace pt
	{
		const string ContactCenterUpdateAction::PARAMETER_CONTACT_CENTER_ID = Action_PARAMETER_PREFIX + "_contact_center_id";
		const string ContactCenterUpdateAction::PARAMETER_DESCRIPTION = Action_PARAMETER_PREFIX + "_description";
		const string ContactCenterUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "_name";
		const string ContactCenterUpdateAction::PARAMETER_PHONE_NUMBER = Action_PARAMETER_PREFIX + "_phone_numbers";
		const string ContactCenterUpdateAction::PARAMETER_PHONE_OPENING_HOURS = Action_PARAMETER_PREFIX + "_phone_opening_hours";
		const string ContactCenterUpdateAction::PARAMETER_WEBSITE_URL = Action_PARAMETER_PREFIX + "_website_url";
		
		
		
		ParametersMap ContactCenterUpdateAction::getParametersMap() const
		{
			ParametersMap map;

			// Contact center
			if(_contactCenter.get())
			{
				map.insert(PARAMETER_CONTACT_CENTER_ID, _contactCenter->getKey());
			}

			return map;
		}
		
		
		
		void ContactCenterUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Contact center
			if(map.isDefined(PARAMETER_CONTACT_CENTER_ID)) try
			{
				_contactCenter = ReservationContactTableSync::GetEditable(
					map.get<RegistryKeyType>(PARAMETER_CONTACT_CENTER_ID),
					*_env
				);
			}
			catch(ObjectNotFoundException<ReservationContact>&)
			{
				throw ActionException("No such contact center");
			}
			else
			{
				_contactCenter.reset(new ReservationContact);
			}

			// Name
			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
			}

			// Phone number
			if(map.isDefined(PARAMETER_PHONE_NUMBER))
			{
				_phoneNumber = map.get<string>(PARAMETER_PHONE_NUMBER);
			}

			// Phone opening hours
			if(map.isDefined(PARAMETER_PHONE_OPENING_HOURS))
			{
				_phoneOpeningHours = map.get<string>(PARAMETER_PHONE_OPENING_HOURS);
			}

			// Website URL
			if(map.isDefined(PARAMETER_WEBSITE_URL))
			{
				_webSiteURL = map.get<string>(PARAMETER_WEBSITE_URL);
			}

			// Description
			if(map.isDefined(PARAMETER_DESCRIPTION))
			{
				_description = map.get<string>(PARAMETER_DESCRIPTION);
			}
		}
		
		
		
		void ContactCenterUpdateAction::run(
			Request& request
		){
			// Name
			if(_name)
			{
				_contactCenter->setName(*_name);
			}

			// Phone number
			if(_phoneNumber)
			{
				_contactCenter->setPhoneExchangeNumber(*_phoneNumber);
			}

			// Phone opening hours
			if(_phoneOpeningHours)
			{
				_contactCenter->setPhoneExchangeOpeningHours(*_phoneOpeningHours);
			}

			// Website URL
			if(_webSiteURL)
			{
				_contactCenter->setWebSiteUrl(*_webSiteURL);
			}

			// Description
			if(_description)
			{
				_contactCenter->setDescription(*_description);
			}

			// Save
			ReservationContactTableSync::Save(_contactCenter.get());

			// Return auto increment
			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_contactCenter->getKey());
			}
		}
		
		
		
		bool ContactCenterUpdateAction::isAuthorized(
			const Session* session
		) const {
			return
				session &&
				session->hasProfile() &&
				session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE)
			;
		}
}	}

