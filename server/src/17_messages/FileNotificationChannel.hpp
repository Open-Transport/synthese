/** FileNotificationChannel class header
	@file FileNotificationChannel.hpp
	@author Yves Martin
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
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
#ifndef SYNTHESE_messages_FileNotificationChannel_hpp__
#define SYNTHESE_messages_FileNotificationChannel_hpp__

#include <NotificationChannel.hpp>
#include <FactorableTemplate.h>
#include <ParametersMap.h>

#include <string>
#include <vector>

namespace synthese
{
	namespace messages {
		class NotificationEvent;	// Forward declaration

		class FileNotificationChannel:
			public util::FactorableTemplate<NotificationChannel,FileNotificationChannel>
		{
		public:
			/// Parameter name for CMS field with file path
			static const std::string PARAMETER_FILE_PATH;

			/// Parameter name for CMS field with file content
			static const std::string PARAMETER_FILE_CONTENT;

		public:
			virtual ~FileNotificationChannel() { };

			virtual std::vector<std::string> _getScriptParameterNames() const;

			virtual bool notifyEvent(const boost::shared_ptr<NotificationEvent> event);
		};
	}
}

#endif /* SYNTHESE_messages_FileNotificationChannel_hpp__ */
