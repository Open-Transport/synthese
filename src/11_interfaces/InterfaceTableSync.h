
/** InterfaceTableSync class header.
	@file InterfaceTableSync.h

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

#ifndef SYNTHESE_InterfaceTableSync_H__
#define SYNTHESE_InterfaceTableSync_H__

#include "Interface.h"

#include <string>
#include <iostream>

#include "SQLiteRegistryTableSyncTemplate.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace interfaces
	{
		/** InterfaceTableSync SQLite table synchronizer.
			@ingroup m11LS refLS
		*/
		class InterfaceTableSync : public db::SQLiteRegistryTableSyncTemplate<InterfaceTableSync,Interface>
		{
		public:
			static const std::string TABLE_COL_NO_SESSION_DEFAULT_PAGE;
			static const std::string TABLE_COL_NAME;
			static const std::string COL_DEFAULT_CLIENT_URL;

			/** Interface page SQLite table constructor.
			*/
			InterfaceTableSync();

			typedef std::vector<std::pair<util::RegistryKeyType, std::string> > OrderedInterfaceLabels;


			/** Gets the label of the interfaces ordered alphabetically.
				@param pageFilter filter on interfaces which contains the specified page
			*/
			static OrderedInterfaceLabels _GetInterfaceLabels(
				boost::optional<std::string> textWithUnknown = boost::optional<std::string>("(inconnue)"),
				boost::optional<std::string> textWithNo = boost::optional<std::string>("(aucune)"),
				boost::optional<std::string> pageFilter = boost::optional<std::string>()
			);


			template<class T>
			static OrderedInterfaceLabels GetInterfaceLabels(
				boost::optional<std::string> textWithUnknown = boost::optional<std::string>("(inconnue)"),
				boost::optional<std::string> textWithNo = boost::optional<std::string>("(aucune)")
			){
				return _GetInterfaceLabels(textWithNo, textWithUnknown, T::FACTORY_KEY);
			}


			static void Search(
				util::Env& env,
				boost::optional<std::string> interfacePageKey = boost::optional<std::string>(),
				bool orderByName = true,
				bool raisingOrder = true,
				boost::optional<int> first = boost::optional<int>(),
				boost::optional<int> number = boost::optional<int>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

		};

	}
}

#endif // SYNTHESE_InterfaceTableSync_H__
