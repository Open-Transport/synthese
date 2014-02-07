
/** SimpleObjectField_Date class header.
	@file SimpleObjectField_Date.hpp

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

#ifndef SYNTHESE__PointerField_hpp__
#define SYNTHESE__PointerField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "DBModule.h" // Temporary modules dependencies rule violation : will be useless when Env::getEditable will be able to chose a registry dynamically.
#include "Env.h"
#include "FrameworkTypes.hpp"
#include "ObjectBase.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	//////////////////////////////////////////////////////////////////////////
	/// Pointer field
	/// @ingroup m00
	template<class C, class T>
	class PointerField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef boost::optional<T&> Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string _pointerToString(const typename PointerField<C, T>::Type& p)
		{
			return
				p ?
				boost::lexical_cast<std::string>(p->getKey()) :
				std::string("0")
			;
		}


	public:
		static bool LoadFromRecord(
			typename PointerField<C, T>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return false;
			}

			typename PointerField<C, T>::Type value(boost::none);
			try
			{
				util::RegistryKeyType id(
					record.getDefault<util::RegistryKeyType>(
						SimpleObjectFieldDefinition<C>::FIELD.name,
						0
				)	);
				if(id > 0)
				{
					value = *env.getEditable<T>(id);
				}
			}
			catch(boost::bad_lexical_cast&)
			{
				util::Log::GetInstance().warn(
					"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
					object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
					record.getValue(SimpleObjectFieldDefinition<C>::FIELD.name) + " is not a valid id."
				);
			}
			catch(util::ObjectNotFoundException<T>&)
			{
				util::Log::GetInstance().warn(
					"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
					object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
					record.getValue(SimpleObjectFieldDefinition<C>::FIELD.name) + " object was not found."
				);
			}

			if(	(!value && !fieldObject) ||
				(value && fieldObject && &*value==&*fieldObject)
			){
				return false;
			}
			else
			{
				fieldObject = value;
				return true;
			}
		}



		static void SaveToFilesMap(
			const typename PointerField<C, T>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_pointerToString
			);
		}



		static void SaveToParametersMap(
			const typename PointerField<C, T>::Type& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_pointerToString
			);
		}



		static void SaveToParametersMap(
			const typename PointerField<C, T>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_pointerToString
			);
		}




		static void SaveToDBContent(
			const typename PointerField<C, T>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			util::RegistryKeyType id(0);
			if(fieldObject)
			{
				id = fieldObject->getKey();
			}
			content.push_back(Cell(id));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
			try
			{
				util::RegistryKeyType id(
					record.getDefault<util::RegistryKeyType>(
						SimpleObjectFieldDefinition<C>::FIELD.name,
						0
				)	);
				if(id > 0)
				{
					list.push_back(id);
				}
			}
			catch(boost::bad_lexical_cast&)
			{
			}
		}
	};



	//////////////////////////////////////////////////////////////////////////
	/// Pointer field specialization for util::Registrable type.
	/// @ingroup m00
	template<class C>
	class PointerField<C, util::Registrable>:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef boost::optional<util::Registrable&> Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string _pointerToString(const typename PointerField<C, util::Registrable>::Type& p)
		{
			return
				p ?
				boost::lexical_cast<std::string>(p->getKey()) :
				std::string("0")
			;
		}


	public:
		static bool LoadFromRecord(
			typename PointerField<C, util::Registrable>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return false;
			}

			typename PointerField<C, util::Registrable>::Type value(boost::none);
			try
			{
				util::RegistryKeyType id(
					record.getDefault<util::RegistryKeyType>(
						SimpleObjectFieldDefinition<C>::FIELD.name,
						0
				)	);
				if(id > 0)
				{
					value = *db::DBModule::GetEditableObject(
						id,
						const_cast<util::Env&>(env)
					); // Temporary modules dependencies rule violation : will be useless when Env::getEditable will be able to chose a registry dynamically.
				}
			}
			catch(boost::bad_lexical_cast&)
			{
				util::Log::GetInstance().warn(
					"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
					object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
					record.getValue(SimpleObjectFieldDefinition<C>::FIELD.name) + " is not a valid id."
				);
			}
			catch(util::ObjectNotFoundException<util::Registrable>&)
			{
				util::Log::GetInstance().warn(
					"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
					object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
					record.getValue(SimpleObjectFieldDefinition<C>::FIELD.name) + " object was not found."
				);
			}

			if(	(!value && !fieldObject) ||
				(value && fieldObject && &*value==&*fieldObject)
			){
				return false;
			}
			else
			{
				fieldObject = value;
				return true;
			}
		}



		static void SaveToFilesMap(
			const typename PointerField<C, util::Registrable>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_pointerToString
			);
		}



		static void SaveToParametersMap(
			const typename PointerField<C, util::Registrable>::Type& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_pointerToString
			);
		}



		static void SaveToParametersMap(
			const typename PointerField<C, util::Registrable>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_pointerToString
			);
		}




		static void SaveToDBContent(
			const typename PointerField<C, util::Registrable>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			util::RegistryKeyType id(0);
			if(fieldObject)
			{
				id = fieldObject->getKey();
			}
			content.push_back(Cell(id));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
			try
			{
				util::RegistryKeyType id(
					record.getDefault<util::RegistryKeyType>(
						SimpleObjectFieldDefinition<C>::FIELD.name,
						0
				)	);
				if(id > 0)
				{
					list.push_back(id);
				}
			}
			catch(boost::bad_lexical_cast&)
			{
			}
		}
	};

	#define FIELD_POINTER(N, T) struct N : public PointerField<N, T> {};
}

#endif
