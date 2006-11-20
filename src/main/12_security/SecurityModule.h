
#include "01_util/ModuleClass.h"

#include "12_security/Profile.h"

namespace synthese
{
	namespace security
	{
		class SecurityModule : public util::ModuleClass
		{
		private:
			static Profile::Registry _profiles;

		public:
			void initialize();

			static Profile::Registry& getProfiles();
		};
	}
}