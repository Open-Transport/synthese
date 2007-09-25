
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

#include "CommercialLineTableSync.h"

#include "15_env/CommercialLine.h"
#include "15_env/TransportNetworkTableSync.h"
#include "15_env/TransportNetwork.h"
#include "15_env/LineTableSync.h"
#include "15_env/Place.h"
#include "15_env/EnvModule.h"

#include "02_db/DBModule.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteException.h"

#include "01_util/Conversion.h"

#include "12_security/Constants.h"
#include "12_security/Right.h"

#include <sstream>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace security;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<CommercialLine>::TABLE_NAME = "t042_commercial_lines";
		template<> const int SQLiteTableSyncTemplate<CommercialLine>::TABLE_ID = 42;
		template<> const bool SQLiteTableSyncTemplate<CommercialLine>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<CommercialLine>::load(CommercialLine* object, const db::SQLiteResultSPtr& rows )
		{
		    object->setKey(rows->getLongLong (TABLE_COL_ID));

		    boost::shared_ptr<const TransportNetwork> tn = 
			TransportNetwork::Get (rows->getLongLong ( CommercialLineTableSync::COL_NETWORK_ID));
		    
		    object->setNetwork (tn.get ());
		    object->setName(rows->getText ( CommercialLineTableSync::COL_NAME));
		    object->setShortName(rows->getText ( CommercialLineTableSync::COL_SHORT_NAME));
		    object->setLongName(rows->getText ( CommercialLineTableSync::COL_LONG_NAME));
		    object->setColor(RGBColor(rows->getText ( CommercialLineTableSync::COL_COLOR)));
		    object->setStyle(rows->getText ( CommercialLineTableSync::COL_STYLE));
		    object->setImage(rows->getText ( CommercialLineTableSync::COL_IMAGE));

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
				shared_ptr<const Place> place(EnvModule::fetchPlace(id));
				object->addOptionalReservationPlace(place.get());
		    }

		}

		template<> void SQLiteTableSyncTemplate<CommercialLine>::save(CommercialLine* object)
		{
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					/// @todo fill fields [,]FIELD=VALUE
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId());
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					/// @todo fill other fields separated by ,
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace env
	{
		const std::string CommercialLineTableSync::COL_NETWORK_ID ("network_id");
		const std::string CommercialLineTableSync::COL_NAME ("name");
		const std::string CommercialLineTableSync::COL_SHORT_NAME ("short_name");
		const std::string CommercialLineTableSync::COL_LONG_NAME ("long_name");
		const std::string CommercialLineTableSync::COL_COLOR ("color");
		const std::string CommercialLineTableSync::COL_STYLE ("style");
		const std::string CommercialLineTableSync::COL_IMAGE ("image");
		const string CommercialLineTableSync::COL_OPTIONAL_RESERVATION_PLACES("optional_reservation_places");

		CommercialLineTableSync::CommercialLineTableSync()
			: SQLiteTableSyncTemplate<CommercialLine>(true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_NETWORK_ID, "INTEGER", true);
			addTableColumn(COL_NAME, "TEXT", true);
			addTableColumn(COL_SHORT_NAME, "TEXT", true);
			addTableColumn(COL_LONG_NAME, "TEXT", true);
			addTableColumn(COL_COLOR, "TEXT", true);
			addTableColumn(COL_STYLE, "TEXT", true);
			addTableColumn(COL_IMAGE, "TEXT", true);
			addTableColumn(COL_OPTIONAL_RESERVATION_PLACES, "TEXT", true);
		}

		void CommercialLineTableSync::rowsAdded(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
				CommercialLine* object(new CommercialLine());
				load(object, rows);
				object->store();
			}
		}

		void CommercialLineTableSync::rowsUpdated(db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				shared_ptr<CommercialLine> object= CommercialLine::GetUpdateable(rows->getLongLong (TABLE_COL_ID));
				load(object.get(), rows);
			}
		}

		void CommercialLineTableSync::rowsRemoved( db::SQLiteQueueThreadExec* sqlite,  db::SQLiteSync* sync, const db::SQLiteResultSPtr& rows )
		{
			while (rows->next ())
			{
				CommercialLine::Remove(rows->getLongLong (TABLE_COL_ID));
			}
		}

		std::vector<shared_ptr<CommercialLine> > CommercialLineTableSync::search(
			const TransportNetwork* network
			, std::string name
			, int first
			, int number
			, bool orderByNetwork
			, bool orderByName
			, bool raisingOrder
		){
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT l.*"
				<< " FROM " << TABLE_NAME << " AS l "
				<< " WHERE 1 ";
			if (network)
				query << " AND l." << COL_NETWORK_ID << "=" << network->getKey();
			if (name.empty())
				query << " AND l." << COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'";
			if (orderByNetwork)
				query << " ORDER BY "
					<< "(SELECT n." << TransportNetworkTableSync::COL_NAME << " FROM " << TransportNetworkTableSync::TABLE_NAME << " AS n WHERE n." << TABLE_COL_ID << "=l." << COL_NETWORK_ID << ")" << (raisingOrder ? " ASC" : " DESC")
					<< ",l." << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY l." << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<CommercialLine> > objects;
				while (rows->next ())
				{
					shared_ptr<CommercialLine> object(new CommercialLine());
					load(object.get(), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		std::vector<boost::shared_ptr<CommercialLine> > CommercialLineTableSync::search(
			const security::RightsOfSameClassMap& rights 
			, bool totalControl 
			, RightLevel neededLevel 
			, int first /*= 0 */
			, int number /*= 0 */
			, bool orderByNetwork /*= true */
			, bool orderByName /*= false */
			, bool raisingOrder /*= true */
			, bool mustBeBookable
		){
			SQLiteHandle* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< getSQLLinesList(rights, totalControl, neededLevel, mustBeBookable, "*");
			if (orderByNetwork)
				query << " ORDER BY "
				<< "(SELECT n." << TransportNetworkTableSync::COL_NAME << " FROM " << TransportNetworkTableSync::TABLE_NAME << " AS n WHERE n." << TABLE_COL_ID << "=" << TABLE_NAME << "." << COL_NETWORK_ID << ")" << (raisingOrder ? " ASC" : " DESC")
				<< "," << TABLE_NAME << "." << COL_SHORT_NAME << (raisingOrder ? " ASC" : " DESC");
			if (orderByName)
				query << " ORDER BY " << TABLE_NAME << "." << COL_NAME << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				vector<shared_ptr<CommercialLine> > objects;
				while (rows->next ())
				{
					shared_ptr<CommercialLine> object(new CommercialLine());
					load(object.get(), rows);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}

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
				if (decodeTableId(Conversion::ToLongLong(it->first)) == TransportNetworkTableSync::TABLE_ID)
				{
					if (it->second->getPublicRightLevel() < neededLevel)
						forbiddenNetworks.insert(Conversion::ToLongLong(it->first));
					else
						allowedNetworks.insert(Conversion::ToLongLong(it->first));
				}
				else if (decodeTableId(Conversion::ToLongLong(it->first)) == CommercialLineTableSync::TABLE_ID)
				{
					if (it->second->getPublicRightLevel() < neededLevel)
						forbiddenLines.insert(Conversion::ToLongLong(it->first));
					else
						allowedLines.insert(Conversion::ToLongLong(it->first));
				}
			}

			stringstream query;
			query << " SELECT " << selectedColumns << " FROM " << TABLE_NAME << " WHERE 1 ";

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
				query << " AND EXISTS(SELECT " << TABLE_COL_ID << " FROM " << LineTableSync::TABLE_NAME << " AS l WHERE l." << LineTableSync::COL_RESERVATIONRULEID << ">0 AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << TABLE_NAME << "." << TABLE_COL_ID << ")";
			}

			return query.str();
		}

	}
}
