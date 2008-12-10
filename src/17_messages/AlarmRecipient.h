
/** AlarmRecipient class header.
	@file AlarmRecipient.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_AlarmRecipient_h__
#define SYNTHESE_AlarmRecipient_h__

#include <ostream>

#include "FactoryBase.h"
#include "12_security/Types.h"
#include "17_messages/Types.h"
#include "ActionFunctionRequest.h"

namespace synthese
{
	namespace html
	{
		class HTMLForm;
	}

	namespace admin
	{
		class AdminRequest;
	}

	namespace messages
	{
		class Alarm;
		class SentAlarm;
		class AlarmAddLinkAction;
		class AlarmRemoveLinkAction;

		/** Alarm recipient class.
			@ingroup m17

			All the available alarm recipients are listed in the @ref refAlarmrecipients page.
		*/
		class AlarmRecipient
		:	public util::FactoryBase<AlarmRecipient>
		{
		public:
			/// Optional arguments for factory.
			struct Args
			{

			};

		public:
			virtual const std::string& getTitle() const = 0;

			virtual void displayBroadcastListEditor(
				std::ostream& stream
				, const messages::Alarm* alarm
				, const server::ParametersMap& request
				, server::FunctionRequest<admin::AdminRequest>& searchRequest
				, server::ActionFunctionRequest<messages::AlarmAddLinkAction,admin::AdminRequest>& addRequest
				, server::ActionFunctionRequest<messages::AlarmRemoveLinkAction, admin::AdminRequest>& removeRequest
				) = 0;

			virtual AlarmRecipientSearchFieldsMap getSearchFields(html::HTMLForm& form, const server::ParametersMap& parameters) const = 0;

			virtual void addObject(const SentAlarm* alarm, uid objectId) = 0;
			virtual void removeObject(const SentAlarm* alarm, uid objectId) = 0;
			virtual AlarmConflict getConflictStatus(const SentAlarm* alarm) const = 0;
			virtual void getStaticParametersLabelsVirtual(security::ParameterLabelsVector& m) = 0;

		};
	}
}

/** @defgroup refAlarmrecipients Alarm recipients.
	@ingroup ref
	@copydoc AlarmRecipient
*/

#endif // SYNTHESE_AlarmRecipient_h__
