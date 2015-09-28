
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

#ifndef SYNTHESE__PtimeField_hpp__
#define SYNTHESE__PtimeField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}



	//////////////////////////////////////////////////////////////////////////
	/// Date time field
	/// @ingroup m00
	template<class C>
	class PtimeField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef boost::posix_time::ptime Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string _ptimeToString(const typename PtimeField<C>::Type& d)
		{
			return 
				d.is_not_a_date_time() ?
				std::string() :
				boost::gregorian::to_iso_extended_string(d.date()) +" "+ boost::posix_time::to_simple_string(d.time_of_day())
			;
		}



	public:


		static bool LoadFromRecord(
			typename PtimeField<C>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			return SimpleObjectFieldDefinition<C>::_LoadFromStringWithDefaultValue(
				fieldObject,
				record,
				boost::posix_time::time_from_string,
				boost::posix_time::ptime(boost::posix_time::not_a_date_time)
			);
		}



		static void SaveToFilesMap(
			const typename PtimeField<C>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_ptimeToString
			);
		}



		static void SaveToParametersMap(
			const typename PtimeField<C>::Type& fieldObject,
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
				_ptimeToString
			);
		}



		static void SaveToParametersMap(
			const typename PtimeField<C>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_ptimeToString
			);
		}



		static void SaveToDBContent(
			const typename PtimeField<C>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			SaveToDBContent(fieldObject, content);
		}



		static void SaveToDBContent(
			const typename PtimeField<C>::Type& fieldObject,
			DBContent& content
		){
			boost::optional<std::string> text;
			if(!fieldObject.is_not_a_date_time())
			{
				text = _ptimeToString(fieldObject);
			}
			content.push_back(Cell(text));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){}
	};

	#define FIELD_PTIME(N) struct N : public PtimeField<N> {};

	FIELD_PTIME(EndTime)
	FIELD_PTIME(StartTime)
	FIELD_PTIME(Time)
	FIELD_PTIME(LastUpdate)
}

#endif
