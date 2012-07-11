
/** Alarm class header.
	@file Alarm.h

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

#ifndef SYNTHESE_MESSAGES_ALARM_H
#define SYNTHESE_MESSAGES_ALARM_H

#include <string>

#include "MessagesTypes.h"
#include "Registrable.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace messages
	{
		class Scenario;

		/** Alarm message.
			@ingroup m17

			An alarm message is intended to be broadcasted at a time period into several destinations :
				- display screens
				- route planner results
				- etc.

			An alarm can be sent individually (single alarm) or in a group built from a scenario (grouped alarm)
			The _scenario attribute points to the group if applicable.
		*/
		class Alarm
			: public virtual util::Registrable
		{
		public:
			static const std::string DATA_MESSAGE_ID;
			static const std::string DATA_CONTENT;
			static const std::string DATA_PRIORITY;
			static const std::string DATA_SCENARIO_ID;
			static const std::string DATA_SCENARIO_NAME;
			static const std::string DATA_TITLE;
			static const std::string DATA_DONE;

		protected:
			AlarmLevel			_level;
			std::string			_shortMessage;  //!< Alarm message
			std::string			_longMessage;  //!< Alarm message
			const Scenario* 	_scenario;
			bool				_rawEditor;
			bool				_done;

			Alarm(
				util::RegistryKeyType key,
				const Scenario* scenario
			);
			Alarm(const Alarm& source);
			Alarm(
				const Alarm& source,
				const Scenario* scenario
			);

		public:
			virtual ~Alarm();

			//! @name Getters
			//@{
				const std::string&		getShortMessage()	const;
				const std::string&		getLongMessage()	const;
				const AlarmLevel&		getLevel()			const;
				const Scenario*			getScenario()		const;
				bool					getRawEditor() const { return _rawEditor; }
				bool					getDone() const { return _done; }
			//@}

			//! @name Setters
			//@{
				void setLevel (const AlarmLevel& level);
				void setShortMessage( const std::string& message);
				void setLongMessage( const std::string& message);
				void setScenario(const Scenario* scenario);
				void setRawEditor(bool value){ _rawEditor = value; }
				void setDone(bool value){ _done = value; }
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Message CMS exporter.
				/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Messages_in_CMS
				//////////////////////////////////////////////////////////////////////////
				/// @param pm parameters map to populate
				/// @param prefix prefix to add to the field names
				/// @author Hugues Romain
				/// @since 3.3.0
				/// @date 2011
				void toParametersMap(
					util::ParametersMap& pm,
					bool withScenario = true,
					std::string prefix = std::string()
				) const;
			//@}
		};
}	}

#endif
