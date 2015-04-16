
/** BaseImportableUpdateAction class implementation.
	@file BaseImportableUpdateAction.cpp

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

#include "BaseImportableUpdateAction.hpp"

#include "Action.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace impex
	{
		const string BaseImportableUpdateAction::PARAMETER_DATA_SOURCE_LINKS = Action_PARAMETER_PREFIX + "_field_data_source_links";



		void BaseImportableUpdateAction::_getImportableUpdateParametersMap(
			ParametersMap& map
		) const	{
			// Data source links
			if(_dataSourceLinks)
			{
				map.insert(
					PARAMETER_DATA_SOURCE_LINKS,
					DataSourceLinks::Serialize(*_dataSourceLinks)
				);
			}
		}



		void BaseImportableUpdateAction::_setImportableUpdateFromParametersMap(
			util::Env& env,
			const util::ParametersMap& map
		){
			// Data source links
			if(map.isDefined(PARAMETER_DATA_SOURCE_LINKS))
			{
				_dataSourceLinks = ImportableTableSync::GetDataSourceLinksFromSerializedString(
					map.get<string>(PARAMETER_DATA_SOURCE_LINKS),
					env
				);
			}
		}



		void BaseImportableUpdateAction::_doImportableUpdate(
			Importable& object,
			server::Request& request
		) const	{
			// Data source links
			if(_dataSourceLinks)
			{
				object.setDataSourceLinksWithoutRegistration(*_dataSourceLinks);
			}
		}
}	}
