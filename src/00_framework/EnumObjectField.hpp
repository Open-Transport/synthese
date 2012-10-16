
/** EnumObjectField class header.
	@file EnumObjectField.hpp

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

#ifndef SYNTHESE__EnumObjectField_hpp__
#define SYNTHESE__EnumObjectField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Enum specialization
	template<class C, typename P>
	class EnumObjectField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef P Type;

	private:
		static std::string _enumToString(
			const typename Type& fieldObject
		){
			return boost::lexical_cast<std::string>(static_cast<int>(fieldObject));
		}

	public:
		static void LoadFromRecord(P& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
		{
			if(record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				fieldObject = static_cast<P>(record.get<int>(SimpleObjectFieldDefinition<C>::FIELD.name));
			}
		}



		static void SaveToFilesMap(
			const P& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_enumToString
			);
		}




		static void SaveToParametersMap(
			const P& fieldObject,
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
				_enumToString
			);
		}




		static void SaveToDBContent(
			const P& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			Cell cell(static_cast<int>(fieldObject));
			content.push_back(cell);
		}



		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record)
		{
		}
	};

	
	#define FIELD_ENUM(N, T) struct N : public EnumObjectField<N, T> {};
}

#endif // SYNTHESE__EnumObjectField_hpp__
