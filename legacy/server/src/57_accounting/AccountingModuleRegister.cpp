
#include "57_accounting/AccountingModule.h"
#include "57_accounting/AccountTableSync.h"
#include "57_accounting/CurrencyTableSync.h"
#include "57_accounting/TransactionPartTableSync.h"
#include "57_accounting/TransactionTableSync.h"

#include "57_accounting/AccountAddAction.h"
#include "57_accounting/AccountUnitPriceUpdateAction.h"
#include "57_accounting/AccountRenameAction.h"
#include "57_accounting/AccountLockAction.h"


#include "AccountingModule.inc.cpp"

void synthese::accounting::moduleRegister()
{

	synthese::accounts::AccountingModule::integrate();

	synthese::accounts::CurrencyTableSync::integrate();
	synthese::accounts::AccountTableSync::integrate();
	synthese::accounts::TransactionTableSync::integrate();
	synthese::accounts::TransactionPartTableSync::integrate();

	synthese::accounts::AccountAddAction::integrate();
	synthese::accounts::AccountRenameAction::integrate();
	synthese::accounts::AccountLockAction::integrate();
	synthese::accounts::AccountUnitPriceUpdateAction::integrate();
}
