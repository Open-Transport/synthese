
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
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/City.h"
#include "15_env/PublicTransportStopZoneConnectionPlace.h"
#include "15_env/EnvModule.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>
#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace env;
	using namespace util;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<PublicTransportStopZoneConnectionPlace>::TABLE_NAME = "t007_connection_places";
		template<> const int SQLiteTableSyncTemplate<PublicTransportStopZoneConnectionPlace>::TABLE_ID = 7;
		template<> const bool SQLiteTableSyncTemplate<PublicTransportStopZoneConnectionPlace>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<PublicTransportStopZoneConnectionPlace>::load(PublicTransportStopZoneConnectionPlace* cp, const db::SQLiteResultSPtr& rows )
		{
			uid id (rows->getLongLong (TABLE_COL_ID));
			std::string name (rows->getText (ConnectionPlaceTableSync::TABLE_COL_NAME));
			uid cityId (rows->getLongLong (ConnectionPlaceTableSync::TABLE_COL_CITYID));
			
			ConnectionPlace::ConnectionType connectionType = 
			    static_cast<ConnectionPlace::ConnectionType>(rows->getInt (ConnectionPlaceTableSync::TABLE_COL_CONNECTIONTYPE));
			
			int defaultTransferDelay (rows->getInt (ConnectionPlaceTableSync::TABLE_COL_DEFAULTTRANSFERDELAY));
			
			std::string transferDelaysStr (rows->getText (ConnectionPlaceTableSync::TABLE_COL_TRANSFERDELAYS));
			uid alarmId (rows->getLongLong (ConnectionPlaceTableSync::TABLE_COL_ALARMID));

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

			cp->setKey(id);
			cp->setName (name);
			cp->setCity(EnvModule::getCities().get(cityId).get());
			cp->setConnectionType (connectionType);
			cp->setDefaultTransferDelay (defaultTransferDelay);

			cp->clearTransferDelays ();    

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
	}

	namespace env
	{
		const std::string ConnectionPlaceTableSync::TABLE_COL_NAME = "name";
		const std::string ConnectionPlaceTableSync::TABLE_COL_CITYID = "city_id";
		const std::string ConnectionPlaceTableSync::TABLE_COL_CONNECTIONTYPE = "connection_type";
		const std::string ConnectionPlaceTableSync::TABLE_COL_ISCITYMAINCONNECTION = "is_city_main_connection";
		const std::string ConnectionPlaceTableSync::TABLE_COL_DEFAULTTRANSFERDELAY = "default_transfer_delay";
		const std::string ConnectionPlaceTableSync::TABLE_COL_TRANSFERDELAYS = "transfer_delays";
		const std::string ConnectionPlaceTableSync::TABLE_COL_ALARMID = "alarm_id";


		ConnectionPlaceTableSync::ConnectionPlaceTableSync ()
		: SQLiteTableSyncTemplate<PublicTransportStopZoneConnectionPlace> (true, false, db::TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (TABLE_COL_ID, "INTEGER", true);
			addTableColumn (TABLE_COL_NAME, "TEXT", true);
			addTableColumn (TABLE_COL_CITYID, "INTEGER", false);
			addTableColumn (TABLE_COL_CONNECTIONTYPE, "INTEGER", true);
			addTableColumn (TABLE_COL_ISCITYMAINCONNECTION, "BOOLEAN", false);
			addTableColumn (TABLE_COL_DEFAULTTRANSFERDELAY, "INTEGER", true);
			addTableColumn (TABLE_COL_TRANSFERDELAYS, "TEXT", true);
			addTableColumn (TABLE_COL_ALARMID, "INTEGER", true);

			vector<string> c;
			c.push_back(TABLE_COL_CITYID);
			c.push_back(TABLE_COL_NAME);
			addTableIndex(c);
		}



		ConnectionPlaceTableSync::~ConnectionPlaceTableSync ()
		{

		}

		    
		void 
			ConnectionPlaceTableSync::rowsAdded (synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows, bool isFirstSync)
		{
			while (rows->next ())
			{
			    uid id (rows->getLongLong (TABLE_COL_ID));
			    
			    if (EnvModule::getPublicTransportStopZones().contains (id)) return;
			    
			    std::string name (rows->getText (TABLE_COL_NAME));
			    uid cityId (rows->getLongLong (TABLE_COL_CITYID));
			    ConnectionPlace::ConnectionType connectionType = (ConnectionPlace::ConnectionType)
				rows->getInt (TABLE_COL_CONNECTIONTYPE);
			    
			    bool isCityMainConnection (	rows->getBool (TABLE_COL_ISCITYMAINCONNECTION));
			    
			    int defaultTransferDelay (	rows->getInt (TABLE_COL_DEFAULTTRANSFERDELAY));
			    std::string transferDelaysStr (rows->getText (TABLE_COL_TRANSFERDELAYS));
			    uid alarmId (rows->getLongLong (TABLE_COL_ALARMID));

			    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			    
			    shared_ptr<City> city = EnvModule::getCities ().getUpdateable (cityId);
			    shared_ptr<PublicTransportStopZoneConnectionPlace> cp(
					new PublicTransportStopZoneConnectionPlace(id, name, city.get(), connectionType, defaultTransferDelay)
				);

			    boost::char_separator<char> sep1 (",");
			    boost::char_separator<char> sep2 (":");
			    tokenizer tripletTokens (transferDelaysStr, sep1);
			    for (tokenizer::iterator tripletIter = tripletTokens.begin();
				 tripletIter != tripletTokens.end (); ++tripletIter)
				{
					tokenizer valueTokens (*tripletIter, sep2);
					tokenizer::iterator valueIter = valueTokens.begin();

					// (departureRank:arrivalRank:transferDelay)
					uid startStop(Conversion::ToLongLong(*valueIter));
					uid endStop(Conversion::ToLongLong(*(++valueIter)));
					cp->addTransferDelay (startStop, endStop, Conversion::ToInt (*(++valueIter)));
				}

				if (isCityMainConnection)
				{
					city->addIncludedPlace (cp.get());
				}

			//    cp->setAlarm (environment.getAlarms ().get (alarmId));

				city->getConnectionPlacesMatcher ().add (cp->getName (), cp.get());
				EnvModule::getPublicTransportStopZones().add (cp);
			}
		}



		void 
			ConnectionPlaceTableSync::rowsUpdated (synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				shared_ptr<PublicTransportStopZoneConnectionPlace> cp = EnvModule::getPublicTransportStopZones().getUpdateable(
				    rows->getLongLong (TABLE_COL_ID)
				);
				
				shared_ptr<City> city = EnvModule::getCities ().getUpdateable (cp->getCity ()->getKey ());
				city->getConnectionPlacesMatcher ().remove (cp->getName ());
				
				load(cp.get(), rows);
				city->getConnectionPlacesMatcher ().add (cp->getName (), cp.get());
			}
		}





		void 
			ConnectionPlaceTableSync::rowsRemoved (synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResultSPtr& rows)
		{
			while (rows->next ())
			{
				// TODO not finished...
			    uid id = rows->getLongLong (TABLE_COL_ID);
			    
			    shared_ptr<const ConnectionPlace> cp = EnvModule::getPublicTransportStopZones().get (id);
			    shared_ptr<City> city = EnvModule::getCities ().getUpdateable (cp->getCity ()->getKey ());
			    city->getConnectionPlacesMatcher ().remove (cp->getName ());
			    
			    EnvModule::getPublicTransportStopZones().remove (id);
			}
		}
	    

	}
}
