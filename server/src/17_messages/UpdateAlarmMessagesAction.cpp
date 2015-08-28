////////////////////////////////////////////////////////////////////////////////
/// UpdateAlarmMessagesAction class implementation.
///	@file UpdateAlarmMessagesAction.cpp
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

#include "UpdateAlarmMessagesAction.h"

#include "ActionException.h"
#include "ObjectNotFoundException.h"
#include "Profile.h"
#include "SentScenarioTableSync.h"
#include "ScenarioTemplateTableSync.h"
#include "Session.h"
#include "User.h"
#include "MessagesModule.h"
#include "Alarm.h"
#include "Alarm.h"
#include "AlarmTableSync.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "ActionException.h"
#include "Request.h"
#include "ParametersMap.h"
#include "DBLogModule.h"
#include "MessagesLibraryLog.h"
#include "MessagesLog.h"
#include "DataSourceTableSync.h"
#include "ImportableTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace db;
	using namespace impex;
	using namespace security;
	using namespace dblog;
	using namespace util;

	template<> const string util::FactorableTemplate<Action, messages::UpdateAlarmMessagesAction>::FACTORY_KEY("muama");

	namespace messages
	{
		const string UpdateAlarmMessagesAction::PARAMETER_SHORT_MESSAGE = Action_PARAMETER_PREFIX + "sme";
		const string UpdateAlarmMessagesAction::PARAMETER_LONG_MESSAGE = Action_PARAMETER_PREFIX + "lme";
		const string UpdateAlarmMessagesAction::PARAMETER_ALARM_ID(Action_PARAMETER_PREFIX + "id");
		const string UpdateAlarmMessagesAction::PARAMETER_TYPE = Action_PARAMETER_PREFIX + "ty";
		const string UpdateAlarmMessagesAction::PARAMETER_RAW_EDITOR = Action_PARAMETER_PREFIX + "raw_editor";
		const string UpdateAlarmMessagesAction::PARAMETER_DONE = Action_PARAMETER_PREFIX + "done";
		const string UpdateAlarmMessagesAction::PARAMETER_SCENARIO_ID = Action_PARAMETER_PREFIX + "_scenario_id";
		const string UpdateAlarmMessagesAction::PARAMETER_ALARM_DATASOURCE_ID = Action_PARAMETER_PREFIX + "_datasource_id";
		const string UpdateAlarmMessagesAction::PARAMETER_DIGITIZED_VERSION = Action_PARAMETER_PREFIX + "_digitized_version";



		ParametersMap UpdateAlarmMessagesAction::getParametersMap() const
		{
			ParametersMap map;
			if (_alarm.get())
			{
				map.insert(PARAMETER_ALARM_ID, _alarm->getKey());
			}
			if(_type)
			{
				map.insert(PARAMETER_TYPE, static_cast<int>(*_type));
			}
			if(_shortMessage)
			{
				map.insert(PARAMETER_SHORT_MESSAGE, *_shortMessage);
			}
			if(_longMessage)
			{
				map.insert(PARAMETER_LONG_MESSAGE, *_longMessage);
			}
			if(_rawEditor)
			{
				map.insert(PARAMETER_RAW_EDITOR, *_rawEditor);
			}
			if (_digitizedVersion)
			{
				map.insert(PARAMETER_DIGITIZED_VERSION, *_digitizedVersion);
			}

			_getImportableUpdateParametersMap(map);

			return map;
		}

		void UpdateAlarmMessagesAction::_setFromParametersMap(const ParametersMap& map)
		{
			// Update an alarm identified by data source
			// (data source id is memorized in case of creation)
			if(	map.getDefault<RegistryKeyType>(PARAMETER_ALARM_DATASOURCE_ID, 0) &&
				!map.getDefault<string>(PARAMETER_ALARM_ID).empty()
			){
				// Data source
				try
				{
					_alarmDataSource = DataSourceTableSync::Get(
						map.get<RegistryKeyType>(PARAMETER_ALARM_DATASOURCE_ID),
						*_env
					);
				}
				catch(ObjectNotFoundException<DataSource>& e)
				{
					throw ActionException("No such data source for alarm id : "+ e.getMessage());
				}

				// Id
				_dataSourceLinkId = map.get<string>(PARAMETER_ALARM_ID);

				// Update or creation ?
				ImportableTableSync::ObjectBySource<AlarmTableSync> alarms(*_alarmDataSource, *_env);
				ImportableTableSync::ObjectBySource<AlarmTableSync>::Set alarmSet(alarms.get(_dataSourceLinkId));
				if(!alarmSet.empty())
				{
					_alarm = _env->getEditableSPtr(*alarmSet.begin());
				}
			}
			else if(map.getDefault<RegistryKeyType>(PARAMETER_ALARM_ID, 0))
			{
				try
				{
					setAlarmId(map.get<RegistryKeyType>(PARAMETER_ALARM_ID));
				}
				catch (ParametersMap::MissingParameterException& e)
				{
					throw ActionException(e, *this);
				}
			}

			if(!_alarm.get())
			{
				_alarm.reset(new Alarm);
			}

			// Scenario
			if(map.getDefault<RegistryKeyType>(PARAMETER_SCENARIO_ID, 0))
			{
				util::RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_SCENARIO_ID));
				util::RegistryTableType tableId(util::decodeTableId(id));
				if (tableId == ScenarioTemplateTableSync::TABLE.ID)
				{
					_scenario = ScenarioTemplateTableSync::GetEditable(
						id,
						*_env
						);
				}
				else
				{
					_scenario = SentScenarioTableSync::GetEditable(
						id,
						*_env
						);
				}
			}


			// Type
			if(map.isDefined(PARAMETER_TYPE))
			{
				_type = static_cast<AlarmLevel>(map.get<int>(PARAMETER_TYPE));
			}

			// Short name
			if(map.isDefined(PARAMETER_SHORT_MESSAGE))
			{
				_shortMessage = map.get<string>(PARAMETER_SHORT_MESSAGE);
			}

			// Long name
			if(map.isDefined(PARAMETER_LONG_MESSAGE))
			{
				_longMessage = map.get<string>(PARAMETER_LONG_MESSAGE);
			}

			// Raw editor
			if(map.isDefined(PARAMETER_RAW_EDITOR))
			{
				_rawEditor = map.get<bool>(PARAMETER_RAW_EDITOR);
			}

			// Done
			if(map.isDefined(PARAMETER_DONE))
			{
				_done = map.get<bool>(PARAMETER_DONE);
			}

			// Digitized version
			if(map.isDefined(PARAMETER_DIGITIZED_VERSION))
			{
				_digitizedVersion = map.get<string>(PARAMETER_DIGITIZED_VERSION);
			}

			_setImportableUpdateFromParametersMap(*_env, map);
		}



		void UpdateAlarmMessagesAction::run(
			Request& request
		){

			stringstream s;

			// Type
			if(_type)
			{
				DBLogModule::appendToLogIfChange(s,
					"type",
					MessagesModule::getLevelLabel(_alarm->getLevel()),
					MessagesModule::getLevelLabel(*_type)
				);
				_alarm->setLevel(*_type);
			}

			if(_alarmDataSource.get() && !_dataSourceLinkId.empty())
			{
				Importable::DataSourceLinks links;
				links.insert(Importable::DataSourceLinks::value_type(_alarmDataSource.get(), _dataSourceLinkId));
				_alarm->setDataSourceLinksWithoutRegistration(links);
				
			}
			_doImportableUpdate(*_alarm, request);
			
			if(_scenario)
			{
				_alarm->setScenario(_scenario->get());
			}

			// Short message
			if(_shortMessage)
			{
				DBLogModule::appendToLogIfChange(s, "message court", _alarm->getShortMessage(), *_shortMessage);
				_alarm->setShortMessage(*_shortMessage);
			}

			// Long message
			if(_longMessage)
			{
				DBLogModule::appendToLogIfChange(s, "message long", _alarm->getLongMessage(), *_longMessage);
				_alarm->setLongMessage(*_longMessage);
			}

			// Raw editor
			if(_rawEditor)
			{
				DBLogModule::appendToLogIfChange(s, "éditeur technique", lexical_cast<string>(_alarm->getRawEditor()), lexical_cast<string>(*_rawEditor));
				_alarm->setRawEditor(*_rawEditor);
			}

			// Done
			if(_done)
			{
				DBLogModule::appendToLogIfChange(s, "tâche", lexical_cast<string>(_alarm->getDone()), lexical_cast<string>(*_done));
				_alarm->setDone(*_done);
			}

			// Digitized version
			if(_digitizedVersion)
			{
				DBLogModule::appendToLogIfChange(s, "version digitalisée", _alarm->getDigitizedVersion(), *_digitizedVersion);
				_alarm->setDigitizedVersion(*_digitizedVersion);
			}

			AlarmTableSync::Save(_alarm.get());

			if(request.getActionWillCreateObject())
			{
				request.setActionCreatedId(_alarm->getKey());
			}

			// Log
			if (_alarm->belongsToTemplate())
			{
				MessagesLibraryLog::addUpdateEntry(_alarm.get(), s.str(), request.getUser().get());
			}
			else
			{
				MessagesLog::addUpdateEntry(_alarm.get(), s.str(), request.getUser().get());
			}
		}



		bool UpdateAlarmMessagesAction::isAuthorized(const Session* session
		) const {
			if (_alarm->belongsToTemplate())
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(WRITE);
			}
			else
			{
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesRight>(WRITE);
			}
		}



		void UpdateAlarmMessagesAction::setAlarmId(
			util::RegistryKeyType id
		){
			try
			{
				_alarm = AlarmTableSync::GetEditable(
					id,
					*_env
					);
			}
			catch (ObjectNotFoundException<Alarm>& e)
			{
				throw ActionException("message", e, *this);
			}
		}
	}
}
