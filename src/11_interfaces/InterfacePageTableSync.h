
/** InterfacePageTableSync class header.
	@file InterfacePageTableSync.h

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

#ifndef SYNTHESE_InterfacePageTableSync_H__
#define SYNTHESE_InterfacePageTableSync_H__

#include <string>
#include <iostream>

#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace interfaces
	{
		class InterfacePage;

		/** InterfacePageTableSync table synchronizer.
			@ingroup m11LS refLS
		*/
		class InterfacePageTableSync:
			public db::DBDirectTableSyncTemplate<
				InterfacePageTableSync,
				InterfacePage,
				db::FullSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		public:
			static const std::string TABLE_COL_INTERFACE;
			static const std::string TABLE_COL_CLASS;
			static const std::string TABLE_COL_PAGE;
			static const std::string TABLE_COL_DIRECT_DISPLAY_ALLOWED;
			static const std::string COL_MIME_TYPE;
			static const std::string TABLE_COL_CONTENT;

			/** Interface page table sync constructor.
			*/
			InterfacePageTableSync() {}

			static boost::shared_ptr<InterfacePage> GetNewObject(const Record& row);

			static boost::shared_ptr<InterfacePage> GetNewObject();

			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> interfaceId = boost::optional<util::RegistryKeyType>(),
				boost::optional<int> first = boost::optional<int>(),
				boost::optional<int> number = boost::optional<int>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_InterfacePageTableSync_H__
