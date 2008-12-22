
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

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace security;

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
	}

	namespace db
	{
		const SQLiteTableFormat CommercialLineTableSync::TABLE(
			CommercialLineTableSync::CreateFormat(
				"t042_commercial_lines",
				SQLiteTableFormat::CreateFields(
					SQLiteTableFormat::Field(CommercialLineTableSync::COL_NETWORK_ID, INTEGER),
					SQLiteTableFormat::Field(CommercialLineTableSync::COL_NAME, TEXT),
					SQLiteTableFormat::Field(CommercialLineTableSync::COL_SHORT_NAME, TEXT),
					SQLiteTableFormat::Field(CommercialLineTableSync::COL_LONG_NAME, TEXT),
					SQLiteTableFormat::Field(CommercialLineTableSync::COL_COLOR, TEXT),
					SQLiteTableFormat::Field(CommercialLineTableSync::COL_STYLE, TEXT),
					SQLiteTableFormat::Field(CommercialLineTableSync::COL_IMAGE, TEXT),
					SQLiteTableFormat::Field(CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES, TEXT),
					SQLiteTableFormat::Field()
				), SQLiteTableFormat::Indexes(
		)	)	);

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

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				const TransportNetwork* tn = 
					TransportNetworkTableSync::Get (rows->getLongLong ( CommercialLineTableSync::COL_NETWORK_ID), env, linkLevel).get();

				object->setNetwork (tn);

//				if (temporary == GET_REGISTRY)
				{
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
							shared_ptr<const Place> place(EnvModule::FetchPlace(id, env));
							object->addOptionalReservationPlace(place.get());
					}
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
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE.NAME << " SET "
					/// @todo fill fields [,]FIELD=VALUE
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId());
                query
					<< " INSERT INTO " << TABLE.NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					/// @todo fill other fields separated by ,
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		CommercialLineTableSync::CommercialLineTableSync()
			: SQLiteRegistryTableSyncTemplate<CommercialLineTableSync,CommercialLine>()
		{
		}


		void CommercialLineTableSync::Search(
			Env& env,
			uid networkId
			, std::string name
			, int first
			, int number
			, bool orderByNetwork
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT l.*"
				<< " FROM " << TABLE.NAME << " AS l "
				<< " WHERE 1 ";
			if (networkId != UNKNOWN_VALUE)
				query << " AND l." << COL_NETWORK_ID << "=" << networkId;
			if (name.empty())
				query << " AND l." << COL_NAME << " LIKE " << Conversion::ToSQLiteString(name);
			if (orderByNetwork)
				query << " ORDER BY "
					<< "(SELECT n." << TransportNetworkTableSync::COL_NAME << " FROM " << TransportNetworkTableSync::TABLE.NAME << " AS n WHERE n." << TABLE_COL_ID << "=l." << COL_NETWORK_ID << ")" << (raisingOrder ? " ASC" : " DESC")
					<< ",l." << COL_SHORT_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY l." << COL_SHORT_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}

		void CommercialLineTableSync::Search(
			Env& env,
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel 
			, int first /*= 0 */
			, int number /*= 0 */
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
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
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
				query << " AND EXISTS(SELECT " << TABLE_COL_ID << " FROM " << LineTableSync::TABLE.NAME << " AS l WHERE l." << LineTableSync::COL_RESERVATIONRULEID << ">0 AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << TABLE.NAME << "." << TABLE_COL_ID << ")";
			}

			return query.str();
		}
	}
}
