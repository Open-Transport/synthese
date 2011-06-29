set_source_groups()

set(accounting_SRCS
Account.cpp
Account.h
AccountAddAction.cpp
AccountAddAction.h
AccountCreateStockAction.h
AccountingModule.cpp
AccountingModule.gen.cpp
AccountingModuleRegister.cpp
AccountingModule.h
AccountingModule.inc.cpp
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

set_source_files_properties(AccountingModule.gen.cpp AccountingModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(57_accounting ${accounting_SRCS})

target_link_libraries(57_accounting
  10_db
  12_security
  15_server
)

install(TARGETS 57_accounting DESTINATION lib)
