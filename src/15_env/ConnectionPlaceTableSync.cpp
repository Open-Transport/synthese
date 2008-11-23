
/** ConnectionPlaceTableSync class implementation.
	@file ConnectionPlaceTableSync.cpp

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

#include "ConnectionPlaceTableSync.h"

#include "01_util/Conversion.h"

#include "02_db/SQLiteResult.h"
#include "02_db/SQLite.h"
#include "02_db/LinkException.h"

#include "15_env/CityTableSync.h"

#include <boost/tokenizer.hpp>
#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace env;
	using namespace util;

	template<> const string util::FactorableTemplate<SQLiteTableSync,env::ConnectionPlaceTableSync>::FACTORY_KEY("15.40.01 Connection places");

	namespace db
	{
		template<> const string SQLiteTableSyncTemplate<ConnectionPlaceTableSync>::TABLE_NAME = "t007_connection_places";
		template<> const int SQLiteTableSyncTemplate<ConnectionPlaceTableSync>::TABLE_ID = 7;
		template<> const bool SQLiteTableSyncTemplate<ConnectionPlaceTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::load(PublicTransportStopZoneConnectionPlace* cp, const db::SQLiteResultSPtr& rows )
		{
			// Reading of the row
			uid id (rows->getLongLong (TABLE_COL_ID));
			string name (rows->getText (ConnectionPlaceTableSync::TABLE_COL_NAME));
			string name13(rows->getText(ConnectionPlaceTableSync::COL_NAME13));
			string name26(rows->getText(ConnectionPlaceTableSync::COL_NAME26));
			ConnectionPlace::ConnectionType connectionType = 
			    static_cast<ConnectionPlace::ConnectionType>(rows->getInt (ConnectionPlaceTableSync::TABLE_COL_CONNECTIONTYPE));
			int defaultTransferDelay (rows->getInt (ConnectionPlaceTableSync::TABLE_COL_DEFAULTTRANSFERDELAY));
			string transferDelaysStr (rows->getText (ConnectionPlaceTableSync::TABLE_COL_TRANSFERDELAYS));

			// Update of the object
			cp->setKey(id);
			cp->setName (name);
			if (!name13.empty())
				cp->setName13(name13);
			if (!name26.empty())
				cp->setName26(name26);
			cp->setConnectionType (connectionType);
			
			cp->clearTransferDelays ();    
			cp->setDefaultTransferDelay (defaultTransferDelay);

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep1 (",");
			boost::char_separator<char> sep2 (":");
			tokenizer tripletTokens (transferDelaysStr, sep1);
			for (tokenizer::iterator tripletIter = tripletTokens.begin();
				tripletIter != tripletTokens.end (); ++tripletIter)
			{
				tokenizer valueTokens (*tripletIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();

				// departureRank:arrivalRank:transferDelay
				uid startStop(Conversion::ToLongLong(*valueIter));
				uid endStop(Conversion::ToLongLong(*(++valueIter)));
				cp->addTransferDelay (startStop, endStop, Conversion::ToInt (*(++valueIter)));
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::save(PublicTransportStopZoneConnectionPlace* obj)
		{

		}

		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::_link(PublicTransportStopZoneConnectionPlace* cp, const SQLiteResultSPtr& rows, GetSource temporary)
		{
			uid cityId (rows->getLongLong (ConnectionPlaceTableSync::TABLE_COL_CITYID));

			try
			{
				cp->setCity(CityTableSync::Get(cityId,cp,true,temporary));

				if (temporary == GET_REGISTRY)
				{
					shared_ptr<City> city = City::GetUpdateable (cp->getCity ()->getKey ());

					bool isCityMainConnection (	rows->getBool (ConnectionPlaceTableSync::TABLE_COL_ISCITYMAINCONNECTION));
					if (isCityMainConnection)
					{
						city->addIncludedPlace (cp);
					}
					city->getConnectionPlacesMatcher ().add (cp->getName (), cp);
					city->getAllPlacesMatcher().add(cp->getName() + " [arrêt]", static_cast<Place*>(cp));
				}
			}
			catch(City::ObjectNotFoundException& e)
			{
				throw LinkException<ConnectionPlaceTableSync>(cp->getKey(), ConnectionPlaceTableSync::TABLE_COL_CITYID, e);
			}
		}

		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::_unlink(PublicTransportStopZoneConnectionPlace* cp)
		{
			shared_ptr<City> city = City::GetUpdateable (cp->getCity ()->getKey ());

			city->getConnectionPlacesMatcher ().remove (cp->getName ());
			city->getAllPlacesMatcher().remove(cp->getName() + " [arrêt]");

			cp->setCity(NULL);
		}
	}

	namespace env
	{
		const string ConnectionPlaceTableSync::TABLE_COL_NAME = "name";
		const string ConnectionPlaceTableSync::TABLE_COL_CITYID = "city_id";
		const string ConnectionPlaceTableSync::TABLE_COL_CONNECTIONTYPE = "connection_type";
		const string ConnectionPlaceTableSync::TABLE_COL_ISCITYMAINCONNECTION = "is_city_main_connection";
		const string ConnectionPlaceTableSync::TABLE_COL_DEFAULTTRANSFERDELAY = "default_transfer_delay";
		const string ConnectionPlaceTableSync::TABLE_COL_TRANSFERDELAYS = "transfer_delays";
		const string ConnectionPlaceTableSync::COL_NAME13("short_display_name");
		const string ConnectionPlaceTableSync::COL_NAME26("long_display_name");


		ConnectionPlaceTableSync::ConnectionPlaceTableSync ()
		: SQLiteRegistryTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace> ()
		{
			addTableColumn (TABLE_COL_ID, "INTEGER", true);
			addTableColumn (TABLE_COL_NAME, "TEXT", true);
			addTableColumn (TABLE_COL_CITYID, "INTEGER", false);
			addTableColumn (TABLE_COL_CONNECTIONTYPE, "INTEGER", true);
			addTableColumn (TABLE_COL_ISCITYMAINCONNECTION, "BOOLEAN", false);
			addTableColumn (TABLE_COL_DEFAULTTRANSFERDELAY, "INTEGER", true);
			addTableColumn (TABLE_COL_TRANSFERDELAYS, "TEXT", true);
			addTableColumn (COL_NAME13, "TEXT");
			addTableColumn (COL_NAME26, "TEXT");

			vector<string> c;
			c.push_back(TABLE_COL_CITYID);
			c.push_back(TABLE_COL_NAME);
			addTableIndex(c);
		}



		ConnectionPlaceTableSync::~ConnectionPlaceTableSync ()
		{

		}



		ConnectionPlaceTableSync::SearchResult ConnectionPlaceTableSync::Search(
			uid cityId /*= UNKNOWN_VALUE */
			, logic::tribool mainConnectionOnly
			, ConnectionPlace::ConnectionType minConnectionType
			, bool orderByCityNameAndName /*= true */
			, bool raisingOrder /*= true */
			, int first /*= 0 */
			, int number /*= 0 */
		){
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE "
				<< TABLE_COL_CONNECTIONTYPE << ">=" << static_cast<int>(minConnectionType);
			if (cityId != UNKNOWN_VALUE)
				query << " AND " << TABLE_COL_CITYID << "=" << cityId;
			if (!logic::indeterminate(mainConnectionOnly))
				query << " AND " << TABLE_COL_ISCITYMAINCONNECTION << "=" << Conversion::ToString(mainConnectionOnly);
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			try
			{
				SQLiteResultSPtr rows = sqlite->execQuery(query.str());
				SearchResult objects;
				while (rows->next ())
				{
					shared_ptr<PublicTransportStopZoneConnectionPlace> object(new PublicTransportStopZoneConnectionPlace());
					load(object.get(), rows);
					link(object.get(), rows, GET_AUTO);
					objects.push_back(object);
				}
				return objects;
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
