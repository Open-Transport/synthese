
#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace server
	{


		/** Client related exception class
		@ingroup m18
		*/

		class ClientException : public synthese::util::Exception
		{
		private:

		public:

			ClientException ( const std::string& message);
			~ClientException () throw ();

		private:


		};




	}

}

