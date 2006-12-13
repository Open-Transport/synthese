
#include "30_server/ServerModule.h"

#include "39_carto/CartoModule.h"
#include "39_carto/MapBackgroundManager.h"


namespace synthese
{
	using namespace server;

	namespace carto
	{

		void CartoModule::initialize()
		{
			// Initialize map background manager
			MapBackgroundManager::SetBackgroundsDir (ServerModule::getConfig().getDataDir () / "backgrounds");
			MapBackgroundManager::Initialize ();
		}
	}
}

