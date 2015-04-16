
#include "Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace server
	{


		/** Client related exception class
		@ingroup m15
		*/

		class ClientException : public synthese::Exception
		{
		private:

		public:

			ClientException ( const std::string& message);
			~ClientException () throw ();

		private:


		};




	}

}

