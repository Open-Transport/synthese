/** HttpNotificationChannel implementation
	@Http HttpNotificationChannel.cpp
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

#include <FactorableTemplate.h>
#include <HttpNotificationChannel.hpp>
#include <ParametersMap.h>

#include <string>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace messages;
	using namespace server;

	namespace util
	{
		template<> const std::string FactorableTemplate<NotificationChannel, HttpNotificationChannel>::FACTORY_KEY("HTTP");
	}

	namespace messages
	{
		const std::string HttpNotificationChannel::PARAMETER_METHOD = "http_method";
		const std::string HttpNotificationChannel::PARAMETER_URL = "http_url";
		const std::string HttpNotificationChannel::PARAMETER_BODY = "http_body";
		const std::string HttpNotificationChannel::PARAMETER_CONNECTION_TIMEOUT = "http_connection_timeout";
		const std::string HttpNotificationChannel::PARAMETER_READ_TIMEOUT = "http_read_timeout";



		// Provide its own script fields list
		std::vector<std::string> HttpNotificationChannel::_getScriptParameterNames() const
		{
			vector<string> result;
			result.push_back(PARAMETER_URL);
			result.push_back(PARAMETER_BODY);
			return result;
		}



		bool HttpNotificationChannel::notifyEvent(const boost::shared_ptr<NotificationEvent> event)
		{
			const NotificationProvider* provider = &(*(event->get<NotificationProvider>()));
			const Alarm* alarm = &(*(event->get<Alarm>()));

			// Create HTTP URL and body from parameters
			ParametersMap fields = generateScriptFields(provider, alarm, event->get<EventType>());

			string httpMethod = provider->get<Parameters>()
					.getDefault<string>(PARAMETER_METHOD, "GET");

			if (!fields.isDefined(PARAMETER_URL)
				|| (httpMethod == "POST" && !fields.isDefined(PARAMETER_BODY)))
			{
				return true;	// Explicitly nothing to notify
			}

			string httpUrl = fields.get<string>(PARAMETER_URL);
			string httpBody = fields.get<string>(PARAMETER_BODY);
			if (httpUrl.empty()
				|| (httpMethod == "POST" && httpBody.empty()))
			{
				return true;	// Explicitly nothing to notify
			}

			boost::optional<int> connectionTimeout;
			boost::optional<int> readTimeout;

			BasicClient::Uri uri = BasicClient::Uri::parseUri(httpUrl);

			// Send request with read timeout
			BasicClient httpClient(
				uri.host,
				uri.port,
				connectionTimeout,
				readTimeout
			);
			// TODO performance improvement: reuse BasicClient if
			// constructor parameters have not changed

			if (httpMethod == "GET")
			{
				httpClient.get(uri.path);
			}
			else if (httpMethod == "POST")
			{
				httpClient.post(uri.path, httpBody);
			}
			else
			{
				throw Exception("Unsupported HTTP method" + httpMethod);
			}

			return true;
		}

	}
}
