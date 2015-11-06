
/** RollingStockFilter class header.
	@file RollingStockFilter.h

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

#ifndef SYNTHESE_transporwebsite_RollingStockFilter_h__
#define SYNTHESE_transporwebsite_RollingStockFilter_h__

#include "Object.hpp"

#include "AccessParameters.h"
#include "StringField.hpp"

#include <string>
#include <set>
#include <boost/foreach.hpp>

namespace synthese
{
	namespace vehicle
	{
		class RollingStock;
	}

	namespace pt_website
	{
		class PTServiceConfig;

		FIELD_POINTER(ServiceConfig, PTServiceConfig)
		FIELD_BOOL(AuthorizedOnly)
		FIELD_STRING(RollingStockIds)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(ServiceConfig),
			FIELD(Rank),
			FIELD(Name),
			FIELD(AuthorizedOnly),
			FIELD(RollingStockIds)
		> RollingStockFilterSchema;

		/** RollingStockFilter class.
			@ingroup m56
		*/
		class RollingStockFilter:
			public Object<RollingStockFilter, RollingStockFilterSchema>
		{
		public:
			typedef std::set<const vehicle::RollingStock*> List;

			/// Chosen registry class.
			typedef util::Registry<RollingStockFilter>	Registry;

		private:
			List _list;

		public:
			RollingStockFilter(util::RegistryKeyType key = 0);
			~RollingStockFilter();

			//! @name Getters
			//@{
				virtual std::string getName() const { return get<Name>(); }
				bool getAuthorizedOnly() const { return get<AuthorizedOnly>(); }
				const List& getList() const { return _list; }
				const PTServiceConfig* getSite() const;
				std::size_t getRank() const { return get<Rank>(); }
			//@}

			//! @name Setters
			//@{
				void setName(const std::string& value) { set<Name>(value); }
				void setAuthorizedOnly(bool value) { set<AuthorizedOnly>(value); }
				void setSite(PTServiceConfig* value);
				void setRank(std::size_t value) { set<Rank>(value); }
			//@}

			//! @name Modifiers
			//@{
				void addRollingStock(const vehicle::RollingStock* value);
				void cleanRollingStocks();
			//@}

			//! @name Queries
			//@{
				graph::AccessParameters::AllowedPathClasses getAllowedPathClasses() const;
			//@}

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_transporwebsite_RollingStockFilter_h__
