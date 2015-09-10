
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
	namespace pt
	{
		const string CommercialLineTableSync::COL_NETWORK_ID ("network_id");
		const string CommercialLineTableSync::COL_NAME ("name");
		const string CommercialLineTableSync::COL_SHORT_NAME ("short_name");
		const string CommercialLineTableSync::COL_LONG_NAME ("long_name");
		const string CommercialLineTableSync::COL_COLOR ("color");
		const string CommercialLineTableSync::COL_FOREGROUND_COLOR ("foreground_color");
		const string CommercialLineTableSync::COL_STYLE ("style");
		const string CommercialLineTableSync::COL_IMAGE ("image");
		const string CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES("optional_reservation_places");
		const string CommercialLineTableSync::COL_CREATOR_ID("creator_id");
		const string CommercialLineTableSync::COL_BIKE_USE_RULE("bike_compliance_id");
		const string CommercialLineTableSync::COL_HANDICAPPED_USE_RULE ("handicapped_compliance_id");
		const string CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE("pedestrian_compliance_id");
		const string CommercialLineTableSync::COL_RESERVATION_CONTACT_ID("reservation_contact_id");
		const string CommercialLineTableSync::COL_CALENDAR_TEMPLATE_ID("calendar_template_id");
		const string CommercialLineTableSync::COL_MAP_URL("map_url");
		const string CommercialLineTableSync::COL_DOC_URL("doc_url");
		const string CommercialLineTableSync::COL_TIMETABLE_ID("timetable_id");
		const string CommercialLineTableSync::COL_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE = "display_duration_before_first_departure";
		const string CommercialLineTableSync::COL_WEIGHT_FOR_SORTING = "weight_for_sorting";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CommercialLineTableSync>::TABLE(
			"t042_commercial_lines"
		);

		template<>
		const Field DBTableSyncTemplate<CommercialLineTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(CommercialLineTableSync::COL_NETWORK_ID, SQL_INTEGER),
			Field(CommercialLineTableSync::COL_NAME, SQL_TEXT),
			Field(CommercialLineTableSync::COL_SHORT_NAME, SQL_TEXT),
			Field(CommercialLineTableSync::COL_LONG_NAME, SQL_TEXT),
			Field(CommercialLineTableSync::COL_COLOR, SQL_TEXT),
			Field(CommercialLineTableSync::COL_FOREGROUND_COLOR, SQL_TEXT),
			Field(CommercialLineTableSync::COL_STYLE, SQL_TEXT),
			Field(CommercialLineTableSync::COL_IMAGE, SQL_TEXT),
			Field(CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES, SQL_TEXT),
			Field(CommercialLineTableSync::COL_CREATOR_ID, SQL_TEXT),
			Field(CommercialLineTableSync::COL_BIKE_USE_RULE, SQL_INTEGER),
			Field(CommercialLineTableSync::COL_HANDICAPPED_USE_RULE, SQL_INTEGER),
			Field(CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE, SQL_INTEGER),
			Field(CommercialLineTableSync::COL_RESERVATION_CONTACT_ID, SQL_INTEGER),
			Field(CommercialLineTableSync::COL_CALENDAR_TEMPLATE_ID, SQL_INTEGER),
			Field(CommercialLineTableSync::COL_MAP_URL, SQL_TEXT),
			Field(CommercialLineTableSync::COL_DOC_URL, SQL_TEXT),
			Field(CommercialLineTableSync::COL_TIMETABLE_ID, SQL_INTEGER),
			Field(CommercialLineTableSync::COL_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE, SQL_INTEGER),
			Field(CommercialLineTableSync::COL_WEIGHT_FOR_SORTING, SQL_INTEGER),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<CommercialLineTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					CommercialLineTableSync::COL_NETWORK_ID.c_str(),
					CommercialLineTableSync::COL_CREATOR_ID.c_str(),
					""
			)	);
			r.push_back(
				DBTableSync::Index(
					CommercialLineTableSync::COL_RESERVATION_CONTACT_ID.c_str(),
					""
			)	);
			return r;
		}



		template<>
		void OldLoadSavePolicy<CommercialLineTableSync, CommercialLine>::Load(
			CommercialLine* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			DBModule::LoadObjects(object->getLinkedObjectsIds(*rows), env, linkLevel);
			object->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				object->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<>
		void OldLoadSavePolicy<CommercialLineTableSync,CommercialLine>::Unlink(
			CommercialLine* obj
		){
			obj->removeParentLink();

			if(Env::GetOfficialEnv().contains(*obj))
			{
				obj->cleanDataSourceLinks(true);
			}
		}



		template<>
		void OldLoadSavePolicy<CommercialLineTableSync,CommercialLine>::Save(
			CommercialLine* object,
			optional<DBTransaction&> transaction
		){
			// Preparation of places with optional reservation
			stringstream optionalReservationPlaces;
			bool first(true);
			BOOST_FOREACH(const StopArea* place, object->getOptionalReservationPlaces())
			{
				if (first)
				{
					first = false;
				}
				else
				{
					optionalReservationPlaces << ",";
				}
				optionalReservationPlaces << place->getKey();
			}

			// The query
			ReplaceQuery<CommercialLineTableSync> query(*object);
			query.addField(object->_getParent() ? object->_getParent()->getKey() : RegistryKeyType(0));
			query.addField(object->getName());
			query.addField(object->getShortName());
			query.addField(object->getLongName());
			query.addField(object->getColor() ? object->getColor()->toXMLColor() : string());
			query.addField(object->getFgColor() ? object->getFgColor()->toXMLColor() : string());
			query.addField(object->getStyle());
			query.addField(object->getImage());
			query.addField(optionalReservationPlaces.str());
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			query.addField(
				object->getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(object->getRule(USER_BIKE)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_BIKE))->getKey() :
				RegistryKeyType(0)
			);
			query.addField(
				object->getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED))->getKey() :
				RegistryKeyType(0)
			);
			query.addField(
				object->getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN))->getKey() :
				RegistryKeyType(0)
			);
			query.addField(
				object->getReservationContact() ? object->getReservationContact()->getKey() : RegistryKeyType(0)
			);
			query.addField(
				object->getCalendarTemplate() ? object->getCalendarTemplate()->getKey() : RegistryKeyType(0)
			);
			query.addField(object->getMapURL());
			query.addField(object->getDocURL());
			query.addField(object->getTimetableId());
			query.addField(
				object->getDisplayDurationBeforeFirstDeparture().is_not_a_date_time() ?
				string() :
				lexical_cast<string>(object->getDisplayDurationBeforeFirstDeparture().total_seconds() / 60)
			);
			query.addField(object->getWeightForSorting());
			query.execute(transaction);
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
					query << " AND (l." << COL_CREATOR_ID << " IS NULL OR l." << COL_CREATOR_ID << "='')";
				else
					query << " AND l." << COL_CREATOR_ID << " LIKE " << Conversion::ToDBString(*creatorId);
			}
			if(name && *name != "%%" && *name != "%")
			{
				if (name->empty())
					query << " AND (l." << COL_NAME << " IS NULL OR l." << COL_NAME << "='')";
				else
					query << " AND l." << COL_NAME << " LIKE " << Conversion::ToDBString(*name);
			}
			if (parentId)
			{
				query << " AND l." << COL_NETWORK_ID << "=" << *parentId;
			}

			// Contact center filter
			if(contactCenterId)
			{
				query << " AND l." << COL_RESERVATION_CONTACT_ID << "=" << *contactCenterId;
			}

			if (orderByNetwork)
				query << " ORDER BY "
					<< "(SELECT n." << SimpleObjectFieldDefinition<Name>::FIELD.name << " FROM " << TransportNetworkTableSync::TABLE.NAME << " AS n WHERE n." << TABLE_COL_ID << "=l." << COL_NETWORK_ID << ")" << (raisingOrder ? " ASC" : " DESC")
					<< ",l." << COL_SHORT_NAME << (raisingOrder ? " ASC" : " DESC");
			else if (orderByName)
				query << " ORDER BY l." << COL_SHORT_NAME << (raisingOrder ? " ASC" : " DESC");
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
				<< "(SELECT n." << SimpleObjectFieldDefinition<Name>::FIELD.name << " FROM " << TransportNetworkTableSync::TABLE.NAME << " AS n WHERE n." << TABLE_COL_ID << "=" << TABLE.NAME << "." << COL_NETWORK_ID << ")" << (raisingOrder ? " ASC" : " DESC")
				<< "," << TABLE.NAME << "." << COL_SHORT_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY " << TABLE.NAME << "." << COL_SHORT_NAME << (raisingOrder ? " ASC" : " DESC");
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
					query << CommercialLineTableSync::COL_NETWORK_ID << "!=" << *it;
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
						query << " OR " << COL_NETWORK_ID << "=" << *it;
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
				query << " AND " << COL_RESERVATION_CONTACT_ID << ">0";
			}

			return query.str();
		}
	}
}
