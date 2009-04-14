
/** Fetcher class header.
	@file Fetcher.h

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

#ifndef SYNTHESE_db_Fetcher_h__
#define SYNTHESE_db_Fetcher_h__

#include "Factory.h"

namespace synthese
{
	namespace db
	{
		/** Fetcher template class.
			@ingroup m02
		*/
		template<class BaseClass>
		class Fetcher:
			public util::FactoryBase<Fetcher<BaseClass> >
		{
		protected:
			virtual boost::shared_ptr<BaseClass> _getEditable(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			) const = 0;

			virtual boost::shared_ptr<const BaseClass> _get(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			) const = 0;

		public:
			Fetcher() {}

			static boost::shared_ptr<BaseClass> FetchEditable(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				string ts(Conversion::ToString(decodeTableId(key)));
				auto_ptr<Fetcher<BaseClass> > pf(Factory<Fetcher<BaseClass> >::create(ts));
				return pf->_getEditable(key, env, linkLevel, autoCreate);
			}

			static boost::shared_ptr<const BaseClass> Fetch(
				util::RegistryKeyType key,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				AutoCreation autoCreate = NEVER_CREATE
			){
				string ts(Conversion::ToString(decodeTableId(key)));
				auto_ptr<Fetcher<BaseClass> > pf(Factory<Fetcher<BaseClass> >::create(ts));
				return pf->_get(key, env, linkLevel, autoCreate);
			}

		};
	}
}

#endif // SYNTHESE_db_Fetcher_h__
