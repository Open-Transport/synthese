
//////////////////////////////////////////////////////////////////////////////////////////
/// ServiceQuota class header.
///	@file ServiceQuota.hpp
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

#ifndef SYNTHESE_ServiceQuota_H
#define SYNTHESE_ServiceQuota_H

#include "Object.hpp"
#include "NumericField.hpp"
#include "PointerField.hpp"
#include "ScheduledService.h"
#include "SimpleMapField.hpp"

#include "UtilConstants.h"

#include <boost/date_time/gregorian/greg_duration.hpp>
#include <boost/fusion/include/map.hpp>

namespace synthese
{
	namespace pt
	{
		typedef std::map<boost::gregorian::date, int> QuotasMap;
		FIELD_MAP(Quotas, QuotasMap)
		FIELD_POINTER(Service, ScheduledService)
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Service),
			FIELD(Quotas)
		> ServiceQuotaSchema;


		//////////////////////////////////////////////////////////////////////////
		/// ServiceQuota.
		///	@ingroup m35
		///	@author Gael Sauvanet
		///	@date 2012
		//////////////////////////////////////////////////////////////////////////
		class ServiceQuota:
			public Object<ServiceQuota, ServiceQuotaSchema>
		{
		public:
			typedef util::Registry<ServiceQuota> Registry;

		private:

		public:

			//! \name Constructor
			//@{
				ServiceQuota(util::RegistryKeyType id = 0);
			//@}

		};
}	}

#endif
