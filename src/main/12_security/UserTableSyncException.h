
#ifndef SYNTHESE_UserTableSyncException_H__
#define SYNTHESE_UserTableSyncException_H__


#include "01_util/Exception.h"

#include <string>


namespace synthese
{
	namespace security
	{
		class UserTableSyncException : public synthese::util::Exception
		{
		public:

			UserTableSyncException ( const std::string& message);
			~UserTableSyncException () throw ();

		};
	}
}
#endif // SYNTHESE_UserTableSyncException_H__

