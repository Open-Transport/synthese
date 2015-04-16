#ifndef SYNTHESE_AccountingModule_H__
#define SYNTHESE_AccountingModule_H__


#include "01_util/ModuleClass.h"
#include "01_util/FactorableTemplate.h"

#include "57_accounting/Currency.h"

#include <vector>
#include <utility>

namespace synthese
{
	/**	@defgroup m57Exceptions 57.01 Exceptions
		@ingroup m57

		@defgroup m57LS 57.10 Table synchronizers
		@ingroup m57
		@image html uml_pt_operation_tables.png

		@defgroup m57Rights 57.12 Rights
		@ingroup m57

		@defgroup m57Logs 57.13 DB Logs
		@ingroup m57

		@defgroup m57Admin 57.14 Administration pages
		@ingroup m57

		@defgroup m57Actions 57.15 Actions
		@ingroup m57

		@defgroup m57Functions 57.15 Services
		@ingroup m57

		@defgroup m57File 57.16 File formats
		@ingroup m57

		@defgroup m57Alarm 57.17 Messages recipient
		@ingroup m57

		@defgroup m57 57 Accounting
		@ingroup m5

	@{
	*/

	/** 57 Accounting module namespace. */
	namespace accounts
	{

		/** 57 Accounting module class. */
		class AccountingModule : public util::FactorableTemplate<util::ModuleClass, AccountingModule>
		{
		public:
			void initialize();

			static std::vector<std::pair<uid, std::string> > getAccountsName(
				uid rightUserId
				, const std::string& className
				, bool emptyLine
			);

			virtual std::string getName() const;

		};
	}
	/** @} */
}


#endif
