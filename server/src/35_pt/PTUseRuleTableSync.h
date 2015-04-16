
/** PTUseRuleTableSync class header.
	@file PTUseRuleTableSync.h

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

#ifndef SYNTHESE_PTUseRulesTableSync_H__
#define SYNTHESE_PTUseRulesTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"
#include "PTUseRule.h"
#include "RuleUser.h"

namespace synthese
{
	namespace pt
	{
		/** Public transportation use rules table synchronizer.
			@ingroup m35LS refLS
		*/
		class PTUseRuleTableSync:
			public db::DBDirectTableSyncTemplate<
				PTUseRuleTableSync,
				PTUseRule,
				db::FullSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_CAPACITY;
			static const std::string COL_RESERVATION_TYPE;
			static const std::string COL_ORIGINISREFERENCE;
			static const std::string COL_MINDELAYMINUTES;
			static const std::string COL_MINDELAYMINUTESEXTERNAL;
			static const std::string COL_MINDELAYDAYS;
			static const std::string COL_MAXDELAYDAYS;
			static const std::string COL_HOURDEADLINE;
			static const std::string COL_RESERVATION_MIN_DEPARTURE_TIME;
			static const std::string COL_RESERVATION_FORBIDDEN_DAYS;
			static const std::string COL_DEFAULT_FARE;
			static const std::string COL_FORBIDDEN_IN_DEPARTURE_BOARDS;
			static const std::string COL_FORBIDDEN_IN_TIMETABLES;
			static const std::string COL_FORBIDDEN_IN_JOURNEY_PLANNING;


			//////////////////////////////////////////////////////////////////////////
			/// Search of use rules.
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// @param env Environment to populate while the search
			/// @param name Filter on the name of the use rule
			/// @param first Rank of the first returned rule
			/// @param number Number of returned rules
			/// @param orderByName Order the results by the name field
			/// @param raisingOrder Order ascendantly
			/// @param linkLevel Level of link when loading objects
			static SearchResult Search(
				util::Env& env,
				boost::optional<std::string> name = boost::optional<std::string>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByName = true,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			static std::string SerializeForbiddenDays(
				const PTUseRule::ReservationForbiddenDays& value
			);

			static PTUseRule::ReservationForbiddenDays UnserializeForbiddenDays(
				const std::string& value
			);

			static graph::RuleUser::Rules UnserializeUseRules(
				const std::string& value,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			static std::string SerializeUseRules(
				const graph::RuleUser::Rules& value
			);
		};
}	}

#endif // SYNTHESE_ServiceDateTableSync_H__
