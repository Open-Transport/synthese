
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::accounts::AccountingModule>("57_accounting");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::accounts::CurrencyTableSync>("57.00 Currency");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::accounts::AccountTableSync>("57.10 Account");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::accounts::TransactionTableSync>("57.20 Transaction");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::accounts::TransactionPartTableSync>("57.30 Transaction Part");


