
#ifndef SYNTHESE_InterfacePageException_H__
#define SYNTHESE_InterfacePageException_H__



#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace interfaces
	{


		/** Request related exception class
		@ingroup m70
		*/

		class InterfacePageException : public synthese::util::Exception
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

