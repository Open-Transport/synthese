
/** CommercialLineTableSync class implementation.
	@file CommercialLineTableSync.cpp

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

// Env
#include "CommercialLineTableSync.h"
#include "CommercialLine.h"
#include "TransportNetworkTableSync.h"
#include "TransportNetwork.h"
#include "LineTableSync.h"
#include "Place.h"
#include "EnvModule.h"
#include "PublicTransportStopZoneConnectionPlace.h"
#include "ConnectionPlaceTableSync.h"
#include "PTUseRule.h"
#include "PTUseRuleTableSync.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "GraphConstants.h"
// Db
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

// Util
#include "Conversion.h"

// Security
#include "12_security/Constants.h"
#include "Right.h"

// Std
#include <sstream>

// Boost
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace security;
	using namespace pt;
	using namespace geography;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,CommercialLineTableSync>::FACTORY_KEY("15.25.01 Commercial lines");
	}
	namespace env
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
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<CommercialLineTableSync>::TABLE(
			"t042_commercial_lines"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<CommercialLineTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_NETWORK_ID, SQL_INTEGER),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_SHORT_NAME, SQL_TEXT),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_LONG_NAME, SQL_TEXT),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_COLOR, SQL_TEXT),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_STYLE, SQL_TEXT),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_IMAGE, SQL_TEXT),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES, SQL_TEXT),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_CREATOR_ID, SQL_TEXT),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_BIKE_USE_RULE, SQL_INTEGER),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_HANDICAPPED_USE_RULE, SQL_INTEGER),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE, SQL_INTEGER),
			SQLiteTableSync::Field(CommercialLineTableSync::COL_RESERVATION_CONTACT_ID, SQL_INTEGER),
			SQLiteTableSync::Field()

		};
		
		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<CommercialLineTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index()
		};

		template<> void SQLiteDirectTableSyncTemplate<CommercialLineTableSync,CommercialLine>::Load(
			CommercialLine* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
		    object->setName(rows->getText ( CommercialLineTableSync::COL_NAME));
		    object->setShortName(rows->getText ( CommercialLineTableSync::COL_SHORT_NAME));
		    object->setLongName(rows->getText ( CommercialLineTableSync::COL_LONG_NAME));
		    object->setColor(RGBColor(rows->getText ( CommercialLineTableSync::COL_COLOR)));
		    object->setStyle(rows->getText ( CommercialLineTableSync::COL_STYLE));
		    object->setImage(rows->getText ( CommercialLineTableSync::COL_IMAGE));
		    object->setCreatorId(rows->getText ( CommercialLineTableSync::COL_CREATOR_ID));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				const TransportNetwork* tn = 
					TransportNetworkTableSync::Get (rows->getLongLong ( CommercialLineTableSync::COL_NETWORK_ID), env, linkLevel).get();

				object->setNetwork (tn);

				typedef tokenizer<char_separator<char> > tokenizer;
				string stops(rows->getText(CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES));

				// Parse all optional reservation places separated by ,
				char_separator<char> sep1 (",");
				tokenizer stopsTokens (stops, sep1);

				for(tokenizer::iterator it(stopsTokens.begin());
					it != stopsTokens.end ();
					++it
				){
					uid id(Conversion::ToLongLong(*it));
					const PublicTransportStopZoneConnectionPlace* place(ConnectionPlaceTableSync::Get(id,env,linkLevel).get());
					object->addOptionalReservationPlace(place);
				}

				uid bikeComplianceId(
					rows->getLongLong (CommercialLineTableSync::COL_BIKE_USE_RULE)
					);
				uid handicappedComplianceId(
					rows->getLongLong (CommercialLineTableSync::COL_HANDICAPPED_USE_RULE)
					);
				uid pedestrianComplianceId(
					rows->getLongLong (CommercialLineTableSync::COL_PEDESTRIAN_USE_RULE)
					);
				uid reservationContactId(
					rows->getLongLong(CommercialLineTableSync::COL_RESERVATION_CONTACT_ID)
				);

				if(bikeComplianceId > 0)
				{
					object->addRule(
						USER_BIKE,
						PTUseRuleTableSync::Get(bikeComplianceId, env, linkLevel).get()
					);
				}
				if(handicappedComplianceId > 0)
				{
					object->addRule(
						USER_HANDICAPPED,
						PTUseRuleTableSync::Get(handicappedComplianceId, env, linkLevel).get()
					);
				}
				if(pedestrianComplianceId > 0)
				{
					object->addRule(
						USER_PEDESTRIAN,
						PTUseRuleTableSync::Get(pedestrianComplianceId, env, linkLevel).get()
					);
				}
				if(reservationContactId > 0)
				{
					object->setReservationContact(
						ReservationContactTableSync::Get(reservationContactId, env, linkLevel).get()
					);
				}

			}
		}



		template<> void SQLiteDirectTableSyncTemplate<CommercialLineTableSync,CommercialLine>::Unlink(
			CommercialLine* obj
		){

		}



		template<> void SQLiteDirectTableSyncTemplate<CommercialLineTableSync,CommercialLine>::Save(CommercialLine* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() <= 0)
				object->setKey(getId());
			query
				<< "REPLACE INTO " << TABLE.NAME << " VALUES("
				<< Conversion::ToString(object->getKey())
				<< "," << (object->getNetwork() ? Conversion::ToString(object->getNetwork()->getKey()) : "0")
				<< "," << Conversion::ToSQLiteString(object->getName())
				<< "," << Conversion::ToSQLiteString(object->getShortName())
				<< "," << Conversion::ToSQLiteString(object->getLongName())
				<< "," << Conversion::ToSQLiteString(object->getColor().toString())
				<< "," << Conversion::ToSQLiteString(object->getStyle())
				<< "," << Conversion::ToSQLiteString(object->getImage())
				<< ",'"
			;
			bool first(true);
			BOOST_FOREACH(const PublicTransportStopZoneConnectionPlace* place, object->getOptionalReservationPlaces())
			{
				if (first)
				{
					first = false;
				}
				else
				{
					query << ",";
				}
				query << place->getKey();
			}
			query << "'"
				<< "," << Conversion::ToSQLiteString(object->getCreatorId())
				<< "," << (
					object->getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(object->getRule(USER_BIKE)) ? 
					lexical_cast<string>(static_cast<const PTUseRule*>(object->getRule(USER_BIKE))->getKey()) :
					"0")
				<< "," << (
					object->getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED)) ? 
					lexical_cast<string>(static_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED))->getKey()) :
					"0")
				<< "," << (
					object->getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN)) ? 
					lexical_cast<string>(static_cast<const PTUseRule*>(object->getRule(USER_PEDESTRIAN))->getKey()) :
					"0")
				<< "," << (
					object->getReservationContact() ? lexical_cast<string>(object->getReservationContact()->getKey()) : "0"
				)

				<< ")";
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
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
					query << " AND l." << COL_CREATOR_ID << " LIKE " << Conversion::ToSQLiteString(*creatorId);
			}
			if(name && *name != "%%" && *name != "%")
			{
				if (name->empty())
					query << " AND (l." << COL_NAME << " IS NULL OR l." << COL_NAME << "='')";
				else
					query << " AND l." << COL_NAME << " LIKE " << Conversion::ToSQLiteString(*name);
			}
			if (networkId && *networkId != UNKNOWN_VALUE)
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
				query << " LIMIT " << Conversion::ToString(*number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

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
			set<uid> allowedNetworks;
			set<uid> forbiddenNetworks;
			set<uid> allowedLines;
			set<uid> forbiddenLines;

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
				for (set<uid>::const_iterator it(forbiddenNetworks.begin()); it != forbiddenNetworks.end(); ++it)
				{
					if (it != forbiddenNetworks.begin())
						query << " AND ";
					query << CommercialLineTableSync::COL_NETWORK_ID << "!=" << *it;
				}
				query << ")";

				if (!allowedLines.empty())
				{
					query << " OR (";
					for (set<uid>::const_iterator it(allowedLines.begin()); it != allowedLines.end(); ++it)
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
					for (set<uid>::const_iterator it(allowedNetworks.begin()); it != allowedNetworks.end(); ++it)
						query << " OR " << COL_NETWORK_ID << "=" << *it;
				}
				if (!allowedLines.empty())
				{
					for (set<uid>::const_iterator it(allowedLines.begin()); it != allowedLines.end(); ++it)
						query << " OR " << TABLE_COL_ID << "=" << *it;
				}
				query << ")";
			}
			if (!forbiddenLines.empty())
			{
				for (set<uid>::const_iterator it(forbiddenLines.begin()); it != forbiddenLines.end(); ++it)
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
