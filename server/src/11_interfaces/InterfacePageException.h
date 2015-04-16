
#ifndef SYNTHESE_InterfacePageException_H__
#define SYNTHESE_InterfacePageException_H__



#include "Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace interfaces
	{


		/** Interface page parameters count related exception class
			@ingroup m11Exceptions
		*/
		class InterfacePageException : public synthese::Exception
		{
		private:

		public:

			InterfacePageException ( const std::string& message);
			~InterfacePageException () throw ();

		private:


		};




	}

}
#endif // SYNTHESE_InterfacePageException_H__

