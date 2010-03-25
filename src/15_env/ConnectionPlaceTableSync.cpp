////////////////////////////////////////////////////////////////////////////////
/// ConnectionPlaceTableSync class implementation.
///	@file ConnectionPlaceTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ConnectionPlaceTableSync.h"
#include "Conversion.h"
#include "ReplaceQuery.h"
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
	using namespace geography;
	using namespace road;

	template<> const string util::FactorableTemplate<SQLiteTableSync,env::ConnectionPlaceTableSync>::FACTORY_KEY("15.40.01 Connection places");
	template<> const string FactorableTemplate<Fetcher<NamedPlace>, ConnectionPlaceTableSync>::FACTORY_KEY("7");

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
		const string ConnectionPlaceTableSync::COL_CODE_BY_SOURCE("code_by_source");
		
		const string ConnectionPlaceTableSync::FORBIDDEN_DELAY_SYMBOL("F");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ConnectionPlaceTableSync>::TABLE(
			"t007_connection_places"
		);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<ConnectionPlaceTableSync>::_FIELDS[] =
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ConnectionPlaceTableSync::TABLE_COL_NAME, SQL_TEXT),
			SQLiteTableSync::Field(ConnectionPlaceTableSync::TABLE_COL_CITYID, SQL_INTEGER),
			SQLiteTableSync::Field(ConnectionPlaceTableSync::TABLE_COL_CONNECTIONTYPE, SQL_INTEGER),
			SQLiteTableSync::Field(ConnectionPlaceTableSync::TABLE_COL_ISCITYMAINCONNECTION, SQL_BOOLEAN),
			SQLiteTableSync::Field(ConnectionPlaceTableSync::TABLE_COL_DEFAULTTRANSFERDELAY, SQL_INTEGER),
			SQLiteTableSync::Field(ConnectionPlaceTableSync::TABLE_COL_TRANSFERDELAYS, SQL_TEXT),
			SQLiteTableSync::Field(ConnectionPlaceTableSync::COL_NAME13, SQL_TEXT),
			SQLiteTableSync::Field(ConnectionPlaceTableSync::COL_NAME26, SQL_TEXT),
			SQLiteTableSync::Field(ConnectionPlaceTableSync::COL_CODE_BY_SOURCE, SQL_TEXT),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<ConnectionPlaceTableSync>::_INDEXES[] =
		{
			SQLiteTableSync::Index(ConnectionPlaceTableSync::TABLE_COL_CITYID.c_str(), ConnectionPlaceTableSync::TABLE_COL_NAME.c_str(), ""),
			SQLiteTableSync::Index(ConnectionPlaceTableSync::COL_CODE_BY_SOURCE.c_str(), ""),
			SQLiteTableSync::Index()
		};


		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::Load(
			PublicTransportStopZoneConnectionPlace* cp,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Reading of the row
			string name (rows->getText (ConnectionPlaceTableSync::TABLE_COL_NAME));
			string name13(rows->getText(ConnectionPlaceTableSync::COL_NAME13));
			string name26(rows->getText(ConnectionPlaceTableSync::COL_NAME26));
			bool connectionType(rows->getBool(ConnectionPlaceTableSync::TABLE_COL_CONNECTIONTYPE));
			posix_time::time_duration defaultTransferDelay(posix_time::minutes(rows->getInt (ConnectionPlaceTableSync::TABLE_COL_DEFAULTTRANSFERDELAY)));
			string transferDelaysStr (rows->getText (ConnectionPlaceTableSync::TABLE_COL_TRANSFERDELAYS));
			string codeBySource(rows->getText(ConnectionPlaceTableSync::COL_CODE_BY_SOURCE));

			// Update of the object
			cp->setName (name);
			if (!name13.empty())
				cp->setName13(name13);
			if (!name26.empty())
				cp->setName26(name26);
			cp->setAllowedConnection(connectionType);
			
			cp->clearTransferDelays ();    
			cp->setDefaultTransferDelay (defaultTransferDelay);
			cp->setCodeBySource(codeBySource);

			typedef tokenizer<char_separator<char> > tokenizer;
			char_separator<char> sep1 (",");
			char_separator<char> sep2 (":");
			tokenizer tripletTokens (transferDelaysStr, sep1);
			for (tokenizer::iterator tripletIter = tripletTokens.begin();
				tripletIter != tripletTokens.end (); ++tripletIter)
			{
				tokenizer valueTokens (*tripletIter, sep2);
				tokenizer::iterator valueIter = valueTokens.begin();

				// departureRank:arrivalRank:transferDelay
				RegistryKeyType startStop(Conversion::ToLongLong(*valueIter));
				RegistryKeyType endStop(Conversion::ToLongLong(*(++valueIter)));
				const string delay(*(++valueIter));
				if(delay == ConnectionPlaceTableSync::FORBIDDEN_DELAY_SYMBOL)
				{
					cp->addForbiddenTransferDelay(startStop, endStop);
				}
				else
				{
					cp->addTransferDelay(startStop, endStop, posix_time::minutes(lexical_cast<long>(delay)));
				}
			}

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				cp->setCity(NULL);
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
						city->addPlaceToMatcher<PublicTransportStopZoneConnectionPlace>(cp);
					}
				}
				catch(ObjectNotFoundException<City>& e)
				{
					throw LinkException<ConnectionPlaceTableSync>(rows, ConnectionPlaceTableSync::TABLE_COL_CITYID, e);
				}
			}

		}

		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::Save(
			PublicTransportStopZoneConnectionPlace* object,
			optional<SQLiteTransaction&> transaction
		){
			// Transfer delay matrix
			stringstream delays;
			bool first(true);
			BOOST_FOREACH(const AddressablePlace::TransferDelaysMap::value_type& td, object->getTransferDelays())
			{
				if(!first) delays << ",";
				delays << td.first.first << ":" << td.first.second << ":";
				if(td.second.is_not_a_date_time())
				{
					delays << ConnectionPlaceTableSync::FORBIDDEN_DELAY_SYMBOL;
				}
				else
				{
					delays << (td.second.total_seconds() / 60);
				}
				first = false;
			}

			// The query
			ReplaceQuery<ConnectionPlaceTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getCity() ? object->getCity()->getKey() : RegistryKeyType(0));
			query.addField(object->getAllowedConnection());
			query.addField(object->getCity() ? object->getCity()->includes(object) : false);
			query.addField(object->getDefaultTransferDelay().total_seconds() / 60);
			query.addField(delays.str());
			query.addField(object->getName13());
			query.addField(object->getName26());
			query.addField(object->getCodeBySource());
			query.execute(transaction);
		}

		
		template<> void SQLiteDirectTableSyncTemplate<ConnectionPlaceTableSync,PublicTransportStopZoneConnectionPlace>::Unlink(
			PublicTransportStopZoneConnectionPlace* cp
		){
			City* city(const_cast<City*>(cp->getCity()));
			if (city != NULL)
			{
				city->removePlaceFromMatcher<PublicTransportStopZoneConnectionPlace>(cp);
			}
		}
	}

	namespace env
	{
		ConnectionPlaceTableSync::SearchResult ConnectionPlaceTableSync::Search(
			Env& env,
			optional<RegistryKeyType> cityId, /*= UNKNOWN_VALUE */
			logic::tribool mainConnectionOnly,
			optional<string> creatorIdFilter,
			optional<string> nameFilter,
			bool orderByCityNameAndName /*= true */
			, bool raisingOrder /*= true */
			, int first /*= 0 */
			, int number /*= 0 */,
			LinkLevel linkLevel
		){
			stringstream query;
			query <<
				" SELECT " <<
				TABLE.NAME << ".*" <<
				" FROM " << TABLE.NAME;
			if(orderByCityNameAndName)
			{
				query << " INNER JOIN " << CityTableSync::TABLE.NAME << " c ON c." << TABLE_COL_ID << "=" << TABLE.NAME << "." << TABLE_COL_CITYID;
			}

			// Filters
			query << " WHERE 1 ";
			if (cityId)
			{
				query << " AND " << TABLE_COL_CITYID << "=" << *cityId;
			}
			if (!logic::indeterminate(mainConnectionOnly))
			{
				query << " AND " << TABLE_COL_ISCITYMAINCONNECTION << "=" <<
					Conversion::ToString(mainConnectionOnly)
				;
			}
			if(creatorIdFilter)
			{
				query << " AND " << COL_CODE_BY_SOURCE << "=" << Conversion::ToSQLiteString(*creatorIdFilter);
			}
			if(nameFilter)
			{
				query << " AND " << TABLE_COL_NAME << " LIKE " << Conversion::ToSQLiteString(*nameFilter);
			}

			// Ordering
			if(orderByCityNameAndName)
			{
				query <<
					" ORDER BY " <<
					CityTableSync::TABLE.NAME << "." << CityTableSync::TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC") << "," <<
					TABLE.NAME << "." << TABLE_COL_NAME << (raisingOrder ? " ASC" : " DESC");
			}
			if (number > 0)
			{
				query << " LIMIT " << Conversion::ToString(number + 1);
				if (first > 0)
				{
					query << " OFFSET " << Conversion::ToString(first);
				}
			}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
