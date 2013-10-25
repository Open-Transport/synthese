
/** DBTransaction class header.
	@file DBTransaction.h

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

#ifndef SYNTHESE_db_DBTransaction_h__
#define SYNTHESE_db_DBTransaction_h__

#include "DB.hpp"
#include "DBRecord.hpp"

#include <boost/variant.hpp>
#include <vector>
#include <string>
#include <set>
#include <utility>

namespace synthese
{
	namespace db
	{
		/** DBTransaction class.
			@ingroup m10
		*/
		class DBTransaction
		{
		public:
			typedef boost::variant<
				std::string,
				DBRecord,
				util::RegistryKeyType
			> Query;
			typedef std::vector<Query> Queries;

			typedef std::vector<DB::DBModifEvent> DBModifEvents;

		private:
			typedef std::set<std::pair<std::string, util::RegistryKeyType> > ModifiedRows;

			Queries _queries;
			DBModifEvents _modifEvents;
			ModifiedRows _modifiedRows;
			const bool _withInterSYNTHESESync;

		public:
			DBTransaction(
				bool withInterSYNTHESESync = true
			);
			void addQuery(const std::string& query);
			void addReplaceStmt(const DBRecord& record);
			void addDeleteStmt(util::RegistryKeyType id);
			void addDBModifEvent(const DB::DBModifEvent& modifEvent);
			const Queries& getQueries() const;
			const DBModifEvents& getDBModifEvents() const;
			void run();
			bool getWithInterSYNTHESESync() const { return _withInterSYNTHESESync; }
		};
}	}

#endif // SYNTHESE_db_DBTransaction_h__
