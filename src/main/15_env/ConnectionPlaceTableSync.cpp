
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

#include "15_env/ConnectionPlace.h"
#include "15_env/City.h"
#include "15_env/EnvModule.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;

namespace synthese
{
	using namespace db;
	using namespace env;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<ConnectionPlace>::TABLE_NAME = "t007_connection_places";
		template<> const int SQLiteTableSyncTemplate<ConnectionPlace>::TABLE_ID = 7;
		template<> const bool SQLiteTableSyncTemplate<ConnectionPlace>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<ConnectionPlace>::load(ConnectionPlace* cp, const db::SQLiteResult& rows, int rowIndex/*=0*/ )
		{
			uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
			std::string name (	rows.getColumn (rowIndex, ConnectionPlaceTableSync::TABLE_COL_NAME));
			uid cityId (Conversion::ToLongLong (rows.getColumn (rowIndex, ConnectionPlaceTableSync::TABLE_COL_CITYID)));
			ConnectionPlace::ConnectionType connectionType = 
				(ConnectionPlace::ConnectionType) Conversion::ToInt (rows.getColumn (rowIndex, ConnectionPlaceTableSync::TABLE_COL_CONNECTIONTYPE));
			int defaultTransferDelay (	Conversion::ToInt (rows.getColumn (rowIndex, ConnectionPlaceTableSync::TABLE_COL_DEFAULTTRANSFERDELAY)));
			std::string transferDelaysStr (	rows.getColumn (rowIndex, ConnectionPlaceTableSync::TABLE_COL_TRANSFERDELAYS));
			uid alarmId (Conversion::ToLongLong (rows.getColumn (rowIndex, ConnectionPlaceTableSync::TABLE_COL_ALARMID)));

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;


			cp->setName (name);
			cp->setConnectionType (connectionType);

			cp->clearTransferDelays ();    

			boost::char_separator<char> sep1 (",");
			boost::char_separator<char> sep2 (":");
			tokenizer tripletTokens (transferDelaysStr, sep1);
			for (tokenizer::iterator tripletIter = tripletTokens.begin();
				tripletIter != tripletTokens.end (); ++tripletIter)
			{
				tokenizer valueTokens (*tripletIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();

				// (departureRank:arrivalRank:transferDelay)
				cp->addTransferDelay (Conversion::ToInt (*valueIter), 
					Conversion::ToInt (*(++valueIter)),
					Conversion::ToInt (*(++valueIter)));
			}

			//    cp->setAlarm (environment.getAlarms ().get (alarmId));
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
		: SQLiteTableSyncTemplate<ConnectionPlace> (TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn (TABLE_COL_ID, "INTEGER", true);
			addTableColumn (TABLE_COL_NAME, "TEXT", true);
			addTableColumn (TABLE_COL_CITYID, "INTEGER", false);
			addTableColumn (TABLE_COL_CONNECTIONTYPE, "INTEGER", true);
			addTableColumn (TABLE_COL_ISCITYMAINCONNECTION, "BOOLEAN", false);
			addTableColumn (TABLE_COL_DEFAULTTRANSFERDELAY, "INTEGER", true);
			addTableColumn (TABLE_COL_TRANSFERDELAYS, "TEXT", true);
			addTableColumn (TABLE_COL_ALARMID, "INTEGER", true);
		}



		ConnectionPlaceTableSync::~ConnectionPlaceTableSync ()
		{

		}

		    
		void 
			ConnectionPlaceTableSync::rowsAdded (const synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResult& rows)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows (); ++rowIndex)
			{
				uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

				if (EnvModule::getConnectionPlaces ().contains (id)) return;

				std::string name (rows.getColumn (rowIndex, TABLE_COL_NAME));
				uid cityId (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_CITYID)));
				ConnectionPlace::ConnectionType connectionType = (ConnectionPlace::ConnectionType)
				Conversion::ToInt (rows.getColumn (rowIndex, TABLE_COL_CONNECTIONTYPE));
				bool isCityMainConnection (	Conversion::ToBool (rows.getColumn (rowIndex, TABLE_COL_ISCITYMAINCONNECTION)));
				int defaultTransferDelay (	Conversion::ToInt (rows.getColumn (rowIndex, TABLE_COL_DEFAULTTRANSFERDELAY)));
				std::string transferDelaysStr (	rows.getColumn (rowIndex, TABLE_COL_TRANSFERDELAYS));
				uid alarmId (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ALARMID)));

				typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

				City* city = EnvModule::getCities ().get (cityId);
				ConnectionPlace* cp = new synthese::env::ConnectionPlace (
					id, name, city, connectionType, defaultTransferDelay);

				boost::char_separator<char> sep1 (",");
				boost::char_separator<char> sep2 (":");
				tokenizer tripletTokens (transferDelaysStr, sep1);
				for (tokenizer::iterator tripletIter = tripletTokens.begin();
					tripletIter != tripletTokens.end (); ++tripletIter)
				{
					tokenizer valueTokens (*tripletIter, sep2);
					tokenizer::iterator valueIter = valueTokens.begin();

					// (departureRank:arrivalRank:transferDelay)
					cp->addTransferDelay (Conversion::ToInt (*valueIter), 
								Conversion::ToInt (*(++valueIter)),
								Conversion::ToInt (*(++valueIter)));
				}

				if (isCityMainConnection)
				{
					city->addIncludedPlace (cp);
				}

			//    cp->setAlarm (environment.getAlarms ().get (alarmId));

				city->getConnectionPlacesMatcher ().add (cp->getName (), cp);
				EnvModule::getConnectionPlaces ().add (cp);
			}
		}



		void 
			ConnectionPlaceTableSync::rowsUpdated (const synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResult& rows)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows (); ++rowIndex)
			{
				ConnectionPlace* cp = EnvModule::getConnectionPlaces().get (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
				City* city = EnvModule::getCities ().get (cp->getCity ()->getKey ());
				city->getConnectionPlacesMatcher ().remove (cp->getName ());

				load(cp, rows, rowIndex);

				city->getConnectionPlacesMatcher ().add (cp->getName (), cp);
			}
		}





		void 
			ConnectionPlaceTableSync::rowsRemoved (const synthese::db::SQLiteQueueThreadExec* sqlite, 
			synthese::db::SQLiteSync* sync,
			const synthese::db::SQLiteResult& rows)
		{
			for (int rowIndex=0; rowIndex<rows.getNbRows (); ++rowIndex)
			{
				// TODO not finished...
				uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));

				ConnectionPlace* cp = EnvModule::getConnectionPlaces ().get (id);
				City* city = EnvModule::getCities ().get (cp->getCity ()->getKey ());
				city->getConnectionPlacesMatcher ().remove (cp->getName ());

				EnvModule::getConnectionPlaces ().remove (id);
			}
		}


	}
}
