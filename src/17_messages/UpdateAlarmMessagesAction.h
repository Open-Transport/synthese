////////////////////////////////////////////////////////////////////////////////
/// UpdateAlarmMessagesAction class header.
///	@file UpdateAlarmMessagesAction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_UpdateAlarmMessagesAction_H__
#define SYNTHESE_UpdateAlarmMessagesAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "MessagesTypes.h"

#include "BaseImportableUpdateAction.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace server
	{
		class ActionException;
	}

	namespace messages
	{
		class Alarm;
		class Scenario;

		/** Message content update action class.

			This action updates :
				- alarm type
				- alarm texts

			@ingroup m17Actions refActions
		*/
		class UpdateAlarmMessagesAction :
			public util::FactorableTemplate<server::Action, UpdateAlarmMessagesAction>,
			public impex::BaseImportableUpdateAction
		{
		public:
			static const std::string PARAMETER_TYPE;
			static const std::string PARAMETER_SHORT_MESSAGE;
			static const std::string PARAMETER_LONG_MESSAGE;
			static const std::string PARAMETER_ALARM_ID;
			static const std::string PARAMETER_RAW_EDITOR;
			static const std::string PARAMETER_DONE;
			static const std::string PARAMETER_SCENARIO_ID;
			static const std::string PARAMETER_ALARM_DATASOURCE_ID;

		private:
			boost::shared_ptr<Alarm>		_alarm;
			boost::shared_ptr<const impex::DataSource> _alarmDataSource;
			boost::optional<boost::shared_ptr<Scenario> >		_scenario;
			boost::optional<AlarmLevel>		_type;
			boost::optional<std::string> _shortMessage;
			boost::optional<std::string> _longMessage;
			boost::optional<bool> _rawEditor;
			boost::optional<bool> _done;

			std::string _dataSourceLinkId;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(
				const util::ParametersMap& map
			);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(
				server::Request& request
			);



			////////////////////////////////////////////////////////////////////
			///	Alarm setter.
			///	@param id ID of the alarm
			///	@author Hugues Romain
			///	@date 2008
			void setAlarmId(
				util::RegistryKeyType id
			);

			virtual bool isAuthorized(const server::Session* session) const;

			void setRawEditor(boost::optional<bool> value){ _rawEditor = value; }
			void setAlarm(boost::shared_ptr<Alarm> value){ _alarm = value; }
		};
	}
}

#endif // SYNTHESE_UpdateAlarmMessagesAction_H__
