
/** TablesVectorField class header.
	@file TablesVectorField.hpp

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

#ifndef SYNTHESE__TablesVectorField_hpp__
#define SYNTHESE__TablesVectorField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "DBModule.h"
#include "DBTableSync.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Pointers vector field.
	/// @ingroup m00
	template<class C>
	class TablesVectorField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef std::vector<boost::shared_ptr<db::DBTableSync> > Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// @return the converted string
		static std::string _vectorToString(const typename TablesVectorField<C>::Type& p)
		{
			std::stringstream s;
			bool first(true);
			BOOST_FOREACH(boost::shared_ptr<db::DBTableSync> table, p)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << table->getFormat().ID;
			}
			return s.str();
		}


	public:
		static bool LoadFromRecord(
			typename TablesVectorField<C>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return false;
			}

			typename TablesVectorField<C>::Type value;
			std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
			if(!text.empty())
			{
				std::vector<std::string> s;
				boost::algorithm::split(s, text, boost::is_any_of(","));
				BOOST_FOREACH(const std::string& item, s)
				{
					try
					{
						value.push_back(
							db::DBModule::GetTableSync(
								boost::lexical_cast<util::RegistryTableType>(item)
						)	);
					}
					catch(boost::bad_lexical_cast&)
					{
						util::Log::GetInstance().warn(
							"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
							object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
							item + " is not a valid table id."
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



		static void SaveToFilesMap(
			const typename TablesVectorField<C>::Type& fieldObject,
			const ObjectBase& object,
			FilesMap& map
		){
			SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
				fieldObject,
				map,
				_vectorToString
			);
		}



		static void SaveToParametersMap(
			const typename TablesVectorField<C>::Type& fieldObject,
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
				_vectorToString
			);
		}



		static void SaveToParametersMap(
			const typename TablesVectorField<C>::Type& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles
		){
			SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
				fieldObject,
				map,
				prefix,
				withFiles,
				_vectorToString
			);
		}




		static void SaveToDBContent(
			const typename TablesVectorField<C>::Type& fieldObject,
			const ObjectBase& object,
			DBContent& content
		){
			std::string s(_vectorToString(fieldObject));
			content.push_back(Cell(s));
		}



		static void GetLinkedObjectsIds(
			LinkedObjectsIds& list, 
			const Record& record
		){
			return;
		}
	};

	#define FIELD_TABLES_VECTOR(N) struct N : public TablesVectorField<N> {};
}

#endif
