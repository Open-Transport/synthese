
/** SQLiteAutoRegisterTableSyncTemplate class header.
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

#ifndef SYNTHESE_db_SQLiteAutoRegisterTableSyncTemplate_h__
#define SYNTHESE_db_SQLiteAutoRegisterTableSyncTemplate_h__

#include "SQLiteDirectTableSyncTemplate.h"

namespace synthese
{
	namespace db
	{
		/** Table sync template class, providing auto-registration of elements at first get.
			@ingroup m10

			The elements are loaded into the registry at the first use of Get with the source parameter on GET_AUTO or GET_REGISTRY.
		*/
		template<class K, class T>
		class SQLiteAutoRegisterTableSyncTemplate : public SQLiteDirectTableSyncTemplate<K,T>
		{
		public:
			SQLiteAutoRegisterTableSyncTemplate() : SQLiteDirectTableSyncTemplate<K,T>(
				typename SQLiteTableSyncTemplate<K>::Args(true, true, TRIGGERS_ENABLED_CLAUSE, true, true))
				{}

			virtual bool getRegisterInSubClassMap() const
			{
				return false;
			}

			template<class C>
			static const T* Get(uid key, C* obj, bool linked, GetSource source)
			{
				if (source == GET_REGISTRY || source == GET_AUTO)
				{
					if (!T::Contains(key))
					{
						SQLiteResultSPtr rows(_GetRow(key));
						T* object(new T);
						load(object, rows);
						link(object, rows, GET_REGISTRY);
						object->store();
					}
					return T::Get(key).get();
				}
				else
					return SQLiteDirectTableSyncTemplate<K,T>::Get(key, obj, linked);
			}

			template<class C>
			static T* GetUpdateable(uid key, C* obj, GetSource source)
			{
				if (source == GET_REGISTRY || source == GET_AUTO)
				{
					if (!T::Contains(key))
					{
						SQLiteResultSPtr rows(_GetRow(key));
						T* object(new T);
						load(object, rows);
						link(object, rows, GET_REGISTRY);
						object->store();
					}
					return T::GetUpdateable(key).get();
				}
				else
					return SQLiteDirectTableSyncTemplate<K,T>::GetUpdateable(key, obj);
			}

			static boost::shared_ptr<const T> Get(uid key, GetSource source = GET_AUTO, bool linked = false)
			{
				if (source == GET_REGISTRY || source == GET_AUTO)
				{
					if (!T::Contains(key))
					{
						SQLiteResultSPtr rows(_GetRow(key));
						T* object(new T);
						load(object, rows);
						link(object, rows, GET_REGISTRY);
						object->store();
					}
					return T::Get(key);
				}
				else
					return boost::shared_ptr<const T>(_Get(key, linked));
			}

			static boost::shared_ptr<T> GetUpdateable(uid key, GetSource source = GET_TEMPORARY)
			{
				if (source == GET_REGISTRY || source == GET_AUTO)
				{
					if (!T::Contains(key))
					{
						SQLiteResultSPtr rows(_GetRow(key));
						T* object(new T);
						load(object, rows);
						link(object, rows, GET_REGISTRY);
						object->store();
					}
					return T::GetUpdateable(key);
				}
				else
					return boost::shared_ptr<T>(_Get(key, true));
			}

		protected:

			/** Action to do on object creation or replacement.
				Only if the event is a replacement, and if the concerned object is already loaded into the registry, then it is updated.
			*/
			void rowsAdded(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
				, bool isFirstSync = false
			){
				while (rows->next ())
				{
					try
					{
						if (T::Contains(rows->getLongLong (TABLE_COL_ID)))
						{
							boost::shared_ptr<T> address(T::GetUpdateable(rows->getLongLong (TABLE_COL_ID)));
							SQLiteDirectTableSyncTemplate<K,T>::unlink(address.get());
							load (address.get(), rows);
							SQLiteDirectTableSyncTemplate<K,T>::link(address.get(), rows, GET_REGISTRY);
						}
					}
					catch(util::Exception& e)
					{
						util::Log::GetInstance().warn("Error on load after row insert/replace or at first sync : ", e);
					}
				}
			}


			/** Action to do on object update.
				The object is updated only if it is already loaded in the corresponding registry.
			*/
			void rowsUpdated(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
			){
				while (rows->next ())
				{
					try
					{
						uid id = rows->getLongLong (TABLE_COL_ID);
						if (T::Contains(id))
						{
							boost::shared_ptr<T> address(T::GetUpdateable(id));
							SQLiteDirectTableSyncTemplate<K,T>::unlink(address.get());
							load (address.get(), rows);
							SQLiteDirectTableSyncTemplate<K,T>::link(address.get(), rows, GET_REGISTRY);
						}
					}
					catch (util::Exception& e)
					{
						util::Log::GetInstance().warn("Error on load after row update : ", e);
					}
				}

			}


			/** Action to do on object deletion.
				If it is registered, this method deletes the corresponding object in its registry, and runs all necessary cleaning actions.
			*/
			void rowsRemoved(
				SQLite* sqlite
				, SQLiteSync* sync
				, const SQLiteResultSPtr& rows
			){
				while (rows->next ())
				{
					try
					{
						uid id = rows->getLongLong (TABLE_COL_ID);
						if (T::Contains(id))
						{
							SQLiteDirectTableSyncTemplate<K,T>::unlink(T::GetUpdateable(id).get());
							T::Remove(id);
						}
					}
					catch (util::Exception& e)
					{
						util::Log::GetInstance().warn("Error on unload after row deletion : ", e);
					}
				}
			}
		};
	}
}

#endif 
