
/** DBModule class header.
	@file DBModule.h

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


#ifndef SYNTHESE_DBModule_H__
#define SYNTHESE_DBModule_H__

#include "01_util/ModuleClass.h"

#include "02_db/Constants.h"

namespace synthese
{
	namespace db
	{


	    class SQLiteQueueThreadExec;
	    

		/** @defgroup m02 02 SQLite database access

		@{
		*/

//		static const std::string TRIGGERS_ENABLED_CLAUSE;

		class DBModule : public util::ModuleClass
		{
		private:
		    
		    static SQLiteQueueThreadExec* _sqliteQueueThreadExec;

		public:

		    void initialize();

		    static SQLiteQueueThreadExec* GetSQLite ();

		};
		/** @} */

	// TEMPORARY
		/// @todo Handle better TRIGGER and co...
		static const std::string CONFIG_TABLE_NAME ("t999_config");
		static const std::string CONFIG_TABLE_COL_PARAMNAME ("param_name");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE ("param_value");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_PORT ("port");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_NBTHREADS ("nb_threads");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_LOGLEVEL ("log_level");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_DATADIR ("data_dir");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_TEMPDIR ("temp_dir");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPDIR ("http_temp_dir");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPURL ("http_temp_url");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_TRIGGERSENABLED ("triggers_enabled");
		static const std::string TRIGGERS_ENABLED_CLAUSE (
			"(SELECT " + CONFIG_TABLE_COL_PARAMVALUE
			+ " FROM " + CONFIG_TABLE_NAME + " WHERE " + CONFIG_TABLE_COL_PARAMNAME
			+ "='" + CONFIG_TABLE_COL_PARAMVALUE_TRIGGERSENABLED + "')");

	}
}
#endif // SYNTHESE_DBModule_H__

