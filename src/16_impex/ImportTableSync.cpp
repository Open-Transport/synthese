////////////////////////////////////////////////////////////////////////////////
///	Import table synchronizer class implementation.
///	@file ImportTableSync.cpp
///	@author Hugues Romain
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "ImportTableSync.hpp"

#include "SelectQuery.hpp"
#include "ReplaceQuery.h"
#include "CoordinatesSystem.hpp"
#include "InterSYNTHESEPackage.hpp"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace impex;
	using namespace inter_synthese;

	template<> const string util::FactorableTemplate<DBTableSync,ImportTableSync>::FACTORY_KEY(
		"16.01 Import"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ImportTableSync>::TABLE(
			"t105_imports"
		);

		template<> const Field DBTableSyncTemplate<ImportTableSync>::_FIELDS[] = { Field() }; // Defined by the record



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ImportTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<ImportTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level
			return true;
		}



		template<> void DBTableSyncTemplate<ImportTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			// Delete interSYNTHESEPackage corresponding to this import (if exists)
			BOOST_FOREACH(const InterSYNTHESEPackage::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<InterSYNTHESEPackage>())
			{
				// Variable
				const InterSYNTHESEPackage& package(*it.second);
				if (package.get<Import>() &&
					package.get<Import>()->getKey() == getKey())
				{
					DBTransaction transaction;
					boost::shared_ptr<DBTableSync> tableSync(
						DBModule::GetTableSync(decodeTableId(package.getKey()))
					);
					tableSync->deleteRecord(NULL, package.getKey(), transaction);
					transaction.run();
				}
			}
		}



		template<> void DBTableSyncTemplate<ImportTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ImportTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace impex
	{
		ImportTableSync::SearchResult ImportTableSync::Search(
			Env& env,
			string name,
			int first /*= 0*/
			, optional<size_t> number /*= 0*/
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<ImportTableSync> query;
			if (!name.empty())
			{
				query.addWhereField(
					Name::FIELD.name,
					"%"+ lexical_cast<string>(name) + "%",
					ComposedExpression::OP_LIKE
				);
			}
			if (orderByName)
			{
				query.addOrderField(Name::FIELD.name, raisingOrder);
			}
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
	}
}
