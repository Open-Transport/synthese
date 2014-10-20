
/** DataSourceLinksField class header.
	@file DataSourceLinksField.hpp

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

#ifndef SYNTHESE_impex_DataSourceLinksField_hpp__
#define SYNTHESE_impex_DataSourceLinksField_hpp__

#include "SimpleObjectFieldDefinition.hpp"

#include "DataSource.h"
#include "FrameworkTypes.hpp"
#include "Importable.h"

#include <boost/algorithm/string.hpp>

namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}

	namespace impex
	{
		/** DataSourceLinksField class.
			@ingroup m16
		*/
		template<class C>
		class DataSourceLinksField:
			public SimpleObjectFieldDefinition<C>
		{
		public:
			typedef void* Type;

			static void UnSerialize(
				Importable::DataSourceLinks& fieldObject,
				const std::string& txt,
				const util::Env& env
			){
				fieldObject.clear();

				if(txt.empty())
				{
					return;
				}

				std::string text = preparse(txt);

				std::vector<std::string> sources = split(text, Importable::SOURCES_SEPARATOR, Importable::SEPARATOR_ESCAPE);
				BOOST_FOREACH(const std::string& source, sources)
				{
					try
					{
						// Parsing of the string
						util::RegistryKeyType sourceId(0);
						std::string code;
						std::vector<std::string> fields = split(source, Importable::FIELDS_SEPARATOR, Importable::SEPARATOR_ESCAPE);
						if( fields.size() < 2 )
						{ // Only datasource
							try
							{
								sourceId = boost::lexical_cast<util::RegistryKeyType>(source);
							}
							catch(boost::bad_lexical_cast)
							{
								code = source;
							}
							if(util::decodeTableId(sourceId) != DataSource::CLASS_NUMBER)
							{
								sourceId = 0;
								code = source;
							}
						}
						else
						{
							sourceId = boost::lexical_cast<util::RegistryKeyType>(fields[0]);
							code = fields[1];
						}

						// Source loading
						boost::shared_ptr<DataSource> source;
						if(sourceId)
						{
							source = env.getEditable<DataSource>(sourceId);
						}

						// Un-escape the code
						for (size_t i = 0; i < code.length(); i++)
						{
							if (code.at(i) == Importable::SEPARATOR_ESCAPE)
							{
								code.erase(i,1);
							}
						}

						// Storage
						fieldObject.insert(std::make_pair(source.get(), code));
					}
					catch(boost::bad_lexical_cast)
					{ // If bad cast, the source is ignored
						continue;
					}
					catch(util::ObjectNotFoundException<DataSource>)
					{ // If data source is not found, it is ignored
						continue;
					}
				}
			}



			static bool LoadFromRecord(
				typename DataSourceLinksField<C>::Type& fieldObject,
				ObjectBase& object,
				const Record& record,
				const util::Env& env
			){
				if(!record.isDefined(SimpleObjectFieldDefinition<C>::FIELD.name))
				{
					return false;
				}

				impex::Importable& impObject(dynamic_cast<impex::Importable&>(object));
				impex::Importable::DataSourceLinks l;
				DataSourceLinksField<C>::UnSerialize(l, record.getValue(SimpleObjectFieldDefinition<C>::FIELD.name), env);

				if(impObject.getDataSourceLinks() == l)
				{
					return false;
				}
				else
				{
					if(&env == &util::Env::GetOfficialEnv())
					{
						impObject.setDataSourceLinksWithRegistration(l);
					}
					else
					{
						impObject.setDataSourceLinksWithoutRegistration(l);
					}
					return true;
				}
			}



			//////////////////////////////////////////////////////////////////////////
			/// Builds storage string.
			/// Each couple datasource/id is separated by , . The source and the id are
			/// separated by | . If a datasource does not define any id then the , is
			/// omitted
			//////////////////////////////////////////////////////////////////////////
			/// @param object the object to serialize
			/// @return the serialized string corresponding to the object
			/// @author Hugues Romain
			/// @since 3.2.1
			/// @date 2010
			static std::string Serialize(
				const impex::Importable::DataSourceLinks& fieldObject
			){
				std::stringstream s;
				bool first(true);
				BOOST_FOREACH(const Importable::DataSourceLinks::value_type& it, fieldObject)
				{
					if(first)
					{
						first = false;
					}
					else
					{
						s << Importable::SOURCES_SEPARATOR;
					}

					std::string code = it.second;

					// Add escape to separators and escapes in the text
					for (size_t i = 0; i < code.length(); i++)
					{
						char c = code.at(i);
						if ( c == Importable::FIELDS_SEPARATOR ||
							 c == Importable::SOURCES_SEPARATOR ||
							 c == Importable::SEPARATOR_ESCAPE )
						{
							code.insert(i, 1, Importable::SEPARATOR_ESCAPE);
							// Skip the inserted character
							i++;
						}
					}

					if(it.first)
					{
						s << it.first->getKey();
						if(!code.empty())
						{
							s << Importable::FIELDS_SEPARATOR;
							s << code;
						}
					}
					else
					{
						s << code;
					}
				}
				return s.str();
			}



			static void SaveToParametersMap(
				void*& fieldObject,
				const ObjectBase& object,
				util::ParametersMap& map,
				const std::string& prefix,
				boost::logic::tribool withFiles
			){
				const Importable& impObject(dynamic_cast<const Importable&>(object));
				SimpleObjectFieldDefinition<C>::_SaveToParametersMap(
					impObject.getDataSourceLinks(),
					map,
					prefix,
					withFiles,
					Serialize
				);
			}



			static void SaveToFilesMap(
				void*& fieldObject,
				const ObjectBase& object,
				FilesMap& map
			){
				const Importable& impObject(dynamic_cast<const Importable&>(object));
				SimpleObjectFieldDefinition<C>::_SaveToFilesMap(
					impObject.getDataSourceLinks(),
					map,
					Serialize
				);
			}



			static void GetLinkedObjectsIdsFromText(LinkedObjectsIds& list, const std::string& txt)
			{

				if(txt.empty())
				{
					return;
				}

				std::string text = preparse(txt);

				std::vector<std::string> sources = split(text, Importable::SOURCES_SEPARATOR, Importable::SEPARATOR_ESCAPE);
				BOOST_FOREACH(const std::string& source, sources)
				{
					try
					{
						// Parsing of the string
						util::RegistryKeyType sourceId(0);
						std::vector<std::string> fields = split(source, Importable::FIELDS_SEPARATOR, Importable::SEPARATOR_ESCAPE);
						if( fields.size() < 2 )
						{ // Only datasource
							try
							{
								sourceId = boost::lexical_cast<util::RegistryKeyType>(source);
							}
							catch(boost::bad_lexical_cast)
							{
							}
							if(util::decodeTableId(sourceId) != DataSource::CLASS_NUMBER)
							{
								sourceId = 0;
							}
						}
						else
						{
							sourceId = boost::lexical_cast<util::RegistryKeyType>(fields[0]);
						}

						// Source loading
						if(sourceId)
						{
							list.push_back(sourceId);
						}
					}
					catch(boost::bad_lexical_cast)
					{ // If bad cast, the source is ignored
						continue;
					}
				}
			}



			static void GetLinkedObjectsIds(LinkedObjectsIds& list, const Record& record)
			{
				DataSourceLinksField<C>::GetLinkedObjectsIdsFromText(
					list,
					record.getValue(SimpleObjectFieldDefinition<C>::FIELD.name, false)
				);
			}



			static void SaveToDBContent(
				const typename DataSourceLinksField<C>::Type& fieldObject,
				const ObjectBase& object,
				DBContent& content
			){
				const Importable& impObject(dynamic_cast<const Importable&>(object));
				std::string s(DataSourceLinksField<C>::Serialize(impObject.getDataSourceLinks()));
				content.push_back(Cell(s));
			}



			/** Pre-parse a string for datasource structure

				This function will add escape characters where a separator
				should not be a separator.

				For ',', it means when the following character until the next '|' are not a number or empty.
				For '|', it means when the last separator in the string also was an '|'.
				For '\', it means when the next character is not a separator or another escape.

				@param text Text to modify
				@return Modified text
			*/
			static std::string preparse (const std::string & txt)
			{
				std::string text = txt;

				// We go through the given text
				char lastSep = ' ';
				for (size_t i = 0; i < text.length(); i++)
				{
					char c = text.at(i);

					// Escape (\)
					if ( c == Importable::SEPARATOR_ESCAPE )
					{
						// If this is the last character, it should be escaped
						if ( i + 1 == text.length() )
						{
							text.insert(i,1,Importable::SEPARATOR_ESCAPE);
						}
						else
						{
							// If the next character is a separator or an escape, everything is OK (if not awesome)
							char c2 = text.at(i+1);
							if ( c2 != Importable::FIELDS_SEPARATOR &&
								 c2 != Importable::SOURCES_SEPARATOR &&
								 c2 != Importable::SEPARATOR_ESCAPE )
							{
								text.insert(i,1,Importable::SEPARATOR_ESCAPE);
							}
						}

						// Note : skips the next character in this case because it IS escaped
						// and therefore should NOT be considered as a separator
						i++;
					}

					// Source separator (,)
					else if ( c == Importable::SOURCES_SEPARATOR )
					{
						lastSep = c;

						// Find the next field separator '|'
						size_t end = text.find_first_of(Importable::FIELDS_SEPARATOR,i);

						// If the data until next separator is not empty and is not an int, add an escape character
						if ( end != i + 1 )
						{
							try
							{
								boost::lexical_cast<util::RegistryKeyType>( text.substr(i + 1, end - i -1) );
							}
							catch(boost::bad_lexical_cast)
							{
								text.insert(i,1,Importable::SEPARATOR_ESCAPE);
								i++;
							}
						}
					}

					// Field separator (|), escape it if the last separator was also a field separator
					else if ( c == Importable::FIELDS_SEPARATOR )
					{
						if ( lastSep == Importable::FIELDS_SEPARATOR )
						{
							text.insert(i,1,Importable::SEPARATOR_ESCAPE);
							i++;
						}
						else
						{
							lastSep = c;
						}
					}
				}

				return text;
			}


			/** Splits a string into a vector using a separator
				 An escape character is used to enable the use of the separator in the text.

				 For example, if a chain to split with ',' contains a legitimate (non-separator) comma,
				 it can be escaped with '\' like this : 'hello\, world'
				 This comma will then be ignored as a separator.

				 @param text Text to split
				 @param sep Separator
				 @param escape Escape character
			*/
			static std::vector<std::string> split (const std::string & text, char sep, char escape)
			{
				std::vector<std::string> out;

				// We go through the given text
				size_t last = 0;
				for (size_t i = 0; i < text.length(); i++)
				{
					// If the character is the separator
					if (text.at(i) == sep)
					{
						// If the character preceding exists and is the escape on, ignore this separator
						if ( i > 0 && text.at(i-1) == escape )
						{
							// Except if it is itself escaped
							if ( ! (i > 1 && text.at(i-2) == escape) )
							{
								continue;
							}
						}

						// If there has been text between this separator and the preceding one,
						// add it to the output
						if ( i != last )
						{
							out.push_back( text.substr(last, i-last) );
						}

						// Set the last to the character after the separator
						last = i + 1;
					}
				}

				// If there is some characters left, add them
				if (last < text.length())
				{
					out.push_back( text.substr(last) );
				}

				return out;
			}
		};
	
		#define FIELD_DATASOURCE_LINKS(N) struct N : public DataSourceLinksField<N> {};
	
		FIELD_DATASOURCE_LINKS(DataSourceLinks)
}	}

#endif // SYNTHESE_impex_DataSourceLinksField_hpp__

