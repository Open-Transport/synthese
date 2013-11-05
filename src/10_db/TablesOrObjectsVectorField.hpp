
/** TablesOrObjectsVectorField class header.
	@file TablesOrObjectsVectorField.hpp

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

#ifndef SYNTHESE__TablesOrObjectsVectorField_hpp__
#define SYNTHESE__TablesOrObjectsVectorField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "DBException.hpp"
#include "DBModule.h"
#include "DBTableSync.hpp"
#include "TableOrObject.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Pointers vector field.
	/// @ingroup m00
	template<class C>
	class TablesOrObjectsVectorField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef std::vector<db::TableOrObject> Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// @return the converted string
		static std::string _vectorToString(const typename TablesOrObjectsVectorField<C>::Type& p)
		{
			std::stringstream s;
			bool first(true);
			BOOST_FOREACH(const typename TablesOrObjectsVectorField<C>::Type::value_type& item, p)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << item.getKey();
			}
			return s.str();
		}


	public:
		static bool LoadFromRecord(
			typename TablesOrObjectsVectorField<C>::Type& fieldObject,
			ObjectBase& object,
			const Record& record,
			const util::Env& env
		){
			if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				return false;
			}

			typename TablesOrObjectsVectorField<C>::Type value;
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
							db::TableOrObject(boost::lexical_cast<util::RegistryKeyType>(item), env)
						);
					}
					catch(boost::bad_lexical_cast&)
					{
						util::Log::GetInstance().warn(
							"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
							object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
							item + " is not a valid value."
						);
					}
					catch(util::ObjectNotFoundException<util::Registrable>&)
					{
						util::Log::GetInstance().warn(
							"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
							object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : item " +
							item + " was not found."
						);
					}
					catch(db::DBException&)
					{
						util::Log::GetInstance().warn(
							"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
							object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : item " +
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
			const typename TablesOrObjectsVectorField<C>::Type& fieldObject,
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
			const typename TablesOrObjectsVectorField<C>::Type& fieldObject,
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
			const typename TablesOrObjectsVectorField<C>::Type& fieldObject,
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
			const typename TablesOrObjectsVectorField<C>::Type& fieldObject,
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

	#define FIELD_TABLES_OR_OBJECTS_VECTOR(N) struct N : public TablesOrObjectsVectorField<N> {};
}

#endif
