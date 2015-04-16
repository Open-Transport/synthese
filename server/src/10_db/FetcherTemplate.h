
/** FetcherTemplate class header.
	@file FetcherTemplate.h

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

#include "Fetcher.h"
#include "FactorableTemplate.h"

#include "DBModule.h"

#include <boost/optional.hpp>

#ifndef SYNTHESE_db_FetcherTemplate_h__
#define SYNTHESE_db_FetcherTemplate_h__

namespace synthese
{
	namespace db
	{
		/** FetcherTemplate class.
			@ingroup m02
		*/
		template<class BaseClass, class TableSync>
		class FetcherTemplate:
			public util::FactorableTemplate<Fetcher<BaseClass>, TableSync>
		{
		protected:
			virtual boost::shared_ptr<BaseClass> _getEditable(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const {
				return boost::static_pointer_cast<BaseClass, typename TableSync::ObjectType>(
					TableSync::GetEditable(key, env, linkLevel)
				);
			}

			virtual boost::shared_ptr<const BaseClass> _get(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const {
				return boost::static_pointer_cast<const BaseClass, const typename TableSync::ObjectType>(
					TableSync::Get(key, env, linkLevel)
				);
			}

			virtual void _save(
				BaseClass& object,
				boost::optional<DBTransaction&> transaction
			) const {
				TableSync::Save(
					&static_cast<typename TableSync::ObjectType&>(object),
					transaction
				);
			}

			virtual void _remove(
				const BaseClass& object,
				boost::optional<DBTransaction&> transaction
			) const {
				DBModule::GetDB()->deleteStmt(
					static_cast<const typename TableSync::ObjectType&>(object).getKey(),
					transaction
				);
			}
		};
}	}

#endif // SYNTHESE_db_FetcherTemplate_h__
