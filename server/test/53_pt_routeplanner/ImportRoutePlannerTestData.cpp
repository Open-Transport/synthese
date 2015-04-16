/** Route planner test data importer.
	@file ImportRoutePlannerTestData.cpp
	@author Sylvain Pasche
	@date 2011

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

// Hack to avoid having a main() generated, so that we can use our own.
#undef BOOST_AUTO_TEST_MAIN

#include "00_framework/Exception.h"
#include "FactorableTemplate.h"
#include "RoutePlannerTestData.inc.hpp"

// All these TableSyncs are used by the SAVE() macro defined below.
#include "CityTableSync.h"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "CrossingTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"
#include "PTUseRuleTableSync.h"
#include "OnlineReservationRuleTableSync.h"
#include "ReservationContactTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "DesignatedLinePhysicalStopInheritedTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "RoadPlaceTableSync.h"
#include "DataSourceTableSync.h"
#include "RoadChunkTableSync.h"
#include "RoadTableSync.h"
#include "LineStopTableSync.h"

#include "DBTransaction.hpp"
#include "102_mysql/MySQLDB.hpp"

using synthese::Exception;
using synthese::util::Log;
using namespace synthese::db;
using namespace synthese::pt;

static void import()
{
	using namespace synthese::util;
	using namespace synthese::geography;
	using namespace synthese::road;

	ScopedRegistrable<DesignatedLinePhysicalStop> scopedDesignatedLinePhysicalStopRegistrable;
	ScopedFactory<DesignatedLinePhysicalStopInheritedTableSync> scopedDesignatedLinePhysicalStopInheritedTableSyncFactory;

	ScopedFactory<MySQLDB> scopedMysqlDb;
	ScopedFactory<SQLiteDB> scopedSQLiteDb;

	if (!::getenv("SYNTHESE_TESTDATA_CONNSTRING"))
	{
		throw Exception("Missing SYNTHESE_TESTDATA_CONNSTRING environment variable");
	}
	DBModule::SetConnectionString(::getenv("SYNTHESE_TESTDATA_CONNSTRING"));

	ScopedModule<synthese::db::DBModule> scopedDBModule;

#define SAVE(_class, object) _class ## TableSync::Save(&object)
	#include "RoutePlannerTestData.hpp"
#undef SAVE
}

int main()
{
	try
	{
		import();
	}
	catch(synthese::Exception& e)
	{
		Log::GetInstance().error("Caught Synthese exception.", e);
		return 1;
	}

	return 0;
}
