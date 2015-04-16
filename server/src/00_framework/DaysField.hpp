
/** DaysField class header.
	@file DaysField.hpp

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

#ifndef SYNTHESE__DaysField_hpp__
#define SYNTHESE__DaysField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

#include <boost/date_time/date_duration.hpp>
#include <boost/lexical_cast.hpp>

namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}

	//////////////////////////////////////////////////////////////////////////
	/// date partial specialization
	template<class C>
	class DaysField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef boost::gregorian::date_duration Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string _daysToString(const typename DaysField<C>::Type& p)
		{
			return boost::lexical_cast<std::string>(static_cast<int>(p.days()));
		}



		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a string into a date.
		/// @param d the date to convert
		/// @return the converted string
		static typename DaysField<C>::Type _stringToDays(const std::string& text)
		{
			return text.empty() ?
				boost::gregorian::days(0) :
				boost::gregorian::days(boost::lexical_cast<int>(text));
		}


	public:
		static bool LoadFromRecord(
			typename DaysField<C>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			return SimpleObjectFieldDefinition<C>::_LoadFromStringWithDefaultValue(
				fieldObject,
				record,
				_stringToDays,
				boost::gregorian::days(0)
			);
		}



		static void SaveToFilesMap(
			const typename DaysField<C>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_daysToString
			);
		}



		static void SaveToParametersMap(
			const typename DaysField<C>::Type& fieldObject,
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
				_daysToString
			);
		}



		static void SaveToParametersMap(
			const typename DaysField<C>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_daysToString
			);
		}



		static void SaveToDBContent(
			const typename DaysField<C>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			SaveToDBContent(fieldObject, content);
		}



		static void SaveToDBContent(
			const typename DaysField<C>::Type& fieldObject,
			DBContent& content
		){
			int i(fieldObject.days());
			content.push_back(Cell(i));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
		}
	};

	#define FIELD_DAYS(N) struct N : public DaysField<N> {};
}

#endif
