
#include "01_util/ModuleClass.h"

#include "30_server/Site.h"

namespace synthese
{
	namespace server
	{
		class ServerModule : public util::ModuleClass
		{
			Site::Registry		_sites;

		public:
			void initialize();

			Site::Registry& getSites();
			const Site::Registry& getSites() const;

		};
	}
}