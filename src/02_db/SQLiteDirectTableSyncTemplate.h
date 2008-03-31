
/** SQLiteDirectTableSyncTemplate class header.
@file SQLiteDirectTableSyncTemplate.h

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

#ifndef SYNTHESE_SQLiteDirectTableSyncTemplate_H__
#define SYNTHESE_SQLiteDirectTableSyncTemplate_H__


#include "02_db/DBModule.h"
#include "02_db/Types.h"
#include "02_db/DbModuleClass.h"
#include "02_db/SQLite.h"
#include "02_db/SQLiteTableSyncTemplate.h"
#include "02_db/SQLiteResult.h"
#include "02_db/DBEmptyResultException.h"
#include "02_db/SQLiteException.h"

#include "01_util/FactorableTemplate.h"

#include <sstream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "01_util/UId.h"
#include "01_util/Constants.h"
#include "01_util/Conversion.h"
#include "01_util/Log.h"


namespace synthese
{
	namespace db
	{

		/** Table synchronizer template.
			@ingroup m10
		*/
		template <class K, class T>
		class SQLiteDirectTableSyncTemplate : public SQLiteTableSyncTemplate<K>
		{
		public:
			typedef T		ObjectType;

		protected:

			SQLiteDirectTableSyncTemplate()	
				: SQLiteTableSyncTemplate<K>() 
			{}

			SQLiteDirectTableSyncTemplate(const SQLiteTableSync::Args& args)
				: SQLiteTableSyncTemplate<K>(args)
			{}


			/** Object links loader from the SQLite database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@param temporary Objects to link must be temporarily and recursively created from the database
				@author Hugues Romain
				@date 2007				
			*/
			static void _link(T* obj, const SQLiteResultSPtr& rows, GetSource temporary);



			/** Remove all the links to the object present in the environment.
				@author Hugues Romain
				@date 2007				
			*/
			static void _unlink(T* obj);


		public:

			/** Object fetcher.
			@param key UID of the object
			@param linked Load on temporary linked object (recursive get)
			@return Pointer to a new C++ object corresponding to the fetched record
			@throw DBEmptyResultException if the object was not found
			*/
			static T* _Get(uid key, bool linked)
			{
				SQLiteResultSPtr rows(_GetRow(key));
				T* object(new T);
				load(object, rows);
				if (linked)
					link(object, rows, GET_TEMPORARY);
				return object;
			}

			static void link(T* obj, const SQLiteResultSPtr& rows, GetSource temporary)
			{
				if (obj->getLinked())
				{
					unlink(obj);
					if (temporary)
						obj->clearChildTemporaryObjects();
				}
				_link(obj, rows, temporary);
				obj->setLinked(true);
			}

			static void unlink(T* obj)
			{
				_unlink(obj);
				obj->setLinked(false);
			}

			/** Object properties loader from the SQLite database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@author Hugues Romain
				@date 2007
				@warning To complete the load when building the RAM environment, follow the properties load by the link method
			*/
			static void load(T* obj, const SQLiteResultSPtr& rows);



			/** Saving of the object in database.
				@param obj Object to save
				@author Hugues Romain
				@date 2007

				The object is recognized by its key :
				- if the object has already a key, then the corresponding record is replaced
				- if the object does not have any key, then the autoincrement function generates one for it.
			*/
			static void save(T* obj);



			/** Gets from the database a temporary object linked by another one.
				@param key UID of the object
				@param obj Parent object which link to the returned object
				@param linked recursive get
				@return Pointer to the created object
				@author Hugues Romain
				@date 2007

				The deletion of the object will be autoamtically done at the deletion of the parent object
			*/
			template<class C>
			static const T* Get(uid key, C* obj, bool linked)
			{
				const T* c(static_cast<const T*>(_Get(key, linked)));
				obj->addChildTemporaryObject(c);
				return c;
			}


			/** Gets from the database a temporary object linked by another one.
			@param key UID of the object
			@param obj Parent object which link to the returned object
			@param linked recursive get
			@return Pointer to the created object
			@author Hugues Romain
			@date 2007

			The deletion of the object will be autoamtically done at the deletion of the parent object
			*/
			template<class C>
			static T* GetUpdateable(uid key, C* obj)
			{
				T* c(_Get(key, true));
				obj->addChildTemporaryObject(c);
				return c;
			}



			template<class C>
			static const T* Get(uid key, C* obj, bool linked, GetSource source)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && T::Contains(key))
					? T::Get(key).get()
					: Get(key, obj, linked);
			}

			template<class C>
			static T* GetUpdateable(uid key, C* obj, GetSource source)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && T::Contains(key))
					? T::GetUpdateable(key).get()
					: GetUpdateable(key, obj);
			}

			static boost::shared_ptr<const T> Get(uid key, GetSource source = GET_AUTO, bool linked = false)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && T::Contains(key))
					? T::Get(key)
					: boost::shared_ptr<const T>(_Get(key, linked));
			}

			static boost::shared_ptr<T> GetUpdateable(uid key, GetSource source = GET_TEMPORARY)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && T::Contains(key))
					? T::GetUpdateable(key)
					: boost::shared_ptr<T>(_Get(key, true));
			}

			static boost::shared_ptr<T> createEmpty();

		};


		template <class K, class T>
		boost::shared_ptr<T> SQLiteDirectTableSyncTemplate<K,T>::createEmpty()
		{
			boost::shared_ptr<T> object(new T);
			save(object.get());
			return object;
		}

	}
}

#endif // SYNTHESE_SQLiteTableSyncTemplate_H__

