
#include "01_util/ModuleClass.h"

namespace synthese
{
	namespace util
	{


		ModuleClass::ModuleClass()
			: _databasePath("")
		{

		}

		void ModuleClass::setDatabasePath( const boost::filesystem::path& databasePath )
		{
			_databasePath = databasePath;
		}
	}
}