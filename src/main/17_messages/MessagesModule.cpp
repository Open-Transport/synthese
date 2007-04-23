
/** MessagesModule class implementation.
	@file MessagesModule.cpp

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

#include "17_messages/MessagesModule.h"
#include "17_messages/SentScenario.h"
#include "17_messages/ScenarioTemplate.h"
#include "17_messages/ScenarioTableSync.h"
#include "17_messages/TextTemplateTableSync.h"
#include "17_messages/TextTemplate.h"

using namespace std;
using namespace boost;

namespace synthese
{
	namespace messages
	{
		SentAlarm::Registry MessagesModule::_alarms;
		SentScenario::Registry MessagesModule::_scenarii;
		AlarmObjectLink::Registry MessagesModule::_alarmLinks;

		SentAlarm::Registry& 
			MessagesModule::getAlarms ()
		{
			return _alarms;
		}

		SentScenario::Registry& MessagesModule::getScenarii()
		{
			return _scenarii;
		}

		std::vector<pair<uid, std::string> > MessagesModule::getScenarioTemplatesLabels( bool withAll /*= false*/ )
		{
			vector<pair<uid,string> > m;
			if (withAll)
				m.push_back(make_pair(0, "(tous)"));
			vector<shared_ptr<ScenarioTemplate> > sc = ScenarioTableSync::searchTemplate();
			for(vector<shared_ptr<ScenarioTemplate> >::const_iterator it = sc.begin(); it != sc.end(); ++it)
				m.push_back(make_pair((*it)->getKey(), (*it)->getName()));
			return m;

		}

		std::vector<pair<AlarmLevel, std::string> > MessagesModule::getLevelLabels( bool withAll /*= false*/ )
		{
			vector<pair<AlarmLevel, string> > m;
			if (withAll)
				m.push_back(make_pair(ALARM_LEVEL_UNKNOWN, "(tous)"));
			m.push_back(make_pair(ALARM_LEVEL_INFO, getLevelLabel(ALARM_LEVEL_INFO)));
			m.push_back(make_pair(ALARM_LEVEL_WARNING, getLevelLabel(ALARM_LEVEL_WARNING)));
			return m;
		}

		std::vector<pair<AlarmConflict, std::string> > MessagesModule::getConflictLabels( bool withAll /*= false*/ )
		{
			vector<pair<AlarmConflict, string> > m;
			if (withAll)
				m.push_back(make_pair(ALARM_CONFLICT_UNKNOWN, "(tous)"));
			m.push_back(make_pair(ALARM_NO_CONFLICT, getConflictLabel(ALARM_NO_CONFLICT)));
			m.push_back(make_pair(ALARM_WARNING_ON_INFO, getConflictLabel(ALARM_WARNING_ON_INFO)));
			m.push_back(make_pair(ALARM_INFO_UNDER_WARNING, getConflictLabel(ALARM_INFO_UNDER_WARNING)));
			m.push_back(make_pair(ALARM_CONFLICT, getConflictLabel(ALARM_CONFLICT)));
			return m;
		}

		std::vector<pair<uid, std::string> > MessagesModule::getTextTemplateLabels(const AlarmLevel& level)
		{
			vector<pair<uid, string> > m;
			vector<shared_ptr<TextTemplate> > v = TextTemplateTableSync::search(level);
			for (vector<shared_ptr<TextTemplate> >::iterator it = v.begin(); it != v.end(); ++it)
			{
				m.push_back(make_pair((*it)->getKey(), (*it)->getName()));
			}
			return m;
		}

		std::string MessagesModule::getLevelLabel( const AlarmLevel& level )
		{
			switch (level)
			{
			case ALARM_LEVEL_INFO : return "Complémentaire";
			case ALARM_LEVEL_WARNING : return "Prioritaire";
			default: return "Inconnu";
			}
		}

		AlarmObjectLink::Registry& MessagesModule::getAlarmLinks()
		{
			return _alarmLinks;
		}

		std::string MessagesModule::getConflictLabel( const AlarmConflict& conflict )
		{
			switch (conflict)
			{
			case ALARM_NO_CONFLICT: return "Sans conflit";
			case ALARM_WARNING_ON_INFO: return "Annulé par un prioritaire";
			case ALARM_INFO_UNDER_WARNING: return "Annule un complémentaire";
			case ALARM_CONFLICT: return "En conflit";
			default: return "Inconnu";
			}
		}

		void initialize()
		{

		}
	}
}
