
/** StandardFieldNames class header.
	@file StandardFieldNames.hpp

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

#ifndef SYNTHESE__StandardFields_hpp__
#define SYNTHESE__StandardFields_hpp__

#include "ObjectField.hpp"

#include "ObjectBase.hpp"
#include "Env.h"
#include "Log.h"
#include "SchemaMacros.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// date partial specialization
	template<class C>
	class ObjectField<C, boost::gregorian::date>:
		public SimpleObjectField<C, boost::gregorian::date>
	{
	public:
		static void UnSerialize(
			boost::gregorian::date& fieldObject,
			const std::string& text
		){
			fieldObject = text.empty() ?
				boost::gregorian::date(boost::gregorian::not_a_date_time) :
				boost::gregorian::from_string(text);
		}

		static void UnSerialize(
			boost::gregorian::date& fieldObject,
			const std::string& text,
			const util::Env& env
		){
			UnSerialize(fieldObject, text);
		}

		static std::string Serialize(
			const boost::gregorian::date& fieldObject,
			SerializationFormat format = FORMAT_INTERNAL
		){
			switch(format)
			{
			case FORMAT_SQL:
				return fieldObject.is_not_a_date() ?
					"NULL" :
					"\""+ boost::gregorian::to_iso_extended_string(fieldObject) +"\"";

			default:
				return fieldObject.is_not_a_date() ?
					std::string() :
					boost::gregorian::to_iso_extended_string(fieldObject);
			}
		}

		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record) {}
	};



	//////////////////////////////////////////////////////////////////////////
	/// posix_time partial specialization
	template<class C>
	class ObjectField<C, boost::posix_time::ptime>:
		public SimpleObjectField<C, boost::posix_time::ptime>
	{
	public:
		static void UnSerialize(
			boost::posix_time::ptime& fieldObject,
			const std::string& text
		){
			fieldObject = text.empty() ?
				boost::posix_time::ptime(boost::posix_time::not_a_date_time) :
				boost::posix_time::time_from_string(text);
		}

		static void UnSerialize(
			boost::posix_time::ptime& fieldObject,
			const std::string& text,
			const util::Env& env
		){
			UnSerialize(fieldObject, text);
		}

		static std::string Serialize(
			const boost::posix_time::ptime& fieldObject,
			SerializationFormat format = FORMAT_INTERNAL
		){
			switch(format)
			{
			case FORMAT_SQL:
				return fieldObject.is_not_a_date_time() ?
					"NULL" :
					"\""+ boost::gregorian::to_iso_extended_string(fieldObject.date()) +" "+ boost::posix_time::to_simple_string(fieldObject.time_of_day()) +"\"";

			default:
				return fieldObject.is_not_a_date_time() ?
					std::string() :
					boost::gregorian::to_iso_extended_string(fieldObject.date()) +" "+ boost::posix_time::to_simple_string(fieldObject.time_of_day());
		}	}

		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record) {}
	};



	//////////////////////////////////////////////////////////////////////////
	/// time_duration partial specialization.
	/// Durations are serialized into a number of minutes.
	template<class C>
	class ObjectField<C, boost::posix_time::time_duration>:
		public SimpleObjectField<C, boost::posix_time::time_duration>
	{
	public:
		static void UnSerialize(
			boost::posix_time::time_duration& fieldObject,
			const std::string& text
		){
			fieldObject = text.empty() ?
				boost::posix_time::time_duration(boost::posix_time::not_a_date_time) :
				boost::posix_time::minutes(boost::lexical_cast<int>(text));
		}

		static void UnSerialize(
			boost::posix_time::time_duration& fieldObject,
			const std::string& text,
			const util::Env& env
		){
			UnSerialize(fieldObject, text);
		}

		static std::string Serialize(
			const boost::posix_time::time_duration& fieldObject,
			SerializationFormat format = FORMAT_INTERNAL
		){
			switch(format)
			{
			case FORMAT_SQL:
				return fieldObject.is_not_a_date_time() ?
					"NULL" :
					boost::lexical_cast<std::string>(fieldObject.total_seconds() / 60);

			default:
				return fieldObject.is_not_a_date_time() ?
					std::string() :
					boost::lexical_cast<std::string>(fieldObject.total_seconds() / 60);
		}	}

		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record) {}
	};



	//////////////////////////////////////////////////////////////////////////
	/// string partial specialization
	template<class C>
	class ObjectField<C, std::string>:
		public SimpleObjectField<C, std::string>
	{
	public:
		static void UnSerialize(
			std::string& fieldObject,
			const std::string& text
		){
			fieldObject = text;
		}

		static void UnSerialize(
			std::string& fieldObject,
			const std::string& text,
			const util::Env& env
		){
			UnSerialize(fieldObject, text);
		}

		static std::string Serialize(
			const std::string& fieldObject,
			SerializationFormat format = FORMAT_INTERNAL
		){
			switch(format)
			{
			case FORMAT_SQL:
				return "\""+ boost::algorithm::replace_all_copy(fieldObject, "\"", "\\\"") + "\"";

			default:
				return fieldObject;
		}	}

		/// Do nothing
		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record) {}
	};



	template<class C, class T>
	class SerializationByLexicalCast:
		public ObjectFieldDefinition<C>
	{
	public:
		typedef T Type;

		static void UnSerialize(
			T& fieldObject,
			const std::string& text
		){
			if(!text.empty())
			{
				fieldObject = boost::lexical_cast<T>(text);
			}
			else
			{
				fieldObject = static_cast<T>(0);
			}
		}

		static void UnSerialize(
			T& fieldObject,
			const std::string& text,
			const util::Env& env
		){
			UnSerialize(fieldObject, text);
		}

		static std::string Serialize(
			const T& fieldObject,
			SerializationFormat format = FORMAT_INTERNAL
		){
			return boost::lexical_cast<std::string>(fieldObject);
		}

		static void LoadFromRecord(T& fieldObject, const Record& record)
		{
			if(!record.isDefined(ObjectFieldDefinition<C>::FIELD.name))
			{
				return;
			}

			ObjectField<C, T>::UnSerialize(
				fieldObject,
				record.getDefault<std::string>(ObjectFieldDefinition<C>::FIELD.name)
			);
		}

		static void LoadFromRecord(T& fieldObject, const Record& record, const util::Env& env)
		{
			LoadFromRecord(fieldObject, record);
		}

		static void LoadFromRecord(T& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
		{
			LoadFromRecord(fieldObject, record);
		}

		static void SaveToParametersMap(const T& fieldObject, util::ParametersMap& map, const std::string& prefix)
		{
			map.insert(
				prefix + ObjectFieldDefinition<C>::FIELD.name,
				ObjectField<C, T>::Serialize(fieldObject)
			);
		}

		static void SaveToParametersMap(const T& fieldObject, const ObjectBase& object, util::ParametersMap& map, const std::string& prefix)
		{
			SaveToParametersMap(fieldObject, map, prefix);
		}
	
		static void SaveToParametersMap(const T& fieldObject, util::ParametersMap& map)
		{
			map.insert(
				ObjectFieldDefinition<C>::FIELD.name,
				ObjectField<C, T>::Serialize(fieldObject)
			);
		}

		static void SaveToParametersMap(const T& fieldObject, const ObjectBase& object, util::ParametersMap& map)
		{
			SaveToParametersMap(fieldObject, map);
		}

		/// Do nothing
		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record) {}
	};



	//////////////////////////////////////////////////////////////////////////
	///  partial specialization
	template<class C>
	class ObjectField<C, util::RegistryKeyType>:
		public SerializationByLexicalCast<C, util::RegistryKeyType>
	{};



	//////////////////////////////////////////////////////////////////////////
	/// size_t partial specialization
	template<class C>
	class ObjectField<C, double>:
		public SerializationByLexicalCast<C, double>
	{};



	//////////////////////////////////////////////////////////////////////////
	/// size_t partial specialization
	template<class C>
	class ObjectField<C, size_t>:
		public SerializationByLexicalCast<C, size_t>
	{};



	//////////////////////////////////////////////////////////////////////////
	/// int partial specialization
	template<class C>
	class ObjectField<C, int>:
		public SerializationByLexicalCast<C, int>
	{};



	/// bool partial specialization
	template<class C>
	class ObjectField<C, bool>:
		public SerializationByLexicalCast<C, bool>
	{};



	//////////////////////////////////////////////////////////////////////////
	/// Pointer specialization
	/// Pointer is replaced by optional reference in order to use the
	/// default constructor of boost::optional
	template<class C, class P>
	class ObjectField<C, boost::optional<P&> >:
		public ObjectFieldDefinition<C>
	{
	public:
		typedef boost::optional<P&> Type;

		static void LoadFromRecord(boost::optional<P&>& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
		{
			if(!record.isDefined(ObjectFieldDefinition<C>::FIELD.name))
			{
				return;
			}

			fieldObject = boost::none;
			try
			{
				util::RegistryKeyType id(record.getDefault<util::RegistryKeyType>(ObjectFieldDefinition<C>::FIELD.name, 0));
				if(id > 0)
				{
					fieldObject = *env.getEditable<P>(id);
				}
			}
			catch(boost::bad_lexical_cast&)
			{
				util::Log::GetInstance().warn(
					"Data corrupted in the "+ ObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
					object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
					record.getValue(ObjectFieldDefinition<C>::FIELD.name) + " is not a valid id."
				);
			}
			catch(util::ObjectNotFoundException<P>&)
			{
				util::Log::GetInstance().warn(
					"Data corrupted in the "+ ObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
					object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
					record.getValue(ObjectFieldDefinition<C>::FIELD.name) + " object was not found."
				);
			}
		}

		static std::string Serialize(const boost::optional<P&>& fieldObject, SerializationFormat format = FORMAT_INTERNAL)
		{
			return fieldObject ? boost::lexical_cast<std::string>(fieldObject->getKey()) : std::string("0");
		}

		static void SaveToParametersMap(const boost::optional<P&> & fieldObject, util::ParametersMap& map, const std::string& prefix)
		{
			map.insert(prefix + ObjectFieldDefinition<C>::FIELD.name, Serialize(fieldObject));
		}

		static void SaveToParametersMap(const boost::optional<P&> & fieldObject, const ObjectBase& object, util::ParametersMap& map, const std::string& prefix)
		{
			SaveToParametersMap(fieldObject, map, prefix);
		}

		static void SaveToParametersMap(const boost::optional<P&> & fieldObject, util::ParametersMap& map)
		{
			map.insert(ObjectFieldDefinition<C>::FIELD.name, Serialize(fieldObject));
		}

		static void SaveToParametersMap(const boost::optional<P&> & fieldObject, const ObjectBase& object, util::ParametersMap& map)
		{
			SaveToParametersMap(fieldObject, map);
		}

		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record)
		{
			try
			{
				util::RegistryKeyType id(record.getDefault<util::RegistryKeyType>(ObjectFieldDefinition<C>::FIELD.name, 0));
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
	/// Pointers vector specialization
	template<class C, class P>
	class ObjectField<C, std::vector<P*> >:
		public ObjectFieldDefinition<C>
	{
	public:
		typedef std::vector<P*> Type;

		static void LoadFromRecord(std::vector<P*>& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
		{
			if(!record.isDefined(ObjectFieldDefinition<C>::FIELD.name))
			{
				return;
			}

			fieldObject.clear();
			std::string text(record.get<std::string>(ObjectFieldDefinition<C>::FIELD.name));
			if(text.empty())
			{
				return;
			}
			std::vector<std::string> s;
			boost::algorithm::split(s, text, boost::is_any_of(","));
			BOOST_FOREACH(const std::string& item, s)
			{
				try
				{
					fieldObject.push_back(
						env.getEditable<P>(boost::lexical_cast<util::RegistryKeyType>(item)).get()
					);
				}
				catch(boost::bad_lexical_cast&)
				{
					util::Log::GetInstance().warn(
						"Data corrupted in the "+ ObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
						object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
						item + " is not a valid id."
					);
				}
				catch(util::ObjectNotFoundException<P>&)
				{
					util::Log::GetInstance().warn(
						"Data corrupted in the "+ ObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
						object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : item " +
						item + " was not found."
					);
				}
			}
		}

		static std::string Serialize(const std::vector<P*>& fieldObject, SerializationFormat format = FORMAT_INTERNAL)
		{
			std::stringstream s;
			if(format == FORMAT_SQL)
			{
				s << "\"";
			}
			bool first(true);
			BOOST_FOREACH(P* ptr, fieldObject)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << ptr->getKey();
			}
			if(format == FORMAT_SQL)
			{
				s << "\"";
			}
			return s.str();
		}

		static void SaveToParametersMap(const std::vector<P*>& fieldObject, util::ParametersMap& map, const std::string& prefix)
		{
			map.insert(prefix + ObjectFieldDefinition<C>::FIELD.name, Serialize(fieldObject));
		}

		static void SaveToParametersMap(const std::vector<P*>& fieldObject, const ObjectBase& object, util::ParametersMap& map, const std::string& prefix)
		{
			SaveToParametersMap(fieldObject, map, prefix);
		}

		static void SaveToParametersMap(const std::vector<P*>& fieldObject, util::ParametersMap& map)
		{
			map.insert(ObjectFieldDefinition<C>::FIELD.name, Serialize(fieldObject));
		}

		static void SaveToParametersMap(const std::vector<P*>& fieldObject, const ObjectBase& object, util::ParametersMap& map)
		{
			SaveToParametersMap(fieldObject, map);
		}

		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record)
		{
			std::string text(record.get<std::string>(ObjectFieldDefinition<C>::FIELD.name));
			if(text.empty())
			{
				return;
			}
			std::vector<std::string> s;
			boost::algorithm::split(s, text, boost::is_any_of(","));
			BOOST_FOREACH(const std::string& item, s)
			{
				try
				{
					list.push_back(boost::lexical_cast<util::RegistryKeyType>(item));
				}
				catch(boost::bad_lexical_cast&)
				{
				}
		}	}
	};

	FIELD_TYPE(Date, boost::gregorian::date)
	FIELD_TYPE(EndDate, boost::gregorian::date)
	FIELD_TYPE(EndTime, boost::posix_time::ptime)
	FIELD_TYPE(Key, util::RegistryKeyType)
	FIELD_TYPE(MimeType, std::string)
	FIELD_TYPE(Rank, size_t)
	FIELD_TYPE(StartDate, boost::gregorian::date)
	FIELD_TYPE(StartTime, boost::posix_time::ptime)
	FIELD_TYPE(Title, std::string)
	FIELD_TYPE(Name, std::string)
}

#endif // SYNTHESE__StandardFieldNames_hpp__
