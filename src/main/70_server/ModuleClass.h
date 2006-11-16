
#include "01_util/Factory.h"

namespace synthese
{
	using synthese::util::Factory;

	namespace server
	{
		class Server;

		/** Module recorder.

			Each module must implement a ModuleClass subclass and register it by the Generic Factory.
			The choosen key indicates the order of loading and can be important in several cases.
		*/
		class ModuleClass
		{
		public:
			virtual void initialize(const Server* server) = 0;
		};
	}
}