
#include "Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace server
	{


		/** Session related exception class
		@ingroup m15
		*/

		class SessionException : public synthese::Exception
		{
		private:

		public:

			SessionException ( const std::string& message);
			~SessionException () throw ();

		private:


		};




	}

}

