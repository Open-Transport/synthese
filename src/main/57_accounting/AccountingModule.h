#ifndef SYNTHESE_AccountingModule_H__
#define SYNTHESE_AccountingModule_H__


#include "01_util/ModuleClass.h"

#include "57_accounting/Currency.h"

namespace synthese
{
	namespace accounts
	{

		/** @defgroup m57 57 Accounts

		@{
		*/

		class AccountingModule : public util::ModuleClass
		{
		private:
			static Currency::Registry _currencies;

		public:
			void initialize();

			static Currency::Registry& getCurrencies();
		};
		/** @} */
	}
}


#endif 
