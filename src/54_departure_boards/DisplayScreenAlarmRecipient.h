
/** DisplayScreenAlarmRecipient class header.
	@file DisplayScreenAlarmRecipient.h

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

#ifndef SYNTHESE_DisplayScreenAlarmRecipient_h__
#define SYNTHESE_DisplayScreenAlarmRecipient_h__

#include "DisplayScreenTableSync.h"
#include "AlarmRecipientTemplate.h"

namespace synthese
{
	namespace departure_boards
	{
		//////////////////////////////////////////////////////////////////////////
		/// 54.17 Alarm recipient : display screen.
		///	@ingroup m54Messages refAlarmrecipients
		/// @author Hugues Romain
		class DisplayScreenAlarmRecipient:
			public messages::AlarmRecipientTemplate<DisplayScreenTableSync, DisplayScreenAlarmRecipient>
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

			virtual void addObject(const messages::SentAlarm* alarm, util::RegistryKeyType objectId);
			virtual void removeObject(const messages::SentAlarm* alarm, util::RegistryKeyType objectId);

			virtual boost::shared_ptr<security::Right> getRight(const std::string& perimeter) const;
		};
	}
}

#endif // SYNTHESE_DisplayScreenAlarmRecipient_h__
