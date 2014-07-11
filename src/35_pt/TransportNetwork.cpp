
/** TransportNetwork class implementation.
	@file TransportNetwork.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "TransportNetwork.h"

#include "CalendarTemplateTableSync.h"
#include "CommercialLineTableSync.h"
#include "DataSourceLinksField.hpp"
#include "DBConstants.h"
#include "ImportableTableSync.hpp"
#include "ParametersMap.h"
#include "TransportNetworkTableSync.h"
#include "TreeFolder.hpp"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"

using namespace std;

namespace synthese
{
	using namespace calendar;
	using namespace db;
	using namespace tree;
	using namespace util;
	using namespace graph;
	using namespace impex;

	namespace util
	{
		template<> const std::string Registry<pt::TransportNetwork>::KEY("TransportNetwork");
	}

	namespace pt
	{
		const string TransportNetwork::DATA_NETWORK_ID("network_id");
		const string TransportNetwork::DATA_NAME("name");



		TransportNetwork::TransportNetwork(
			util::RegistryKeyType id,
			std::string name
		):	util::Registrable(id),
			graph::PathClass(),
			_daysCalendarsParent(NULL),
			_periodsCalendarsParent(NULL),
			_timezone(""),
			_lang(""),
			_contact(NULL),
			_fareContact(NULL)
		{}



		TransportNetwork::~TransportNetwork()
		{}



        PathClass::Identifier TransportNetwork::getIdentifier() const
        {
			return getKey();
        }



		void TransportNetwork::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{

			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(prefix + TransportNetworkTableSync::COL_NAME, getName());
			pm.insert(
				prefix + TransportNetworkTableSync::COL_CREATOR_ID, 
				synthese::DataSourceLinks::Serialize(getDataSourceLinks())
			);
			pm.insert(
				prefix + TransportNetworkTableSync::COL_DAYS_CALENDARS_PARENT_ID,
				getDaysCalendarsParent() ? getDaysCalendarsParent()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + TransportNetworkTableSync::COL_PERIODS_CALENDARS_PARENT_ID,
				getPeriodsCalendarsParent() ? getPeriodsCalendarsParent()->getKey() : RegistryKeyType(0)
			);
			pm.insert(prefix + TransportNetworkTableSync::COL_TIMEZONE, getTimezone());
			pm.insert(prefix + TransportNetworkTableSync::COL_LANG, getLang());
			pm.insert(
				prefix + TransportNetworkTableSync::COL_CONTACT_ID,
				getContact() ? getContact()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + TransportNetworkTableSync::COL_FARE_CONTACT_ID,
				getFareContact() ? getFareContact()->getKey() : RegistryKeyType(0)
			);
			pm.insert(prefix + DATA_NETWORK_ID, getKey());
			pm.insert(prefix + DATA_NAME, getName());
		}


		
		bool TransportNetwork::loadFromRecord(
			const Record& record,
			util::Env& env
		){
			bool result(false);

			// Name
			if(record.isDefined(TransportNetworkTableSync::COL_NAME))
			{
				std::string name(
					record.get<string>(TransportNetworkTableSync::COL_NAME)
				);
				if(name != _name)
				{
					result = true;
					_name = name;
				}
			}

			// Data source links
			if(record.isDefined(TransportNetworkTableSync::COL_CREATOR_ID))
			{
				std::string creatorId(
					record.get<string>(TransportNetworkTableSync::COL_CREATOR_ID)
				);
				DataSourceLinks links(
					ImportableTableSync::GetDataSourceLinksFromSerializedString(creatorId, env)
				);
				if(getDataSourceLinks() != links)
				{
					setDataSourceLinksWithoutRegistration(links);
					result = true;
				}
			}

			// Days calendars parent
			if(record.isDefined(TransportNetworkTableSync::COL_DAYS_CALENDARS_PARENT_ID))
			{
				CalendarTemplate* value(NULL);
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						TransportNetworkTableSync::COL_DAYS_CALENDARS_PARENT_ID,
						0
				)	);
				if(id > 0) try
				{
					value = CalendarTemplateTableSync::GetEditable(
						id, env
					).get();
				}
				catch(ObjectNotFoundException<CalendarTemplate>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TransportNetworkTableSync::TABLE.NAME + "/" + TransportNetworkTableSync::COL_DAYS_CALENDARS_PARENT_ID, e);
				}

				if(value != _daysCalendarsParent)
				{
					_daysCalendarsParent = value;
					result = true;
				}
			}

			// Periods calendars parent
			if(record.isDefined(TransportNetworkTableSync::COL_PERIODS_CALENDARS_PARENT_ID))
			{
				CalendarTemplate* value(NULL);

				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						TransportNetworkTableSync::COL_PERIODS_CALENDARS_PARENT_ID,
						0
				)	);
				if(id > 0) try
				{
					value = CalendarTemplateTableSync::GetEditable(
						id, env
					).get();
				}
				catch(ObjectNotFoundException<CalendarTemplate>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TransportNetworkTableSync::TABLE.NAME + "/" + TransportNetworkTableSync::COL_PERIODS_CALENDARS_PARENT_ID, e);
				}

				if(value != _periodsCalendarsParent)
				{
					_periodsCalendarsParent = value;
					result = true;
				}
			}

			// Timezone
			if(record.isDefined(TransportNetworkTableSync::COL_TIMEZONE))
			{
				std::string timezone(
					record.get<string>(TransportNetworkTableSync::COL_TIMEZONE)
				);
				if(timezone != _timezone)
				{
					result = true;
					_timezone = timezone;
				}
			}

			// Lang
			if(record.isDefined(TransportNetworkTableSync::COL_LANG))
			{
				std::string lang(
					record.get<string>(TransportNetworkTableSync::COL_LANG)
				);
				if(lang != _lang)
				{
					result = true;
					_lang = lang;
				}
			}

			// Contact
			if(record.isDefined(TransportNetworkTableSync::COL_CONTACT_ID))
			{
				ReservationContact* value(NULL);

				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						TransportNetworkTableSync::COL_CONTACT_ID,
						0
				)	);
				if(id > 0) try
				{
					value = ReservationContactTableSync::GetEditable(
						id, env
					).get();
				}
				catch(ObjectNotFoundException<ReservationContact>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TransportNetworkTableSync::TABLE.NAME + "/" + TransportNetworkTableSync::COL_CONTACT_ID, e);
				}

				if(value != _contact)
				{
					_contact = value;
					result = true;
				}
			}

			// Fare contact
			if(record.isDefined(TransportNetworkTableSync::COL_FARE_CONTACT_ID))
			{
				ReservationContact* value(NULL);

				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						TransportNetworkTableSync::COL_FARE_CONTACT_ID,
						0
				)	);
				if(id > 0) try
				{
					value = ReservationContactTableSync::GetEditable(
						id, env
					).get();
				}
				catch(ObjectNotFoundException<ReservationContact>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + TransportNetworkTableSync::TABLE.NAME + "/" + TransportNetworkTableSync::COL_FARE_CONTACT_ID, e);
				}

				if(value != _fareContact)
				{
					_fareContact = value;
					result = true;
				}
			}

			return result;
		}



		synthese::LinkedObjectsIds TransportNetwork::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;
			{
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						TransportNetworkTableSync::COL_DAYS_CALENDARS_PARENT_ID,
						0
				)	);
				if(id) result.push_back(id);
			}
			{
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						TransportNetworkTableSync::COL_PERIODS_CALENDARS_PARENT_ID,
						0
				)	);
				if(id) result.push_back(id);
			}
			{
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						TransportNetworkTableSync::COL_CONTACT_ID,
						0
				)	);
				if(id) result.push_back(id);
			}
			{
				RegistryKeyType id(
					record.getDefault<RegistryKeyType>(
						TransportNetworkTableSync::COL_FARE_CONTACT_ID,
						0
				)	);
				if(id) result.push_back(id);
			}
			return result;
		}

		void TransportNetwork::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}
}	}
