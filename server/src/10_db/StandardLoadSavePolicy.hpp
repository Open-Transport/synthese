
/** StandardLoadSavePolicy class header.
	@file StandardLoadSavePolicy.hpp

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

#ifndef SYNTHESE_db_StandardLoadSavePolicy_hpp__
#define SYNTHESE_db_StandardLoadSavePolicy_hpp__

namespace synthese
{
	namespace db
	{
		/** StandardLoadSavePolicy class.
			@ingroup m10
		*/
		template<class K, class T>
		class StandardLoadSavePolicy
		{
		public:
			StandardLoadSavePolicy() {}



			static FieldsList GetFieldsList();




			/** Object properties loader from the database.
				@param obj Pointer to the object to load from the database
				@param rows Row to read
				@param environment Environment to read and populate
				@author Hugues Romain
				@date 2007
				@warning To complete the load when building the RAM environment, follow the properties load by the link method
			*/
			static void Load(
				T* obj,
				const DBResultSPtr& rows,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
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
				T* obj,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			);



			//////////////////////////////////////////////////////////////////////////
			/// This methods breaks all links from other object that points to the current object.
			static void Unlink(
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
				const Record& row
			);



			static boost::shared_ptr<T> GetNewObject(
			);
		};



		template<class K, class T>
		void StandardLoadSavePolicy<K, T>::Unlink( T* obj )
		{
			obj->unlink();
		}


		template<class K, class T>
		void StandardLoadSavePolicy<K, T>::Save(
			T* obj,
			boost::optional<DBTransaction&> transaction /*= boost::optional<DBTransaction&>() */
		){
			DBModule::GetDB()->replaceStmt(*obj, transaction);
		}



		template<class K, class T>
		void StandardLoadSavePolicy<K, T>::Load(
			T* obj,
			const DBResultSPtr& rows,
			util::Env& env,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			DBModule::LoadObjects(obj->getLinkedObjectsIds(*rows), env, linkLevel);
			obj->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				obj->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<class K, class T>
		FieldsList StandardLoadSavePolicy<K, T>::GetFieldsList()
		{
			T object;
			return object.getFields();
		}



		template<class K, class T>
		boost::shared_ptr<T> StandardLoadSavePolicy<K, T>::GetNewObject(
			const Record& row
		){
			return boost::shared_ptr<T>(new T(row.getDefault<util::RegistryKeyType>(TABLE_COL_ID, 0)));
		}



		template<class K, class T>
		boost::shared_ptr<T> synthese::db::StandardLoadSavePolicy<K, T>::GetNewObject()
		{
			return boost::shared_ptr<T>(new T);
		}
}	}

#endif // SYNTHESE_db_StandardLoadSavePolicy_hpp__

