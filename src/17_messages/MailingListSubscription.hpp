
/** MailingListSubscription class header.
	@file MailingListSubscription.hpp

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

#ifndef SYNTHESE_pt_website_MailingListSubscription_hpp__
#define SYNTHESE_pt_website_MailingListSubscription_hpp__

#include "Object.hpp"

#include "MailingList.hpp"
#include "NumericField.hpp"
#include "ParametersMapField.hpp"
#include "SchemaMacros.hpp"
#include "StringField.hpp"

namespace synthese
{
	FIELD_STRING(EMailAddress)

	namespace messages
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(MailingList),
			FIELD(EMailAddress), //TODO add user id as an alternative to email
			FIELD(Parameters)
		> MailingListSubscriptionRecord;



		/** MailingListSubscription class.
			@ingroup m56
		*/
		class MailingListSubscription:
			public Object<MailingListSubscription, MailingListSubscriptionRecord>
		{
		public:
			MailingListSubscription(util::RegistryKeyType id=0);

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}

			const std::string& getAddress() const;
		};
	}
}

#endif // SYNTHESE_pt_website_MailingListSubscription_hpp__

