
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::accounts::AccountingModule>("57_accounting");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::accounts::CurrencyTableSync>("57.00 Currency");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::accounts::AccountTableSync>("57.10 Account");

