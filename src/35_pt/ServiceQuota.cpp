
//////////////////////////////////////////////////////////////////////////////////////////
/// ServiceQuota class implementation.
///	@file ServiceQuota.cpp
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

#include "ServiceQuota.hpp"

#include "PTConstants.h"
#include "PTModule.h"
#include "Env.h"

#include "Exception.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace pt;

	CLASS_DEFINITION(ServiceQuota, "t089_service_quotas", 89)
	FIELD_DEFINITION_OF_TYPE(Quotas, "quotas", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Service, "service_id", SQL_INTEGER)

	namespace pt
	{
		ServiceQuota::ServiceQuota(
			RegistryKeyType id
		):	Registrable(id),
			Object<ServiceQuota, ServiceQuotaSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Service),
					FIELD_DEFAULT_CONSTRUCTOR(Quotas)
			)	)
		{}
}	}
