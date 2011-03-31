
set(accounting_SRCS
Account.cpp
Account.h
AccountAddAction.cpp
AccountAddAction.h
AccountCreateStockAction.h
AccountingModule.cpp
AccountingModule.h
AccountLockAction.cpp
AccountLockAction.h
AccountRenameAction.cpp
AccountRenameAction.h
AccountTableSync.cpp
AccountTableSync.h
AccountUnitPriceUpdateAction.cpp
AccountUnitPriceUpdateAction.h
Currency.cpp
Currency.h
CurrencyTableSync.cpp
CurrencyTableSync.h
Transaction.cpp
Transaction.h
TransactionPart.cpp
TransactionPart.h
TransactionPartTableSync.cpp
TransactionPartTableSync.h
TransactionTableSync.cpp
TransactionTableSync.h
)

add_library(57_accounting ${accounting_SRCS})

install(TARGETS 57_accounting DESTINATION lib)
