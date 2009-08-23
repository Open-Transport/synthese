
synthese::resa::ResaModule::integrate();

synthese::resa::ResaRight::integrate();

synthese::resa::BookReservationAction::integrate();
synthese::resa::CancelReservationAction::integrate();
synthese::resa::CallBeginAction::integrate();
synthese::resa::CallEndAction::integrate();
synthese::resa::ResaLogEntryUpdateAction::integrate();
synthese::resa::ReservationUserUpdateAction::integrate();
synthese::resa::SendPasswordAction::integrate();

synthese::resa::XMLReservationFunction::integrate();
synthese::resa::ResaCustomerHtmlOptionListFunction::integrate();
synthese::resa::ReservationEditFunction::integrate();

synthese::resa::ReservationItemInterfacePage::integrate();
synthese::resa::ReservationConfirmationEMailInterfacePage::integrate();
synthese::resa::ReservationConfirmationEMailItemInterfacePage::integrate();
synthese::resa::CustomerPasswordEMailContentInterfacePage::integrate();
synthese::resa::ReservationConfirmationEMailSubjectInterfacePage::integrate();
synthese::resa::CustomerPasswordEMailSubjectInterfacePage::integrate();
synthese::resa::ReservationEditInterfacePage::integrate();
synthese::resa::ReservationCancellationEMailSubjectInterfacePage::integrate();
synthese::resa::ReservationCancellationEMailContentInterfacePage::integrate();

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
synthese::util::Env::Integrate<synthese::resa::Reservation>();
synthese::util::Env::Integrate<synthese::resa::ReservationTransaction>();
synthese::util::Env::Integrate<synthese::resa::OnlineReservationRule>();