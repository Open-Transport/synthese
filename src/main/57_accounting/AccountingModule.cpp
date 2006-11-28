
#include "57_accounting/AccountingModule.h"

namespace synthese
{
	namespace accounts
	{
		Currency::Registry AccountingModule::_currencies;

		void AccountingModule::initialize()
		{

		}

		Currency::Registry& AccountingModule::getCurrencies()
		{
			return _currencies;
		}
	}
}