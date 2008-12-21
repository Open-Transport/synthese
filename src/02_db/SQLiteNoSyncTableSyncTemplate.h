
/** SQLiteRegistryTableSyncTemplate class header.
@file SQLiteRegistryTableSyncTemplate.h

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

#ifndef SYNTHESE_db_SQLiteNoSyncTableSyncTemplate_h__
#define SYNTHESE_db_SQLiteNoSyncTableSyncTemplate_h__

#include "SQLiteDirectTableSyncTemplate.h"

namespace synthese
{
	namespace db
	{
		/** SQLiteNoSyncTableSyncTemplate class.
		@ingroup m10
		*/
		template<class K, class T>
		class SQLiteNoSyncTableSyncTemplate : public SQLiteDirectTableSyncTemplate<K,T>
		{
		public:
			SQLiteNoSyncTableSyncTemplate() : SQLiteDirectTableSyncTemplate<K,T>()
				{}

			virtual bool getRegisterInSubClassMap() const
			{
				return false;
			}

			static SQLiteTableFormat CreateFormat(
				std::string name,
				SQLiteTableFormat::Fields fields,
				SQLiteTableFormat::Indexes indexes = SQLiteTableFormat::Indexes()
			){
				return SQLiteDirectTableSyncTemplate<K,T>::CreateFormat(
					name,
					fields,
					indexes,
					true
				);
			}


			/** Action to do on DisplayType creation.
			This method loads a new object in ram.
			*/
			void rowsAdded(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
				, bool isFirstSync = false
			){
			}


			/** Action to do on DisplayType creation.
			This method updates the corresponding object in ram.
			*/
			void rowsUpdated(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
			){
			}


			/** Action to do on DisplayType deletion.
			This method deletes the corresponding object in ram and runs 
			all necessary cleaning actions.
			*/
			void rowsRemoved(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
			){
			}

		};
	}
}

#endif // SYNTHESE_db_SQLiteRegistryTableSyncTemplate_h__
