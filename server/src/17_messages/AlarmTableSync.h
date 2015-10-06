////////////////////////////////////////////////////////////////////////////////
/// AlarmTableSync class header.
///	@file AlarmTableSync.h
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

#ifndef SYNTHESE_AlarmTableSync_H__
#define SYNTHESE_AlarmTableSync_H__

#include "DBDirectTableSyncTemplate.hpp"
#include "Alarm.h"
#include "ScenarioFolder.h"
#include "Conversion.h"


#include <vector>
#include <string>
#include <iostream>

namespace synthese
{
	namespace messages
	{

		////////////////////////////////////////////////////////////////////
		/// Alarm table synchronizer.
		///	@ingroup m17LS refLS
		///
		///
		class AlarmTableSync:
			public db::DBDirectTableSyncTemplate<
				AlarmTableSync,
				Alarm
			>
		{
		public:

			AlarmTableSync() {}
			~AlarmTableSync() {}

			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> scenarioId = boost::optional<util::RegistryKeyType>()
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>()
				, bool orderByLevel = false
				, bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			template<class OutputIterator>
				static void
				Search(
				util::Env& env,
				OutputIterator result,
				boost::optional<util::RegistryKeyType> scenarioId = boost::optional<util::RegistryKeyType>()
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>()
				, bool orderByLevel = false
				, bool raisingOrder = false,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			virtual bool allowList( const server::Session* session ) const;
		};


		template<class OutputIterator>
		void
		AlarmTableSync::Search(
			util::Env& env,
			OutputIterator result,
			boost::optional<util::RegistryKeyType> scenarioId,
			int first,
			boost::optional<std::size_t> number,
			bool orderByLevel,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			std::stringstream query;
			query
				<< " SELECT a.*"
				<< " FROM " << TABLE.NAME << " AS a"
				<< " WHERE 1";
			if(scenarioId)
			{
				query << " AND " << ParentScenario::FIELD.name << "=" << *scenarioId;
			}
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			SearchResult searchResult =
				LoadFromQuery(query.str(), env, linkLevel);
			std::copy(searchResult.begin(), searchResult.end(), result);
		}
		
}	}

#endif // SYNTHESE_AlarmTableSync_H__
