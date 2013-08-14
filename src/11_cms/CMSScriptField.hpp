
/** CMSScriptField class header.
	@file CMSScriptField.hpp

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

#ifndef SYNTHESE_cms_CMSScriptField_hpp__
#define SYNTHESE_cms_CMSScriptField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "CMSScript.hpp"

namespace synthese
{
	namespace cms
	{
		/** CMSScriptField class.
			@ingroup m11
		*/
		template<class C>
		class CMSScriptField:
			public SimpleObjectFieldDefinition<C>
		{
		private:
			static std::string CMSScriptToString(
				const CMSScript& object
			){
				return object.getCode();
			}

			static CMSScript StringToCMSScript(
				const std::string& s
			){
				return CMSScript(s);
			}

		public:
			typedef CMSScript Type;
			
			static bool LoadFromRecord(
				CMSScript& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			){
				return SimpleObjectFieldDefinition<C>::_LoadFromStringWithDefaultValue(
					fieldObject,
					record,
					StringToCMSScript,
					CMSScript()
				);
			}



			static void SaveToFilesMap(
				const CMSScript& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			){
				SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
					fieldObject,
					map,
					CMSScriptToString
				);
			}



			static void SaveToParametersMap(
				const CMSScript& fieldObject,
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
					CMSScriptToString
				);
			}



			static void SaveToParametersMap(
				const CMSScript& fieldObject,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			){
				SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
					fieldObject,
					map,
					prefix,
					withFiles,
					CMSScriptToString
				);
			}




			static void SaveToDBContent(
				const CMSScript& fieldObject,
				const ObjectBase& object,
				DBContent& content
			){
				SaveToDBContent(fieldObject, content);
			}



			static void SaveToDBContent(
				const CMSScript& fieldObject,
				DBContent& content
			){
				Blob b;
				b.first = const_cast<char*>(fieldObject.getCode().c_str());
				b.second = fieldObject.getCode().size();
				content.push_back(Cell(b));
			}



			static void GetLinkedObjectsIds(
				LinkedObjectsIds& list, 
				const Record& record
			){}
		};
}	}

#define FIELD_CMS_SCRIPT(N) struct N : public synthese::cms::CMSScriptField<N> {};

#endif // SYNTHESE_cms_CMSScriptField_hpp__

