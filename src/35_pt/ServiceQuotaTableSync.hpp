
//////////////////////////////////////////////////////////////////////////////////////////
/// ServiceQuotaTableSync class header.
///	@file ServiceQuotaTableSync.hpp
///	@author Gael Sauvanet
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_ServiceQuotaTableSync_H__
#define SYNTHESE_ServiceQuotaTableSync_H__

#include "DBDirectTableSyncTemplate.hpp"
#include "ServiceQuota.hpp"

#include <string>
#include <iostream>

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.10 Table : ServiceQuota.
		///	@ingroup m35
		/// @author Gael Sauvanet
		//////////////////////////////////////////////////////////////////////////
		/// Table name : t089_service_quotas
		class ServiceQuotaTableSync:
			public db::DBDirectTableSyncTemplate<ServiceQuotaTableSync, ServiceQuota>
		{
		public:
			/** ServiceQuota search.
			@author Gael Sauvanet
			@date 2012
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> serviceId = boost::optional<util::RegistryKeyType>(),
				size_t first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);
		};
	}
}

#endif // SYNTHESE_SiteTableSync_H__
