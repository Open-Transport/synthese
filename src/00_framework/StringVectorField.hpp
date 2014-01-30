
/** String vector field class header.
	@file StringVectorField.hpp

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

#ifndef SYNTHESE__StringVectorField_hpp__
#define SYNTHESE__StringVectorField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// String vector field.
	/// @ingroup m00
	template<class C>
	class StringVectorField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef std::vector<std::string> Type;

	
		
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string ToString(const typename StringVectorField<C>::Type& p)
		{
			std::stringstream s;
			bool first(true);
			BOOST_FOREACH(const std::string& item, p)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				std::string striped(
					boost::algorithm::replace_all_copy(item, "\\", "\\\\")
				);
				boost::algorithm::replace_all(striped, ",", "\\,");
				s << striped;
			}
			return s.str();
		}



		static bool LoadFromRecord(
			typename StringVectorField<C>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return false;
			}

			typename StringVectorField<C>::Type value;
			std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
			typename StringVectorField<C>::Type::reverse_iterator it(value.rend());

			for(size_t i(0); i<text.size(); ++i)
			{
				// Initialize the first item
				if(it == value.rend())
				{
					value.push_back(string());
					it = value.rbegin();
				}

				// Case separator
				if(	text[i] == ',')
				{
					value.push_back(string());
					it = value.rbegin();
					continue;
				}

				// Case backslash
				if(	text[i] == '\\' &&
					i+1 < text.size()
				){
					if(text[i+1] == ',')
					{
						it->push_back(',');
						++i;
						continue;
					}

					if(text[i+1] == '\\')
					{
						it->push_back('\\');
						++i;
						continue;
					}
				}

				// Default case
				it->push_back(text[i]);
			}
		
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
			const typename StringVectorField<C>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				ToString
			);
		}



		static void SaveToParametersMap(
			const typename StringVectorField<C>::Type& fieldObject,
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
				ToString
			);
		}



		static void SaveToParametersMap(
			const typename StringVectorField<C>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				ToString
			);
		}




		static void SaveToDBContent(
			const typename StringVectorField<C>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			std::string s(ToString(fieldObject));
			content.push_back(Cell(s));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
		}
	};


	#define FIELD_STRING_VECTOR(N) struct N : public StringVectorField<N> {};
}

#endif
