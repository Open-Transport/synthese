
/** SimpleObjectField class header.
	@file SimpleObjectField.hpp

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

#ifndef SYNTHESE_framework_SimpleObjectField_hpp__
#define SYNTHESE_framework_SimpleObjectField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

namespace synthese
{
	namespace util
	{
		class Env;
		class ParametersMap;
	}

	class ObjectBase;
	class Record;



	//////////////////////////////////////////////////////////////////////////
	/// Part of schema using one single field.
	///	@ingroup m00
	template<class C, class T>
	class SimpleObjectField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef T Type;

		static void LoadFromRecord(T& fieldObject, const Record& record);
		static void LoadFromRecord(T& fieldObject, const Record& record, const util::Env& env);
		static void LoadFromRecord(T& fieldObject, ObjectBase& object, const Record& record, const util::Env& env);
		static void SaveToParametersMap(const T& fieldObject, const ObjectBase& object, util::ParametersMap& map, const std::string& prefix);
		static void SaveToParametersMap(const T& fieldObject, const ObjectBase& object, util::ParametersMap& map);
		static void SaveToParametersMap(const T& fieldObject, util::ParametersMap& map, const std::string& prefix);
		static void SaveToParametersMap(const T& fieldObject, util::ParametersMap& map);
		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record);
	};

	// Implementations ///////////////////////////////////////////////////////////////////////
	


	template<class C, class T>
	void SimpleObjectField<C, T>::LoadFromRecord(
		T& fieldObject,
		const Record& record
	){
		if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
		{
			return;
		}

		ObjectField<C, T>::UnSerialize(
			fieldObject,
			record.getDefault<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name)
		);
	}



	template<class C, class T>
	void SimpleObjectField<C, T>::LoadFromRecord(
		T& fieldObject,
		const Record& record,
		const util::Env& env
	){
		if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
		{
			return;
		}

		ObjectField<C, T>::UnSerialize(
			fieldObject,
			record.getDefault<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name),
			env
		);
	}



	template<class C, class T>
	void SimpleObjectField<C, T>::LoadFromRecord(
		T& fieldObject,
		ObjectBase& object,
		const Record& record,
		const util::Env& env
	){
		LoadFromRecord(fieldObject, record, env);
	}



	template<class C, class T>
	void SimpleObjectField<C, T>::SaveToParametersMap(
		const T& fieldObject,
		const ObjectBase& object,
		util::ParametersMap& map,
		const std::string& prefix
	){
		SaveToParametersMap(fieldObject, map, prefix);
	}



	template<class C, class T>
	void SimpleObjectField<C, T>::SaveToParametersMap(
		const T& fieldObject,
		util::ParametersMap& map,
		const std::string& prefix
	){
		map.insert(
			prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
			ObjectField<C, T>::Serialize(fieldObject, map.getFormat())
		);
	}



	template<class C, class T>
	void SimpleObjectField<C, T>::SaveToParametersMap(
		const T& fieldObject,
		const ObjectBase& object,
		util::ParametersMap& map
	){
		SaveToParametersMap(fieldObject, map);
	}



	template<class C, class T>
	void SimpleObjectField<C, T>::SaveToParametersMap(
		const T& fieldObject,
		util::ParametersMap& map
	){
		map.insert(
			SimpleObjectFieldDefinition<C>::FIELD.name,
			ObjectField<C, T>::Serialize(fieldObject, map.getFormat())
		);
	}



	template<class C, class T>
	void SimpleObjectField<C, T>::GetLinkedObjectsIds(
		LinkedObjectsIds& list,
		const Record& record
	){
		ObjectField<C, T>::GetLinkedObjectsIdsFromText(
			list,
			record.getDefault<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name)
		);
	}
}

#endif // SYNTHESE_framework_SimpleObjectField_hpp__
