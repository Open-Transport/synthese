
#ifndef SYNTHESE_AdminModule_H__
#define SYNTHESE_AdminModule_H__

#include "01_util/ModuleClass.h"

namespace synthese
{
	namespace admin
	{

		/** @defgroup m32 32 Generic administration console
		@{
		*/

		class AdminModule : public util::ModuleClass
		{
		private:

		public:
			static const std::string TABLE_COL_ID;

			void initialize();
		};
		/** @} */
	}
}

#endif // SYNTHESE_AdminModule_H__
