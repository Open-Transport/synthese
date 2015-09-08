
/** Classes and fields schema Macros.
	@file SchemaMacros.hpp

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

#ifndef SYNTHESE__RecordMacros_hpp__
#define SYNTHESE__RecordMacros_hpp__

#define FIELD(N) boost::fusion::pair<N, N::Type>
#define FIELD_DEFAULT_CONSTRUCTOR(N) boost::fusion::make_pair<N>(N::Type())
#define FIELD_VALUE_CONSTRUCTOR(N, V) boost::fusion::make_pair<N>(V)
#define FIELD_COPY_CONSTRUCTOR(N, V) boost::fusion::make_pair<N>(N::Type(V))
#define FIELD_DEFINITION_OF_OBJECT(N, F, VF) 	template<> const Field SimpleObjectFieldDefinition<N>::FIELD = Field(F, SQL_INTEGER); \
	template<> const Field SimpleObjectFieldDefinition<N::Vector>::FIELD = Field(VF, SQL_TEXT);
#define FIELD_DEFINITION_OF_TYPE(N, F, T) 	template<> const Field SimpleObjectFieldDefinition<N>::FIELD = Field(F, T);
#define CLASS_DEFINITION(N, T, NUM)	namespace util \
	{ template<> const std::string util::Registry<N>::KEY(#N); } \
	template<> const std::string Object<N, N::Schema>::CLASS_NAME = #N; \
	template<> const std::string Object<N, N::Schema>::TABLE_NAME = T; \
	template<> const util::RegistryTableType Object<N, N::Schema>::CLASS_NUMBER = NUM;

// new macros defined below

#define ABSTRACT_OBJECT(ObjectClass_)      \
public:\
	ObjectClass_(util::RegistryKeyType id=0);


#define ABSTRACT_GETSET(F)    \
	virtual std::string get##F() const = 0; \
	virtual void set##F(const std::string& value) = 0;


#define RECORD_WRAPPER(ObjectClass_, Schema_)      \
\
public:\
	typedef Schema_ Schema; \
	struct Vector: public PointersVectorField<Vector, ObjectClass_> {}; \
	\
private:\
	struct LoadOperator;\
	struct SaveOperator;\
	mutable Schema _schema; \
	\
protected: \
	   \
		ObjectClass_(const Schema& schema): _schema(schema) {} \
public:\
	\
	virtual FieldsList getFields() const { \
		FieldsList l; \
		GetFieldsOperator op(l); \
		boost::fusion::for_each(_schema, op); \
		return l; \
	} \
\
	ObjectClass_(util::RegistryKeyType id=0);   \
\
private: \
	\
		struct LoadOperator \
		{ \
			const Record& _record; \
			const util::Env& _env; \
			ObjectClass_& _object; \
			bool& _loadResult; \
	 \
			LoadOperator( \
				const synthese::Record& record, \
				ObjectClass_& object, \
				const util::Env& env, \
				bool& loadResult \
			):	_record(record), \
				_env(env), \
				_object(object), \
				_loadResult(loadResult) \
			{} \
	 \
			template <typename Pair> \
			void operator()(Pair& data) const \
			{ \
				_loadResult |= Pair::first_type::LoadFromRecord(data.second, _object, _record, _env); \
			} \
		}; \
	 \
	 \
		struct SaveOperator \
		{ \
			util::ParametersMap& _map; \
			const ObjectClass_& _object; \
			const boost::logic::tribool _withFiles; \
			const std::string& _prefix; \
	 \
			SaveOperator( \
				util::ParametersMap& map, \
				const ObjectClass_& object, \
				boost::logic::tribool withFiles, \
				const std::string& prefix \
			):	_map(map), \
				_object(object), \
				_withFiles(withFiles), \
				_prefix(prefix) \
			{} \
	 \
			template <typename Pair> \
			void operator()(Pair& data) const \
			{ \
				Pair::first_type::SaveToParametersMap( \
					data.second, \
					static_cast<const ObjectBase&>(_object), \
					_map, \
					_prefix, \
					_withFiles \
				); \
			} \
		}; \
	 \
	 \
	 \
		struct ToDBContentOperator \
		{ \
			DBContent& _content; \
			const ObjectClass_& _object; \
	 \
			ToDBContentOperator( \
				DBContent& content, \
				const ObjectClass_& object \
			):	_content(content), \
				_object(object) \
			{} \
	 \
			template <typename Pair> \
			void operator()(Pair& data) const \
			{ \
				Pair::first_type::SaveToDBContent( \
					data.second, \
					static_cast<const ObjectBase&>(_object), \
					_content \
				); \
			} \
		}; \
	 \
	 \
		struct SaveFileOperator \
		{ \
			FilesMap& _map; \
			const ObjectClass_& _object; \
	 \
			SaveFileOperator( \
				FilesMap& map, \
				const ObjectClass_& object \
			):	_map(map), \
				_object(object) \
			{} \
	 \
			template <typename Pair> \
			void operator()(Pair& data) const \
			{ \
				Pair::first_type::SaveToFilesMap( \
					data.second, \
					static_cast<const ObjectBase&>(_object), \
					_map \
				); \
			} \
		}; \
	 \
	 \
	 \
		struct DynamicGetOperator \
		{ \
			const std::string& _fieldKey; \
			const void* & _result; \
	 \
			DynamicGetOperator( \
				const void* & result, \
				const std::string& fieldKey \
			):	_fieldKey(fieldKey), \
				_result(result) \
			{} \
	 \
			template <typename Pair> \
			void operator()(Pair& data) const \
			{ \
				if(_result) \
				{ \
					return; \
				} \
	 \
				if(	Pair::first_type::GetFieldKey() == _fieldKey \
				){ \
					_result = (void*) &data.second; \
				} \
			} \
		}; \
	 \
	 \
	 \
		struct DynamicSetOperator \
		{ \
			const std::string& _fieldKey; \
			const void* _value; \
	 \
			DynamicSetOperator( \
				const void* value, \
				const std::string& fieldKey \
			):	_fieldKey(fieldKey), \
				_value(value) \
			{} \
	 \
			template <typename Pair> \
			void operator()(Pair& data) const \
			{ \
				if(	Pair::first_type::GetFieldKey() == _fieldKey \
				){ \
					data.second = *((const typename Pair::second_type*) _value); \
				} \
			} \
		}; \
	\
		struct GetFieldsOperator \
		{ \
			FieldsList& _list; \
	 \
			GetFieldsOperator( \
				FieldsList& list \
			):	_list(list) \
			{} \
	 \
			template <typename Pair> \
			void operator()(Pair& data) const \
			{ \
				Pair::first_type::AddFields(_list); \
			} \
		}; \
\
		struct GetLinkedObjectsIdsOperator \
		{ \
			const synthese::Record& _record; \
			LinkedObjectsIds& _list; \
	 \
			GetLinkedObjectsIdsOperator( \
				const synthese::Record& record, \
				LinkedObjectsIds& list \
			):	_record(record), \
				_list(list) \
			{} \
	 \
			template <typename Pair> \
			void operator()(Pair& data) const \
			{ \
				Pair::first_type::GetLinkedObjectsIds(_list, _record); \
			} \
		}; \
		 \
public: \
\
	template<class Field> typename boost::fusion::result_of::at_key<Schema, Field>::type get() const { \
		return boost::fusion::at_key<Field>(_schema); \
	}\
\
	template<class Field> void set(const typename Field::Type& value) { \
		boost::fusion::at_key<Field>(_schema) = value; \
	}\
\
	virtual const void* _dynamic_get(const std::string& fieldKey) const {\
		const void* result(NULL); \
		DynamicGetOperator op(result, fieldKey); \
		boost::fusion::for_each(_schema, op); \
		return result; \
	} \
	\
	virtual void _dynamic_set(const void* value, const std::string& fieldKey) {\
		DynamicSetOperator op(value, fieldKey); \
		boost::fusion::for_each(_schema, op); \
	}\
	virtual util::RegistryTableType getClassNumber() const { return CLASS_NUMBER; } \
	virtual const std::string& getClassName() const { return CLASS_NAME; } \
	virtual const std::string& getTableName() const { return TABLE_NAME; } \
\
	virtual util::RegistryKeyType getKey() const { return get<Key>(); } \
	virtual void setKey(util::RegistryKeyType value){ set<Key>(value); } \
	\
	virtual LinkedObjectsIds getLinkedObjectsIds(const Record& record) const {\
		LinkedObjectsIds r;\
		GetLinkedObjectsIdsOperator op(record, r);\
		boost::fusion::for_each(_schema, op);\
		return r;\
	}\
\
	virtual bool loadFromRecord(const Record& record, util::Env& env) {\
		bool loadResult(false); \
		LoadOperator op(record, *this, env, loadResult); \
		boost::fusion::for_each(_schema, op); \
		return loadResult; \
	}\
\
	virtual void toParametersMap(util::ParametersMap& map,	\
								 bool withAdditionalParameters = false,\
								 boost::logic::tribool withFiles = boost::logic::indeterminate,\
								 std::string prefix = std::string()) const {\
		SaveOperator op(map, *this, withFiles, prefix); \
		boost::fusion::for_each(_schema, op); \
		if(withAdditionalParameters) addAdditionalParameters(map, prefix); \
	}\
\
	virtual DBContent toDBContent() const {\
		DBContent result; \
		ToDBContentOperator op(result, *this); \
		boost::fusion::for_each(_schema, op); \
		return result; \
	}\
\
	virtual void toFilesMap(FilesMap& map) const {\
		SaveFileOperator op(map, *this); \
		boost::fusion::for_each(_schema, op); \
	}\
\
	virtual boost::shared_ptr<ObjectBase> copy() const {\
		boost::shared_ptr<ObjectClass_> object(new ObjectClass_); \
		object->_schema = _schema; \
		return boost::static_pointer_cast<ObjectBase, ObjectClass_>(object); \
	}



#define RECORD_GETSET(F)    \
	virtual std::string get##F() const { return get<F>(); } \
	virtual void set##F(const std::string& value) { set<F>(value); }

#define RECORD_FIELD_IMPL(N, F, T) 	template<> const Field SimpleObjectFieldDefinition<N>::FIELD = Field(F, T);


#define TABLE_REGISTRY_DECL(ObjectClass_) \
	static const std::string CLASS_NAME; \
	static const util::RegistryTableType CLASS_NUMBER; \
	static const std::string TABLE_NAME; \
	typedef util::Registry<ObjectClass_>	Registry;


#define TABLE_REGISTRY_IMPL(N, T, NUM)	namespace util \
	{ \
	  template<> const std::string util::Registry<N>::KEY(#N); \
	}\
	const std::string N::CLASS_NAME = #N; \
	const std::string N::TABLE_NAME = T; \
	const util::RegistryTableType N::CLASS_NUMBER = NUM;






#endif


