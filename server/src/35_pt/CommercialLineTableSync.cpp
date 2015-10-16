
/** CommercialLineTableSync class implementation.
	@file CommercialLineTableSync.cpp

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

#include "CommercialLineTableSync.h"

#include "CommercialLine.h"
#include "DataSourceLinksField.hpp"
#include "Profile.h"
#include "Session.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "User.h"
#include "JourneyPatternTableSync.hpp"
#include "TreeFolderTableSync.hpp"
#include "Place.h"
#include "PTModule.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "PTUseRule.h"
#include "PTUseRuleTableSync.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "GraphConstants.h"
#include "AllowedUseRule.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "CalendarTemplateTableSync.h"
#include "ReplaceQuery.h"
#include "ImportableTableSync.hpp"
#include "TransportNetworkRight.h"

// Util
#include "Conversion.h"

// Security
#include "SecurityConstants.hpp"
#include "Right.h"

// Std
#include <sstream>

// Boost
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace security;
	using namespace geography;
	using namespace graph;
	using namespace calendar;
	using namespace impex;
	using namespace tree;
	

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,CommercialLineTableSync>::FACTORY_KEY("35.25.01 Commercial lines");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CommercialLineTableSync>::TABLE(
			"t042_commercial_lines"
		);

		template<>
		const Field DBTableSyncTemplate<CommercialLineTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CommercialLineTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					Network::FIELD.name.c_str(),
					pt::CreatorId::FIELD.name.c_str(),
					""
			)	);
			r.push_back(
				DBTableSync::Index(
					LineReservationContact::FIELD.name.c_str(),
					""
			)	);
			return r;
		}



		template<>
		bool DBTableSyncTemplate<CommercialLineTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level for this service
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT, UNKNOWN_RIGHT_LEVEL, string());
		}



		template<>
		void DBTableSyncTemplate<CommercialLineTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			JourneyPatternTableSync::SearchResult routes(JourneyPatternTableSync::Search(env, id));
			BOOST_FOREACH(const JourneyPatternTableSync::SearchResult::value_type& route, routes)
			{
				JourneyPatternTableSync::Remove(NULL, route->getKey(), transaction, false);
			}
		}



		template<>
		void DBTableSyncTemplate<CommercialLineTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<>
		void DBTableSyncTemplate<CommercialLineTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log line removal
		}
	}

	namespace pt
	{
		CommercialLineTableSync::SearchResult CommercialLineTableSync::Search(
			Env& env,
			optional<RegistryKeyType> parentId,
			optional<string> name,
			optional<string> creatorId,
			int first,
			boost::optional<std::size_t> number,
			bool orderByNetwork,
			bool orderByName,
			bool raisingOrder,
			LinkLevel linkLevel,
			boost::optional<const security::RightsOfSameClassMap&> rights,
			security::RightLevel neededLevel,
			boost::optional<util::RegistryKeyType> contactCenterId
		){
			stringstream query;
			query
				<< " SELECT l.*"
				<< " FROM " << TABLE.NAME << " AS l "
				<< " WHERE 1";

			// Rights
			if(rights && neededLevel > FORBIDDEN)
			{
				query <<
					getSQLLinesList(
						*rights,
						true,
						neededLevel,
						false,
						GLOBAL_PERIMETER
					)
				;
			}

			// Creator id
			if(creatorId && *creatorId != "%%" && *creatorId != "%")
			{
				if (creatorId->empty())
					query << " AND (l." << pt::CreatorId::FIELD.name << " IS NULL OR l." << pt::CreatorId::FIELD.name << "='')";
				else
					query << " AND l." << pt::CreatorId::FIELD.name << " LIKE " << Conversion::ToDBString(*creatorId);
			}
			if(name && *name != "%%" && *name != "%")
			{
				if (name->empty())
					query << " AND (l." << SimpleObjectFieldDefinition<Name>::FIELD.name << " IS NULL OR l." << SimpleObjectFieldDefinition<Name>::FIELD.name << "='')";
				else
					query << " AND l." << SimpleObjectFieldDefinition<Name>::FIELD.name << " LIKE " << Conversion::ToDBString(*name);
			}
			if (parentId)
			{
				query << " AND l." << Network::FIELD.name << "=" << *parentId;
			}

			// Contact center filter
			if(contactCenterId)
			{
				query << " AND l." << LineReservationContact::FIELD.name << "=" << *contactCenterId;
			}

			if (orderByNetwork)
				query << " ORDER BY "
					<< "(SELECT n." << SimpleObjectFieldDefinition<Name>::FIELD.name << " FROM " << TransportNetworkTableSync::TABLE.NAME << " AS n WHERE n." << TABLE_COL_ID << "=l." << Network::FIELD.name << ")" << (raisingOrder ? " ASC" : " DESC")
					<< ",l." << ShortName::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			else if (orderByName)
				query << " ORDER BY l." << ShortName::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			if(number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
					query << " OFFSET " << first;
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		CommercialLineTableSync::SearchResult CommercialLineTableSync::Search(
			Env& env,
			const security::RightsOfSameClassMap& rights,
			bool totalControl,
			RightLevel neededLevel,
			int first,
			boost::optional<std::size_t> number,
			bool orderByNetwork, /*= true */
			bool orderByName, /*= false */
			bool raisingOrder, /*= true */
			bool mustBeBookable,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< getSQLLinesList(rights, totalControl, neededLevel, mustBeBookable, "*");
			if (orderByNetwork)
				query << " ORDER BY "
				<< "(SELECT n." << SimpleObjectFieldDefinition<Name>::FIELD.name << " FROM " << TransportNetworkTableSync::TABLE.NAME << " AS n WHERE n." << TABLE_COL_ID << "=" << TABLE.NAME << "." << Network::FIELD.name << ")" << (raisingOrder ? " ASC" : " DESC")
				<< "," << TABLE.NAME << "." << ShortName::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY " << TABLE.NAME << "." << ShortName::FIELD.name << (raisingOrder ? " ASC" : " DESC");
			if (number)
				query << " LIMIT " << (*number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}



		std::string CommercialLineTableSync::getSQLLinesList(
			const security::RightsOfSameClassMap& rights,
			bool totalControl,
			RightLevel neededLevel,
			bool mustBeBookable,
			std::string selectedColumns
		){
			RightsOfSameClassMap::const_iterator it;
			bool all(totalControl);
			set<util::RegistryKeyType> allowedNetworks;
			set<util::RegistryKeyType> forbiddenNetworks;
			set<util::RegistryKeyType> allowedLines;
			set<util::RegistryKeyType> forbiddenLines;

			// All ?
			it = rights.find(GLOBAL_PERIMETER);
			if (it != rights.end())
			{
				all = it->second->getPublicRightLevel() >= neededLevel;
			}

			for (RightsOfSameClassMap::const_iterator it = rights.begin(); it != rights.end(); ++it)
			{
				if(it->first.empty() || it->first == GLOBAL_PERIMETER)
				{
					continue;
				}
				try
				{
					if (decodeTableId(lexical_cast<RegistryKeyType>(it->first)) == TransportNetworkTableSync::TABLE.ID)
					{
						if (it->second->getPublicRightLevel() < neededLevel)
							forbiddenNetworks.insert(lexical_cast<RegistryKeyType>(it->first));
						else
							allowedNetworks.insert(lexical_cast<RegistryKeyType>(it->first));
					}
					else if (decodeTableId(lexical_cast<RegistryKeyType>(it->first)) == CommercialLineTableSync::TABLE.ID)
					{
						if (it->second->getPublicRightLevel() < neededLevel)
							forbiddenLines.insert(lexical_cast<RegistryKeyType>(it->first));
						else
							allowedLines.insert(lexical_cast<RegistryKeyType>(it->first));
					}
				}
				catch(bad_lexical_cast&)
				{
				}
			}

			stringstream query;
			query << " SELECT " << selectedColumns << " FROM " << TABLE.NAME << " WHERE 1 ";

			if (all && !forbiddenNetworks.empty())
			{
				query << " AND ((";
				for (set<util::RegistryKeyType>::const_iterator it(forbiddenNetworks.begin()); it != forbiddenNetworks.end(); ++it)
				{
					if (it != forbiddenNetworks.begin())
						query << " AND ";
					query << Network::FIELD.name << "!=" << *it;
				}
				query << ")";

				if (!allowedLines.empty())
				{
					query << " OR (";
					for (set<util::RegistryKeyType>::const_iterator it(allowedLines.begin()); it != allowedLines.end(); ++it)
					{
						if (it != allowedLines.begin())
							query << " OR ";
						query << TABLE_COL_ID << "=" << *it;
					}
					query << ")";
				}
				query << ")";
			}
			else if (!all)
			{
				query << " AND (0";
				if (!allowedNetworks.empty())
				{
					for (set<util::RegistryKeyType>::const_iterator it(allowedNetworks.begin()); it != allowedNetworks.end(); ++it)
						query << " OR " << Network::FIELD.name << "=" << *it;
				}
				if (!allowedLines.empty())
				{
					for (set<util::RegistryKeyType>::const_iterator it(allowedLines.begin()); it != allowedLines.end(); ++it)
						query << " OR " << TABLE_COL_ID << "=" << *it;
				}
				query << ")";
			}
			if (!forbiddenLines.empty())
			{
				for (set<util::RegistryKeyType>::const_iterator it(forbiddenLines.begin()); it != forbiddenLines.end(); ++it)
					query << " AND " << TABLE_COL_ID << "!=" << *it;
			}
			if (mustBeBookable)
			{
				query << " AND " << LineReservationContact::FIELD.name << ">0";
			}

			return query.str();
		}

		bool CommercialLineTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::READ);
		}
	}
}
