
/** Object class header.
	@file Object.hpp

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

#ifndef SYNTHESE__Object_hpp__
#define SYNTHESE__Object_hpp__

#include "ObjectField.hpp"
#include "UtilTypes.h"
#include "ObjectBase.hpp"
#include "ParametersMap.h"

#include <string>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/map_fwd.hpp>
#include <boost/fusion/include/prior.hpp>
#include <boost/fusion/include/end.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	struct GetLinkedObjectsIdsOperator;
		

	//////////////////////////////////////////////////////////////////////////
	/// Object template designed for all operations on the schema.
	/// Each object defines a field type usable in schema definitions for links
	/// by id.
	///	@ingroup m00
	/// @author Hugues Romain
	/// @since 3.4.0
	template<class ObjectClass_, class Schema_>
	class Object:
		public ObjectBase,
		public ObjectField<ObjectClass_, boost::optional<ObjectClass_&> >
	{
	public:
/*		template<class R>
		struct GetFieldStruct
		{
			void operator()(FieldsList& l)
			{
				GetFieldStruct<boost::fusion::result_of::next<R::type> >()(l);
				boost::fusion::result_of::value_of<typename R::type>::type::first_type::AddFields(l);
			}
		};

		template<>
		struct GetFieldStruct<boost::fusion::result_of::end<Schema_> >
		{
			void operator()(FieldsList& l)
			{
			}
		};


		static FieldsList GetFields()
		{
			FieldsList l;
			GetFieldStruct<boost::fusion::result_of::begin<Schema_> >()(l);
			return l;
		}
*/

		//////////////////////////////////////////////////////////////////////////
		/// Dynamic version of GetFields.
		/// @return the list of all fields defined by the schema
		virtual FieldsList getFields() const;

		//////////////////////////////////////////////////////////////////////////
		/// Auto generated object vector field
		class Vector:
			public ObjectField<Vector, std::vector<ObjectClass_*> >
		{
		};

	private:
		struct LoadOperator;
		struct SaveOperator;

	public:
		typedef Schema_ Schema;

	protected:
		//////////////////////////////////////////////////////////////////////////
		/// Constructor.
		/// @param schema the initial value of the record
		Object(const Schema& schema): _schema(schema) {}

	public:
		/// The name of the template instantiation (must be unique within SYNTHESE)
		static const std::string CLASS_NAME;

		/// The number of the template instantiation (must be unique within SYNTHESE)
		static const util::RegistryTableType CLASS_NUMBER;

		/// The name of the table corresponding to the template instantiation (must be unique within SYNTHESE)
		static const std::string TABLE_NAME;

		
	private:
		/// Mutable to allow references to the object for the get method (boost::fusion does not handle const references)
		mutable Schema _schema;

	protected:

	public:
		//////////////////////////////////////////////////////////////////////////
		/// Generic getter.
		/// @return the value of the specified attribute
		template<class Field>
		typename boost::fusion::result_of::at_key<Schema, Field>::type get() const;



		//////////////////////////////////////////////////////////////////////////
		/// Generic setter.
		/// @param value the new value for the specified attribute
		template<class Field>
		void set(const typename Field::Type& value);



		//////////////////////////////////////////////////////////////////////////
		/// Dynamic class number getter.
		/// @return the id of the class of the object
		virtual util::RegistryTableType getClassNumber() const { return CLASS_NUMBER; }



		//////////////////////////////////////////////////////////////////////////
		/// Dynamic class name getter.
		/// @return the name of the class of the object
		virtual const std::string& getClassName() const { return CLASS_NAME; }



		//////////////////////////////////////////////////////////////////////////
		/// Dynamic table name getter.
		/// @return the name of the table where the object are stored
		virtual const std::string& getTableName() const { return TABLE_NAME; }



		//////////////////////////////////////////////////////////////////////////
		/// Builds the list of object to get in the environment for current
		/// object loading, according to the content of the record to load.
		/// @param record the record to load
		virtual LinkedObjectsIds getLinkedObjectsIds(
			const Record& record
		) const;



		//////////////////////////////////////////////////////////////////////////
		/// Loads the content of a record into the current object.
		/// @param record the record to load
		/// @param env the environment to read to get the linked objects
		/// @warning be sure the environment is populated before the load
		virtual void loadFromRecord(
			const Record& record,
			util::Env& env
		);



		//////////////////////////////////////////////////////////////////////////
		/// Exports the content of the object into a ParametersMap object.
		/// @param withFiles Exports fields according to their EXPORT_CONTENT_AS_FILE
		/// attribute.
		/// @param withAdditionalParameters if true the map is filled up by
		/// addAdditionalParameters
		/// @param map the generated ParametersMap
		virtual void toParametersMap(
			util::ParametersMap& map,
			bool withAdditionalParameters = false,
			boost::logic::tribool withFiles = boost::logic::indeterminate,
			std::string prefix = std::string()
		) const;



		//////////////////////////////////////////////////////////////////////////
		/// Reads the id of the object in the schema instead of the _key attribute
		/// of Registrable.
		/// @return the id of the object
		virtual util::RegistryKeyType getKey() const { return get<Key>(); }



		//////////////////////////////////////////////////////////////////////////
		/// Writes the id of the object in the schema instead of the _key attribute
		/// of Registrable
		/// @param value the id of the object
		virtual void setKey(util::RegistryKeyType value){ set<Key>(value); }


	private:
		//////////////////////////////////////////////////////////////////////////
		/// Operators for fusion::foreach purpose

		//////////////////////////////////////////////////////////////////////////
		/// Loads the content of a record into the object.
		struct LoadOperator
		{
			const Record& _record;
			const util::Env& _env;
			Object<ObjectClass_, Schema_>& _object;

			LoadOperator(
				const Record& record,
				Object<ObjectClass_, Schema_>& object,
				const util::Env& env
			):	_record(record),
				_object(object),
				_env(env)
			{}

			template <typename Pair>
			void operator()(Pair& data) const
			{
				Pair::first_type::LoadFromRecord(data.second, _object, _record, _env);
			}
		};


		//////////////////////////////////////////////////////////////////////////
		/// Saves the object content into a parameters map
		struct SaveOperator
		{
			util::ParametersMap& _map;
			const Object<ObjectClass_, Schema_>& _object;
			const boost::logic::tribool _withFiles;
			const std::string& _prefix;
			
			SaveOperator(
				util::ParametersMap& map,
				const Object<ObjectClass_, Schema_>& object,
				boost::logic::tribool withFiles,
				const std::string& prefix
			):	_map(map),
				_object(object),
				_withFiles(withFiles),
				_prefix(prefix)
			{}

			template <typename Pair>
			void operator()(Pair& data) const
			{
				if(	Pair::first_type::EXPORT_CONTENT_AS_FILE && _withFiles != false ||
					!Pair::first_type::EXPORT_CONTENT_AS_FILE && _withFiles != true
				){
					Pair::first_type::SaveToParametersMap(data.second, static_cast<const ObjectBase&>(_object), _map, _prefix);
				}
			}
		};
	};



	template<class ObjectClass_, class Schema_>
	FieldsList synthese::Object<ObjectClass_, Schema_>::getFields() const
	{
		FieldsList l;
		boost::fusion::for_each(_schema, GetFieldsOperator(l));
		return l;
	}



	template<class ObjectClass_, class Schema_>
	template<class Field>
	typename boost::fusion::result_of::at_key<Schema_, Field>::type Object<ObjectClass_, Schema_>::get(
	) const {
		return boost::fusion::at_key<Field>(_schema);
	}


	template<class ObjectClass_, class Schema_>
	template<class Field>
	void Object<ObjectClass_, Schema_>::set(
		const typename Field::Type& value
	){
		boost::fusion::at_key<Field>(_schema) = value;
	}



	template<class ObjectClass_, class Schema_>
	LinkedObjectsIds Object<ObjectClass_, Schema_>::getLinkedObjectsIds(
		const Record& record
	) const {
		LinkedObjectsIds r;
		boost::fusion::for_each(_schema, GetLinkedObjectsIdsOperator(record, r));
		return r;
	}



	template<class ObjectClass_, class Schema_>
	void Object<ObjectClass_, Schema_>::loadFromRecord(
		const Record& record,
		util::Env& env
	){
		boost::fusion::for_each(_schema, LoadOperator(record, *this, env));
	}



	template<class ObjectClass_, class Schema_>
	void Object<ObjectClass_, Schema_>::toParametersMap(
		util::ParametersMap& map,
		bool withAdditionalParameters,
		boost::logic::tribool withFiles,
		std::string prefix
	) const {
		boost::fusion::for_each(_schema, SaveOperator(map, *this, withFiles, prefix));
		if(withAdditionalParameters)
		{
			addAdditionalParameters(map, prefix);
		}
	}



	struct GetFieldsOperator
	{
		FieldsList& _list;

		GetFieldsOperator(
			FieldsList& list
		):	_list(list)
		{}

		template <typename Pair>
		void operator()(Pair& data) const
		{
			Pair::first_type::AddFields(_list);
		}
	};



	struct GetLinkedObjectsIdsOperator
	{
		const Record& _record;
		LinkedObjectsIds& _list;

		GetLinkedObjectsIdsOperator(
			const Record& record,
			LinkedObjectsIds& list
		):	_record(record),
			_list(list)
		{}

		template <typename Pair>
		void operator()(Pair& data) const
		{
			Pair::first_type::GetLinkedObjectsIds(_list, _record);
		}
	};
}

#endif // SYNTHESE__Object_hpp__

