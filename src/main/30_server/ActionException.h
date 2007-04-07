
#ifndef SYNTHESE_ActionException_H__
#define SYNTHESE_ActionException_H__


#include "01_util/Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
	namespace server
	{


		/** Action related exception class
		@ingroup m30/exception
		*/

		class ActionException : public synthese::util::Exception
		{
		private:

		public:

			ActionException ( const std::string& message);
			~ActionException () throw ();

		private:


		};




	}

}
#endif // SYNTHESE_ActionException_H__

