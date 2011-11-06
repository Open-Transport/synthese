////////////////////////////////////////////////////////////////////////////////
///	Importable class header.
///	@file Importable.h
///	@author Hugues Romain (RCS)
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_Importable_h__
#define SYNTHESE_Importable_h__

#include "Registrable.h"
#include "Exception.h"
#include "DataSource.h"

#include <string>
#include <map>
#include <vector>
#include <boost/foreach.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;

		////////////////////////////////////////////////////////////////////////
		/// Importable class.
		/// Defines links between data sources and an object.
		/// A link can contain the id of the object in the source database. Such
		/// an id can follow any pattern The only one limitation is not to
		/// contain the | or , characters.
		/// For historical reasons, an importable object can know its code in
		/// the database of its owner, without knowing its datasource.
		//////////////////////////////////////////////////////////////////////////
		/// @author Hugues Romain
		/// @ingroup m16
		class Importable:
			virtual public util::Registrable
		{
		public:
			typedef std::multimap<const DataSource*, std::string> DataSourceLinks;

			class NotLinkedWithSourceException : public synthese::Exception
			{
			public:
				NotLinkedWithSourceException(
					const Importable& object,
					const DataSource& source
				);
			};

			class NotHaveUnknownDataSource : public Exception
			{
			public:
				NotHaveUnknownDataSource(
					const Importable& object
				);
			};

		protected:
			// Attributes
			DataSourceLinks _dataSourceLinks;

		public:
			/////////////////////////////////////////////////////////////////////
			/// DataSource Constructor.
			Importable();

			//! @name Getters
			//@{
				const DataSourceLinks& getDataSourceLinks() const { return _dataSourceLinks; }
			//@}

			//! @name Setters
			//@{
			//@}

			//! @name Modifiers
			//@{
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Checks if the object is linked with the specified data source.
				/// @param source the source with which the object must be linked
				/// @return true if the object is linked with the specified data source.
				bool hasLinkWithSource(const DataSource& source) const;

				bool hasUnknownOwner() const;

				//////////////////////////////////////////////////////////////////////////
				/// Returns the first code for the datasource.
				const std::string& getACodeBySource(const DataSource& source) const;


				bool hasCodeBySource(const DataSource& source, const std::string& code) const;


				std::vector<std::string> getCodesBySource(const DataSource& source) const;


				//////////////////////////////////////////////////////////////////////////
				/// Gets the code in the object owner database.
				/// @throws NotLinkedWithSourceException if the object does not have an
				/// anonymous owner.
				const std::string& getACodeBySource() const;

				std::string getCodeBySources() const;
			//@}

			//! @name Static algorithms
			//@{
			//@}
		};


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



			void setDataSourceLinks(
				const DataSourceLinks& value,
				bool storeLinkInDataSource = false
			){
				if(storeLinkInDataSource)
				{
					BOOST_FOREACH(const DataSourceLinks::value_type& link, _dataSourceLinks)
					{
						link.first->removeLink(static_cast<T&>(*this), link.second);
				}	}
				_dataSourceLinks = value;
				if(storeLinkInDataSource)
				{
					BOOST_FOREACH(const DataSourceLinks::value_type& link, _dataSourceLinks)
					{
						link.first->addLink(static_cast<T&>(*this), link.second);
				}	}
			}



			void addCodeBySource(
				const DataSource& source,
				const std::string& code,
				bool storeLinkInDataSource = false
			){
				// Check if the code is not already registered
				pair<DataSourceLinks::const_iterator, DataSourceLinks::const_iterator> range(
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
}	}

#endif // SYNTHESE_Importable_h__
