
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

#ifndef SYNTHESE__NumericField_hpp__
#define SYNTHESE__NumericField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"
#include "SchemaMacros.hpp"

#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	class ObjectBase;

	//////////////////////////////////////////////////////////////////////////
	/// Numeric field.
	/// @ingroup m00
	template<class C, class T>
	class NumericField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef T Type;


		static void LoadFromRecord(
			typename NumericField<C, T>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			SimpleObjectFieldDefinition<C>::_LoadFromStringWithDefaultValue(
				fieldObject,
				record,
				boost::lexical_cast<typename NumericField<C, T>::Type, std::string>,
				typename NumericField<C, T>::Type(0)
			);
		}



		static void SaveToFilesMap(
			const typename NumericField<C, T>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				boost::lexical_cast<std::string, typename NumericField<C, T>::Type>
			);
		}



		static void SaveToParametersMap(
			const typename NumericField<C, T>::Type& fieldObject,
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
				boost::lexical_cast<std::string, typename NumericField<C, T>::Type>
			);
		}



		static void SaveToParametersMap(
			const typename NumericField<C, T>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				boost::lexical_cast<std::string, typename NumericField<C, T>::Type>
			);
		}




		static void SaveToDBContent(
			const typename NumericField<C, T>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			content.push_back(Cell(fieldObject));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){}
	};

	#define FIELD_BOOL(N) struct N : public NumericField<N, bool> {};
	#define FIELD_SIZE_T(N) struct N : public NumericField<N, size_t> {};
	#define FIELD_INT(N) struct N : public NumericField<N, int> {};
	#define FIELD_ID(N) struct N : public NumericField<N, util::RegistryKeyType> {};
	#define FIELD_DOUBLE(N) struct N : public NumericField<N, double> {};

	FIELD_ID(Key)
	FIELD_SIZE_T(Rank)
	FIELD_BOOL(Active)
}

#endif
