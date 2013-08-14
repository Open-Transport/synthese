
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

#ifndef SYNTHESE__TreeNodeField_hpp__
#define SYNTHESE__TreeNodeField_hpp__

#include "ComplexObjectFieldDefinition.hpp"

#include "NumericField.hpp"

#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace tree
	{
		//////////////////////////////////////////////////////////////////////////
		/// Tree node field.
		/// @ingroup m20
		template<class C, class T>
		class TreeNodeField:
			public ComplexObjectFieldDefinition<C>
		{
		public:
			typedef void* Type;

		private:


		public:
			static bool LoadFromRecord(
				typename TreeNodeField<C, T>::Type& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			){
				assert(dynamic_cast<T*>(&object));
				T& t(static_cast<T&>(object));
				bool result(false);

				if(record.isDefined(ComplexObjectFieldDefinition<C>::FIELDS[2].name))
				{
					size_t value(
						record.getDefault<size_t>(
							ComplexObjectFieldDefinition<C>::FIELDS[2].name,
							0
					)	);
					if(value != t.getRank())
					{
						result = true;
						t.setRank(value);
					}
				}

				if(record.isDefined(ComplexObjectFieldDefinition<C>::FIELDS[1].name))
				{
					util::RegistryKeyType up_id(
						record.getDefault<util::RegistryKeyType>(
							ComplexObjectFieldDefinition<C>::FIELDS[1].name,
							0
					)	);
					if(up_id > 0)
					{
						try
						{
							T* value(
								env.getEditable<T>(up_id).get()
							);
							if(value != t.getParent())
							{
								t.setParent(env.getEditable<T>(up_id).get());
								result = true;
							}
						}
						catch(util::ObjectNotFoundException<T>&)
						{
							util::Log::GetInstance().warn(
								"Data corrupted in on node " + boost::lexical_cast<std::string>(object.getKey()) +" : up node " +
								boost::lexical_cast<std::string>(up_id) + " not found"
							);
						}
					}
					else
					{
						if(t.getParent() != NULL)
						{
							t.setParent(NULL);
							result = true;
						}
					}
				}

				if(record.isDefined(ComplexObjectFieldDefinition<C>::FIELDS[0].name))
				{
					util::RegistryKeyType id(
						record.getDefault<util::RegistryKeyType>(
							ComplexObjectFieldDefinition<C>::FIELDS[0].name,
							0
					)	);
					if(id > 0)
					{
						try
						{
							typename T::RootType* value(env.getEditable<typename T::RootType>(id).get());
							if(t.getRoot() != value)
							{
								t.setRoot(value);
								result = true;
							}
						}
						catch(util::ObjectNotFoundException<typename T::RootType>&)
						{
							util::Log::GetInstance().warn(
								"Data corrupted in on node " + boost::lexical_cast<std::string>(object.getKey()) +" : root " +
								boost::lexical_cast<std::string>(id) + " not found"
							);
						}
					}
				}

				return result;
			}



			static void SaveToFilesMap(
				const typename TreeNodeField<C, T>::Type& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			){
			}



			static void SaveToParametersMap(
				const typename TreeNodeField<C, T>::Type& fieldObject,
				const ObjectBase& object,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			){
				if(withFiles == true)
				{
					return;
				}

				assert(dynamic_cast<const T*>(&object));
				const T& t(static_cast<const T&>(object));

				map.insert(prefix + ComplexObjectFieldDefinition<C>::FIELDS[0].name, t.getRoot() ? t.getRoot()->getKey() : util::RegistryKeyType(0));
				map.insert(prefix + ComplexObjectFieldDefinition<C>::FIELDS[1].name, t.getParent(true) ? t.getParent()->getKey() : util::RegistryKeyType(0));
				map.insert(prefix + ComplexObjectFieldDefinition<C>::FIELDS[2].name, t.getRank());
			}



			static void SaveToDBContent(
				const typename TreeNodeField<C, T>::Type& fieldObject,
				const ObjectBase& object,
				DBContent& content
			){
				assert(dynamic_cast<const T*>(&object));
				const T& t(static_cast<const T&>(object));

				content.push_back(Cell(t.getRoot() ? t.getRoot()->getKey() : util::RegistryKeyType(0)));
				content.push_back(Cell(t.getParent(true) ? t.getParent()->getKey() : util::RegistryKeyType(0)));
				content.push_back(Cell(t.getRank()));
			}



			static void GetLinkedObjectsIds(
				LinkedObjectsIds& list, 
				const Record& record
			){
				util::RegistryKeyType id(
					record.getDefault<util::RegistryKeyType>(
						ComplexObjectFieldDefinition<C>::FIELDS[0].name,
						0
				)	);
				if (id > 0)
				{
					list.push_back(id);
				}

				util::RegistryKeyType up_id(
					record.getDefault<util::RegistryKeyType>(
						ComplexObjectFieldDefinition<C>::FIELDS[1].name,
						0
				)	);
				if (up_id > 0)
				{
					list.push_back(up_id);
				}
			}
		};

		#define FIELD_TREE_NODE(N, T) struct N : public tree::TreeNodeField<N, T> {};
}	}

#endif
