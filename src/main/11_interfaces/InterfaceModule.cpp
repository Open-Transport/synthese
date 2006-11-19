
#include "01_util/Thread.h"

#include "02_db/SQLiteThreadExec.h"
#include "02_db/SQLiteSync.h"

#include "11_interfaces/InterfaceTableSync.h"
#include "11_interfaces/InterfacePageTableSync.h"
#include "11_interfaces/InterfaceModule.h"

namespace synthese
{
	using namespace db;

	namespace interfaces
	{
		void InterfaceModule::initialize()
		{
			// Register all synchronizers
			synthese::db::SQLiteThreadExec* sqliteExec = new synthese::db::SQLiteThreadExec (_databasePath);

			// Start the db sync thread right now
			synthese::util::Thread sqliteThread (sqliteExec, "sqlite");
			sqliteThread.start ();

			synthese::db::SQLiteSync* syncHook = new synthese::db::SQLiteSync (TABLE_COL_ID);

			InterfaceTableSync* interfaceSync = new InterfaceTableSync (_interfaces, db::TRIGGERS_ENABLED_CLAUSE);
			InterfacePageTableSync* interfacePageSync = new InterfacePageTableSync (_interfaces, db::TRIGGERS_ENABLED_CLAUSE);

			syncHook->addTableSynchronizer (interfaceSync);
			syncHook->addTableSynchronizer (interfacePageSync);

			sqliteExec->registerUpdateHook (syncHook);

			// Interfaces are populated. Server config is filled.
			sqliteThread.waitForReadyState ();
		}



		Interface::Registry& 
			InterfaceModule::getInterfaces ()
		{
			return _interfaces;
		}



		const Interface::Registry& 
			InterfaceModule::getInterfaces () const
		{
			return _interfaces;
		}



	}
}
