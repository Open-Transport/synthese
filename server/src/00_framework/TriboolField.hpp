
/** TriboolField class header.
	@file TriboolField.hpp

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

#ifndef SYNTHESE__TriboolField_hpp__
#define SYNTHESE__TriboolField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

#include <boost/date_time/date_duration.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/logic/tribool.hpp>

namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}

	//////////////////////////////////////////////////////////////////////////
	/// tribool specialization
	template<class C>
	class TriboolField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef boost::logic::tribool Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a tribool into a string to be stored (SQL format).
		/// @param value the tribool to convert
		/// @return the converted string
		static std::string _triboolToString(const typename TriboolField<C>::Type& value)
		{
			return boost::lexical_cast<std::string>(value == true ? 1 : value == false ? 0 : -1);
		}

		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a string into a tribool.
		/// @param text the text to convert
		/// @return the converted tribool
		static typename TriboolField<C>::Type _stringToTribool(const std::string& text)
		{
			std::string str = boost::to_lower_copy (text);
			boost::trim (str);
			if(	str == "false" ||
				str == "no" ||
				str == "0"
			){
				return false;
			}
			try
			{
				if(	str == "true" ||
					str == "yes" ||
					(!str.empty() && boost::lexical_cast<int>(str)>0)
				){
					return true;
				}
			}
			catch(boost::bad_lexical_cast&)
			{
			}
			return boost::logic::indeterminate;
		}

	public:
		static bool LoadFromRecord(
			typename TriboolField<C>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			return SimpleObjectFieldDefinition<C>::_LoadFromStringWithDefaultValue(
				fieldObject,
				record,
				_stringToTribool,
				boost::logic::tribool(boost::logic::indeterminate)
			);
		}



		static void SaveToFilesMap(
			const typename TriboolField<C>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_triboolToString
			);
		}



		static void SaveToParametersMap(
			const typename TriboolField<C>::Type& fieldObject,
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
				_triboolToString
			);
		}



		static void SaveToParametersMap(
			const typename TriboolField<C>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_triboolToString
			);
		}



		static void SaveToDBContent(
			const typename TriboolField<C>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			SaveToDBContent(fieldObject, content);
		}



		static void SaveToDBContent(
			const typename TriboolField<C>::Type& fieldObject,
			DBContent& content
		){
			content.push_back(Cell(fieldObject));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
		}
	};

	#define FIELD_TRIBOOL(N) struct N : public TriboolField<N> {};
}

#endif
