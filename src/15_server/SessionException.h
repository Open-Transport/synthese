
#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace server
	{


		/** Session related exception class
		@ingroup m15
		*/

		class SessionException : public synthese::util::Exception
		{
		private:

		public:

			SessionException ( const std::string& message);
			~SessionException () throw ();

		private:


		};




	}

}

