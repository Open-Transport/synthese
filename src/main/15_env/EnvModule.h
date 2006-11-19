
#include "01_util/ModuleClass.h"

#include "15_env/Environment.h"

namespace synthese
{
	namespace env
	{
		class EnvModule : public util::ModuleClass
		{
			Environment::Registry		_environments;

		public:
			
			void initialize();
			
			Environment::Registry& getEnvironments ();
			(const Environment::Registry)& getEnvironments () const;

		};
	}
}