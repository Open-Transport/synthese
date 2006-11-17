
#include "39_carto/CartoModule.h"
#include "39_carto/MapBackgroundManager.h"
#include "70_server/Server.h"



namespace synthese
{
	using namespace carto;

	namespace server
	{

		const std::string CartoModule::_factory_key = Factory<ModuleClass>::integrate<CartoModule>("80_carto");

		void CartoModule::initialize( const Server* server )
		{
			// Initialize map background manager
			MapBackgroundManager::SetBackgroundsDir (server->getConfig ().getDataDir () / "backgrounds");
			MapBackgroundManager::Initialize ();
		}
	}
}
