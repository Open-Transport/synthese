
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
		private:
			const std::string _key;
			const std::string _ip;
			synthese::security::User* _user;
			synthese::time::DateTime _lastUse;

			static const size_t KEY_LENGTH;
			static const int MAX_MINUTES_DURATION;
			static std::string generateKey();

		public:
			//!	\name Constructor and destructor
			//@{
				Session(const std::string& ip);
			
				/** Unregisters the session in the global session map.
				*/
				~Session();
			//@}

			//! \name Modifiers
			//@{
				/** Refresh the last use date after several controls.

					The controls are :
						- expiration of the session
						- ip must not have change

					If a control fails, then a SessionException is thrown
				*/
				void controlAndRefresh(const std::string& ip);
			//@}

			//! \name Setters
			//@{
				void setUser(security::User* user);
			//@}

			//! \name Getters
			//@{
				const std::string getKey() const;
			//@}
		};
	}
}