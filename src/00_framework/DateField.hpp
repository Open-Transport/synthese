
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

#ifndef SYNTHESE__DateField_hpp__
#define SYNTHESE__DateField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

#include <boost/date_time/date.hpp>

namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}


	//////////////////////////////////////////////////////////////////////////
	/// Date field.
	/// @ingroup m00
	template<class C>
	class DateField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string DateToString(const boost::gregorian::date& d)
		{
			return 
				d.is_not_a_date() ?
				std::string() :
				boost::gregorian::to_iso_extended_string(d)
			;
		}



		typedef boost::gregorian::date Type;


		static bool LoadFromRecord(
			boost::gregorian::date& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			return SimpleObjectFieldDefinition<C>::_LoadFromStringWithDefaultValue(
				fieldObject,
				record,
				boost::gregorian::from_string,
				boost::gregorian::date(boost::gregorian::not_a_date_time)
			);
		}



		static void SaveToFilesMap(
			const boost::gregorian::date& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				DateToString
			);
		}



		static void SaveToParametersMap(
			const boost::gregorian::date& fieldObject,
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
				DateToString
			);
		}



		static void SaveToParametersMap(
			const boost::gregorian::date& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				DateToString
			);
		}




		static void SaveToDBContent(
			const boost::gregorian::date& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			SaveToDBContent(fieldObject, content);
		}



		static void SaveToDBContent(
			const boost::gregorian::date& fieldObject,
			DBContent& content
		){
			boost::optional<std::string> text;
			if(!fieldObject.is_not_a_date())
			{
				text = DateToString(fieldObject);
			}
			content.push_back(Cell(text));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){}
	};

	#define FIELD_DATE(N) struct N : public DateField<N> {};

	FIELD_DATE(Date)
	FIELD_DATE(EndDate)
	FIELD_DATE(StartDate)
}

#endif
