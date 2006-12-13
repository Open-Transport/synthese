
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

//		static const std::string TRIGGERS_ENABLED_CLAUSE;
		static const std::string TABLE_COL_ID("id");

		class DBModule : public util::ModuleClass
		{
		public:

			void initialize();
		};
		/** @} */

	// TEMPORARY
		/// @todo Handle better TRIGGER and co...
		static const std::string CONFIG_TABLE_NAME ("t999_config");
		static const std::string CONFIG_TABLE_COL_PARAMNAME ("param_name");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE ("param_value");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_PORT ("port");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_NBTHREADS ("nb_threads");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_LOGLEVEL ("log_level");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_DATADIR ("data_dir");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_TEMPDIR ("temp_dir");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPDIR ("http_temp_dir");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_HTTPTEMPURL ("http_temp_url");
		static const std::string CONFIG_TABLE_COL_PARAMVALUE_TRIGGERSENABLED ("triggers_enabled");
		static const std::string TRIGGERS_ENABLED_CLAUSE (
			"(SELECT " + CONFIG_TABLE_COL_PARAMVALUE
			+ " FROM " + CONFIG_TABLE_NAME + " WHERE " + CONFIG_TABLE_COL_PARAMNAME
			+ "=" + CONFIG_TABLE_COL_PARAMVALUE_TRIGGERSENABLED + ")");

	}
}
#endif // SYNTHESE_DBModule_H__

