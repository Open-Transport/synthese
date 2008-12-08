
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

#include "MessagesModule.h"

#include "SentScenario.h"
#include "ScenarioTemplate.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "ScenarioTableSync.h"
#include "ScenarioFolder.h"
#include "ScenarioFolderTableSync.h"
#include "TextTemplateTableSync.h"
#include "TextTemplate.h"

#include "Env.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	
	namespace util
	{
		template<> const std::string util::FactorableTemplate<ModuleClass, messages::MessagesModule>::FACTORY_KEY("17_messages");
	}

	namespace messages
	{
		std::vector<pair<uid, std::string> > MessagesModule::GetScenarioTemplatesLabels(
			bool withAll /*= false*/
			, uid folderId
			, string prefix
		){
			vector<pair<uid,string> > m;
			if (withAll)
				m.push_back(make_pair(0, "(tous)"));

			Env env;
			ScenarioTemplateInheritedTableSync::Search(env, folderId);
			BOOST_FOREACH(shared_ptr<ScenarioTemplate> st, env.template getRegistry<ScenarioTemplate>())
				m.push_back(make_pair(st->getKey(), prefix + st->getName()));

			if (folderId != UNKNOWN_VALUE)
			{
				ScenarioFolderTableSync::Search(env, folderId);
				BOOST_FOREACH(shared_ptr<ScenarioFolder> folder, env.template getRegistry<ScenarioFolder>())
				{
					std::vector<pair<uid, std::string> > r(GetScenarioTemplatesLabels(false, folder->getKey(), prefix + folder->getName() +"/"));
					m.insert(m.end(),r.begin(), r.end());
				}
			}
			return m;
		}



		std::vector<std::pair<uid, std::string> > MessagesModule::GetScenarioFoldersLabels(
			uid folderId /*= 0 */
			, std::string prefix /*= std::string()  */
			, uid forbiddenFolderId
		){
			vector<pair<uid,string> > m;
			if (folderId == 0)
				m.push_back(make_pair(0, "(racine)"));

			Env env;
			ScenarioFolderTableSync::Search(env, folderId);
			BOOST_FOREACH(shared_ptr<ScenarioFolder> folder, env.template getRegistry<ScenarioFolder>())
			{
				if (folder->getKey() == forbiddenFolderId)
					continue;

				m.push_back(make_pair(folder->getKey(), prefix + folder->getName()));
				
				vector<pair<uid, string> > r(GetScenarioFoldersLabels(folder->getKey(), prefix + folder->getName() +"/", forbiddenFolderId));
				m.insert(m.end(),r.begin(), r.end());
			}
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
			Env env;
			vector<pair<uid, string> > m;
			TextTemplateTableSync::Search(env, level);
			BOOST_FOREACH(shared_ptr<TextTemplate> text, env.template getRegistry<TextTemplate>())
			{
				m.push_back(make_pair(text->getKey(), text->getName()));
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

		void MessagesModule::initialize()
		{

		}

		std::string MessagesModule::getName() const
		{
			return "Gestion d'actualités";
		}
	}
}
