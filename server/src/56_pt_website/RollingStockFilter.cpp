
/** RollingStockFilter class implementation.
	@file RollingStockFilter.cpp

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

#include "RollingStockFilter.h"

#include "Env.h"
#include "Profile.h"
#include "PTServiceConfig.hpp"
#include "RollingStock.hpp"
#include "RollingStockTableSync.hpp"
#include "User.h"
 
#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace pt_website;
	using namespace util;
	using namespace vehicle;

	CLASS_DEFINITION(RollingStockFilter, "t062_rolling_stock_filters", 62)
	FIELD_DEFINITION_OF_OBJECT(RollingStockFilter, "rolling_stock_filter_id", "rolling_stock_filter_ids")

	FIELD_DEFINITION_OF_TYPE(ServiceConfig, "site_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(AuthorizedOnly, "authorized_only", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(RollingStockIds, "rolling_stock_ids", SQL_TEXT)

	namespace pt_website
	{
		RollingStockFilter::RollingStockFilter(
			util::RegistryKeyType key
		) :
			util::Registrable(key),
			Object<RollingStockFilter, RollingStockFilterSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(ServiceConfig),
					FIELD_DEFAULT_CONSTRUCTOR(Rank),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(AuthorizedOnly),
					FIELD_DEFAULT_CONSTRUCTOR(RollingStockIds)
			)	)
		{}

		RollingStockFilter::~RollingStockFilter()
		{
			if (get<ServiceConfig>())
			{
				get<ServiceConfig>()->removeRollingStockFilter(*this);
			}
		}

		graph::AccessParameters::AllowedPathClasses RollingStockFilter::getAllowedPathClasses() const
		{
			graph::AccessParameters::AllowedPathClasses result;
			result.insert(0);
			if(getAuthorizedOnly())
			{
				BOOST_FOREACH(const List::value_type& element, _list)
				{
					result.insert(element->getIdentifier());
				}
			}
			else
			{
				BOOST_FOREACH(RollingStock::Registry::value_type element, Env::GetOfficialEnv().getEditableRegistry<RollingStock>())
				{
					if(_list.find(element.second.get()) != _list.end())
					{
						continue;
					}
					result.insert(element.second->getIdentifier());
				}
			}
			return result;
		}

		const PTServiceConfig* RollingStockFilter::getSite() const
		{
			if (get<ServiceConfig>())
			{
				return get<ServiceConfig>().get_ptr();
			}

			return NULL;
		}

		void RollingStockFilter::setSite(PTServiceConfig* value)
		{
			if (get<ServiceConfig>())
			{
				get<ServiceConfig>()->removeRollingStockFilter(*this);
			}
			set<ServiceConfig>(value
				? boost::optional<PTServiceConfig&>(*value)
				: boost::none);
			if (get<ServiceConfig>())
			{
				get<ServiceConfig>()->addRollingStockFilter(*this);
			}
		}

		void RollingStockFilter::addRollingStock(const vehicle::RollingStock* value)
		{
			_list.insert(value);
			bool first(true);
			string rollingStocks;
			BOOST_FOREACH(const RollingStock* item, getList())
			{
				if(!first) rollingStocks += ",";
				rollingStocks += item->getKey();
				first = false;
			}
			set<RollingStockIds>(rollingStocks);
		}

		void RollingStockFilter::cleanRollingStocks()
		{
			_list.clear();
			set<RollingStockIds>("");
		}

		void RollingStockFilter::link(util::Env& env, bool withAlgorithmOptimizations)
		{
			if (get<ServiceConfig>())
			{
				get<ServiceConfig>()->addRollingStockFilter(*this);
			}

			vector< string > parsed_ids;
			const string rollingStocksString(get<RollingStockIds>());
			split(parsed_ids, rollingStocksString, is_any_of(","));
			_list.clear();
			BOOST_FOREACH(const string& id, parsed_ids)
			{
				if(id.empty())
				{
					continue;
				}
				try
				{
					addRollingStock(
						RollingStockTableSync::Get(
							lexical_cast<RegistryKeyType>(id),
							env
						).get()
					);
				}
				catch(ObjectNotFoundException<RollingStock> e)
				{
					Log::GetInstance().warn("Rolling stock not found in rolling stock filter "+ lexical_cast<string>(getKey()));
				}
			}
		}

		bool RollingStockFilter::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool RollingStockFilter::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool RollingStockFilter::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}
	}
}
