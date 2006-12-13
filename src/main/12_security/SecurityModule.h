
#include "01_util/ModuleClass.h"

#include "12_security/Profile.h"

namespace synthese
{
	namespace security
	{
		class SecurityModule : public util::ModuleClass
		{
		public:
			static const std::string ROOT_PROFILE;
			static const std::string ROOT_RIGHTS;
			static const std::string ROOT_USER;

		private:
			static Profile::Registry _profiles;

		public:
			void initialize();

			static Profile::Registry& getProfiles();
		};
	}
}
