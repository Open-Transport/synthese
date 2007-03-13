
#include "30_server/ServerModule.h"

#include "39_map/MapModule.h"
#include "39_map/MapBackgroundManager.h"


namespace synthese
{
	using namespace server;

	namespace map
	{

		void MapModule::initialize()
		{
			// Initialize map background manager
			MapBackgroundManager::SetBackgroundsDir (ServerModule::getConfig().getDataDir () / "backgrounds");
			MapBackgroundManager::Initialize ();
		}
	}
}

