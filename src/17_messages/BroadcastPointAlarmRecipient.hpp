
/** BroadcastPointAlarmRecipient class header.
	@file BroadcastPointAlarmRecipient.h

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

#ifndef SYNTHESE_BroadcastPointAlarmRecipient_h__
#define SYNTHESE_BroadcastPointAlarmRecipient_h__

#include "AlarmRecipientTemplate.h"

namespace synthese
{
	namespace messages
	{
		class CustomBroadcastPoint;

		//////////////////////////////////////////////////////////////////////////
		/// 54.17 Alarm recipient : display screen.
		///	@ingroup m54Messages refAlarmrecipients
		/// @author Hugues Romain
		class BroadcastPointAlarmRecipient:
			public AlarmRecipientTemplate<BroadcastPointAlarmRecipient>
		{
		public:
			static const std::string PARAMETER_SEARCH_CITY_NAME;
			static const std::string PARAMETER_SEARCH_STOP_NAME;
			static const std::string PARAMETER_SEARCH_NAME;
			static const std::string PARAMETER_SEARCH_LINE;
			static const std::string PARAMETER_SEARCH_TYPE;
			static const std::string PARAMETER_SEARCH_STATUS;
			static const std::string PARAMETER_SEARCH_MESSAGE;

			void displayBroadcastListEditor(
				std::ostream& stream
				, const messages::Alarm* alarm
				, const util::ParametersMap& request
				, server::Request& searchRequest
				, server::StaticActionRequest<messages::AlarmAddLinkAction>& addRequest
				, server::StaticActionRequest<messages::AlarmRemoveLinkAction>& removeRequest
			);

			virtual messages::AlarmRecipientSearchFieldsMap getSearchFields(html::HTMLForm& form, const util::ParametersMap& parameters) const;

			virtual boost::shared_ptr<security::Right> getRight(const std::string& perimeter) const;
		};
}	}

#endif // SYNTHESE_BroadcastPointAlarmRecipient_h__
