
#include <string>
#include "04_time/DateTime.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace server
	{
		class Session
		{
			const std::string _key;
			const std::string _ip;
			synthese::security::User* _user;
			synthese::time::DateTime _lastUse;

		public:
			Session(const std::string& ip);
			bool dateControl() const;
		};
	}
}