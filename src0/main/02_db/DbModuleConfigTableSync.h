
/** DbModuleConfigTableSync class header.
	@file DbModuleConfigTableSync.h

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


#ifndef SYNTHESE_DB_DBMODULECONFIGTABLESYNC_H
#define SYNTHESE_DB_DBMODULECONFIGTABLESYNC_H


#include <string>
#include <iostream>


#include "02_db/SQLiteTableSync.h"
#include "02_db/SQLiteResult.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{

	namespace db
	{
		class SQLite;



		/** DbModuleConfig SQLite table synchronizer.
			@ingroup m02LS refLS
		*/
		class DbModuleConfigTableSync : public util::FactorableTemplate<db::SQLiteTableSync, DbModuleConfigTableSync>
		{
		 public:

			static const std::string TABLE_NAME;
			static const std::string COL_PARAMNAME;
			static const std::string COL_PARAMVALUE;


			DbModuleConfigTableSync ();
			~DbModuleConfigTableSync ();

		 protected:

			 virtual const std::string& getTableName() const;
			void rowsAdded (synthese::db::SQLite* sqlite, 
					synthese::db::SQLiteSync* sync,
					const synthese::db::SQLiteResultSPtr& rows, bool isFirstSync = false);

			void rowsUpdated (synthese::db::SQLite* sqlite, 
					  synthese::db::SQLiteSync* sync,
					  const synthese::db::SQLiteResultSPtr& rows);

			void rowsRemoved (synthese::db::SQLite* sqlite, 
					  synthese::db::SQLiteSync* sync,
					  const synthese::db::SQLiteResultSPtr& rows);

		};


	}

}
#endif

