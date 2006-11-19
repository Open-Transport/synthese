
#include "01_util/ModuleClass.h"

namespace synthese
{
	namespace util
	{
		boost::filesystem::path ModuleClass::_databasePath;

		ModuleClass::ModuleClass()
		{

		}

		void ModuleClass::setDatabasePath( const boost::filesystem::path& databasePath )
		{
			_databasePath = databasePath;
		}
	}
}