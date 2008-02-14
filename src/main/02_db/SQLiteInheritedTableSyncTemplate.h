
/** SQLiteInheritedTableSyncTemplate class header.
	@file SQLiteInheritedTableSyncTemplate.h

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

#ifndef SYNTHESE_db_SQLiteInheritedTableSyncTemplate_h__
#define SYNTHESE_db_SQLiteInheritedTableSyncTemplate_h__

#include "02_db/Types.h"
#include "02_db/SQLiteResult.h"

#include "01_util/FactorableTemplate.h"
#include "01_util/UId.h"

namespace synthese
{
	namespace db
	{
		/** SQLiteInheritedTableSyncTemplate class.
			@ingroup m10

			@warning The implementations of rowAdded, rowUpdated, and rowRemoved must handle only one row at each execution.
		*/
		template<class ParentTableSyncClass, class TableSyncClass, class ObjectClass>
		class SQLiteInheritedTableSyncTemplate
			: public util::FactorableTemplate<ParentTableSyncClass,TableSyncClass>
		{
		protected:
			/** Object links loader from the SQLite database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@param temporary Objects to link must be temporarily and recursively created from the database
				@author Hugues Romain
				@date 2007

				This method must be implemented by each template instantiation
			*/
			static void _Link(ObjectClass* obj, const SQLiteResultSPtr& rows, GetSource temporary);



			/** Remove all the links to the object present in the environment.
				@param obj Pointer to the object to unlink
				@author Hugues Romain
				@date 2007				

				This method must be implemented by each template instantiation
			*/
			static void _Unlink(ObjectClass* obj);


		public:

			/** Object fetcher.
				@param key UID of the object
				@param linked Load on temporary linked object (recursive get)
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException if the object was not found
			*/
			static ObjectClass* GetUpdateable(const SQLiteResultSPtr& row, bool linked=false)
			{
				ObjectClass* object(new ObjectClass);
				Load(object, row);
				if (linked)
					Link(object, row, GET_TEMPORARY);
				return object;
			}

			/** Object fetcher.
				@param key UID of the object
				@param linked Load on temporary linked object (recursive get)
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException if the object was not found
			*/
			static ObjectClass* GetUpdateable(uid key, bool linked=false)
			{
				SQLiteResultSPtr rows(_GetRow(key));
				return GetUpdateable(rows, linked);
			}



			/** Object fetcher.
				@param key UID of the object
				@param linked Load on temporary linked object (recursive get)
				@return Pointer to a new C++ object corresponding to the fetched record
				@throw DBEmptyResultException if the object was not found
			*/
			static const ObjectClass* Get(uid key, bool linked=false)
			{
				return const_cast<const ObjectClass*>(GetUpdateable(key,linked));
			}



			SQLiteInheritedTableSyncTemplate()
				: util::FactorableTemplate<ParentTableSyncClass,TableSyncClass>()
			{
			}

			virtual typename ParentTableSyncClass::ObjectType* create()
			{
				return new ObjectClass;
			}

			virtual typename ParentTableSyncClass::ObjectType* get(SQLiteResultSPtr& row, bool linked)
			{
				return GetUpdateable(row, linked);
			}

			static void Link(ObjectClass* obj, const SQLiteResultSPtr& rows, GetSource temporary)
			{
				if (obj->getLinked())
				{
					Unlink(obj);
					if (temporary)
						obj->clearChildTemporaryObjects();
				}
				_Link(obj, rows, temporary);
				obj->setLinked(true);
			}

			static void Unlink(ObjectClass* obj)
			{
				_Unlink(obj);
				obj->setLinked(false);
			}

			/** Gets from the database a temporary object linked by another one.
				@param key UID of the object
				@param obj Parent object which link to the returned object
				@param linked recursive get
				@return Pointer to the created object
				@author Hugues Romain
				@date 2007

			The deletion of the object will be automatically done at the deletion of the parent object
			*/
			template<class LinkedClass>
			static const ObjectClass* Get(uid key, LinkedClass* obj, bool linked=false)
			{
				const ObjectClass* c(Get(key, linked));
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

				The deletion of the object will be automatically done at the deletion of the parent object
			*/
			template<class LinkedClass>
			static ObjectClass* GetUpdateable(uid key, LinkedClass* obj, bool linked=false)
			{
				ObjectClass* c(GetUpdateable(key, linked));
				obj->addChildTemporaryObject(c);
				return c;
			}



/*			static boost::shared_ptr<const ObjectClass> Get(uid key, GetSource source = GET_AUTO, bool linked = false)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && ObjectClass::Contains(key))
					? ObjectClass::Get(key)
					: boost::shared_ptr<const ObjectClass>(Get(key, linked));
			}


			static boost::shared_ptr<ObjectClass> GetUpdateable(uid key, GetSource source = GET_AUTO, bool linked = false)
			{
				return
					(source == GET_REGISTRY || source == GET_AUTO && ObjectClass::Contains(key))
					? static_pointer_cast<ObjectClass,ObjectClass::ValueType>(ObjectClass::GetUpdateable(key))
					: boost::shared_ptr<ObjectClass>(GetUpdateable(key, linked));
			}
*/		};
	}
}

#endif // SYNTHESE_db_SQLiteInheritedTableSyncTemplate_h__
