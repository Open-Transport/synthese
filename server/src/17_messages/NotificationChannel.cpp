/** NotificationChannel abstract implementation
	@file NotificationChannel.cpp
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

#include <AlarmObjectLink.h>
#include <CMSScript.hpp>
#include <CommercialLine.h>
#include <MessageAlternative.hpp>
#include <NotificationChannel.hpp>
#include <NotificationEvent.hpp>
#include <ParametersMap.h>
#include <StopArea.hpp>

#include <string>
#include <vector>

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace messages;

	namespace messages
	{
		const string NotificationChannel::ATTR_KEY = "key";

		const string NotificationChannel::VARIABLE_SHORT_MESSAGE = "short_message";
		const string NotificationChannel::VARIABLE_MESSAGE = "message";
		const string NotificationChannel::VARIABLE_STOP_IDS = "stop_ids";
		const string NotificationChannel::VARIABLE_LINE_IDS = "line_ids";
		const string NotificationChannel::VARIABLE_ID_SEPARATOR = ",";

		//////////////////////////////////////////////////////////////////////////
		/// Exports the notification channel into a parameters map.
		/// @param pm the parameters map to populate
		void NotificationChannel::toParametersMap(
			util::ParametersMap& pm
		) const {
			pm.insert(ATTR_KEY, getFactoryKey());
		}



		// Default implementation for channel implementation with no script field
		std::vector<std::string> NotificationChannel::_getScriptParameterNames() const
		{
			vector<string> empty;
			return empty;
		}



		void NotificationChannel::_setMessageAlternative(
			ParametersMap& scriptParameters,
			const boost::shared_ptr<Alarm> alarm,
			boost::optional<MessageType&> type
		) const {
			Alarm::MessageAlternatives::const_iterator it(
				alarm->getMessageAlternatives().find(&(*type))
			);
			if(it == alarm->getMessageAlternatives().end())
			{
				// TODO log: lacking message alternative for channel
				scriptParameters.insert(VARIABLE_MESSAGE, alarm->getLongMessage());
			}
			else
			{
				scriptParameters.insert(VARIABLE_MESSAGE, it->second->get<Content>());

			}
		}



		util::ParametersMap NotificationChannel::generateScriptFields(
			const boost::shared_ptr<NotificationProvider> provider,
			const boost::shared_ptr<Alarm> alarm,
			const NotificationType eventType
		) const {
			// Generate variables for rendering
			ParametersMap scriptParameters;
			scriptParameters.insert(VARIABLE_SHORT_MESSAGE, alarm->getShortMessage());

			// Use begin message by default
			boost::optional<MessageType&> type = provider->get<MessageTypeBegin>();
			if (eventType == END)
			{
				type = provider->get<MessageTypeEnd>();
			}
			_setMessageAlternative(scriptParameters, alarm, type);

			// Browse alarm linked objects to build lines and stops ID lists

			std::stringstream linesStream;
			bool firstLineInStream = true;
			BOOST_FOREACH(const AlarmObjectLink* link, alarm->getLinkedObjects("line"))
			{
				boost::shared_ptr<const pt::CommercialLine> line = Env::GetOfficialEnv().get<pt::CommercialLine>(link->getObjectId());
				if(line.get())
				{
					if(!firstLineInStream)
					{
						linesStream << VARIABLE_ID_SEPARATOR;
						firstLineInStream = false;
					}
					linesStream << line->getName();
					// boost::lexical_cast<std::string>(
				}
			}
			scriptParameters.insert(VARIABLE_LINE_IDS, linesStream.str());

			std::stringstream stopsStream;
			bool firstStopInStream = true;

			BOOST_FOREACH(const AlarmObjectLink* link, alarm->getLinkedObjects("stoparea"))
			{
				boost::shared_ptr<const pt::StopArea> stop = Env::GetOfficialEnv().get<pt::StopArea>(link->getObjectId());
				if(stop.get())
				{
					if(!firstStopInStream)
					{
						stopsStream << VARIABLE_ID_SEPARATOR;
						firstStopInStream = false;
					}
					stopsStream << stop->getName();
				}
			}
			scriptParameters.insert(VARIABLE_STOP_IDS, stopsStream.str());

			// TODO insert point for derived classes may add their own script parameters

			// Render script fields
			ParametersMap result;
			ParametersMap providerParameters = provider->get<Parameters>();

			BOOST_FOREACH(std::string paramName, _getScriptParameterNames())
			{
				string code = providerParameters.get<string>(paramName);
				if (!code.empty()) {
					cms::CMSScript script(code);
					if (!script.empty()) {
						string value = script.eval(scriptParameters);
						if (!value.empty()) {
							result.insert(paramName, value);
						}
					}
				}
			}
			return result;
		}
	}
}
