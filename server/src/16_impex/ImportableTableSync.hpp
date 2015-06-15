
/** ImportableTableSync class header.
	@file ImportableTableSync.hpp

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

#ifndef SYNTHESE_impex_ImportableTableSync_hpp__
#define SYNTHESE_impex_ImportableTableSync_hpp__

#include "Importable.h"
#include "DataSourceTableSync.h"
#include "Env.h"

#include <string>
#include <map>
#include <set>
#include <boost/foreach.hpp>

namespace synthese
{
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
		public:
			// Don't attempt to define such a constant : it will be not initialized
			// at calls in _FIELDS initialization
			// static const std::string COL_DATASOURCE_LINKS;

			template<class T>
			class ObjectBySource
			{
			public:
				typedef std::set<typename T::ObjectType*> Set;
				typedef std::map<std::string, Set> Map;

			private:
				const DataSource& _source;
				Map _map;

			public:
				ObjectBySource(
					const DataSource& source,
					util::Env& env
				);

				template<class S>
				void append();
				
				bool contains(const std::string& code) const;

				Set get(const std::string& code) const;

				Set getBeginWith(const std::string& code) const;

				void add(typename T::ObjectType& object);

				void remove(const std::string& code, typename T::ObjectType& object);

				const Map& getMap() const { return _map; }
			};



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
			boost::shared_ptr<const DataSource> psource(DataSourceTableSync::Get(source.getKey(), env));
			typename T::SearchResult objects(T::Search(env));
			BOOST_FOREACH(const typename T::SearchResult::value_type& v, objects)
			{
				if(!v->hasLinkWithSource(*psource))
				{
					continue;
				}
				add(*v);
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
		typename ImportableTableSync::ObjectBySource<T>::Set ImportableTableSync::ObjectBySource<T>::getBeginWith(
			const std::string& code
		) const	{

			typename ImportableTableSync::ObjectBySource<T>::Set result;

			BOOST_FOREACH(const typename Map::value_type& item, _map)
			{
				if(	item.first.size() >= code.size() &&
					item.first.substr(0, code.size()) == code
				){
					result.insert(item.second.begin(), item.second.end());
				}
			}

			return result;
		}



		template<class T>
		void ImportableTableSync::ObjectBySource<T>::add(
			typename T::ObjectType& object
		){
			std::vector<std::string> codes(object.getCodesBySource(_source));
			BOOST_FOREACH(const std::string& code, codes)
			{
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
		}



		template<class T>
		void synthese::impex::ImportableTableSync::ObjectBySource<T>::remove(
			const std::string& code,
			typename T::ObjectType& object
		){
			typename Map::iterator it(_map.find(code));
			if(it != _map.end())
			{
				it->second.erase(&object);
				if(it->second.empty())
				{
					_map.erase(it);
				}
			}
		}
}	}

#endif // SYNTHESE_impex_ImportableTableSync_hpp__
