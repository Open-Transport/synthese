
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

#include "SimpleObjectField.hpp"

#include "CoordinatesSystem.hpp"
#include "Env.h"
#include "Log.h"
#include "MimeTypes.hpp"
#include "ObjectBase.hpp"
#include "SchemaMacros.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/io/ParseException.h>

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
			util::ParametersMap::SerializationFormat format
		){
			switch(format)
			{
			case util::ParametersMap::FORMAT_SQL:
				return fieldObject.is_not_a_date() ?
					"NULL" :
					"'"+ boost::gregorian::to_iso_extended_string(fieldObject) +"'";

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
			util::ParametersMap::SerializationFormat format
		){
			switch(format)
			{
			case util::ParametersMap::FORMAT_SQL:
				return fieldObject.is_not_a_date_time() ?
					"NULL" :
					"'"+ boost::gregorian::to_iso_extended_string(fieldObject.date()) +" "+ boost::posix_time::to_simple_string(fieldObject.time_of_day()) +"'";

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
			util::ParametersMap::SerializationFormat format
		){
			switch(format)
			{
			case util::ParametersMap::FORMAT_SQL:
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
			util::ParametersMap::SerializationFormat format
		){
			switch(format)
			{
			case util::ParametersMap::FORMAT_SQL:
				return "'"+ boost::algorithm::replace_all_copy(fieldObject, "'", "''") + "'";

			default:
				return fieldObject;
		}	}

		/// Do nothing
		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record) {}
	};



	template<class C, class T>
	class SerializationByLexicalCast:
		public SimpleObjectFieldDefinition<C>
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
			util::ParametersMap::SerializationFormat format
		){
			return boost::lexical_cast<std::string>(fieldObject);
		}

		static void LoadFromRecord(T& fieldObject, const Record& record)
		{
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return;
			}

			ObjectField<C, T>::UnSerialize(
				fieldObject,
				record.getDefault<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name)
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


		static void SaveToFilesMap(
			const T& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			if(	SimpleObjectFieldDefinition<C>::FIELD.exportOnFile == true
			){
				FilesMap::File item;
				item.content = ObjectField<C, T>::Serialize(fieldObject, util::ParametersMap::FORMAT_INTERNAL);
				item.mimeType = util::MimeTypes::TEXT;
				map.insert(
					SimpleObjectFieldDefinition<C>::FIELD.name,
					item
				);
			}
		}



		static void SaveToParametersMap(
			const T& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix
		){
			map.insert(
				prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
				ObjectField<C, T>::Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToParametersMap(
			const T& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			if(	boost::logic::indeterminate(withFiles) ||
				SimpleObjectFieldDefinition<C>::FIELD.exportOnFile == withFiles
			){
				SaveToParametersMap(fieldObject, map, prefix);
			}
		}



		static void SaveToParametersMap(
			const T& fieldObject,
			util::ParametersMap& map
		){
			map.insert(
				SimpleObjectFieldDefinition<C>::FIELD.name,
				ObjectField<C, T>::Serialize(fieldObject, map.getFormat())
			);
		}

		static void SaveToParametersMap(const T& fieldObject, const ObjectBase& object, util::ParametersMap& map)
		{
			SaveToParametersMap(fieldObject, map);
		}

		/// Do nothing
		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record) {}
	};



	template<class C, class T>
	class GeometrySerialization:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef boost::shared_ptr<T> Type;

		static void UnSerialize(
			Type& fieldObject,
			const std::string& text
		){
			fieldObject = Type();
			if(text.empty())
			{
				return;
			}

			geos::io::WKTReader reader(&CoordinatesSystem::GetStorageCoordinatesSystem().getGeometryFactory());

			try
			{
				fieldObject = boost::static_pointer_cast<T, geos::geom::Geometry>(
					CoordinatesSystem::GetInstanceCoordinatesSystem().convertGeometry(
						*boost::shared_ptr<geos::geom::Geometry>(reader.read(text))
				)	);
			}
			catch(geos::io::ParseException&)
			{
			}
		}



		static void UnSerialize(
			Type& fieldObject,
			const std::string& text,
			const util::Env& env
		){
			UnSerialize(fieldObject, text);
		}



		static std::string Serialize(
			const Type& fieldObject,
			util::ParametersMap::SerializationFormat format
		){
			if(!fieldObject.get() || fieldObject->isEmpty())
			{
				return std::string();
			}

			boost::shared_ptr<geos::geom::Geometry> projected(fieldObject);
			if(CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() !=
				static_cast<CoordinatesSystem::SRID>(fieldObject->getSRID()))
			{
				projected = CoordinatesSystem::GetStorageCoordinatesSystem().convertGeometry(*fieldObject);
			}

			std::stringstream str;
			if(format == util::ParametersMap::FORMAT_SQL)
			{
				str << "GeomFromText('";
			}
			geos::io::WKTWriter writer;
			str << writer.write(projected.get());
			if(format == util::ParametersMap::FORMAT_SQL)
			{
				str << "'," << CoordinatesSystem::GetStorageCoordinatesSystem().getSRID() << ")";
			}
			return str.str();
		}



		static void LoadFromRecord(Type& fieldObject, const Record& record)
		{
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return;
			}

			GeometrySerialization<C, T>::UnSerialize(
				fieldObject,
				record.getDefault<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name)
			);
		}



		static void LoadFromRecord(Type& fieldObject, const Record& record, const util::Env& env)
		{
			LoadFromRecord(fieldObject, record);
		}



		static void LoadFromRecord(Type& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
		{
			LoadFromRecord(fieldObject, record);
		}



		static void SaveToFilesMap(
			const Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){}



		static void SaveToParametersMap(
			const Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix
		){
			map.insert(
				prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
				GeometrySerialization<C, T>::Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToParametersMap(
			const Type& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			if(	boost::logic::indeterminate(withFiles) ||
				FIELD.exportOnFile == withFiles
			){
				SaveToParametersMap(fieldObject, map, prefix);
			}
		}



		static void SaveToParametersMap(
			const Type& fieldObject,
			util::ParametersMap& map
		){
			map.insert(
				SimpleObjectFieldDefinition<C>::FIELD.name,
				GeometrySerialization<C, T>::Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToParametersMap(
			const Type& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map
		){
			SaveToParametersMap(fieldObject, map);
		}



		/// Do nothing
		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record) {}
	};



	//////////////////////////////////////////////////////////////////////////
	/// Point partial specialization
	template<class C>
	class ObjectField<C, boost::shared_ptr<geos::geom::Point> >:
		public GeometrySerialization<C, geos::geom::Point>
	{};



	//////////////////////////////////////////////////////////////////////////
	/// LineString partial specialization
	template<class C>
	class ObjectField<C, boost::shared_ptr<geos::geom::LineString> >:
		public GeometrySerialization<C, geos::geom::LineString>
	{};



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



#ifndef _WIN64
	//////////////////////////////////////////////////////////////////////////
	/// size_t partial specialization
	template<class C>
	class ObjectField<C, size_t>:
		public SerializationByLexicalCast<C, size_t>
	{};
#endif



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
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef boost::optional<P&> Type;

		//////////////////////////////////////////////////////////////////////////
		/// Auto generated object vector field
		class Vector:
			public ObjectField<Vector, std::vector<P*> >
		{
		};

		static void LoadFromRecord(boost::optional<P&>& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
		{
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return;
			}

			fieldObject = boost::none;
			try
			{
				util::RegistryKeyType id(record.getDefault<util::RegistryKeyType>(SimpleObjectFieldDefinition<C>::FIELD.name, 0));
				if(id > 0)
				{
					fieldObject = *env.getEditable<P>(id);
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
			catch(util::ObjectNotFoundException<P>&)
			{
				util::Log::GetInstance().warn(
					"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
					object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
					record.getValue(SimpleObjectFieldDefinition<C>::FIELD.name) + " object was not found."
				);
			}
		}



		static std::string Serialize(
			const boost::optional<P&>& fieldObject,
			util::ParametersMap::SerializationFormat format
		){
			return fieldObject ? boost::lexical_cast<std::string>(fieldObject->getKey()) : std::string("0");
		}



		static void SaveToParametersMap(
			const boost::optional<P&> & fieldObject,
			util::ParametersMap& map,
			const std::string& prefix
		){
			map.insert(
				prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
				Serialize(fieldObject, map.getFormat())
			);
		}


		static void SaveToFilesMap(
			const boost::optional<P&> & fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){}



		static void SaveToParametersMap(
			const boost::optional<P&> & fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			if(	boost::logic::indeterminate(withFiles) ||
				FIELD.exportOnFile == withFiles
			){
				SaveToParametersMap(fieldObject, map, prefix);
			}
		}



		static void SaveToParametersMap(
			const boost::optional<P&> & fieldObject,
			util::ParametersMap& map
		){
			map.insert(
				SimpleObjectFieldDefinition<C>::FIELD.name,
				Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToParametersMap(
			const boost::optional<P&> & fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map
		){
			SaveToParametersMap(fieldObject, map);
		}



		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record)
		{
			try
			{
				util::RegistryKeyType id(record.getDefault<util::RegistryKeyType>(SimpleObjectFieldDefinition<C>::FIELD.name, 0));
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
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef std::vector<P*> Type;

		static void LoadFromRecord(std::vector<P*>& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
		{
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return;
			}

			fieldObject.clear();
			std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
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
						"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
						object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
						item + " is not a valid id."
					);
				}
				catch(util::ObjectNotFoundException<P>&)
				{
					util::Log::GetInstance().warn(
						"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
						object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : item " +
						item + " was not found."
					);
				}
			}
		}



		static std::string Serialize(
			const std::vector<P*>& fieldObject,
			util::ParametersMap::SerializationFormat format
		){
			std::stringstream s;
			if(format == util::ParametersMap::FORMAT_SQL)
			{
				s << "'";
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
			if(format == util::ParametersMap::FORMAT_SQL)
			{
				s << "'";
			}
			return s.str();
		}



		static void SaveToParametersMap(
			const std::vector<P*>& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix
		){
			map.insert(
				prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
				Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToParametersMap(
			const std::vector<P*>& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			if(	boost::logic::indeterminate(withFiles) ||
				FIELD.exportOnFile == withFiles
			){
				SaveToParametersMap(fieldObject, map, prefix);
			}
		}



		static void SaveToParametersMap(
			const std::vector<P*>& fieldObject,
			util::ParametersMap& map
		){
			map.insert(
				SimpleObjectFieldDefinition<C>::FIELD.name,
				Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToParametersMap(
			const std::vector<P*>& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map
		){
			SaveToParametersMap(fieldObject, map);
		}



		static void SaveToFilesMap(
			const std::vector<P*>& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list,
			const Record& record
		){
			std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
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
					util::RegistryKeyType id(
						boost::lexical_cast<util::RegistryKeyType>(item)
					);
					if(id > 0)
					{
						list.push_back(id);
					}
				}
				catch(boost::bad_lexical_cast&)
				{
				}
		}	}
	};



	//////////////////////////////////////////////////////////////////////////
	/// map V1->V2 specialization
	template<class C, class V1, class V2>
	class ObjectField<C, std::map<V1, V2> >:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef std::map<V1, V2> Type;

		static void LoadFromRecord(std::map<V1, V2>& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
		{
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return;
			}

			fieldObject.clear();
			std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
			if(text.empty())
			{
				return;
			}
			std::vector<std::string> s;
			boost::algorithm::split(s, text, boost::is_any_of("|"));
			BOOST_FOREACH(const std::string& pair, s)
			{
				std::vector<std::string> v;
				boost::algorithm::split(v, pair, boost::is_any_of(","));
				try
				{
					fieldObject.insert(std::pair<V1, V2> (
						boost::lexical_cast<V1>(v[0]),
						boost::lexical_cast<V2>(v[1])
						)	);
				}
				catch(boost::bad_lexical_cast&)
				{
					util::Log::GetInstance().warn(
						"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
						object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey())
						);
				}
			}
		}



		static std::string Serialize(
			const std::map<V1, V2>& fieldObject,
			util::ParametersMap::SerializationFormat format
		){
			std::stringstream s;
			if(format == util::ParametersMap::FORMAT_SQL)
			{
				s << "'";
			}
			bool first(true);
			typedef std::pair<V1, V2> mpair; 
			BOOST_FOREACH(const mpair& pair, fieldObject)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << "|";
				}
				s << pair.first << "," << pair.second;
			}
			if(format == util::ParametersMap::FORMAT_SQL)
			{
				s << "'";
			}
			return s.str();
		}



		static void SaveToParametersMap(
			const std::map<V1, V2>& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix
		){
			map.insert(
				prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
				Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToParametersMap(
			const std::map<V1, V2>& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			if(	boost::logic::indeterminate(withFiles) ||
				FIELD.exportOnFile == withFiles
			){
				SaveToParametersMap(fieldObject, map, prefix);
			}
		}



		static void SaveToParametersMap(
			const std::map<V1, V2>& fieldObject,
			util::ParametersMap& map
		){
			map.insert(
				SimpleObjectFieldDefinition<C>::FIELD.name,
				Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToParametersMap(
			const std::map<V1, V2>& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map
		){
			SaveToParametersMap(fieldObject, map);
		}



		static void SaveToFilesMap(
			const std::map<V1, V2>& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list,
			const Record& record
		){
			return;
		}
	};


	FIELD_TYPE(Date, boost::gregorian::date)
	FIELD_TYPE(EndDate, boost::gregorian::date)
	FIELD_TYPE(EndTime, boost::posix_time::ptime)
	FIELD_TYPE(Key, util::RegistryKeyType)
	FIELD_TYPE(Rank, size_t)
	FIELD_TYPE(StartDate, boost::gregorian::date)
	FIELD_TYPE(StartTime, boost::posix_time::ptime)
	FIELD_TYPE(Title, std::string)
	FIELD_TYPE(Name, std::string)
	FIELD_TYPE(PointGeometry, boost::shared_ptr<geos::geom::Point>)
	FIELD_TYPE(LineStringGeometry, boost::shared_ptr<geos::geom::LineString>)
}

#endif // SYNTHESE__StandardFieldNames_hpp__
