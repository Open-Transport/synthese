
#include <stdlib.h>
#include <time.h>

#include "02_db/SQLiteSync.h"

#include "11_interfaces/InterfaceModule.h"

#include "15_env/EnvModule.h"

#include "30_server/ServerModule.h"
#include "30_server/SiteTableSync.h"
#include "30_server/Server.h"

namespace synthese
{
	using namespace interfaces;
	using namespace env;
	using namespace db;

	namespace server
	{


		void ServerModule::initialize()
		{

			synthese::db::SQLiteSync* syncHook = new SQLiteSync (TABLE_COL_ID);

			SiteTableSync* siteSync = 
				new SiteTableSync (_sites, TRIGGERS_ENABLED_CLAUSE, ((InterfaceModule*) Server::GetInstance()->GetModule<InterfaceModule>())->getInterfaces(), ((EnvModule*) Server::GetInstance()->GetModule<EnvModule>())->getEnvironments());

			syncHook->addTableSynchronizer (siteSync);

			srand( (unsigned) std::time( NULL ) );


		}

		Site::Registry& ServerModule::getSites()
		{
			return _sites;
		}

		const Site::Registry& ServerModule::getSites() const
		{
			return _sites;
		}
	}
}