////////////////////////////////////////////////////////////////////////////////
///	DataSource class implementation.
///	@file DataSource.cpp
///	@author Hugues Romain (RCS)
///	@date sam fev 21 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
#include "GlobalRight.h"
#include "Importer.hpp"
#include "Importable.h"
#include "Profile.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	CLASS_DEFINITION(impex::DataSource, "t059_data_sources", 59)
	FIELD_DEFINITION_OF_OBJECT(impex::DataSource, "data_source_id", "data_source_ids")
	FIELD_DEFINITION_OF_TYPE(Icon, "icon", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Charset, "charset", SQL_TEXT)

	namespace impex
	{
		DataSource::DataSource(
			RegistryKeyType id
		):	Registrable(id),
			Object<DataSource, DataSourceRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(Icon),
					FIELD_DEFAULT_CONSTRUCTOR(Charset),
					FIELD_VALUE_CONSTRUCTOR(CoordinatesSystem, &CoordinatesSystem::GetInstanceCoordinatesSystem())
			)	)
		{}



		const CoordinatesSystem& DataSource::getActualCoordinateSystem() const
		{
			return get<CoordinatesSystem>() ? *get<CoordinatesSystem>() : CoordinatesSystem::GetInstanceCoordinatesSystem();
		}


		bool DataSource::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DataSource::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool DataSource::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
}	}
