
/** AlarmObjectLink class implementation.
	@file AlarmObjectLink.cpp

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

#include "AlarmObjectLink.h"

#include "Alarm.h"
#include "AlarmRecipient.h"
#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "Factory.h"
#include "Registry.h"
#include "SentAlarm.h"

#include <boost/foreach.hpp>

using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const std::string Registry<messages::AlarmObjectLink>::KEY("AlarmObjectLink");
	}

	namespace messages
	{
		AlarmObjectLink::AlarmObjectLink(
			RegistryKeyType key
		):	Registrable(key),
			_objectId(0),
			_alarm(NULL)
		{}



		void AlarmObjectLink::setRecipient( const std::string& key )
		{
			_recipient.reset(
				Factory<AlarmRecipient>::create(key)
			);
		}

		void AlarmObjectLink::toParametersMap( util::ParametersMap& pm, bool withScenario, boost::logic::tribool withRecipients /*= false */, std::string prefix /*= std::string()*/ ) const
		{
			// Inter synthese package
			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + AlarmObjectLinkTableSync::COL_RECIPIENT_KEY,
				getRecipient()->getFactoryKey()
			);
			pm.insert(
				prefix + AlarmObjectLinkTableSync::COL_OBJECT_ID,
				getObjectId()
			);
			pm.insert(
				prefix + AlarmObjectLinkTableSync::COL_ALARM_ID,
				getAlarm() ? getAlarm()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				prefix + AlarmObjectLinkTableSync::COL_PARAMETER,
				getParameter()
			);
		}

		bool AlarmObjectLink::loadFromRecord(
			const Record& record,
			util::Env& env
		){
			bool result(false);

			// Recipient
			if(record.isDefined(AlarmObjectLinkTableSync::COL_RECIPIENT_KEY))
			{
				string value(
					record.get<string>(AlarmObjectLinkTableSync::COL_RECIPIENT_KEY)
				);
				if((getRecipient() && value != getRecipient()->getFactoryKey()) ||
					(!getRecipient() && !value.empty()))
				{
					setRecipient(value);
					result = true;
				}
			}

			// Parameter
			if(record.isDefined(AlarmObjectLinkTableSync::COL_PARAMETER))
			{
				string value(
					record.get<string>(AlarmObjectLinkTableSync::COL_PARAMETER)
				);
				if(value != getParameter())
				{
					setParameter(value);
					result = true;
				}
			}

			// Object id
			if(record.isDefined(AlarmObjectLinkTableSync::COL_OBJECT_ID))
			{
				RegistryKeyType value(
					record.getDefault<RegistryKeyType>(
						AlarmObjectLinkTableSync::COL_OBJECT_ID,
						0
				)	);
				if(value != getObjectId())
				{
					setObjectId(value);
					result = true;
				}
			}

			// Alarm
			if(record.isDefined(AlarmObjectLinkTableSync::COL_ALARM_ID))
			{
				Alarm* value(NULL);
				try
				{
					RegistryKeyType alarmId(
						record.getDefault<RegistryKeyType>(
							AlarmObjectLinkTableSync::COL_ALARM_ID,
							0
					)	);
					if(decodeTableId(alarmId) == AlarmTableSync::TABLE.ID)
					{
						value = AlarmTableSync::GetEditable(alarmId, env).get();
					}
				}
				catch(ObjectNotFoundException<Alarm>&)
				{
					Log::GetInstance().warn("No such alarm in alarm object link "+ lexical_cast<string>(getKey()));
				}

				if(value != getAlarm())
				{
					if(value)
					{
						setAlarm(value);
						getAlarm()->addLinkedObject(*this);
						SentAlarm* sentAlarm(
							dynamic_cast<SentAlarm*>(
								value
						)	);
						if(sentAlarm)
						{
							sentAlarm->clearBroadcastPointsCache();
						}
					}
					else
					{
						setAlarm(NULL);
					}
					result = true;
				}
			}

			return result;
		}
}	}

/** @class AlarmObjectLink
	@ingroup m17

	Link between an alarm and an object.
*/
