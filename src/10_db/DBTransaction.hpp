
/** DBTransaction class header.
	@file DBTransaction.h

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

#ifndef SYNTHESE_db_DBTransaction_h__
#define SYNTHESE_db_DBTransaction_h__

#include "10_db/DB.hpp"

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
			typedef std::vector<std::string> Queries;
			typedef std::vector<DB::DBModifEvent> DBModifEvents;

		private:
			typedef std::set<std::pair<std::string, util::RegistryKeyType> > DeletedRows;

			Queries _queries;
			DBModifEvents _modifEvents;
			DeletedRows _deletedRows;

		public:
			void addQuery(const std::string& query);
			void addDBModifEvent(const DB::DBModifEvent& modifEvent);
			const Queries getQueries() const;
			const DBModifEvents getDBModifEvents() const;
			void run();
		};
	}
}

#endif // SYNTHESE_db_DBTransaction_h__
