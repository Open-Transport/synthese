
/** MailingList class implementation.
	@file MailingList.cpp

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

#include "MailingList.hpp"

#include "BroadcastPointAlarmRecipient.hpp"
#include "MailingListSubscription.hpp"
#include "ServerModule.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace messages;
	using namespace server;
	using namespace util;

	CLASS_DEFINITION(messages::MailingList, "t107_mailing_lists", 107)
	FIELD_DEFINITION_OF_OBJECT(messages::MailingList, "mailing_list_id", "mailing_list_ids")

	FIELD_DEFINITION_OF_TYPE(Sender, "sender", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(SenderName, "sender_name", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(EMailFormat, "format", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ManualSending, "manual_sending", SQL_BOOLEAN)

	namespace util
	{
		template<>
		const string FactorableTemplate<BroadcastPoint, MailingList>::FACTORY_KEY = "MailingList";
	}

	namespace messages
	{
		const string MailingList::TAG_SUBSCRIPTION = "subscription";



		MailingList::MailingList(
			RegistryKeyType id /*= 0*/
		):	Registrable(id),
			Object<MailingList, MailingListRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(Sender),
					FIELD_DEFAULT_CONSTRUCTOR(SenderName),
					FIELD_VALUE_CONSTRUCTOR(EMailFormat, EMail::EMAIL_TEXT),
					FIELD_DEFAULT_CONSTRUCTOR(MessageType),
					FIELD_VALUE_CONSTRUCTOR(ManualSending, false)
			)	)
		{}



		void MailingList::send(
			const string& subject,
			const string& content
		) const	{

			// Lock of the mutex
			mutex::scoped_lock(_subscriptionsMutex);

			// The e-mail
			EMail email(
				ServerModule::GetEMailSender()
			);
			email.setSubject(subject);
			email.setContent(content);
			email.setFormat(get<EMailFormat>());
			email.setSender(get<Sender>());
			email.setSenderName(get<SenderName>());

			// Loop on each recipient
			BOOST_FOREACH(const Subscriptions::value_type& subscription, _subscriptions)
			{
				// TODO check if the subscription parameters filters the mailing

				// Add the recipient
				email.addRecipient(subscription->getAddress());
			}

			// Send the e-mail to its recipients
			email.send();
		}



		void MailingList::addSubscription(
			const MailingListSubscription& subscription
		) const {

			// Lock of the mutex
			mutex::scoped_lock(_subscriptionsMutex);

			// Insertion
			_subscriptions.insert(&subscription);
		}



		void MailingList::removeSubscription(
			const MailingListSubscription& subscription
		) const {
			// Lock of the mutex
			mutex::scoped_lock(_subscriptionsMutex);

			// Insertion
			_subscriptions.erase(&subscription);
		}



		void MailingList::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix /*= std::string() */
		) const	{

			// Lock of the mutex
			mutex::scoped_lock(_subscriptionsMutex);

			// Loop on subscriptions
			BOOST_FOREACH(const Subscriptions::value_type& subscription, _subscriptions)
			{
				// New submap
				boost::shared_ptr<ParametersMap> subPM(new ParametersMap);

				// Populate the submap
				subscription->toParametersMap(*subPM);

				// Store the submap
				map.insert(TAG_SUBSCRIPTION, subPM);
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Check which message type should be used in the mailing list
		MessageType* MailingList::getMessageType() const
		{
			return get<MessageType>() ? &*get<MessageType>() : NULL;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Check of a message should be sent to the mailing list according to its recipients.
		/// A mailing list can only be sent if it is explicitly linked to the message.
		/// @param recipients the recipients list to check
		/// @param parameters the broadcast parameters
		bool MailingList::displaysMessage(
			const Alarm::LinkedObjects& recipients,
			const util::ParametersMap& parameters
		) const	{

			// in broad cast points recipients
			Alarm::LinkedObjects::const_iterator it(
				recipients.find(
					BroadcastPointAlarmRecipient::FACTORY_KEY
			)	);

			// No broadcast recipient = no display
			if(it == recipients.end())
			{
				return false;
			}

			// Loop on each recipient
			BOOST_FOREACH(
				const Alarm::LinkedObjects::mapped_type::value_type& link,
				it->second
			){
				// Search for explicitly specified broadcast
				if(	link->getObjectId() == getKey())
				{
					return true;
				}
			}

			// The mailing list was not found
			return false;
		}



		//////////////////////////////////////////////////////////////////////////
		/// Builds the list of the broadcast points for displaying in admin pages.
		/// @retval result the list to populate
		void MailingList::getBroadcastPoints( BroadcastPoints& result ) const
		{
			BOOST_FOREACH(
				const MailingList::Registry::value_type& it,
				Env::GetOfficialEnv().getRegistry<MailingList>()
			){
				result.push_back(it.second.get());
			}
		}
}	}

