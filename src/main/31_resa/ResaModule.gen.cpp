
synthese::resa::ResaRight::integrate();

synthese::resa::BookReservationAction::integrate();

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::resa::ReservationTableSync>("31.1 Reservation Table Sync");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::resa::ReservationTransactionTableSync>("31.2 Reservation Transaction Table Sync");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::resa::OnlineReservationRuleTableSync>("31.5 Online Reservation Rule Table Sync");
