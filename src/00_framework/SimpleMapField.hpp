
/** StandardFieldNames class header.
	@file StandardFieldNames.hpp

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

#ifndef SYNTHESE__SimpleMapField_hpp__
#define SYNTHESE__SimpleMapField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "FrameworkTypes.hpp"

#include <boost/algorithm/string.hpp>
#include <map>

namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}



	//////////////////////////////////////////////////////////////////////////
	/// Simple map field.
	/// Limitation : items must not contain any , and | characters.
	template<class C, class V1, class V2>
	class SimpleMapField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef std::map<V1, V2> Type;

	private:
		static std::string _mapToString(const typename SimpleMapField<C, V1, V2>::Type& m)
		{
			std::stringstream s;
			bool first(true);
			typedef std::pair<V1, V2> mpair; 
			BOOST_FOREACH(const mpair& pair, m)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << "|";
				}
				s << pair.first << "," << pair.second;
			}
			return s.str();
		}
	public:


		static bool LoadFromRecord(
			typename SimpleMapField<C, V1, V2>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return false;
			}

			typename SimpleMapField<C, V1, V2>::Type value;
			std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
			if(!text.empty())
			{
				std::vector<std::string> s;
				boost::algorithm::split(s, text, boost::is_any_of("|"));
				BOOST_FOREACH(const std::string& pair, s)
				{
					std::vector<std::string> v;
					boost::algorithm::split(v, pair, boost::is_any_of(","));
					try
					{
						value.insert(
							std::make_pair(
								boost::lexical_cast<V1>(v[0]),
								boost::lexical_cast<V2>(v[1])
						)	);
					}
					catch(boost::bad_lexical_cast&)
					{
						util::Log::GetInstance().warn(
							"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
							object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey())
						);
					}
				}
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




		static void SaveToParametersMap(
			const typename SimpleMapField<C, V1, V2>::Type& fieldObject,
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
				_mapToString
			);
		}




		static void SaveToParametersMap(
			const typename SimpleMapField<C, V1, V2>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_mapToString
			);
		}




		static void SaveToFilesMap(
			const typename SimpleMapField<C, V1, V2>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_mapToString
			);
		}



		static void SaveToDBContent(
			const typename SimpleMapField<C, V1, V2>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			boost::optional<std::string> s;
			s = _mapToString(fieldObject);
			content.push_back(Cell(s));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list,
			const Record& record
		){
		}
	};

	#define FIELD_MAP(N, M) struct N : public SimpleMapField<N, M::key_type, M::mapped_type> {};
}

#endif
