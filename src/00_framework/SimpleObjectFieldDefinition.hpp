
/** SimpleObjectFieldDefinition class header.
	@file SimpleObjectFieldDefinition.hpp

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

#ifndef SYNTHESE__ObjectFieldDefinition_hpp__
#define SYNTHESE__ObjectFieldDefinition_hpp__

#include "Field.hpp"
#include "FilesMap.hpp"
#include "MimeTypes.hpp"
#include "ParametersMap.h"
#include "Record.hpp"

#include <boost/logic/tribool.hpp>

namespace synthese
{
	//////////////////////////////////////////////////////////////////////////
	/// Template to use to define a simple field of a schema.
	///	@ingroup m00
	template<class C>
	class SimpleObjectFieldDefinition
	{
	public:
		static const Field FIELD;
		static void AddFields(FieldsList& l)
		{
			l.push_back(FIELD);
		}

		static const std::string& GetFieldKey()
		{
			return FIELD.name;
		}



		template<class T>
		static bool _LoadFromStringWithDefaultValue(
			T& fieldObject,
			const Record& record,
			T (*reader)(const std::string&),
			const T& default_value
		){
			if(record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				try
				{
					std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
					T value(text.empty() ? default_value : reader(text));
					if(fieldObject == value)
					{
						return false;
					}
					else
					{
						fieldObject = value;
						return true;
					}
				}
				catch(...)
				{
					if(fieldObject == default_value)
					{
						return false;
					}
					else
					{
						fieldObject = default_value;
						return true;
					}
				}
			}

			return false;
		}



		template<class T>
		static bool _LoadFromStringWithDefaultValue(
			T& fieldObject,
			const Record& record,
			T (*reader)(const std::string),
			const T& default_value
		){
			bool result(false);
			if(record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				try
				{
					std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
					T value(text.empty() ? default_value : reader(text));
					result = (fieldObject == value);
					fieldObject = value;
				}
				catch(...)
				{
					result = (fieldObject == default_value);
					fieldObject = default_value;
				}
			}
			return result;
		}


		template<class T>
		static bool _UpdateFromString(
			T& fieldObject,
			const Record& record,
			bool (T::*reader)(const std::string&)
		){
			bool result(false);
			if(record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
			{
				try
				{
					std::string text(record.get<std::string>(SimpleObjectFieldDefinition<C>::FIELD.name));
					result = (fieldObject.*reader)(text);
				}
				catch(...)
				{
				}
			}
			return result;
		}



		template<class T>
		static void _SaveToFilesMap(
			const T& fieldObject,
			FilesMap& map,
			std::string (*writer)(const T&)
		){
			if(SimpleObjectFieldDefinition<C>::FIELD.exportOnFile)
			{
				FilesMap::File item;
				try
				{
					item.content = writer(fieldObject);
				}
				catch(...)
				{
				}
				item.mimeType = util::MimeTypes::TEXT;
				map.insert(
					SimpleObjectFieldDefinition<C>::FIELD.name,
					item
				);
			}
		}



		template<class T>
		static void _SaveToFilesMap(
			const T& fieldObject,
			FilesMap& map,
			std::string (T::*writer)() const
		){
			if(SimpleObjectFieldDefinition<C>::FIELD.exportOnFile)
			{
				FilesMap::File item;
				try
				{
					item.content = (fieldObject.*writer)();
				}
				catch(...)
				{
				}
				item.mimeType = util::MimeTypes::TEXT;
				map.insert(
					SimpleObjectFieldDefinition<C>::FIELD.name,
					item
				);
			}
		}



		template<class T>
		static void _SaveToParametersMap(
			const T& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles,
			std::string (*writer)(const T&)
		){
			if(	boost::logic::indeterminate(withFiles) ||
				SimpleObjectFieldDefinition<C>::FIELD.exportOnFile == withFiles
			){
				try
				{
					map.insert(
						prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
						writer(fieldObject)
					);
				}
				catch(...)
				{
				}
			}
		}



		template<class T>
		static void _SaveToParametersMap(
			const T& fieldObject,
			util::ParametersMap& map,
			const std::string& prefix,
			boost::logic::tribool withFiles,
			std::string (T::*writer)() const
		){
			if(	boost::logic::indeterminate(withFiles) ||
				SimpleObjectFieldDefinition<C>::FIELD.exportOnFile == withFiles
			){
				try
				{
					map.insert(
						prefix + SimpleObjectFieldDefinition<C>::FIELD.name,
						(fieldObject.*writer)()
					);
				}
				catch(...)
				{
				}
			}
		}
	};
}

#endif // SYNTHESE__ObjectFieldDefinition_hpp__

