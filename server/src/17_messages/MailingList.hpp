
/** MailingList class header.
	@file MailingList.hpp

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

#ifndef SYNTHESE_pt_website_MailingList_hpp__
#define SYNTHESE_pt_website_MailingList_hpp__

#include "BroadcastPoint.hpp"
#include "FactorableTemplate.h"
#include "Object.hpp"

#include "EMail.h"
#include "EnumObjectField.hpp"
#include "MessageType.hpp"
#include "NumericField.hpp"
#include "SchemaMacros.hpp"
#include "StringField.hpp"

namespace synthese
{
	FIELD_STRING(Sender)
	FIELD_STRING(SenderName)
	FIELD_ENUM(EMailFormat, util::EMail::Format)
	FIELD_BOOL(ManualSending)

	namespace messages
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Sender),
			FIELD(SenderName),
			FIELD(EMailFormat),
			FIELD(messages::MessageType),
			FIELD(ManualSending)
		> MailingListRecord;

		class MailingListSubscription;

		/** MailingList class.
			@ingroup m57
		*/
		class MailingList:
			public Object<MailingList, MailingListRecord>,
			public util::FactorableTemplate<messages::BroadcastPoint, MailingList>
		{
		private:
			static const std::string TAG_SUBSCRIPTION;

			typedef std::set<const MailingListSubscription*> Subscriptions;

			mutable Subscriptions _subscriptions;
			mutable boost::mutex _subscriptionsMutex;

		public:
			MailingList(util::RegistryKeyType id=0);

			void send(
				const std::string& subject,
				const std::string& content
			) const;

			void addSubscription(const MailingListSubscription& subscription) const;
			void removeSubscription(const MailingListSubscription& subscription) const;

			virtual void addAdditionalParameters(
				util::ParametersMap& map,
				std::string prefix = std::string()
			) const;

			/// @name BroadcastPoint virtual methods
			//@{
				virtual messages::MessageType* getMessageType() const;

				virtual bool displaysMessage(
					const messages::Alarm::LinkedObjects& recipients,
					const util::ParametersMap& parameters
				) const;

				virtual void getBroadcastPoints(BroadcastPoints& result) const;
			//@}
		};
	}
}

#endif // SYNTHESE_pt_website_MailingList_hpp__

