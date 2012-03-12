
/** ObjectField class header.
	@file ObjectField.hpp

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

#ifndef SYNTHESE__ObjectField_hpp__
#define SYNTHESE__ObjectField_hpp__

#include "Field.hpp"
#include "FrameworkTypes.hpp"
#include "ParametersMap.h"
#include "Record.hpp"
#include "ObjectBase.hpp"

namespace synthese
{
	namespace util
	{
		class Env;
		class ParametersMap;
	}

	class Record;



	template<class C>
	class ObjectFieldDefinition
	{
	public:
		static const bool EXPORT_CONTENT_AS_FILE;
		static const Field FIELD;
		static void AddFields(FieldsList& l)
		{
			l.push_back(FIELD);
		}
	};



	template<class C, class T>
	class SimpleObjectField:
		public ObjectFieldDefinition<C>
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



	//////////////////////////////////////////////////////////////////////////
	/// Field of standard object using various database fields.
	/// ComplexObjectField are part of schemas of Object classes.
	/// Partial and total template specialzations are present in StandardFields.hpp
	///	@ingroup m00
	/// @author Hugues Romain
	/// @date 2012
	template<class C, class T>
	class ObjectField:
		public SimpleObjectField<C, T>
	{
	public:
		typedef T Type;

		static void UnSerialize(
			T& fieldObject,
			const std::string& text,
			const util::Env& env
		);

		static std::string Serialize(
			const T& fieldObject,
			SerializationFormat format = FORMAT_INTERNAL
		);

		static void GetLinkedObjectsIdsFromText(
			LinkedObjectsIds& list,
			const std::string& text
		);
	};


	template<class C, class T>
	void SimpleObjectField<C, T>::LoadFromRecord(T& fieldObject, const Record& record)
	{
		ObjectField<C, T>::UnSerialize(
			fieldObject,
			record.getDefault<std::string>(ObjectFieldDefinition<C>::FIELD.name)
		);
	}

	template<class C, class T>
	void SimpleObjectField<C, T>::LoadFromRecord(T& fieldObject, const Record& record, const util::Env& env)
	{
		ObjectField<C, T>::UnSerialize(
			fieldObject,
			record.getDefault<std::string>(ObjectFieldDefinition<C>::FIELD.name),
			env
		);
	}

	template<class C, class T>
	void SimpleObjectField<C, T>::LoadFromRecord(T& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
	{
		LoadFromRecord(fieldObject, record, env);
	}

	template<class C, class T>
	void SimpleObjectField<C, T>::SaveToParametersMap(const T& fieldObject, const ObjectBase& object, util::ParametersMap& map, const std::string& prefix)
	{
		SaveToParametersMap(fieldObject, map, prefix);
	}


	template<class C, class T>
	void SimpleObjectField<C, T>::SaveToParametersMap(const T& fieldObject, util::ParametersMap& map, const std::string& prefix)
	{
		map.insert(
			prefix + ObjectFieldDefinition<C>::FIELD.name,
			ObjectField<C, T>::Serialize(fieldObject)
		);
	}

	template<class C, class T>
	void SimpleObjectField<C, T>::SaveToParametersMap(const T& fieldObject, const ObjectBase& object, util::ParametersMap& map)
	{
		SaveToParametersMap(fieldObject, map);
	}


	template<class C, class T>
	void SimpleObjectField<C, T>::SaveToParametersMap(const T& fieldObject, util::ParametersMap& map)
	{
		map.insert(
			ObjectFieldDefinition<C>::FIELD.name,
			ObjectField<C, T>::Serialize(fieldObject)
		);
	}

	template<class C, class T>
	void SimpleObjectField<C, T>::GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record)
	{
		ObjectField<C, T>::GetLinkedObjectsIdsFromText(list, record.getDefault<std::string>(ObjectFieldDefinition<C>::FIELD.name));
	}
}

#endif
