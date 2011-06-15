////////////////////////////////////////////////////////////////////////////////
///	DataSource class implementation.
///	@file DataSource.cpp
///	@author Hugues Romain (RCS)
///	@date sam fev 21 2009
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

#include "DataSource.h"
#include "Exception.h"
#include "FileFormat.h"
#include "Factory.h"
#include "Importer.hpp"
#include "Importable.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<impex::DataSource>::KEY("DataSource");
	}

	namespace impex
	{
		DataSource::DataSource(
			RegistryKeyType id
		):	Registrable(id),
			_coordinatesSystem(NULL)
		{}



		boost::shared_ptr<Importer> DataSource::getImporter(
			util::Env& env
		) const {
			shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(_format));
			return fileFormat->getImporter(env, *this);
		}



		bool DataSource::canImport() const
		{
			if(!Factory<FileFormat>::contains(_format))
			{
				return false;
			}

			shared_ptr<FileFormat> fileFormat(Factory<FileFormat>::create(_format));
			return fileFormat->canImport();
		}



		void DataSource::addLink( Importable& object ) const
		{
			_links.insert(
				make_pair(object.getCodeBySource(*this), &object)
			);
		}



		void DataSource::removeLink( Importable& object ) const
		{
			Links::iterator it(_links.find(object.getCodeBySource(*this)));
			if(it != _links.end())
			_links.erase(it);
		}



		Importable* DataSource::getObjectByCode( const std::string& code ) const
		{
			Links::const_iterator it(_links.find(code));
			if(it == _links.end())
			{
				return NULL;
			}
			else
			{
				return it->second;
			}
		}



		const CoordinatesSystem& DataSource::getActualCoordinateSystem() const
		{
			return _coordinatesSystem ? *_coordinatesSystem : CoordinatesSystem::GetInstanceCoordinatesSystem();
		}
}	}
