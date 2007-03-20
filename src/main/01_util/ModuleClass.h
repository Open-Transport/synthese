
#ifndef SYNTHESE_ModuleClass_H__
#define SYNTHESE_ModuleClass_H__

#include <boost/filesystem/path.hpp>

#include "01_util/Factorable.h"

namespace synthese
{
	namespace util
	{
		/** Module recorder.

			Each module must implement a ModuleClass subclass and register it by the Generic Factory.
			Chosen key indicates the order of loading and can be important in several cases.
		*/
		class ModuleClass : public util::Factorable
		{
		public:
			ModuleClass();

			virtual void preInit () {};
			virtual void initialize () {};

		};
	}
}
#endif // SYNTHESE_ModuleClass_H__

