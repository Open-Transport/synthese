
/** Route planner test data includes.
	@file RoutePlannerTestData.inc.hpp

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

#include "City.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Address.h"
#include "Crossing.h"
#include "TransportNetwork.h"
#include "CommercialLine.h"
#include "AllowedUseRule.h"
#include "PTUseRule.h"
#include "OnlineReservationRule.h"
#include "ReservationContact.h"
#include "JourneyPattern.hpp"
#include "RollingStock.h"
#include "LineStop.h"
#include "ScheduledService.h"
#include "ContinuousService.h"
#include "RoadPlace.h"
#include "Road.h"
#include "RoadChunk.h"
#include "DataSource.h"
#include "Env.h"
#include "ResaModule.h"
#include "RuleUser.h"
#include "NonConcurrencyRule.h"
#include "GeographyModule.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
