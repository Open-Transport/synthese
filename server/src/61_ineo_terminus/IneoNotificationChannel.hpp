/** IneoNotificationChannel class header
	@Ineo IneoNotificationChannel.hpp
	@author Camille Hue
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
#ifndef SYNTHESE_ineo_terminus_IneoNotificationChannel_hpp__
#define SYNTHESE_ineo_terminus_IneoNotificationChannel_hpp__

#include <FactorableTemplate.h>
#include <NotificationChannel.hpp>
#include <NotificationEvent.hpp>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <string>
#include <vector>

namespace synthese
{
	namespace ineo_terminus {
		class IneoNotificationChannel:
				public util::FactorableTemplate<messages::NotificationChannel,IneoNotificationChannel>
		{
		private:

		public:
			static const std::string PARAMETER_CMS_INTERPRETER;
			static const std::string PARAMETER_INEO_MESSAGE_TYPE;
			/// Parameters needs_
			static const std::string PARAMETER_NEEDS_REPEAT_INTERVAL;
			static const std::string PARAMETER_NEEDS_WITH_ACK;
			static const std::string PARAMETER_NEEDS_MULTIPLE_STOPS;
			static const std::string PARAMETER_NEEDS_PLAY_TTS;
			static const std::string PARAMETER_NEEDS_LIGHT;
			static const std::string PARAMETER_NEEDS_DIRECTION_SIGN_CODE;
			static const std::string PARAMETER_NEEDS_START_STOP_POINT;
			static const std::string PARAMETER_NEEDS_END_STOP_POINT;


			virtual ~IneoNotificationChannel() { };

			virtual std::vector<std::string> _getScriptParameterNames() const;

			virtual bool notifyEvent(const boost::shared_ptr<messages::NotificationEvent> event);
		};
	}
}

#endif /* SYNTHESE_ineo_terminus_IneoNotificationChannel_hpp__ */
