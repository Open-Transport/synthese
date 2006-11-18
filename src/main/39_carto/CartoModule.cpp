
#include "39_carto/CartoModule.h"
#include "39_carto/MapBackgroundManager.h"
#include "30_server/Server.h"



namespace synthese
{
	using namespace server;

	namespace carto
	{

		void CartoModule::initialize()
		{
			// Initialize map background manager
			MapBackgroundManager::SetBackgroundsDir (Server::GetInstance()->getConfig ().getDataDir () / "backgrounds");
			MapBackgroundManager::Initialize ();
		}
	}
}
