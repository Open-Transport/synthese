////////////////////////////////////////////////////////////////////////////////
/// StopAreaTableSync class implementation.
///	@file StopAreaTableSync.cpp
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

#include "StopAreaTableSync.hpp"
#include "ReplaceQuery.h"
#include "LinkException.h"
#include "CityTableSync.h"
#include "SelectQuery.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkRight.h"

#include <boost/tokenizer.hpp>
#include <assert.h>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace util;
	using namespace geography;
	using namespace road;
	using namespace impex;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync,pt::StopAreaTableSync>::FACTORY_KEY("35.40.01 Connection places");
	template<> const string FactorableTemplate<Fetcher<NamedPlace>, StopAreaTableSync>::FACTORY_KEY("7");

	namespace pt
	{
		const string StopAreaTableSync::TABLE_COL_NAME = "name";
		const string StopAreaTableSync::TABLE_COL_CITYID = "city_id";
		const string StopAreaTableSync::TABLE_COL_CONNECTIONTYPE = "connection_type";
		const string StopAreaTableSync::TABLE_COL_ISCITYMAINCONNECTION = "is_city_main_connection";
		const string StopAreaTableSync::TABLE_COL_DEFAULTTRANSFERDELAY = "default_transfer_delay";
		const string StopAreaTableSync::TABLE_COL_TRANSFERDELAYS = "transfer_delays";
		const string StopAreaTableSync::COL_NAME13("short_display_name");
		const string StopAreaTableSync::COL_NAME26("long_display_name");
		const string StopAreaTableSync::COL_CODE_BY_SOURCE("code_by_source");
		const string StopAreaTableSync::COL_TIMETABLE_NAME("timetable_name");

		const string StopAreaTableSync::FORBIDDEN_DELAY_SYMBOL("F");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<StopAreaTableSync>::TABLE(
			"t007_connection_places"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<StopAreaTableSync>::_FIELDS[] =
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(StopAreaTableSync::TABLE_COL_NAME, SQL_TEXT),
			DBTableSync::Field(StopAreaTableSync::TABLE_COL_CITYID, SQL_INTEGER),
			DBTableSync::Field(StopAreaTableSync::TABLE_COL_CONNECTIONTYPE, SQL_INTEGER),
			DBTableSync::Field(StopAreaTableSync::TABLE_COL_ISCITYMAINCONNECTION, SQL_BOOLEAN),
			DBTableSync::Field(StopAreaTableSync::TABLE_COL_DEFAULTTRANSFERDELAY, SQL_INTEGER),
			DBTableSync::Field(StopAreaTableSync::TABLE_COL_TRANSFERDELAYS, SQL_TEXT),
			DBTableSync::Field(StopAreaTableSync::COL_NAME13, SQL_TEXT),
			DBTableSync::Field(StopAreaTableSync::COL_NAME26, SQL_TEXT),
			DBTableSync::Field(StopAreaTableSync::COL_CODE_BY_SOURCE, SQL_TEXT),
			DBTableSync::Field(StopAreaTableSync::COL_TIMETABLE_NAME, SQL_TEXT),
			DBTableSync::Field()
		};

		template<> const DBTableSync::Index DBTableSyncTemplate<StopAreaTableSync>::_INDEXES[] =
		{
			DBTableSync::Index(StopAreaTableSync::TABLE_COL_CITYID.c_str(), StopAreaTableSync::TABLE_COL_NAME.c_str(), ""),
			DBTableSync::Index(StopAreaTableSync::COL_CODE_BY_SOURCE.c_str(), ""),
			DBTableSync::Index()
		};


		template<> void DBDirectTableSyncTemplate<StopAreaTableSync,StopArea>::Load(
			StopArea* cp,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Reading of the row
			string name (rows->getText (StopAreaTableSync::TABLE_COL_NAME));
			string name13(rows->getText(StopAreaTableSync::COL_NAME13));
			string name26(rows->getText(StopAreaTableSync::COL_NAME26));
			bool connectionType(rows->getBool(StopAreaTableSync::TABLE_COL_CONNECTIONTYPE));
			posix_time::time_duration defaultTransferDelay(posix_time::minutes(rows->getInt (StopAreaTableSync::TABLE_COL_DEFAULTTRANSFERDELAY)));
			string transferDelaysStr (rows->getText (StopAreaTableSync::TABLE_COL_TRANSFERDELAYS));

			// Update of the object
			cp->setName (name);
			if (!name13.empty())
				cp->setName13(name13);
			if (!name26.empty())
				cp->setName26(name26);
			cp->setTimetableName(rows->getText(StopAreaTableSync::COL_TIMETABLE_NAME));

			cp->setAllowedConnection(connectionType);

			cp->clearTransferDelays ();
			cp->setDefaultTransferDelay (defaultTransferDelay);

			cp->setDataSourceLinks(
				ImportableTableSync::GetDataSourceLinksFromSerializedString(
					rows->getText(StopAreaTableSync::COL_CODE_BY_SOURCE),
					env
			)	);

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
				if(delay == StopAreaTableSync::FORBIDDEN_DELAY_SYMBOL)
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
				util::RegistryKeyType cityId (rows->getLongLong (StopAreaTableSync::TABLE_COL_CITYID));

				try
				{
					cp->setCity(CityTableSync::Get(cityId, env, linkLevel).get());

//					if (temporary == GET_REGISTRY)
					{
						shared_ptr<City> city = CityTableSync::GetEditable (cp->getCity ()->getKey (), env, linkLevel);

						bool isCityMainConnection (	rows->getBool (StopAreaTableSync::TABLE_COL_ISCITYMAINCONNECTION));
						if (isCityMainConnection)
						{
							city->addIncludedPlace (cp);
						}
						else
						{
							city->removeIncludedPlace(cp);
						}
						city->addPlaceToMatcher<StopArea>(env.getEditableSPtr(cp));
					}
				}
				catch(ObjectNotFoundException<City>& e)
				{
					throw LinkException<StopAreaTableSync>(rows, StopAreaTableSync::TABLE_COL_CITYID, e);
				}
			}

		}

		template<> void DBDirectTableSyncTemplate<StopAreaTableSync,StopArea>::Save(
			StopArea* object,
			optional<DBTransaction&> transaction
		){
			// Transfer delay matrix
			stringstream delays;
			bool first(true);
			BOOST_FOREACH(const StopArea::TransferDelaysMap::value_type& td, object->getTransferDelays())
			{
				if(!first) delays << ",";
				delays << td.first.first << ":" << td.first.second << ":";
				if(td.second.is_not_a_date_time())
				{
					delays << StopAreaTableSync::FORBIDDEN_DELAY_SYMBOL;
				}
				else
				{
					delays << (td.second.total_seconds() / 60);
				}
				first = false;
			}

			// The query
			ReplaceQuery<StopAreaTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getCity() ? object->getCity()->getKey() : RegistryKeyType(0));
			query.addField(object->getAllowedConnection());
			query.addField(object->getCity() ? object->getCity()->includes(object) : false);
			query.addField(object->getDefaultTransferDelay().total_seconds() / 60);
			query.addField(delays.str());
			query.addField(object->getName13());
			query.addField(object->getName26());
			query.addField(ImportableTableSync::SerializeDataSourceLinks(object->getDataSourceLinks()));
			query.addField(object->getTimetableName());
			query.execute(transaction);
		}


		template<> void DBDirectTableSyncTemplate<StopAreaTableSync,StopArea>::Unlink(
			StopArea* cp
		){
			City* city(const_cast<City*>(cp->getCity()));
			if (city != NULL)
			{
				/// @todo make the remove : segfautl !!!!
//				city->removePlaceFromMatcher<StopArea>(cp);
				city->removeIncludedPlace(cp);
			}
		}



		template<> bool DBTableSyncTemplate<StopAreaTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<StopAreaTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			StopPointTableSync::SearchResult stops(StopPointTableSync::Search(env, id));
			BOOST_FOREACH(const StopPointTableSync::SearchResult::value_type& stop, stops)
			{
				StopPointTableSync::Remove(NULL, stop->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<StopAreaTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<StopAreaTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		StopAreaTableSync::SearchResult StopAreaTableSync::Search(
			Env& env,
			optional<RegistryKeyType> cityId,
			logic::tribool mainConnectionOnly,
			optional<string> creatorIdFilter,
			optional<string> nameFilter,
			optional<string> cityNameFilter,
			bool orderByCityNameAndName /*= true */
			, bool raisingOrder /*= true */
			, int first /*= 0 */
			, int number /*= 0 */,
			LinkLevel linkLevel
		){
			SelectQuery<StopAreaTableSync> query;
			if(orderByCityNameAndName || cityNameFilter)
			{
				query.addTableAndEqualJoin<CityTableSync>(TABLE_COL_ID, TABLE_COL_CITYID);
			}

			// Filters
			if (cityId)
			{
				query.addWhereField(TABLE_COL_CITYID, *cityId);
			}
			if (!logic::indeterminate(mainConnectionOnly))
			{
				query.addWhereField(TABLE_COL_ISCITYMAINCONNECTION, mainConnectionOnly);
			}
			if(creatorIdFilter)
			{
				query.addWhereField(COL_CODE_BY_SOURCE, *creatorIdFilter);
			}
			if(nameFilter)
			{
				query.addWhereField(TABLE_COL_NAME, *nameFilter, ComposedExpression::OP_LIKE);
			}
			if(cityNameFilter)
			{
				query.addWhereFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, *cityNameFilter, ComposedExpression::OP_LIKE);
			}

			// Ordering
			if(orderByCityNameAndName)
			{
				query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderField(TABLE_COL_NAME, raisingOrder);
			}
			if (number > 0)
			{
				query.setNumber(number + 1);
				if (first > 0)
				{
					query.setFirst(first);
				}
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
