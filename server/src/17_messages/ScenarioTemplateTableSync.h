////////////////////////////////////////////////////////////////////////////////
/// ScenarioTemplateTableSync class header.
///	@file ScenarioTemplateTableSync.h
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

#ifndef SYNTHESE_ScenarioTemplateTableSync_H__
#define SYNTHESE_ScenarioTemplateTableSync_H__

#include "InheritanceLoadSavePolicy.hpp"

#include "Scenario.h"
#include "AlarmTableSync.h"
#include "DBDirectTableSyncTemplate.hpp"
#include "Alarm.h"
#include "ScenarioTemplate.h"
#include "ScenarioTableSync.h"
#include "Conversion.h"


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
		///
		class ScenarioTemplateTableSync:
			public db::DBDirectTableSyncTemplate<
				ScenarioTemplateTableSync,
				ScenarioTemplate
			>
		{
		public:

			ScenarioTemplateTableSync() {}
			~ScenarioTemplateTableSync() {}


			static void CopyMessages(
				util::RegistryKeyType sourceId,
				Scenario& dest,
				boost::optional<db::DBTransaction&> transaction = boost::optional<db::DBTransaction&>()
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
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> folderId = boost::optional<util::RegistryKeyType>(),
				const std::string name = std::string(),
				const ScenarioTemplate* scenarioToBeDifferentWith = NULL,
				int first = 0,
				boost::optional<size_t> number = boost::optional<size_t>(),
				bool orderByName = true,
				bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			template<class OutputIterator>
			static 
				void
				Search(
				util::Env& env,
				OutputIterator result,
				boost::optional<util::RegistryKeyType> folderId,
				const std::string name = std::string(),
				const ScenarioTemplate* scenarioToBeDifferentWith = NULL,
				int first = 0,
				boost::optional<size_t> number = boost::optional<size_t>(),
				bool orderByName = true,
				bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
			

		};

		template<class OutputIterator>
		void
		ScenarioTemplateTableSync::Search(
			util::Env& env,
			OutputIterator result,
			boost::optional<util::RegistryKeyType> folderId,
			const string name /*= string()*/,
			const ScenarioTemplate* scenarioToBeDifferentWith /*= NULL*/,
			int first /*= 0*/,
			boost::optional<size_t> number /*= optional<size_t>()*/,
			bool orderByName /*= true*/,
			bool raisingOrder /*= false*/,
			util::LinkLevel linkLevel /*= UP_LINKS_LOAD_LEVEL */
		){
			std::stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME << " WHERE 1";

			if(folderId)
			{
				query << " AND (" << Folder::FIELD.name << "=" << *folderId;
				if (*folderId == 0)
					query << " OR " << Folder::FIELD.name << " IS NULL";
				query << ")";
			}
			if (!name.empty())
				query << " AND " << Name::FIELD.name << "=" << util::Conversion::ToDBString(name);
			if (scenarioToBeDifferentWith)
				query << " AND " << db::TABLE_COL_ID << "!=" << scenarioToBeDifferentWith->getKey();
			if (orderByName)
				query << " ORDER BY " << Name::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			SearchResult searchResult =
				LoadFromQuery(query.str(), env, linkLevel);
			std::copy(searchResult.begin(), searchResult.end(), result);
		}

		
}	}

#endif // SYNTHESE_ScenarioTemplateTableSync_H__
