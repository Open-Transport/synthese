#ifndef SYNTHESE_AccountingModule_H__
#define SYNTHESE_AccountingModule_H__


#include "01_util/ModuleClass.h"
#include "01_util/FactorableTemplate.h"

#include "57_accounting/Currency.h"

namespace synthese
{
	/** @defgroup m57 37 Accounting
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

		};
	}
	/** @} */
}


#endif 
