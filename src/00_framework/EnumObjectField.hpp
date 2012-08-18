
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

		static void LoadFromRecord(P& fieldObject, ObjectBase& object, const Record& record, const util::Env& env)
		{
			if(record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				fieldObject = static_cast<P>(record.get<int>(SimpleObjectFieldDefinition<C>::FIELD.name));
			}
		}



		static std::string Serialize(
			const P& fieldObject,
			util::ParametersMap::SerializationFormat format
		){
			return boost::lexical_cast<std::string>(static_cast<int>(fieldObject));
		}



		static void SaveToParametersMap(
			const P& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix
		){
			map.insert(
				prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
				Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToFilesMap(
			const P& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			if(	SimpleObjectFieldDefinition<C>::FIELD.exportOnFile == true
			){
				FilesMap::File item;
				item.content = Serialize(fieldObject, util::ParametersMap::FORMAT_INTERNAL);
				item.mimeType = util::MimeTypes::TEXT;
				map.insert(
					SimpleObjectFieldDefinition<C>::FIELD.name,
					item
				);
			}
		}



		static void SaveToParametersMap(
			const P& fieldObject,
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
			const P& fieldObject,
			util::ParametersMap& map
		){
			map.insert(
				SimpleObjectFieldDefinition<C>::FIELD.name,
				Serialize(fieldObject, map.getFormat())
			);
		}



		static void SaveToParametersMap(
			const P& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map
		){
			SaveToParametersMap(fieldObject, map);
		}



		static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record)
		{
		}
	};
}

#endif // SYNTHESE__EnumObjectField_hpp__

