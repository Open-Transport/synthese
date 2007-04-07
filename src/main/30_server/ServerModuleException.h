
#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace server
	{


		/** Module related exception class
		@ingroup m30
		*/

		class ServerModuleException : public synthese::util::Exception
		{
		private:

		public:

			ServerModuleException ( const std::string& message);
			~ServerModuleException () throw ();

		private:


		};




	}

}

