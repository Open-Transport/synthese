/** HttpNotificationChannel class header
	@Http HttpNotificationChannel.hpp
	@author Yves Martin
	@date 2015

	This Http belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
#ifndef SYNTHESE_messages_HttpNotificationChannel_hpp__
#define SYNTHESE_messages_HttpNotificationChannel_hpp__

#include <BasicClient.h>
#include <FactorableTemplate.h>
#include <NotificationChannel.hpp>
#include <NotificationEvent.hpp>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <string>
#include <vector>

namespace synthese
{
	namespace messages {
		class NotificationEvent;	// Forward declaration

		class HttpNotificationChannel:
				public util::FactorableTemplate<NotificationChannel,HttpNotificationChannel>
		{
		private:
			// boost::shared_ptr<server::BasicClient> _httpClient;

		public:
			/// Parameter name for method GET or POST
			static const std::string PARAMETER_METHOD;

			/// Parameter name for CMS field with URL
			static const std::string PARAMETER_URL;

			/// Parameter name for CMS field with body
			static const std::string PARAMETER_BODY;

			/// Parameter name for tcp connection timeout
			static const std::string PARAMETER_CONNECTION_TIMEOUT;

			/// Parameter name for answer's read timeout
			static const std::string PARAMETER_READ_TIMEOUT;

			virtual ~HttpNotificationChannel() { };

			virtual std::vector<std::string> _getScriptParameterNames() const;

			virtual bool notifyEvent(const boost::shared_ptr<NotificationEvent> event);
		};
	}
}

#endif /* SYNTHESE_messages_HttpNotificationChannel_hpp__ */
