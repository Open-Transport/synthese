////////////////////////////////////////////////////////////////////////////////
/// SQLiteDirectTableSyncTemplate class header.
///	@file SQLiteDirectTableSyncTemplate.h
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

#ifndef SYNTHESE_SQLiteDirectTableSyncTemplate_H__
#define SYNTHESE_SQLiteDirectTableSyncTemplate_H__


#include "DBModule.h"
#include "02_db/Types.h"
#include "DbModuleClass.h"
#include "SQLite.h"
#include "SQLiteTableSyncTemplate.h"
#include "SQLiteResult.h"
#include "DBEmptyResultException.h"
#include "SQLiteException.h"

#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "FactorableTemplate.h"
#include "Constants.h"
#include "Conversion.h"
#include "Env.h"
#include "01_util/Log.h"


namespace synthese
{
	namespace db
	{
		////////////////////////////////////////////////////////////////////////
		///	Table synchronizer template (abstract class) for standard SYNTHESE
		///	tables.
		///	
		/// A standard SYNTHESE table has the following properties :
		///		- Removal allowed
		///		- Autoincrement activated
		///		- an internal class corresponds to the records of the table
		///		- a Load and a Save function convert records into class object
		///			and vice versa 
		///	@ingroup m10
		////////////////////////////////////////////////////////////////////////
		template <class K, class T>
		class SQLiteDirectTableSyncTemplate : public SQLiteTableSyncTemplate<K>
		{
		public:
			typedef K		TableSync;
			typedef T		ObjectType;

		protected:
			////////////////////////////////////////////////////////////////////
			///	SQLiteDirectTableSyncTemplate constructor.
			////////////////////////////////////////////////////////////////////
			SQLiteDirectTableSyncTemplate()
			{
			}


		public:
			////////////////////////////////////////////////////////////////////
			/// Object fetcher, with read/write permissions.
			///	@param key UID of the object
			///	@param env Environment to write the object and read/write the links.
			///		- if Default/NULL : no environment, then object is only read from the database
			///		- if non NULL : the object is read from the environment if exists, else from the database
			///		- use Env::GetOfficialEnv() to use the official environment
			///	@param linkLevel Level of load recursion (see each TableSync to know precisely the signification of each level for each class)
			///	@param autoCreate :
			///		- AUTO_CREATE : returns an empty object if the specified key was not found in the table (the returned object is <u>not</u> stored in the database).
			///		- NEVER_CREATE (default) : throws a ObjectNotFound<T> exception if the specified key was not found in the table
			///	@return shared pointer Pointer to an object corresponding to the fetched record.
			///	@throw ObjectNotFound<T> if the object was not found and if autoCreate is deactivated
			///	@todo implement upgrade of load level
			static boost::shared_ptr<T> GetEditable(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				if (env.getRegistry<T>().contains(key))
				{
					return env.getEditableRegistry<T>().getEditable(key);
				}

				boost::shared_ptr<T> object;
				try
				{
					SQLiteResultSPtr rows(SQLiteTableSyncTemplate<K>::_GetRow(key));
					object.reset(new T(rows->getKey()));
					Load(object.get(), rows, env, linkLevel);
				}
				catch (typename db::DBEmptyResultException<K>&)
				{
					if (autoCreate == NEVER_CREATE)
					{
						throw util::ObjectNotFoundException<T>(key, "Object not found in "+ K::TABLE.NAME);
					}
					object.reset(new T(key));
				}
				
				env.getEditableRegistry<T>().add(object);
				return object;
			}



			/** Object fetcher, with read only permissions.
				@param key UID of the object
				@param environment Environment to write the object and read/write the links.
					- if Default/NULL : no environment, then object is only read from the database
					- if non NULL : the object is read from the environment if exists, else from the database
					- use Env::GetOfficialEnv() to use the official environment
				@param linkLevel Level of load recursion (see each TableSync to know precisely the signification of each level for each class)
				@return shared pointer Pointer to an object corresponding to the fetched record.
				@throw DBEmptyResultException if the object was not found
				
				Note : in case of writing in the environment, the object present in the environment will have read/write permissions.
			*/
			static boost::shared_ptr<const T> Get(
				util::RegistryKeyType key,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				return boost::const_pointer_cast<const T>(GetEditable(key,environment,linkLevel,autoCreate));
			}

			/** Object properties loader from the SQLite database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@param environment Environment to read and populate
				@author Hugues Romain
				@date 2007
				@warning To complete the load when building the RAM environment, follow the properties load by the link method
			*/
			static void Load(
				T* obj,
				const SQLiteResultSPtr& rows,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			static void Unlink(
				T* obj
			);


			/** Saving of the object in database.
				@param obj Object to save
				@author Hugues Romain
				@date 2007

				The object is recognized by its key :
				- if the object has already a key, then the corresponding record is replaced
				- if the object does not have any key, then the autoincrement function generates one for it.
			*/
			static void Save(
				T* obj
			);



			////////////////////////////////////////////////////////////////////
			///	Object creator helper.
			///	@param row data row
			///	@return boost::shared_ptr<T> The created object
			///	@author Hugues Romain
			///	@date 2008
			/// This static method can be overloaded.
			////////////////////////////////////////////////////////////////////
			static boost::shared_ptr<T> GetNewObject(
				const SQLiteResultSPtr& row
			){
				return boost::shared_ptr<T>(new T(row->getKey()));
			}


			
			////////////////////////////////////////////////////////////////////
			/// Load objects into an environment, from a SQL query.
			///	@param query SQL query
			///	@param env Environment to write
			///	@param linkLevel Link level
			///	@throws Exception if the load failed
			////////////////////////////////////////////////////////////////////
			static void LoadFromQuery(
				const std::string& query,
				util::Env& env,
				util::LinkLevel linkLevel
			){
				try
				{
					util::Registry<T>& registry(env.template getEditableRegistry<T>());
					SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query);
					while (rows->next ())
					{
						boost::shared_ptr<T> object(K::GetNewObject(rows));
						Load(object.get(), rows, env, linkLevel);
						registry.add(object);
					}
				}
				catch(SQLiteException& e)
				{
					throw util::Exception(e.getMessage());
				}
			}
		};
	}
}

#endif // SYNTHESE_SQLiteTableSyncTemplate_H__
