
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

#include "02_db/DbModuleClass.h"
#include "02_db/Constants.h"

#include "01_util/FactorableTemplate.h"
#include "01_util/UId.h"

#ifdef WITH_DBRING

#else
  
#endif



namespace synthese
{
	/** @defgroup m10 10 SQLite database access
		@ingroup m1

	@{
	*/

	/** 02 SQLite database access module namespace.
	*/
	namespace db
	{

	    class SQLite;
	    class SQLiteHandle;
	    

//		static const std::string TRIGGERS_ENABLED_CLAUSE;

		class DBModule : public util::FactorableTemplate<DbModuleClass, DBModule>
		{
		public:
			typedef std::map<uid, std::string>	SubClassMap;

		private:
		    
		    static SQLiteHandle*	_sqlite;
			static SubClassMap		_subClassMap;

		public:

		    void preInit ();
		    void initialize();

		    static SQLite* GetSQLite ();

		    /** Called whenever a parameter registered by this module is changed
		     */
		    static void ParameterCallback (const std::string& name, 
						   const std::string& value);

			static void AddSubClass(uid, const std::string&);
			static std::string GetSubClass(uid id);

			virtual std::string getName() const;
		};
		

	}

	/** @} */

}
#endif // SYNTHESE_DBModule_H__

