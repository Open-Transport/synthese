
#include "CartoModule.h"
#include "MapBackgroundManager.h"
#include "70_server/Server.h"



namespace synthese
{
	using namespace carto;

	namespace server
	{
		static std::string test = Factory<ModuleClass>::integrate<CartoModule>("80_carto");


		const std::string CartoModule::_factory_key = Factory<ModuleClass>::integrate<CartoModule>("80_carto");

		void CartoModule::initialize( const Server* server )
		{
			// Initialize map background manager
			MapBackgroundManager::SetBackgroundsDir (server->getConfig ().getDataDir () / "backgrounds");
			MapBackgroundManager::Initialize ();
		}
	}
}