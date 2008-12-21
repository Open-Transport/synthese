
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

#include "Conversion.h"

#include "SQLiteResult.h"
#include "SQLite.h"
#include "LinkException.h"

#include "CityTableSync.h"

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
	}

	namespace db
	{
		template<> const SQLiteTableFormat SQLiteTableSyncTemplate<ConnectionPlaceTableSync>::TABLE(
			ConnectionPlaceTableSync::CreateFormat(
				"t007_connection_places",
				SQLiteTableFormat::CreateFields(
					SQLiteTableFormat::Field(ConnectionPlaceTableSync::TABLE_COL_NAME, TEXT),
					SQLiteTableFormat::Field(ConnectionPlaceTableSync::TABLE_COL_CITYID, INTEGER),
					SQLiteTableFormat::Field(ConnectionPlaceTableSync::TABLE_COL_CONNECTIONTYPE, INTEGER),
					SQLiteTableFormat::Field(ConnectionPlaceTableSync::TABLE_COL_ISCITYMAINCONNECTION, BOOLEAN),
					SQLiteTableFormat::Field(ConnectionPlaceTableSync::TABLE_COL_DEFAULTTRANSFERDELAY, INTEGER),
					SQLiteTableFormat::Field(ConnectionPlaceTableSync::TABLE_COL_TRANSFERDELAYS, TEXT),
					SQLiteTableFormat::Field(ConnectionPlaceTableSync::COL_NAME13, TEXT),
					SQLiteTableFormat::Field(ConnectionPlaceTableSync::COL_NAME26, TEXT),
					SQLiteTableFormat::Field()
				), SQLiteTableFormat::CreateIndexes(
					SQLiteTableFormat::Index(
						"cityname",
						SQLiteTableFormat::Index::CreateFieldsList(
							ConnectionPlaceTableSync::TABLE_COL_CITYID,
							ConnectionPlaceTableSync::TABLE_COL_NAME,
							string()
					)	),
					SQLiteTableFormat::Index()
		)	)	);



		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::Load(
			PublicTransportStopZoneConnectionPlace* cp,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			// Reading of the row
			string name (rows->getText (ConnectionPlaceTableSync::TABLE_COL_NAME));
			string name13(rows->getText(ConnectionPlaceTableSync::COL_NAME13));
			string name26(rows->getText(ConnectionPlaceTableSync::COL_NAME26));
			ConnectionPlace::ConnectionType connectionType = 
			    static_cast<ConnectionPlace::ConnectionType>(rows->getInt (ConnectionPlaceTableSync::TABLE_COL_CONNECTIONTYPE));
			int defaultTransferDelay (rows->getInt (ConnectionPlaceTableSync::TABLE_COL_DEFAULTTRANSFERDELAY));
			string transferDelaysStr (rows->getText (ConnectionPlaceTableSync::TABLE_COL_TRANSFERDELAYS));

			// Update of the object
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

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				uid cityId (rows->getLongLong (ConnectionPlaceTableSync::TABLE_COL_CITYID));

				try
				{
					cp->setCity(CityTableSync::Get(cityId, env, linkLevel).get());

//					if (temporary == GET_REGISTRY)
					{
						shared_ptr<City> city = CityTableSync::GetEditable (cp->getCity ()->getKey (), env, linkLevel);

						bool isCityMainConnection (	rows->getBool (ConnectionPlaceTableSync::TABLE_COL_ISCITYMAINCONNECTION));
						if (isCityMainConnection)
						{
							city->addIncludedPlace (cp);
						}
						city->getConnectionPlacesMatcher ().add (cp->getName (), cp);
						city->getAllPlacesMatcher().add(cp->getName() + " [arrêt]", static_cast<Place*>(cp));
					}
				}
				catch(ObjectNotFoundException<City>& e)
				{
					throw LinkException<ConnectionPlaceTableSync>(cp->getKey(), ConnectionPlaceTableSync::TABLE_COL_CITYID, e);
				}
			}

		}

		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::Save(PublicTransportStopZoneConnectionPlace* obj)
		{

		}

		
		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::Unlink(
			PublicTransportStopZoneConnectionPlace* cp,
			Env* env
		){
			City* city(const_cast<City*>(cp->getCity()));
			if (city != NULL)
			{
				city->getConnectionPlacesMatcher().remove (cp->getName ());
				city->getAllPlacesMatcher().remove(cp->getName() + " [arrêt]");
				cp->setCity(NULL);
			}
		}
	}

	namespace env
	{
		ConnectionPlaceTableSync::ConnectionPlaceTableSync ()
		: SQLiteRegistryTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace> ()
		{
		}



		ConnectionPlaceTableSync::~ConnectionPlaceTableSync ()
		{

		}



		void ConnectionPlaceTableSync::Search(
			Env& env,
			RegistryKeyType cityId /*= UNKNOWN_VALUE */
			, logic::tribool mainConnectionOnly
			, ConnectionPlace::ConnectionType minConnectionType
			, bool orderByCityNameAndName /*= true */
			, bool raisingOrder /*= true */
			, int first /*= 0 */
			, int number /*= 0 */,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
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

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
