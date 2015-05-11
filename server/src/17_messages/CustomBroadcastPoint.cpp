
/** CustomBroadcastPoint class implementation.
	@file CustomBroadcastPoint.cpp

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

#include "CustomBroadcastPoint.hpp"

#include "Alarm.h"
#include "BroadcastPointAlarmRecipient.hpp"
#include "MessagesModule.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace messages;
	using namespace util;

	CLASS_DEFINITION(CustomBroadcastPoint, "t106_custom_broadcast_points", 106)
	FIELD_DEFINITION_OF_OBJECT(CustomBroadcastPoint, "custom_broadcast_point_id", "custom_broadcast_point_ids")

	FIELD_DEFINITION_OF_TYPE(BroadcastRule, "broadcast_rule", SQL_TEXT)

	template<> const Field ComplexObjectFieldDefinition<CustomBroadcastPointTreeNode>::FIELDS[] = {
		Field("root_id", SQL_INTEGER),
		Field("up_id", SQL_INTEGER),
		Field("rank", SQL_INTEGER),
	Field() };

	namespace util
	{
		template<>
		const string FactorableTemplate<BroadcastPoint, CustomBroadcastPoint>::FACTORY_KEY = "CustomBroadcastPoint";
	}

	namespace messages
	{
		CustomBroadcastPoint::CustomBroadcastPoint(
			util::RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<CustomBroadcastPoint, CustomBroadcastPointRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(CustomBroadcastPointTreeNode),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(MessageType),
					FIELD_DEFAULT_CONSTRUCTOR(BroadcastRule)
			)	)
		{
		}



		MessageType* CustomBroadcastPoint::getMessageType() const
		{
			return
				get<MessageType>() ?
				&*get<MessageType>() :
				NULL
			;
		}



		bool CustomBroadcastPoint::displaysMessage(
			const Alarm::LinkedObjects& recipients,
			const util::ParametersMap& parameters
		) const	{

			//////////////////////////////////////////////////////////////////////////
			// Check if the broadcast point or one of its parents is in the recipients list

			// Check if the message has at least a broadcast point linked
			Alarm::LinkedObjects::const_iterator it(
				recipients.find(BroadcastPointAlarmRecipient::FACTORY_KEY)
			);
			if(it == recipients.end())
			{ // No broadcast point is linked
				return false;
			}

			// Search in the linked broadcast points the current broadcast point or one of its parents
			bool ok(false);
			for(const CustomBroadcastPoint* cbp(this); cbp != NULL; cbp = cbp->getParent())
			{
				BOOST_FOREACH(const AlarmObjectLink* link, it->second)
				{
					if(link->getObjectId() == cbp->getKey())
					{
						ok = true;
						break;
					}
				}
			}
			BOOST_FOREACH(const AlarmObjectLink* link, it->second)
			{
				// Search for general broadcast on all custombroadcastpoints
				// OR on all kind of displayscreen recipients
				if (link->getObjectId() == CLASS_NUMBER || link->getObjectId() == 0)
				{
					ok = true;
					break;
				}
			}

			if(!ok)
			{
				return false;
			}

			//////////////////////////////////////////////////////////////////////////
			// Now check the parameters according to the custom display rule defined in the object

			// Populates the parameters map
			ParametersMap pm(parameters);
			Alarm::LinkedObjectsToParametersMap(recipients, pm);

			// If the rule has no code, then the message is always displayed
			if(get<BroadcastRule>().empty())
			{
				return true;
			}

			// Evaluation of the customized rule
			string s(get<BroadcastRule>().eval(pm));
			trim(s);

			// Result must be positive
			return !s.empty() && s != "0";
		}



		void CustomBroadcastPoint::getBroadcastPoints( BroadcastPoints& result ) const
		{
			BOOST_FOREACH(
				const CustomBroadcastPoint::Registry::value_type& it,
				Env::GetOfficialEnv().getRegistry<CustomBroadcastPoint>()
			){
				result.push_back(it.second.get());
			}
		}



		void CustomBroadcastPoint::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			MessagesModule::ClearAllBroadcastCaches();
		}



		void CustomBroadcastPoint::unlink()
		{
			MessagesModule::ClearAllBroadcastCaches();
		}
}	}

