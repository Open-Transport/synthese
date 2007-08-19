#ifndef SYNTHESE_AccountingModule_H__
#define SYNTHESE_AccountingModule_H__


#include "01_util/ModuleClass.h"

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
		class AccountingModule : public util::ModuleClass
		{
		private:
			static Currency::Registry _currencies;

		public:
			void initialize();

			static Currency::Registry& getCurrencies();
		};
	}
	/** @} */
}


#endif 
