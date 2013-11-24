
/** ImportableTemplate class header.
	@file ImportableTemplate.hpp

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

#ifndef SYNTHESE_impex_ImportableTemplate_hpp__
#define SYNTHESE_impex_ImportableTemplate_hpp__

#include "Importable.h"

#include "DataSource.h"

namespace synthese
{
	namespace impex
	{
		/** ImportableTemplate class.
			@ingroup m16
		*/
		template<class T>
		class ImportableTemplate:
			public Importable
		{
		public:
			void cleanDataSourceLinks(bool updateDataSource = false)
			{
				if(updateDataSource)
				{
					BOOST_FOREACH(const DataSourceLinks::value_type& link, _dataSourceLinks)
					{
						link.first->removeLinks(static_cast<T&>(*this));
				}	}
				_dataSourceLinks.clear();
			}


			void removeSourceLink(const DataSource& source, const std::string& code, bool updateDataSource = false)
			{
				//TODO protect this code by a mutex
				std::pair<DataSourceLinks::iterator, DataSourceLinks::iterator> range(
					_dataSourceLinks.equal_range(&source)
				);
				for(DataSourceLinks::iterator it(range.first); it != range.second; ++it)
				{
					if(it->second == code)
					{
						if(updateDataSource)
						{
							source.removeLink(static_cast<T&>(*this), code);
						}
						_dataSourceLinks.erase(it);
						return;
					}
				}
			}



			void removeSourceLinks(const DataSource& source, bool updateDataSource = false)
			{
				//TODO protect this code by a mutex
				std::pair<DataSourceLinks::iterator, DataSourceLinks::iterator> range(
					_dataSourceLinks.equal_range(&source)
				);
				std::vector<DataSourceLinks::iterator> iterators;
				for(DataSourceLinks::iterator it(range.first); it != range.second; ++it)
				{
					iterators.push_back(it);
				}
				BOOST_FOREACH(DataSourceLinks::iterator it, iterators)
				{
					if(updateDataSource)
					{
						source.removeLink(static_cast<T&>(*this), it->second);
					}
					_dataSourceLinks.erase(it);
				}
			}



			virtual void setDataSourceLinksWithRegistration(
				const DataSourceLinks& value
			){
				// Unregister old links
				BOOST_FOREACH(const DataSourceLinks::value_type& link, _dataSourceLinks)
				{
					if(link.first)
					{
						link.first->removeLink(static_cast<T&>(*this), link.second);
					}
				}

				// Saving new links
				setDataSourceLinksWithoutRegistration(value);

				// Registering new links
				BOOST_FOREACH(const DataSourceLinks::value_type& link, _dataSourceLinks)
				{
					if(link.first)
					{
						link.first->addLink(static_cast<T&>(*this), link.second);
					}
				}
			}



			void addCodeBySource(
				const DataSource& source,
				const std::string& code,
				bool storeLinkInDataSource = false
			){
				// Check if the code is not already registered
				std::pair<DataSourceLinks::const_iterator, DataSourceLinks::const_iterator> range(
					_dataSourceLinks.equal_range(&source)
				);
				for(DataSourceLinks::const_iterator it(range.first); it != range.second; ++it)
				{
					if(it->second == code)
					{
						return;
					}
				}

				// Registration of the code
				_dataSourceLinks.insert(make_pair(&source, code));
				if(storeLinkInDataSource)
				{
					source.addLink(static_cast<T&>(*this), code);
				}
			}
		};
	}
}

#endif // SYNTHESE_impex_ImportableTemplate_hpp__

