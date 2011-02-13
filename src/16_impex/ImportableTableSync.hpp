
/** ImportableTableSync class header.
	@file ImportableTableSync.hpp

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

#ifndef SYNTHESE_impex_ImportableTableSync_hpp__
#define SYNTHESE_impex_ImportableTableSync_hpp__

#include "Importable.h"
#include "DataSource.h"

#include <string>
#include <map>
#include <set>
#include <boost/foreach.hpp>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	namespace impex
	{
		//////////////////////////////////////////////////////////////////////////
		/// Table sync helpers for Importable child classes.
		///	@ingroup m16
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.2.1
		class ImportableTableSync
		{
		private:
			static const std::string SOURCES_SEPARATOR;
			static const std::string FIELDS_SEPARATOR;

		public:
			template<class T>
			class ObjectBySource
			{
			public:
				typedef std::set<typename T::ObjectType*> Set;

			private:
				typedef std::map<std::string, Set> Map;

				const DataSource& _source;
				Map _map;

			public:
				ObjectBySource(
					const DataSource& source,
					util::Env& env
				);

				bool contains(const std::string& code) const;

				Set get(const std::string& code) const;

				void add(typename T::ObjectType& object);

				const Map& getMap() const { return _map; }
			};

			static const std::string COL_DATA_SOURCE_LINKS;


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
			static std::string SerializeDataSourceLinks(
				const Importable::DataSourceLinks& object
			);



			//////////////////////////////////////////////////////////////////////////
			/// Reads storage string.
			//////////////////////////////////////////////////////////////////////////
			/// @param object the object to update
			/// @param serializedString string to read
			/// @param env environment to populate when loading data sources
			/// @author Hugues Romain
			/// @since 3.2.1
			/// @date 2010
			static Importable::DataSourceLinks GetDataSourceLinksFromSerializedString(
				const std::string& serializedString,
				util::Env& env
			);
		};




		template<class T>
		ImportableTableSync::ObjectBySource<T>::ObjectBySource(
			const DataSource& source,
			util::Env& env
		):	_source(source)
		{
			_map.clear();
			boost::shared_ptr<const DataSource> psource(Env::GetOfficialEnv().get<DataSource>(source.getKey()));
			const Registry<typename T::ObjectType>& registry(Env::GetOfficialEnv().getRegistry<typename T::ObjectType>());
			BOOST_FOREACH(const Registry<typename T::ObjectType>::value_type& v, registry)
			{
				if(!v.second->hasLinkWithSource(*psource))
				{
					continue;
				}
				add(*T::GetEditable(v.first, env));
		}	}



		template<class T>
		bool synthese::impex::ImportableTableSync::ObjectBySource<T>::contains(
			const std::string& code
		) const	{
			return _map.find(code) != _map.end();
		}




		template<class T>
		typename ImportableTableSync::ObjectBySource<T>::Set ImportableTableSync::ObjectBySource<T>::get(
			const std::string& code
		) const	{
			typename Map::const_iterator it(_map.find(code));
			if(it != _map.end())
			{
				return it->second;
			}
			return typename ImportableTableSync::ObjectBySource<T>::Set();
		}



		template<class T>
		void ImportableTableSync::ObjectBySource<T>::add(
			typename T::ObjectType& object
		){
			const std::string& code(object.getCodeBySource(_source));
			typename Map::iterator it(_map.find(code));
			if(it != _map.end())
			{
				it->second.insert(&object);
			}
			else
			{
				Set s;
				s.insert(&object);
				_map.insert(std::make_pair(code,s));
			}
		}
}	}

#endif // SYNTHESE_impex_ImportableTableSync_hpp__
