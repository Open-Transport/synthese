
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

#ifndef SYNTHESE__StringField_hpp__
#define SYNTHESE__StringField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>

namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}

	//////////////////////////////////////////////////////////////////////////
	/// String field.
	/// @ingroup m00
	template<class C>
	class StringField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef std::string Type;

	private:



	public:
		static bool LoadFromRecord(
			typename StringField<C>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return false;
			}

			typename StringField<C>::Type value(
				record.get<typename StringField<C>::Type>(SimpleObjectFieldDefinition<C>::FIELD.name)
			);

			if(value == fieldObject)
			{
				return false;
			}
			else
			{
				fieldObject = value;
				return true;
			}
		}



		static void SaveToFilesMap(
			const typename StringField<C>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			if(SimpleObjectFieldDefinition<C>::FIELD.exportOnFile)
			{
				FilesMap::File item;
				item.content = fieldObject;
				item.mimeType = util::MimeTypes::TEXT;
				map.insert(
					SimpleObjectFieldDefinition<C>::FIELD.name,
					item
				);
			}
		}



		static void SaveToParametersMap(
			const typename StringField<C>::Type& fieldObject,
			const ObjectBase& object,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SaveToParametersMap(fieldObject, map, prefix, withFiles);
		}



		static void SaveToParametersMap(
			const typename StringField<C>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			if(	boost::logic::indeterminate(withFiles) ||
				SimpleObjectFieldDefinition<C>::FIELD.exportOnFile == withFiles
			){
				map.insert(
					prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
					fieldObject
				);
			}
		}




		static void SaveToDBContent(
			const typename StringField<C>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			SaveToDBContent(fieldObject, content);
		}



		static void SaveToDBContent(
			const typename StringField<C>::Type& fieldObject,
			DBContent& content
		){
			Blob blob(
				std::make_pair(
					const_cast<char*>(fieldObject.c_str()),
					fieldObject.size()
			)	);
			content.push_back(Cell(fieldObject));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
		}
	};

	#define FIELD_STRING(N) struct N : public StringField<N> {};

	FIELD_STRING(Title)
	FIELD_STRING(Name)
	FIELD_STRING(Code)
}

#endif
