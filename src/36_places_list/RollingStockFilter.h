
/** RollingStockFilter class header.
	@file RollingStockFilter.h

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

#ifndef SYNTHESE_transporwebsite_RollingStockFilter_h__
#define SYNTHESE_transporwebsite_RollingStockFilter_h__

#include "Registrable.h"
#include "Registry.h"
#include "AccessParameters.h"

#include <string>
#include <set>
#include <boost/foreach.hpp>

namespace synthese
{
	namespace env
	{
		class RollingStock;
	}

	namespace transportwebsite
	{
		class Site;

		/** RollingStockFilter class.
			@ingroup m36
		*/
		class RollingStockFilter:
			public util::Registrable
		{
		public:
			typedef std::set<const env::RollingStock*> List;

			/// Chosen registry class.
			typedef util::Registry<RollingStockFilter>	Registry;

		private:

			std::string _name;
			const Site* _site;
			List _list;
			bool _authorized_only;

		public:
			RollingStockFilter(util::RegistryKeyType key = UNKNOWN_VALUE)
				: util::Registrable(key) {}

			~RollingStockFilter() {}

			//! @name Getters
			//@{
				const std::string& getName() const { return _name; }
				bool getAuthorizedOnly() const { return _authorized_only; }
				const List& getList() const { return _list; }
				const Site* getSite() const { return _site; }
			//@}

			//! @name Setters
			//@{
				void setName(const std::string& value) { _name=value; }
				void setAuthorizedOnly(bool value) { _authorized_only = value; }
				void setSite(const Site* value) { _site = value; }
			//@}

			//! @name Modifiers
			//@{
				void addRollingStock(const env::RollingStock* value) { _list.insert(value); }
			//@}

			//! @name Queries
			//@{
				graph::AccessParameters::AllowedPathClasses getAllowedPathClasses() const;
			//@}
		};
	}
}

#endif // SYNTHESE_transporwebsite_RollingStockFilter_h__
