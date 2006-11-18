
#include "30_server/ModuleClass.h"

namespace synthese
{
	namespace security
	{
		class SecurityModule : public server::ModuleClass
		{
		public:
			void initialize(const server::Server* server);
		};
	}
}