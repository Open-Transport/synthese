
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

#ifndef SYNTHESE__VectorField_hpp__
#define SYNTHESE__VectorField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "DBModule.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Pointers vector field.
	/// @ingroup m00
	template<class C, class T>
	class PointersVectorField:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef std::vector<T*> Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string _vectorToString(const typename PointersVectorField<C, T>::Type& p)
		{
			std::stringstream s;
			bool first(true);
			BOOST_FOREACH(T* ptr, p)
			{
				if(!ptr)
				{
					continue;
				}
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << ptr->getKey();
			}
			return s.str();
		}


	public:
		static void LoadFromRecord(
			typename PointersVectorField<C, T>::Type& fieldObject,
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
			std::vector<std::string> s;
			boost::algorithm::split(s, text, boost::is_any_of(","));
			BOOST_FOREACH(const std::string& item, s)
			{
				try
				{
					fieldObject.push_back(
						env.getEditable<T>(boost::lexical_cast<util::RegistryKeyType>(item)).get()
					);
				}
				catch(boost::bad_lexical_cast&)
				{
					util::Log::GetInstance().warn(
						"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
						object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
						item + " is not a valid id."
					);
				}
				catch(util::ObjectNotFoundException<T>&)
				{
					util::Log::GetInstance().warn(
						"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
						object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : item " +
						item + " was not found."
					);
				}
			}
		}



		static void SaveToFilesMap(
			const typename PointersVectorField<C, T>::Type& fieldObject,
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
			const typename PointersVectorField<C, T>::Type& fieldObject,
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
			const typename PointersVectorField<C, T>::Type& fieldObject,
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
			const typename PointersVectorField<C, T>::Type& fieldObject,
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
			std::string text(record.getDefault<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
			if(text.empty())
			{
				return;
			}
			std::vector<std::string> s;
			boost::algorithm::split(s, text, boost::is_any_of(","));
			BOOST_FOREACH(const std::string& item, s)
			{
				try
				{
					util::RegistryKeyType id(
						boost::lexical_cast<util::RegistryKeyType>(item)
					);
					if(id > 0)
					{
						list.push_back(id);
					}
				}
				catch(boost::bad_lexical_cast&)
				{
				}
		}	}
	};



	//////////////////////////////////////////////////////////////////////////
	/// ObjectBase Pointers vector field.
	/// @ingroup m00
	template<class C>
	class PointersVectorField<C, ObjectBase>:
		public SimpleObjectFieldDefinition<C>
	{
	public:
		typedef std::vector<ObjectBase*> Type;

	private:
		//////////////////////////////////////////////////////////////////////////
		/// Conversion of a date into a string to be stored (SQL format).
		/// @param d the date to convert
		/// @return the converted string
		static std::string _vectorToString(const typename PointersVectorField<C, ObjectBase>::Type& p)
		{
			std::stringstream s;
			bool first(true);
			BOOST_FOREACH(ObjectBase* ptr, p)
			{
				if(!ptr)
				{
					continue;
				}
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << ptr->getKey();
			}
			return s.str();
		}


	public:
		static void LoadFromRecord(
			typename PointersVectorField<C, ObjectBase>::Type& fieldObject,
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
			std::vector<std::string> s;
			boost::algorithm::split(s, text, boost::is_any_of(","));
			BOOST_FOREACH(const std::string& item, s)
			{
				try
				{
					ObjectBase* object(
						dynamic_cast<ObjectBase*>(
							db::DBModule::GetEditableObject(
								boost::lexical_cast<util::RegistryKeyType>(item),
								const_cast<util::Env&>(env)
							).get()
					)	);
					if(!object)
					{
						continue;
					}
					fieldObject.push_back(
						object
					);
				}
				catch(boost::bad_lexical_cast&)
				{
					util::Log::GetInstance().warn(
						"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
						object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : " +
						item + " is not a valid id."
					);
				}
				catch(util::ObjectNotFoundException<ObjectBase>&)
				{
					util::Log::GetInstance().warn(
						"Data corrupted in the "+ SimpleObjectFieldDefinition<C>::FIELD.name +" field at the load of the "+
						object.getClassName() +" object " + boost::lexical_cast<std::string>(object.getKey()) +" : item " +
						item + " was not found."
					);
				}
			}
		}



		static void SaveToFilesMap(
			const typename PointersVectorField<C, ObjectBase>::Type& fieldObject,
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
			const typename PointersVectorField<C, ObjectBase>::Type& fieldObject,
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
			const typename PointersVectorField<C, ObjectBase>::Type& fieldObject,
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
			const typename PointersVectorField<C, ObjectBase>::Type& fieldObject,
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
			std::string text(record.getDefault<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
			if(text.empty())
			{
				return;
			}
			std::vector<std::string> s;
			boost::algorithm::split(s, text, boost::is_any_of(","));
			BOOST_FOREACH(const std::string& item, s)
			{
				try
				{
					util::RegistryKeyType id(
						boost::lexical_cast<util::RegistryKeyType>(item)
					);
					if(id > 0)
					{
						list.push_back(id);
					}
				}
				catch(boost::bad_lexical_cast&)
				{
				}
		}	}
	};


	#define FIELD_POINTERS_VECTOR(N, T) struct N : public PointersVectorField<N, T> {};
}

#endif
