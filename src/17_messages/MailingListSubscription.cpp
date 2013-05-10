
/** MailingListSubscription class implementation.
	@file MailingListSubscription.cpp

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

#include "MailingListSubscription.hpp"

namespace synthese
{
	CLASS_DEFINITION(messages::MailingListSubscription, "t108_mailing_list_subscriptions", 108)
	FIELD_DEFINITION_OF_OBJECT(messages::MailingListSubscription, "mailing_list_subscription_id", "mailing_list_subscription_ids")

	FIELD_DEFINITION_OF_TYPE(EMailAddress, "email", SQL_TEXT)

	namespace messages
	{
		MailingListSubscription::MailingListSubscription(
			util::RegistryKeyType id/*=0*/
		):	util::Registrable(id),
			Object<MailingListSubscription, MailingListSubscriptionRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(MailingList),
					FIELD_DEFAULT_CONSTRUCTOR(EMailAddress),
					FIELD_DEFAULT_CONSTRUCTOR(Parameters)
			)	)
		{}



		//////////////////////////////////////////////////////////////////////////
		/// Gets the address where the messages must be sent
		/// @todo : read object id when it will be available
		const std::string& MailingListSubscription::getAddress() const
		{
			return get<EMailAddress>();
		}



		void MailingListSubscription::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			// Update of the cache of the mailing list
			if(get<MailingList>())
			{
				get<MailingList>()->addSubscription(*this);
			}
		}



		void MailingListSubscription::unlink()
		{
			// Update of the cache of the mailing list
			if(get<MailingList>())
			{
				get<MailingList>()->removeSubscription(*this);
			}
		}
}	}

