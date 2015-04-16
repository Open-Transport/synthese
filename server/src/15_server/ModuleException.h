
#include "Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace server
	{


		/** Module related exception class
			@ingroup m70
		*/

		class ServerModuleException : public synthese::Exception
		{
		private:

		public:

			ServerModuleException ( const std::string& message);
			~ServerModuleException () throw ();

		private:


		};




	}

}

