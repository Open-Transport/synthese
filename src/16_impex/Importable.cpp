
/** Importable class implementation.
	@file Importable.cpp

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

#include "Importable.h"

#include "DataSource.h"
#include "Env.h"
#include "FrameworkTypes.hpp"
#include "ObjectBase.hpp"
#include "ParametersMap.h"
#include "Record.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace impex;
	using namespace util;
	
	FIELD_DEFINITION_OF_TYPE(DataSourceLinks, "data_source_links", SQL_TEXT)

	void ObjectField<DataSourceLinks, void*>::LoadFromRecord(
		void*& fieldObject,
		ObjectBase& object,
		const Record& record,
		const util::Env& env
	){
		if(!record.isDefined(FIELD.name))
		{
			return;
		}

		impex::Importable& impObject(dynamic_cast<impex::Importable&>(object));
		impex::Importable::DataSourceLinks l;
		UnSerialize(l, record.getValue(FIELD.name), env);
		if(&env == &util::Env::GetOfficialEnv())
		{
			impObject.setDataSourceLinksWithoutRegistration(l);
		}
		else
		{
			impObject.setDataSourceLinksWithRegistration(l);
		}
	}



	void ObjectField<DataSourceLinks, DataSourceLinks::Type>::SaveToParametersMap(
		void*& fieldObject,
		const ObjectBase& object,
		util::ParametersMap& map,
		const std::string& prefix
	){
		const Importable& impObject(dynamic_cast<const Importable&>(object));
		string s(Serialize(impObject.getDataSourceLinks()));
		map.insert(prefix + FIELD.name, s);
	}
	


	void ObjectField<DataSourceLinks, DataSourceLinks::Type>::GetLinkedObjectsIds(
		LinkedObjectsIds& list,
		const Record& record
	){
		GetLinkedObjectsIdsFromText(list, record.getValue(FIELD.name));
	}



	void ObjectField<DataSourceLinks, DataSourceLinks::Type>::GetLinkedObjectsIdsFromText(
		LinkedObjectsIds& list,
		const string& text
	){
		if(text.empty())
		{
			return;
		}

		vector<string> sources;
		split(sources, text, is_any_of(Importable::SOURCES_SEPARATOR));
		BOOST_FOREACH(const string& source, sources)
		{
			try
			{
				// Parsing of the string
				RegistryKeyType sourceId(0);
				if(!find_first(source, Importable::FIELDS_SEPARATOR))
				{ // Only datasource
					try
					{
						sourceId = lexical_cast<RegistryKeyType>(source);
					}
					catch(bad_lexical_cast)
					{
					}
					if(decodeTableId(sourceId) != DataSource::CLASS_NUMBER)
					{
						sourceId = 0;
					}
				}
				else
				{
					vector<string> fields;
					split(fields, source, is_any_of(Importable::FIELDS_SEPARATOR));
					sourceId = lexical_cast<RegistryKeyType>(fields[0]);
				}

				// Source loading
				if(sourceId)
				{
					list.push_back(sourceId);
				}
			}
			catch(bad_lexical_cast)
			{ // If bad cast, the source is ignored
				continue;
			}
		}
	}



	//////////////////////////////////////////////////////////////////////////
	/// Reads storage string.
	//////////////////////////////////////////////////////////////////////////
	/// @param object the object to update
	/// @param serializedString string to read
	/// @param env environment to populate when loading data sources
	/// @author Hugues Romain
	/// @since 3.2.1
	/// @date 2010
	void ObjectField<DataSourceLinks, DataSourceLinks::Type>::UnSerialize(
		impex::Importable::DataSourceLinks& fieldObject,
		const std::string& text,
		const util::Env& env
	){
		fieldObject.clear();
		if(text.empty())
		{
			return;
		}
		vector<string> sources;
		split(sources, text, is_any_of(Importable::SOURCES_SEPARATOR));
		BOOST_FOREACH(const string& source, sources)
		{
			try
			{
				// Parsing of the string
				RegistryKeyType sourceId(0);
				string code;
				if(!find_first(source, Importable::FIELDS_SEPARATOR))
				{ // Only datasource
					try
					{
						sourceId = lexical_cast<RegistryKeyType>(source);
					}
					catch(bad_lexical_cast)
					{
						code = source;
					}
					if(decodeTableId(sourceId) != DataSource::CLASS_NUMBER)
					{
						sourceId = 0;
						code = source;
					}
				}
				else
				{
					vector<string> fields;
					split(fields, source, is_any_of(Importable::FIELDS_SEPARATOR));
					sourceId = lexical_cast<RegistryKeyType>(fields[0]);
					code = fields[1];
				}

				// Source loading
				shared_ptr<DataSource> source;
				if(sourceId)
				{
					source = env.getEditable<DataSource>(sourceId);
				}

				// Storage
				fieldObject.insert(make_pair(source.get(), code));
			}
			catch(bad_lexical_cast)
			{ // If bad cast, the source is ignored
				continue;
			}
			catch(ObjectNotFoundException<DataSource>)
			{ // If data source is not found, it is ignored
				continue;
			}
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
	string ObjectField<DataSourceLinks, DataSourceLinks::Type>::Serialize(
		const impex::Importable::DataSourceLinks& fieldObject,
		SerializationFormat format
	){
		stringstream s;
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
					s << Importable::FIELDS_SEPARATOR << it.second;
				}
			}
			else
			{
				s << it.second;
			}
		}
		return s.str();
	}


	namespace impex
	{
		const string Importable::SOURCES_SEPARATOR = ",";
		const string Importable::FIELDS_SEPARATOR = "|";


		Importable::Importable():
			util::Registrable()
		{}



		Importable::NotLinkedWithSourceException::NotLinkedWithSourceException(
			const Importable& object,
			const DataSource& source
		):	Exception(
			"Object "+ boost::lexical_cast<std::string>(object.getKey()) +
			" not linked with the source "+ source.getName() +
			" ("+ boost::lexical_cast<std::string>(source.getKey()) +")"
		){}



		Importable::NotHaveUnknownDataSource::NotHaveUnknownDataSource(
			const Importable& object
		):	Exception(
			"Object "+ boost::lexical_cast<std::string>(object.getKey()) +
			" has no unknown source"
		){}


		bool Importable::hasLinkWithSource( const DataSource& source ) const
		{
			return _dataSourceLinks.find(&source) != _dataSourceLinks.end();
		}



		const std::string& Importable::getACodeBySource( const DataSource& source ) const
		{
			pair<DataSourceLinks::const_iterator, DataSourceLinks::const_iterator> range(
				_dataSourceLinks.equal_range(&source)
			);
			if(range.first == range.second)
			{
				throw NotLinkedWithSourceException(*this, source);
			}
			return range.first->second;
		}



		const std::string& Importable::getACodeBySource() const
		{

			DataSourceLinks::const_iterator it(_dataSourceLinks.find(NULL));
			if(it == _dataSourceLinks.end())
			{
				throw NotHaveUnknownDataSource(*this);
			}
			return it->second;
		}



		bool Importable::hasUnknownOwner() const
		{
			return _dataSourceLinks.find(NULL) != _dataSourceLinks.end();
		}



		std::string Importable::getCodeBySources() const
		{
			string result;
			BOOST_FOREACH(const DataSourceLinks::value_type& l, _dataSourceLinks)
			{
				if(l.second.empty())
				{
					continue;
				}
				if(!result.empty())
				{
					result += ",";
				}
				result += l.second;
			}
			return result;
		}



		std::vector<std::string> Importable::getCodesBySource( const DataSource& source ) const
		{
			vector<string> result;
			pair<DataSourceLinks::const_iterator, DataSourceLinks::const_iterator> range(
				_dataSourceLinks.equal_range(&source)
			);
			for(DataSourceLinks::const_iterator it(range.first); it != range.second; ++it)
			{
				result.push_back(it->second);
			}
			return result;
		}



		bool Importable::hasCodeBySource( const DataSource& source, const std::string& code ) const
		{
			pair<DataSourceLinks::const_iterator, DataSourceLinks::const_iterator> range(
				_dataSourceLinks.equal_range(&source)
			);
			for(DataSourceLinks::const_iterator it(range.first); it != range.second; ++it)
			{
				if(it->second == code)
				{
					return true;
				}
			}
			return false;
		}
}	}
