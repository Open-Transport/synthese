
/** CalendarField class header.
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

#ifndef SYNTHESE__CalendarField_hpp__
#define SYNTHESE__CalendarField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "Calendar.h"
#include "ObjectBase.hpp"
#include "SchemaMacros.hpp"

#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace calendar
	{
		//////////////////////////////////////////////////////////////////////////
		/// Calendar field.
		/// @ingroup m31
		template<class C>
		class CalendarField:
			public SimpleObjectFieldDefinition<C>
		{
		public:
			typedef void* Type;

		private:


		public:
			static bool LoadFromRecord(
				typename CalendarField<C>::Type& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			){
				assert(dynamic_cast<Calendar*>(&object));
				Calendar& t(dynamic_cast<Calendar&>(object));

				if(record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
				{
					Calendar value;
					value.setFromSerializedString(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
					if(value != t)
					{
						t = value;
						return true;
					}
				}

				return false;
			}



			static void SaveToFilesMap(
				const typename CalendarField<C>::Type& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			){
			}



			static void SaveToParametersMap(
				const typename CalendarField<C>::Type& fieldObject,
				const ObjectBase& object,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			){
				if(withFiles == true)
				{
					return;
				}

				assert(dynamic_cast<const Calendar*>(&object));
				const Calendar& t(dynamic_cast<const Calendar&>(object));

				std::stringstream s;
				t.serialize(s);
				map.insert(prefix + SimpleObjectFieldDefinition<C>::FIELD.name, s.str());
			}



			static void SaveToDBContent(
				const typename CalendarField<C>::Type& fieldObject,
				const ObjectBase& object,
				DBContent& content
			){
				assert(dynamic_cast<const Calendar*>(&object));
				const Calendar& t(dynamic_cast<const Calendar&>(object));

				std::stringstream s;
				t.serialize(s);

				content.push_back(Cell(s.str()));
			}



			static void GetLinkedObjectsIds(
				LinkedObjectsIds& list, 
				const Record& record
			){
			}
		};

		#define FIELD_CALENDAR_NODE(N) struct N : public calendar::CalendarField<N> {};
	}

	FIELD_CALENDAR_NODE(Dates)
}

#endif
