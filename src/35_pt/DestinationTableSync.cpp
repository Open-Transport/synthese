
//////////////////////////////////////////////////////////////////////////
///	DestinationTableSync class implementation.
///	@file DestinationTableSync.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "DestinationTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace pt;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DestinationTableSync>::FACTORY_KEY("35.26 Destinations");
	}

	namespace pt
	{
		const string DestinationTableSync::COL_COMMENT("comment");
		const string DestinationTableSync::COL_DISPLAYED_TEXT("displayed_text");
		const string DestinationTableSync::COL_TTS_TEXT("tts_text");
		const string DestinationTableSync::COL_DATA_SOURCE_LINKS("data_source_links");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DestinationTableSync>::TABLE(
			"t074_destinations"
		);



		template<> const Field DBTableSyncTemplate<DestinationTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(DestinationTableSync::COL_DISPLAYED_TEXT, SQL_TEXT),
			Field(DestinationTableSync::COL_TTS_TEXT, SQL_TEXT),
			Field(DestinationTableSync::COL_COMMENT, SQL_TEXT),
			Field(DestinationTableSync::COL_DATA_SOURCE_LINKS, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DestinationTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void OldLoadSavePolicy<DestinationTableSync,Destination>::Load(
			Destination* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setDisplayedText(rows->getText(DestinationTableSync::COL_DISPLAYED_TEXT));
			object->setTTSText(rows->getText(DestinationTableSync::COL_TTS_TEXT));
			object->setComment(rows->getText(DestinationTableSync::COL_COMMENT));

			object->setDataSourceLinksWithoutRegistration(
				ImportableTableSync::GetDataSourceLinksFromSerializedString(
					rows->getText(DestinationTableSync::COL_DATA_SOURCE_LINKS),
					env
			)	);
		}



		template<> void OldLoadSavePolicy<DestinationTableSync,Destination>::Save(
			Destination* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<DestinationTableSync> query(*object);
			query.addField(object->getDisplayedText());
			query.addField(object->getTTSText());
			query.addField(object->getComment());
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<DestinationTableSync,Destination>::Unlink(
			Destination* obj
		){
		}



		template<> bool DBTableSyncTemplate<DestinationTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<DestinationTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DestinationTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DestinationTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace pt
	{
		DestinationTableSync::SearchResult DestinationTableSync::Search(
			util::Env& env,
			// boost::optional<util::RegistryKeyType> parameterId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<DestinationTableSync> query;
			// if(parameterId)
			// {
			// 	query.addWhereField(COL_PARENT_ID, *parentFolderId);
			// }
			// if(orderByName)
			// {
			// 	query.addOrderField(COL_NAME, raisingOrder);
			// }
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}



		db::RowsList DestinationTableSync::SearchForAutoComplete(
				const boost::optional<std::string> prefix,
				const boost::optional<std::size_t> limit,
				const boost::optional<std::string> optionalParameter
		) const {
			RowsList result;

			SelectQuery<DestinationTableSync> query;
			Env env;
			if(prefix) query.addWhereField(DestinationTableSync::COL_DISPLAYED_TEXT, "%"+ *prefix +"%", ComposedExpression::OP_LIKE);
			if(limit) query.setNumber(*limit);
			DestinationTableSync::SearchResult destinations(DestinationTableSync::LoadFromQuery(query, env, UP_LINKS_LOAD_LEVEL));
			BOOST_FOREACH(const boost::shared_ptr<Destination>& destination, destinations)
			{
				result.push_back(std::make_pair(destination->getKey(), destination->getDisplayedText()));
			}
			return result;
		} ;
}	}
