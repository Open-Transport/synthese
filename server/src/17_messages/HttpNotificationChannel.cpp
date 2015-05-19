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

namespace synthese
{
	using namespace util;
	using namespace messages;

	namespace util
	{
		template<> const std::string FactorableTemplate<NotificationChannel, HttpNotificationChannel>::FACTORY_KEY("HTTP");
	}

	namespace messages
	{
		HttpNotificationChannel::HttpNotificationChannel()
		{
			// Prepare internal state for notifications
		};



		bool HttpNotificationChannel::notify(const boost::shared_ptr<NotificationEvent> event)
		{
			// TODO notify
			return true;
		}

		// Use curl to establish a connection with connection timeout
		// Create HTTP URL and body from parameters
		// Send request with read timeout
		// Intercept exception and ask for retry

	}
}
