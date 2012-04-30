
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

#define FIELD_TYPE(N, T) struct N : public ObjectField<N, T> {};
#define FIELD_TYPE_EXTERNAL_DATA(N) struct N : public ObjectField<N, void*> {};
#define FIELD_COMPLEX_TYPE(N, T) struct N : public ComplexObjectField<N, T> {};
#define FIELD_COMPLEX_TYPE_EXTERNAL_DATA(N) struct N : public ComplexObjectField<N, void*> {};
#define FIELD(N) boost::fusion::pair<N, N::Type>
#define FIELD_DEFAULT_CONSTRUCTOR(N) boost::fusion::make_pair<N>(N::Type())
#define FIELD_VALUE_CONSTRUCTOR(N, V) boost::fusion::make_pair<N>(V)
#define FIELD_COPY_CONSTRUCTOR(N, V) boost::fusion::make_pair<N>(N::Type(V))
#define FIELD_COMPLEX_NO_LINKED_OBJECT_ID(N)	template<> \
	void ComplexObjectField<N, N::Type>::GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record) {}
#define FIELD_NO_LINKED_OBJECT_ID(N)	template<> \
	void ObjectField<N, N::Type>::GetLinkedObjectsIdsFromText(LinkedObjectsIds& list, const std::string& record) {}
#define FIELD_DEFINITION_OF_OBJECT(N, F, VF) 	template<> const Field SimpleObjectFieldDefinition<N>::FIELD = Field(F, SQL_INTEGER); \
	template<> const bool SimpleObjectFieldDefinition<N>::EXPORT_CONTENT_AS_FILE = false; \
	template<> const Field SimpleObjectFieldDefinition<N::Vector>::FIELD = Field(VF, SQL_TEXT); \
	template<> const bool SimpleObjectFieldDefinition<N::Vector>::EXPORT_CONTENT_AS_FILE = false;
#define FIELD_DEFINITION_OF_TYPE(N, F, T) 	template<> const Field SimpleObjectFieldDefinition<N>::FIELD = Field(F, T); \
	template<> const bool SimpleObjectFieldDefinition<N>::EXPORT_CONTENT_AS_FILE = false;
#define CLASS_DEFINITION(N, T, NUM)	namespace util \
	{ template<> const std::string util::Registry<N>::KEY(#N); } \
	template<> const std::string Object<N, N::Schema>::CLASS_NAME = #N; \
	template<> const std::string Object<N, N::Schema>::TABLE_NAME = T; \
	template<> const util::RegistryTableType Object<N, N::Schema>::CLASS_NUMBER = NUM;

#endif
