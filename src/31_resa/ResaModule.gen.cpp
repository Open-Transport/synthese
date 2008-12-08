
synthese::resa::ResaModule::integrate();

synthese::resa::ResaRight::integrate();

synthese::resa::BookReservationAction::integrate();
synthese::resa::CancelReservationAction::integrate();
synthese::resa::CallBeginAction::integrate();
synthese::resa::CallEndAction::integrate();
synthese::resa::ResaLogEntryUpdateAction::integrate();

synthese::resa::ReservationsListFunction::integrate();
synthese::resa::ResaCustomerHtmlOptionListFunction::integrate();

synthese::resa::ReservationsListInterfacePage::integrate();
synthese::resa::ReservationItemInterfacePage::integrate();

synthese::resa::ReservationsListInterfaceElement::integrate();
synthese::resa::ResaCallButtonInterfaceElement::integrate();

synthese::resa::ReservationTableSync::integrate();
synthese::resa::ReservationTransactionTableSync::integrate();
synthese::resa::OnlineReservationRuleTableSync::integrate();

synthese::resa::ResaCustomerAdmin::integrate();
synthese::resa::ResaCustomersAdmin::integrate();
synthese::resa::BookableCommercialLineAdmin::integrate();
synthese::resa::BookableCommercialLinesAdmin::integrate();
synthese::resa::ReservationRoutePlannerAdmin::integrate();
synthese::resa::ResaLogAdmin::integrate();
synthese::resa::ResaEditLogEntryAdmin::integrate();

synthese::resa::ResaDBLog::integrate();

// Registries
synthese::util::Env::template Integrate<synthese::resa::Reservation>();
synthese::util::Env::template Integrate<synthese::resa::ReservationTransaction>();
synthese::util::Env::template Integrate<synthese::resa::OnlineReservationRule>();