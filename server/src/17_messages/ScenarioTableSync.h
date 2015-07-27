////////////////////////////////////////////////////////////////////////////////
/// ScenarioTableSync class header.
///	@file ScenarioTableSync.h
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

#ifndef SYNTHESE_ScenarioTableSync_H__
#define SYNTHESE_ScenarioTableSync_H__

#include "InheritanceLoadSavePolicy.hpp"

#include "Scenario.h"
#include "AlarmTableSync.h"
#include "AlarmTemplate.h"
#include "SentAlarm.h"
#include "ScenarioTemplate.h"
#include "SentScenarioDao.hpp"


#include <vector>
#include <string>
#include <iostream>

namespace synthese
{
	namespace messages
	{
		class SentScenario;
		class ScenarioTemplate;

		////////////////////////////////////////////////////////////////////
		/// Scenario table synchronizer.
		///	@ingroup m17LS refLS
		///
		///	@note As Scenario is an abstract class, do not use the get static
		/// method. Use getAlarm instead.
		///
		/// Only sent scenarios store its variables into the table. The
		/// scenario template contains all definitions within the text of
		/// its alarms.
		///
		/// The format of the variables column is :
		///		- for sent scenarios : <variable>|<value>, ...
		///
		class ScenarioTableSync:
			public db::DBDirectTableSyncTemplate<
				ScenarioTableSync,
				Scenario,
				db::FullSynchronizationPolicy,
				db::InheritanceLoadSavePolicy
			>,
			public SentScenarioDao
		{
		public:
			static const std::string COL_IS_TEMPLATE;
			static const std::string COL_ENABLED;
			static const std::string COL_NAME;
			static const std::string COL_PERIODSTART;
			static const std::string COL_PERIODEND;
			static const std::string COL_FOLDER_ID;
			static const std::string COL_VARIABLES;
			static const std::string COL_TEMPLATE;
			static const std::string COL_DATASOURCE_LINKS;
			static const std::string COL_SECTIONS;
			static const std::string COL_EVENT_START;
			static const std::string COL_EVENT_END;
			static const std::string COL_ARCHIVED;

			static const std::string VARIABLES_SEPARATOR;
			static const std::string VARIABLES_OPERATOR;
			static const std::string SECTIONS_SEPARATOR;

			ScenarioTableSync() {}
			~ScenarioTableSync() {}



			static ScenarioTemplate::VariablesMap GetVariables(
				util::RegistryKeyType scenarioId
			);



			static void CopyMessages(
				util::RegistryKeyType sourceId,
				Scenario& dest,
				boost::optional<db::DBTransaction&> transaction = boost::optional<db::DBTransaction&>()
			);



			/** Rewrites the messages of a sent scenario considering the values of the variables.
				@param scenario sent scenario to rewrite
				@author Hugues Romain
				@date 2009
			*/
			static void WriteVariablesIntoMessages(
				const SentScenario& scenario
			);



			/** Template scenario search.
				@param env Environment to populate
				@param name Name of the template
				@param first First Scenario object to answer
				@param number Number of Scenario objects to answer (0 = all)
				The size of the vector is less or equal to number, then all users were returned despite of
				the number limit. If the size is greater than number (actually equal to number + 1) then
				there is others accounts to show. Test it to know if the situation needs a "click for more"
				button.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult SearchTemplates(
				util::Env& env,
				boost::optional<util::RegistryKeyType> folderId,
				const std::string name = std::string(),
				const ScenarioTemplate* scenarioToBeDifferentWith = NULL,
				int first = 0,
				boost::optional<size_t> number = boost::optional<size_t>(),
				bool orderByName = true,
				bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			static SearchResult SearchSentScenarios(
				util::Env& env,
				boost::optional<std::string> name = boost::optional<std::string>(),
				boost::optional<bool> inArchive = boost::optional<bool>(),
				boost::optional<bool> isActive = boost::optional<bool>(),
				boost::optional<util::RegistryKeyType> scenarioId = boost::optional<util::RegistryKeyType>(),
				boost::optional<int> first = boost::optional<int>(),
				boost::optional<size_t> number = boost::optional<size_t>(),
				bool orderByDate = true,
				bool orderByName = false,
				bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			static SearchResult Search(
				util::Env& env,
				boost::optional<std::string> name = boost::optional<std::string>(),
				boost::optional<int> first = boost::optional<int>(),
				boost::optional<size_t> number = boost::optional<size_t>(),
				bool orderByName = false,
				bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			virtual std::vector<boost::shared_ptr<SentScenario> > list() const;
			virtual void save(boost::shared_ptr<SentScenario>& sentScenario) const;

		};
}	}

#endif // SYNTHESE_ScenarioTableSync_H__
