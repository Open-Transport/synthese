
/** ParametersMapField class header.
	@file ParametersMapField.hpp

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

#ifndef SYNTHESE__ParametersMapField_hpp__
#define SYNTHESE__ParametersMapField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"
#include "ParametersMap.h"
#include "URI.hpp"

namespace synthese
{
	namespace util
	{
		class Env;
	}

	class ObjectBase;

	/** ParametersMapField class.
		@ingroup m00
	*/
	template<class C>
	class ParametersMapField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef util::ParametersMap Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string _pmToString(const util::ParametersMap& p)
		{
			std::stringstream s;
			bool first(true);
			BOOST_FOREACH(const util::ParametersMap::Map::value_type& item, p.getMap())
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << util::URI::PARAMETER_SEPARATOR;
				}
				s << item.first << util::URI::PARAMETER_ASSIGNMENT << util::URI::Encode(item.second);
			}
			return s.str();
		}


	public:
		static void LoadFromRecord(
			typename ParametersMapField<C>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return;
			}

			fieldObject.clear();
			std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
			if(text.empty())
			{
				return;
			}

			fieldObject = util::ParametersMap(text);
		}



		static void SaveToFilesMap(
			const typename ParametersMapField<C>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_pmToString
			);
		}



		static void SaveToParametersMap(
			const typename ParametersMapField<C>::Type& fieldObject,
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
				_pmToString
			);
		}



		static void SaveToParametersMap(
			const typename ParametersMapField<C>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_pmToString
			);
		}




		static void SaveToDBContent(
			const typename ParametersMapField<C>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			std::string s(_pmToString(fieldObject));
			content.push_back(Cell(s));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
		}
	};

	#define FIELD_PARAMETERS_MAP(N) struct N : public ParametersMapField<N> {};

	FIELD_PARAMETERS_MAP(Parameters)
}

#endif // SYNTHESE__ParametersMapField_hpp__

