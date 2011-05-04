
/** ImportableTableSync class implementation.
	@file ImportableTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "ImportableTableSync.hpp"
#include "Importable.h"
#include "DataSource.h"
#include "DataSourceTableSync.h"

#include <strstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace boost::algorithm;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace impex
	{
		const string ImportableTableSync::FIELDS_SEPARATOR("|");
		const string ImportableTableSync::SOURCES_SEPARATOR(",");



		std::string ImportableTableSync::SerializeDataSourceLinks(
			const Importable::DataSourceLinks& object
		){
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const Importable::DataSourceLinks::value_type& it, object)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << SOURCES_SEPARATOR;
				}

				if(it.first)
				{
					s << it.first->getKey();
					if(!it.second.empty())
					{
						s << FIELDS_SEPARATOR << it.second;
					}
				}
				else
				{
					s << it.second;
				}
			}
			return s.str();
		}



		Importable::DataSourceLinks ImportableTableSync::GetDataSourceLinksFromSerializedString(
			const std::string& serializedString,
			Env& env
		){
			Importable::DataSourceLinks result;
			if(!serializedString.empty())
			{
				vector<string> sources;
				split(sources, serializedString, is_any_of(SOURCES_SEPARATOR));
				BOOST_FOREACH(const string& source, sources)
				{
					try
					{
						// Parsing of the string
						RegistryKeyType sourceId(0);
						string code;
						if(!find_first(source, FIELDS_SEPARATOR))
						{ // Only datasource
							try
							{
								sourceId = lexical_cast<RegistryKeyType>(source);
							}
							catch(bad_lexical_cast)
							{
								code = source;
							}
							if(decodeTableId(sourceId) != DataSourceTableSync::TABLE.ID)
							{
								sourceId = 0;
								code = source;
							}
						}
						else
						{
							vector<string> fields;
							split(fields, source, is_any_of(FIELDS_SEPARATOR));
							sourceId = lexical_cast<RegistryKeyType>(fields[0]);
							code = fields[1];
						}

						// Source loading
						shared_ptr<DataSource> source;
						if(sourceId)
						{
							source = DataSourceTableSync::GetEditable(sourceId, env);
						}

						// Storage
						result[source.get()] = code;
					}
					catch(bad_lexical_cast)
					{ // If bad cast, the source is ignored
						continue;
					}
					catch(ObjectNotFoundException<DataSource>)
					{ // If data source is not found, it is ignored
						continue;
					}
			}	}
			return result;
		}
}	}
