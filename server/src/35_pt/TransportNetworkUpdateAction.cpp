
//////////////////////////////////////////////////////////////////////////
/// TransportNetworkUpdateAction class implementation.
/// @file TransportNetworkUpdateAction.cpp
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

#include "TransportNetworkUpdateAction.hpp"

#include "ActionException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"
#include "Request.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "CalendarTemplateTableSync.h"
#include "ReservationContactTableSync.h"
#include "Record.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace impex;
	using namespace util;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<Action, pt::TransportNetworkUpdateAction>::FACTORY_KEY("network_update");
	}

	namespace pt
	{
		const string TransportNetworkUpdateAction::PARAMETER_NETWORK_ID = Action_PARAMETER_PREFIX + "ni";
		const string TransportNetworkUpdateAction::PARAMETER_NAME = Action_PARAMETER_PREFIX + "na";
		const string TransportNetworkUpdateAction::PARAMETER_DAYS_CALENDARS_PARENT_ID = Action_PARAMETER_PREFIX + "days_calendars_parent_id";
		const string TransportNetworkUpdateAction::PARAMETER_PERIODS_CALENDARS_PARENT_ID = Action_PARAMETER_PREFIX + "periods_calendars_parent_id";
		const string TransportNetworkUpdateAction::PARAMETER_IMAGE = Action_PARAMETER_PREFIX + "im";
		const string TransportNetworkUpdateAction::PARAMETER_TIMEZONE = Action_PARAMETER_PREFIX + "timezone";
		const string TransportNetworkUpdateAction::PARAMETER_LANG = Action_PARAMETER_PREFIX + "lang";
		const string TransportNetworkUpdateAction::PARAMETER_CONTACT_ID = Action_PARAMETER_PREFIX + "contact_id";
		const string TransportNetworkUpdateAction::PARAMETER_FARE_CONTACT_ID = Action_PARAMETER_PREFIX + "fare_contact_id";
		const string TransportNetworkUpdateAction::PARAMETER_COUNTRY_CODE = Action_PARAMETER_PREFIX + "country_code";


		ParametersMap TransportNetworkUpdateAction::getParametersMap() const
		{
			ParametersMap map;
			if(_network.get() && _network->getKey())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			if(_name)
			{
				map.insert(PARAMETER_NAME, *_name);
			}
			if(_image)
			{
				map.insert(PARAMETER_IMAGE, *_image);
			}
			if (_countryCode)
			{
				map.insert(PARAMETER_COUNTRY_CODE, *_countryCode);
			}

			// Importable
			_getImportableUpdateParametersMap(map);

			return map;
		}



		void TransportNetworkUpdateAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Network object
			if(map.getOptional<RegistryKeyType>(PARAMETER_NETWORK_ID)) try
			{
				_network = TransportNetworkTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), *_env);
			}
			catch(ObjectNotFoundException<TransportNetwork>&)
			{
				throw ActionException("No such network");
			}
			else
			{
				_network.reset(new TransportNetwork);
			}

			// Name
			if(map.isDefined(PARAMETER_NAME))
			{
				_name = map.get<string>(PARAMETER_NAME);
				if (_name->empty())
				{
					throw ActionException("Le nom du réseau ne peut pas être vide.");
				}

				TransportNetworkTableSync::SearchResult networks(
					TransportNetworkTableSync::Search(*_env, *_name, string())
				);
				BOOST_FOREACH(TransportNetworkTableSync::SearchResult::value_type network, networks)
				{
					if(network != _network)
					{
						throw ActionException("Un réseau nommé "+ *_name +" existe déjà.");
				}	}
			}

			// Image
			if (map.isDefined(PARAMETER_IMAGE))
			{
				_image = map.get<string>(PARAMETER_IMAGE);
			}

			// Data source links
			_setImportableUpdateFromParametersMap(*_env, map);

			// Days calendars parent
			if(map.isDefined(PARAMETER_DAYS_CALENDARS_PARENT_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_DAYS_CALENDARS_PARENT_ID));
				if(id > 0) try
				{
					_daysCalendarsParent = CalendarTemplateTableSync::GetEditable(
						id, *_env, UP_LINKS_LOAD_LEVEL
					);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw ActionException("No such days calendar id");
				}
			}

			// Periods calendars parent
			if(map.isDefined(PARAMETER_PERIODS_CALENDARS_PARENT_ID))
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_PERIODS_CALENDARS_PARENT_ID));
				if(id > 0) try
				{
					_periodsCalendarsParent = CalendarTemplateTableSync::GetEditable(
						id, *_env, UP_LINKS_LOAD_LEVEL
					);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw ActionException("No such periods calendar id");
				}
			}

			// Language
			if(map.isDefined(PARAMETER_LANG))
			{
				_lang = map.getValue(PARAMETER_LANG,false);
			}

			// Timezone
			if(map.isDefined(PARAMETER_TIMEZONE))
			{
				_timeZone = map.getValue(PARAMETER_TIMEZONE,false);
			}

			// Contact (Optional)
			if(map.isDefined(PARAMETER_CONTACT_ID))
			{
				RegistryKeyType id(0);
				std::string str = map.getValue(PARAMETER_CONTACT_ID, false);
				if ( ! str.empty() )
				{
					id = boost::lexical_cast<RegistryKeyType>(str);
				}

				if(id > 0) try
				{
					_contact = ReservationContactTableSync::GetEditable(
						id, *_env, UP_LINKS_LOAD_LEVEL
					);
				}
				catch(ObjectNotFoundException<ReservationContact>&)
				{
					throw ActionException("No such contact id");
				}
			}

			// Fare contact (Optional)
			if(map.isDefined(PARAMETER_FARE_CONTACT_ID))
			{
				RegistryKeyType id(0);
				std::string str = map.getValue(PARAMETER_FARE_CONTACT_ID, false);
				if ( ! str.empty() )
				{
					id = boost::lexical_cast<RegistryKeyType>(str);
				}

				if(id > 0) try
				{
					_fareContact = ReservationContactTableSync::GetEditable(
						id, *_env, UP_LINKS_LOAD_LEVEL
					);
				}
				catch(ObjectNotFoundException<ReservationContact>&)
				{
					throw ActionException("No such fare contact id");
				}
			}

			if (map.isDefined(PARAMETER_COUNTRY_CODE))
			{
				_countryCode = map.get<string>(PARAMETER_COUNTRY_CODE);
			}
		}



		void TransportNetworkUpdateAction::run(
			Request& request
		){
			// Name
			if(_name)
			{
				_network->set<Name>(*_name);
			}

			// Image
			if(_image)
			{
				_network->set<Image>(*_image);
			}

			// Data source links
			_doImportableUpdate(*_network, request);

			// Days calendars parent
			if(_daysCalendarsParent)
			{
				_network->set<DaysCalendarParent>(
					*(_daysCalendarsParent->get())
				);
			}

			// Periods calendars parent
			if(_periodsCalendarsParent)
			{
				_network->set<PeriodsCalendarParent>(
					*(_periodsCalendarsParent->get())
				);
			}

			// Language
			if (_lang)
			{
				_network->set<Lang>(*_lang);
			}

			// Timezone
			if (_timeZone)
			{
				_network->set<Timezone>(*_timeZone);
			}

			// Contact
			if (_contact)
			{
				_network->set<Contact>(
					*(_contact->get())
				);
			}

			// Fare contact
			if (_fareContact)
			{
				_network->set<FareContact>(
					*(_fareContact->get())
				);
			}

			// Country code
			if (_countryCode)
			{
				_network->set<CountryCode>(*_countryCode);
			}

			// Action
			TransportNetworkTableSync::Save(_network.get());
//			::AddCreationEntry(object, request.getUser().get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_network->getKey());
			}
		}



		bool TransportNetworkUpdateAction::isAuthorized(
			const Session* session
		) const {
			//TODO test if the user has sufficient right level for this transport network
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(WRITE, UNKNOWN_RIGHT_LEVEL, string());
		}
}	}
