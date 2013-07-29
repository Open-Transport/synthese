
/** Route planner test data includes.
	@file RoutePlannerTestData.inc.hpp

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
#include "RollingStock.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "ScheduledService.h"
#include "ContinuousService.h"
#include "RoadPlace.h"
#include "DataSource.h"
#include "Env.h"
#include "ResaModule.h"
#include "RuleUser.h"
#include "NonConcurrencyRule.h"
#include "GeographyModule.h"
#include "CoordinatesSystem.hpp"
#include "MainRoadChunk.hpp"
#include "MainRoadPart.hpp"
#include "10_db/101_sqlite/SQLiteDB.h"
#include "DBModule.h"

#include "TestUtils.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/shared_ptr.hpp>

// Test cases using the CoordinatesSystem class should instanciate this class in the scope of the test.
struct ScopedCoordinatesSystemUser
{
	// FIXME: We need to initialize the database before using CoordinatesSystem.
	// We should break that dependency which kind of sucks.
	ScopedFactory<synthese::db::SQLiteDB> _scopedSqliteDb;
	boost::shared_ptr<ScopedModule<synthese::db::DBModule> > _scopedDBModule;

	ScopedCoordinatesSystemUser()
	{
		synthese::db::DBModule::SetConnectionString("sqlite://debug=1");
		_scopedDBModule.reset(new ScopedModule<synthese::db::DBModule>());
	}
};
