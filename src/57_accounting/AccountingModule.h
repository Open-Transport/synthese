#ifndef SYNTHESE_AccountingModule_H__
#define SYNTHESE_AccountingModule_H__


#include "01_util/ModuleClass.h"
#include "01_util/FactorableTemplate.h"

#include "57_accounting/Currency.h"

#include <vector>
#include <utility>

namespace synthese
{
	/** @defgroup m37 37 Accounting
		@ingroup m3

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
