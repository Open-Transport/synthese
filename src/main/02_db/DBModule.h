
#ifndef SYNTHESE_DBModule_H__
#define SYNTHESE_DBModule_H__

#include "01_util/ModuleClass.h"

namespace synthese
{
	namespace db
	{

		/** @defgroup m02 02 SQLite database access

		@{
		*/

		static const std::string TRIGGERS_ENABLED_CLAUSE;
		static const std::string TABLE_COL_ID;

		class DBModule : public util::ModuleClass
		{

		public:

			void initialize();
		};
		/** @} */
	}
}
#endif // SYNTHESE_DBModule_H__
