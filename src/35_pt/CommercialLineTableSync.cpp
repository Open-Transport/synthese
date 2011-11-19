
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

// Env
#include "CommercialLineTableSync.h"
#include "CommercialLine.h"
#include "TransportNetworkTableSync.h"
#include "TransportNetwork.h"
#include "JourneyPatternTableSync.hpp"
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
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CommercialLineTableSync>::TABLE(
			"t042_commercial_lines"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<CommercialLineTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(CommercialLineTableSync::COL_NETWORK_ID, SQL_INTEGER),
			DBTableSync::Field(CommercialLineTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_SHORT_NAME, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_LONG_NAME, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_COLOR, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_STYLE, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_IMAGE, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_CREATOR_ID, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_BIKE_USE_RULE, SQL_INTEGER),
			DBTableSync::Field(CommercialLineTableSync::COL_HANDICAPPED_USE_RULE, SQL_INTEGER),
			DBTableSync::Field(CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE, SQL_INTEGER),
			DBTableSync::Field(CommercialLineTableSync::COL_RESERVATION_CONTACT_ID, SQL_INTEGER),
			DBTableSync::Field(CommercialLineTableSync::COL_CALENDAR_TEMPLATE_ID, SQL_INTEGER),
			DBTableSync::Field(CommercialLineTableSync::COL_MAP_URL, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_DOC_URL, SQL_TEXT),
			DBTableSync::Field(CommercialLineTableSync::COL_TIMETABLE_ID, SQL_INTEGER),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<CommercialLineTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(
				CommercialLineTableSync::COL_NETWORK_ID.c_str(),
				CommercialLineTableSync::COL_CREATOR_ID.c_str(),
				""
			),
			DBTableSync::Index()
		};

		template<> void DBDirectTableSyncTemplate<CommercialLineTableSync,CommercialLine>::Load(
			CommercialLine* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText ( CommercialLineTableSync::COL_NAME));
			object->setShortName(rows->getText ( CommercialLineTableSync::COL_SHORT_NAME));
			object->setLongName(rows->getText ( CommercialLineTableSync::COL_LONG_NAME));
			object->setMapURL(rows->getText(CommercialLineTableSync::COL_MAP_URL));
			object->setDocURL(rows->getText(CommercialLineTableSync::COL_DOC_URL));
			object->setTimetableId(rows->getLongLong(CommercialLineTableSync::COL_TIMETABLE_ID));

			// Color
			string color(rows->getText(CommercialLineTableSync::COL_COLOR));
			if(!color.empty())
			{
				try
				{
					object->setColor(RGBColor::FromXMLColor(color));
				}
				catch(RGBColor::Exception&)
				{
					Log::GetInstance().warn("No such color "+ color +" in commercial line "+ lexical_cast<string>(object->getKey()));
				}
			}
			object->setStyle(rows->getText ( CommercialLineTableSync::COL_STYLE));
			object->setImage(rows->getText ( CommercialLineTableSync::COL_IMAGE));

			object->setDataSourceLinks(
				ImportableTableSync::GetDataSourceLinksFromSerializedString(
					rows->getText ( CommercialLineTableSync::COL_CREATOR_ID),
					env
			)	);

			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Transport network
				try
				{
					object->setNetwork (
						TransportNetworkTableSync::Get(rows->getLongLong(CommercialLineTableSync::COL_NETWORK_ID), env, linkLevel).get()
					);
				}
				catch(ObjectNotFoundException<TransportNetwork>&)
				{
					Log::GetInstance().warn("No such network in commercial line "+ lexical_cast<string>(object->getKey()));
				}

				// Calendar template
				if(rows->getLongLong(CommercialLineTableSync::COL_CALENDAR_TEMPLATE_ID) > 0)
				{
					try
					{
						object->setCalendarTemplate(
							CalendarTemplateTableSync::GetEditable(rows->getLongLong(CommercialLineTableSync::COL_CALENDAR_TEMPLATE_ID), env, linkLevel).get()
						);
					}
					catch(ObjectNotFoundException<CalendarTemplate>&)
					{
						Log::GetInstance().warn("No such calendar template in commercial line "+ lexical_cast<string>(object->getKey()));
					}
				}
				else
				{
					object->setCalendarTemplate(NULL);
				}

				// Places with optional reservation
				// Parse all optional reservation places separated by ,
				std::vector<std::string> stops;
				CommercialLine::PlacesSet placesWithOptionalReservation;
				string colORP(rows->getText(CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES));
				boost::split(
					stops,
					colORP,
					boost::is_any_of(",")
				);
				BOOST_FOREACH(const string& stop, stops)
				{
					if(stop.empty()) continue;
					try
					{
						placesWithOptionalReservation.insert(
							StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(stop),env,linkLevel).get()
						);
					}
					catch(ObjectNotFoundException<StopArea>&)
					{
						Log::GetInstance().warn("No such place "+ stop +" in optional reservation places of commercial line "+ lexical_cast<string>(object->getKey()));
					}
				}
				object->setOpionalReservationPlaces(placesWithOptionalReservation);

				// Bike compliance
				util::RegistryKeyType bikeComplianceId(
					rows->getLongLong (CommercialLineTableSync::COL_BIKE_USE_RULE)
				);
				if(bikeComplianceId > 0)
				{
					rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(bikeComplianceId, env, linkLevel).get();
				}

				// Handicapped compliance
				util::RegistryKeyType handicappedComplianceId(
					rows->getLongLong (CommercialLineTableSync::COL_HANDICAPPED_USE_RULE)
				);
				if(handicappedComplianceId > 0)
				{
					rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(handicappedComplianceId, env, linkLevel).get();
				}

				// Pedestrian compliance
				util::RegistryKeyType pedestrianComplianceId(
					rows->getLongLong (CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE)
				);
				if(pedestrianComplianceId > 0)
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = PTUseRuleTableSync::Get(pedestrianComplianceId, env, linkLevel).get();
				}

				// Reservation contact
				util::RegistryKeyType reservationContactId(
					rows->getLongLong(CommercialLineTableSync::COL_RESERVATION_CONTACT_ID)
				);
				if(reservationContactId > 0)
				{
					object->setReservationContact(
						ReservationContactTableSync::Get(reservationContactId, env, linkLevel).get()
					);
				}
			}
			object->setRules(rules);
		}



		template<> void DBDirectTableSyncTemplate<CommercialLineTableSync,CommercialLine>::Unlink(
			CommercialLine* obj
		){
		}



		template<> void DBDirectTableSyncTemplate<CommercialLineTableSync,CommercialLine>::Save(
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
			query.addField(object->getNetwork() ? object->getNetwork()->getKey() : RegistryKeyType(0));
			query.addField(object->getName());
			query.addField(object->getShortName());
			query.addField(object->getLongName());
			query.addField(object->getColor() ? object->getColor()->toXMLColor() : string());
			query.addField(object->getStyle());
			query.addField(object->getImage());
			query.addField(optionalReservationPlaces.str());
			query.addField(ImportableTableSync::SerializeDataSourceLinks(object->getDataSourceLinks()));
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
			query.execute(transaction);
		}



		template<> bool DBTableSyncTemplate<CommercialLineTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<CommercialLineTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			JourneyPatternTableSync::SearchResult routes(JourneyPatternTableSync::Search(env, id));
			BOOST_FOREACH(const JourneyPatternTableSync::SearchResult::value_type& route, routes)
			{
				JourneyPatternTableSync::Remove(NULL, id, transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<CommercialLineTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<CommercialLineTableSync>::LogRemoval(
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
			optional<RegistryKeyType> networkId,
			optional<string> name,
			optional<string> creatorId,
			int first
			, boost::optional<std::size_t> number
			, bool orderByNetwork
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT l.*"
				<< " FROM " << TABLE.NAME << " AS l "
				<< " WHERE 1";
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
			if (networkId)
				query << " AND l." << COL_NETWORK_ID << "=" << *networkId;
			if (orderByNetwork)
				query << " ORDER BY "
					<< "(SELECT n." << TransportNetworkTableSync::COL_NAME << " FROM " << TransportNetworkTableSync::TABLE.NAME << " AS n WHERE n." << TABLE_COL_ID << "=l." << COL_NETWORK_ID << ")" << (raisingOrder ? " ASC" : " DESC")
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
			const security::RightsOfSameClassMap& rights
			, bool totalControl
			, RightLevel neededLevel,
			int first
			, boost::optional<std::size_t> number
			, bool orderByNetwork /*= true */
			, bool orderByName /*= false */
			, bool raisingOrder /*= true */
			, bool mustBeBookable,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< getSQLLinesList(rights, totalControl, neededLevel, mustBeBookable, "*");
			if (orderByNetwork)
				query << " ORDER BY "
				<< "(SELECT n." << TransportNetworkTableSync::COL_NAME << " FROM " << TransportNetworkTableSync::TABLE.NAME << " AS n WHERE n." << TABLE_COL_ID << "=" << TABLE.NAME << "." << COL_NETWORK_ID << ")" << (raisingOrder ? " ASC" : " DESC")
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
			const security::RightsOfSameClassMap& rights
			, bool totalControl
			, RightLevel neededLevel
			, bool mustBeBookable
			, std::string selectedColumns
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
				if (decodeTableId(Conversion::ToLongLong(it->first)) == TransportNetworkTableSync::TABLE.ID)
				{
					if (it->second->getPublicRightLevel() < neededLevel)
						forbiddenNetworks.insert(Conversion::ToLongLong(it->first));
					else
						allowedNetworks.insert(Conversion::ToLongLong(it->first));
				}
				else if (decodeTableId(Conversion::ToLongLong(it->first)) == CommercialLineTableSync::TABLE.ID)
				{
					if (it->second->getPublicRightLevel() < neededLevel)
						forbiddenLines.insert(Conversion::ToLongLong(it->first));
					else
						allowedLines.insert(Conversion::ToLongLong(it->first));
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
