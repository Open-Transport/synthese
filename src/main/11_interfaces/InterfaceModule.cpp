
#include "01_util/Thread.h"

#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLiteSync.h"

#include "11_interfaces/InterfaceTableSync.h"
#include "11_interfaces/InterfacePageTableSync.h"
#include "11_interfaces/InterfaceModule.h"

namespace synthese
{
	using namespace db;

	namespace interfaces
	{
		Interface::Registry	InterfaceModule::_interfaces;

		void InterfaceModule::initialize()
		{
		}

		Interface::Registry& 
			InterfaceModule::getInterfaces ()
		{
			return _interfaces;
		}
	}
}

