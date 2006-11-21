
#ifndef SYNTHESE_UserException_H__
#define SYNTHESE_UserException_H__


#include "01_util/Exception.h"

#include <string>


namespace synthese
{
	namespace security
	{
		class UserException : public synthese::util::Exception
		{
		public:

			UserException ( const std::string& message);
			~UserException () throw ();

		};
	}
}
#endif
