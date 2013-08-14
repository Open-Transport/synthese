
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
				const std::string& text,
				const util::Env& env
			){
				fieldObject.clear();
				if(text.empty())
				{
					return;
				}
				std::vector<std::string> sources;
				boost::algorithm::split(sources, text, boost::is_any_of(Importable::SOURCES_SEPARATOR));
				BOOST_FOREACH(const std::string& source, sources)
				{
					try
					{
						// Parsing of the string
						util::RegistryKeyType sourceId(0);
						std::string code;
						if(!boost::algorithm::find_first(source, Importable::FIELDS_SEPARATOR))
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
							std::vector<std::string> fields;
							boost::algorithm::split(fields, source, boost::is_any_of(Importable::FIELDS_SEPARATOR));
							sourceId = boost::lexical_cast<util::RegistryKeyType>(fields[0]);
							code = fields[1];
						}

						// Source loading
						boost::shared_ptr<DataSource> source;
						if(sourceId)
						{
							source = env.getEditable<DataSource>(sourceId);
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

					if(it.first)
					{
						s << it.first->getKey();
						if(!it.second.empty())
						{
							s << Importable::FIELDS_SEPARATOR;
							s << it.second;
						}
					}
					else
					{
						s << it.second;
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



			static void GetLinkedObjectsIdsFromText(LinkedObjectsIds& list, const std::string& text)
			{
				if(text.empty())
				{
					return;
				}

				std::vector<std::string> sources;
				boost::algorithm::split(sources, text, boost::is_any_of(Importable::SOURCES_SEPARATOR));
				BOOST_FOREACH(const std::string& source, sources)
				{
					try
					{
						// Parsing of the string
						util::RegistryKeyType sourceId(0);
						if(!boost::algorithm::find_first(source, Importable::FIELDS_SEPARATOR))
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
							std::vector<std::string> fields;
							boost::algorithm::split(fields, source, boost::is_any_of(Importable::FIELDS_SEPARATOR));
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
		};
	
		#define FIELD_DATASOURCE_LINKS(N) struct N : public DataSourceLinksField<N> {};
	
		FIELD_DATASOURCE_LINKS(DataSourceLinks)
}	}

#endif // SYNTHESE_impex_DataSourceLinksField_hpp__

