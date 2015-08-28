////////////////////////////////////////////////////////////////////////////////
/// SentScenarioTableSync class header.
///	@file SentScenarioTableSync.h
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

#ifndef SYNTHESE_SentScenarioTableSync_H__
#define SYNTHESE_SentScenarioTableSync_H__

#include "SentScenario.h"
#include "Conversion.h"
#include "AlarmTableSync.h"
#include "DBDirectTableSyncTemplate.hpp"
#include "Alarm.h"
#include "ScenarioTemplate.h"
#include "ScenarioTableSync.h"
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
		class SentScenarioTableSync:
			public db::DBDirectTableSyncTemplate<
				SentScenarioTableSync,
				SentScenario
			>,
			public SentScenarioDao
		{
		public:

			SentScenarioTableSync() {}
			~SentScenarioTableSync() {}

			virtual std::vector<boost::shared_ptr<SentScenario> > list() const;
			virtual void save(boost::shared_ptr<SentScenario>& sentScenario) const;


			static SearchResult Search(
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

			
			template<class OutputIterator>
			static 
				void Search(
				util::Env& env,
				OutputIterator result,
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

		};

		template<class OutputIterator>
		void
			SentScenarioTableSync::Search(
				util::Env& env,
				OutputIterator result,
				boost::optional<string> name /*= optional<string>()*/,
				boost::optional<bool> inArchive,
				boost::optional<bool> isActive,
				boost::optional<util::RegistryKeyType> scenarioId /*= optional<RegistryKeyType>()*/,
				boost::optional<int> first /*= optional<int>()*/,
				boost::optional<size_t> number /*= optional<size_t>()*/,
				bool orderByDate /*= true*/,
				bool orderByName /*= false*/,
				bool raisingOrder /*= false*/,
				util::LinkLevel linkLevel /*= UP_LINKS_LOAD_LEVEL */
		){
			std::stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME << " WHERE 1";

			if (name)
			{
				query << " AND " << Name::FIELD.name << " LIKE " << util::Conversion::ToDBString(*name);
			}

			// Archive filter
			if(inArchive)
			{
				boost::posix_time::ptime date(boost::posix_time::second_clock::local_time());
				if(*inArchive)
				{
					query << " AND " << Archived::FIELD.name << " = 1 "
					;
				}
				else
				{
					query << " AND " << Archived::FIELD.name << " = 0 "
					;
				}
			}

			// Active filter
			if(isActive)
			{
				query << " AND " << Enabled::FIELD.name << "=" << *isActive;
			}
		
			if(scenarioId)
			{
				query << " AND " << Template::FIELD.name << "=" << *scenarioId;
			}

			if(orderByDate)
			{
				query <<
					" ORDER BY " <<
					PeriodStart::FIELD.name << " IS NULL " << (raisingOrder ? "DESC" : "ASC") << "," <<
					PeriodStart::FIELD.name << (raisingOrder ? " ASC" : " DESC") << "," <<
					PeriodEnd::FIELD.name << " IS NULL " << (raisingOrder ? "ASC" : "DESC") << "," <<
					PeriodEnd::FIELD.name << (raisingOrder ? " ASC" : " DESC")
				;
			}

			if (orderByName)
				query << " ORDER BY " << Name::FIELD.name << (raisingOrder ? " ASC" : " DESC");

			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first)
					query << " OFFSET " << *first;
			}

			SearchResult searchResult =
				LoadFromQuery(query.str(), env, linkLevel);
			std::copy(searchResult.begin(), searchResult.end(), result);
		}

		
}	}

#endif // SYNTHESE_SentScenarioTableSync_H__
