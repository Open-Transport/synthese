
/** DBTransaction class implementation.
	@file DBTransaction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DBTransaction.hpp"
#include "DB.hpp"
#include "DBModule.h"

#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	namespace db
	{
		void DBTransaction::addQuery(const string& sql)
		{
			_queries.push_back(Query(sql));
		}



		void DBTransaction::addReplaceStmt( const DBRecord& record )
		{
			_queries.push_back(Query(record));
		}



		void DBTransaction::addDeleteStmt(util::RegistryKeyType id)
		{
			_queries.push_back(Query(id));
		}



		void DBTransaction::addDBModifEvent(const DB::DBModifEvent& modifEvent)
		{
			// Checks if the row was already modified by older queries in the transaction
			ModifiedRows::value_type key(make_pair(modifEvent.table, modifEvent.id));
			if(_modifiedRows.find(key) != _modifiedRows.end())
			{
				return;
			}

			// Adds the event in the list
			_modifEvents.push_back(modifEvent);

			// Keep the modification (delete or update) for further checks
			_modifiedRows.insert(key);
		}



		void DBTransaction::run(
		){
			if(_queries.empty())
			{
				return;
			}
			DBModule::GetDB()->execTransaction(*this);
			_queries.clear();
			_modifiedRows.clear();
		}



		const DBTransaction::Queries& DBTransaction::getQueries() const
		{
			return _queries;
		}



		const DBTransaction::DBModifEvents& DBTransaction::getDBModifEvents() const
		{
			return _modifEvents;
		}
}	}
