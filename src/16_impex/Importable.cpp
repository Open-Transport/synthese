
/** Importable class implementation.
	@file Importable.cpp

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

#include "Importable.h"
#include "DataSource.h"

using namespace std;

namespace synthese
{
	namespace impex
	{
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



		const std::string& Importable::getCodeBySource( const DataSource& source ) const
		{
			DataSourceLinks::const_iterator it(_dataSourceLinks.find(&source));
			if(it == _dataSourceLinks.end())
			{
				throw NotLinkedWithSourceException(*this, source);
			}
			return it->second;
		}



		const std::string& Importable::getCodeBySource() const
		{

			DataSourceLinks::const_iterator it(_dataSourceLinks.find(NULL));
			if(it == _dataSourceLinks.end())
			{
				throw NotHaveUnknownDataSource(*this);
			}
			return it->second;
		}



		void Importable::setCodeBySource( const DataSource& source, const std::string& code )
		{
			_dataSourceLinks[&source] = code;
		}



		bool Importable::hasUnknownOwner() const
		{
			return _dataSourceLinks.find(NULL) != _dataSourceLinks.end();
		}



		void Importable::cleanDataSourceLinks()
		{
			_dataSourceLinks.clear();
		}
}	}
