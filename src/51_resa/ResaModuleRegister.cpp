
#include "ResaModule.h"

#include "ReservationTableSync.h"
#include "ReservationTransactionTableSync.h"
#include "OnlineReservationRuleTableSync.h"

#include "ResaRight.h"

#include "ResaDBLog.h"

#include "BookReservationAction.h"
#include "CancelReservationAction.h"
#include "CallBeginAction.h"
#include "CallEndAction.h"
#include "ResaLogEntryUpdateAction.h"
#include "ReservationTransactionUpdateAction.hpp"
#include "ReservationUserUpdateAction.h"
#include "SendPasswordAction.h"
#include "ReservationUpdateAction.hpp"

#include "XMLReservationFunction.h"
#include "ResaCustomerHtmlOptionListFunction.h"
#include "ReservationEditFunction.h"
#include "CSVResaStatisticsFunction.h"
#include "ReservationsListService.hpp"

#include "ReservationItemInterfacePage.h"
#include "ReservationConfirmationEMailInterfacePage.h"
#include "ReservationConfirmationEMailItemInterfacePage.h"
#include "CustomerPasswordEMailContentInterfacePage.h"
#include "ReservationConfirmationEMailSubjectInterfacePage.h"
#include "CustomerPasswordEMailSubjectInterfacePage.h"
#include "ReservationEditInterfacePage.h"
#include "ReservationCancellationEMailSubjectInterfacePage.h"
#include "ReservationCancellationEMailContentInterfacePage.h"

#include "BookableCommercialLineAdmin.h"
#include "BookableCommercialLinesAdmin.h"
#include "FreeDRTBookingAdmin.hpp"
#include "ResaCustomerAdmin.h"
#include "ResaCustomersAdmin.h"
#include "ReservationRoutePlannerAdmin.h"
#include "ResaLogAdmin.h"
#include "ResaEditLogEntryAdmin.h"
#include "CallStatisticsAdmin.h"
#include "ResaStatisticsAdmin.h"
#include "ResaStatisticsMenuAdmin.h"
#include "ReservationAdmin.hpp"

// Registries
#include "Reservation.h"
#include "ReservationTransaction.h"
#include "OnlineReservationRule.h"


#include "ResaModule.inc.cpp"

void synthese::resa::moduleRegister()
{

	synthese::resa::ResaModule::integrate();

	synthese::resa::ResaRight::integrate();

	synthese::resa::BookReservationAction::integrate();
	synthese::resa::CancelReservationAction::integrate();
	synthese::resa::CallBeginAction::integrate();
	synthese::resa::CallEndAction::integrate();
	synthese::resa::ResaLogEntryUpdateAction::integrate();
	synthese::resa::ReservationTransactionUpdateAction::integrate();
	synthese::resa::ReservationUserUpdateAction::integrate();
	synthese::resa::SendPasswordAction::integrate();
	synthese::resa::ReservationUpdateAction::integrate();

	synthese::resa::XMLReservationFunction::integrate();
	synthese::resa::ResaCustomerHtmlOptionListFunction::integrate();
	synthese::resa::ReservationEditFunction::integrate();
	synthese::resa::CSVResaStatisticsFunction::integrate();
	synthese::resa::ReservationsListService::integrate();

	synthese::resa::ReservationItemInterfacePage::integrate();
	synthese::resa::ReservationConfirmationEMailInterfacePage::integrate();
	synthese::resa::ReservationConfirmationEMailItemInterfacePage::integrate();
	synthese::resa::CustomerPasswordEMailContentInterfacePage::integrate();
	synthese::resa::ReservationConfirmationEMailSubjectInterfacePage::integrate();
	synthese::resa::CustomerPasswordEMailSubjectInterfacePage::integrate();
	synthese::resa::ReservationEditInterfacePage::integrate();
	synthese::resa::ReservationCancellationEMailSubjectInterfacePage::integrate();
	synthese::resa::ReservationCancellationEMailContentInterfacePage::integrate();

	synthese::resa::ReservationTableSync::integrate();
	synthese::resa::ReservationTransactionTableSync::integrate();
	synthese::resa::OnlineReservationRuleTableSync::integrate();

	synthese::resa::BookableCommercialLineAdmin::integrate();
	synthese::resa::BookableCommercialLinesAdmin::integrate();
	synthese::resa::FreeDRTBookingAdmin::integrate();
	synthese::resa::ResaCustomerAdmin::integrate();
	synthese::resa::ResaCustomersAdmin::integrate();
	synthese::resa::ReservationRoutePlannerAdmin::integrate();
	synthese::resa::ResaLogAdmin::integrate();
	synthese::resa::ResaEditLogEntryAdmin::integrate();
	synthese::resa::CallStatisticsAdmin::integrate();
	synthese::resa::ResaStatisticsAdmin::integrate();
	synthese::resa::ResaStatisticsMenuAdmin::integrate();
	synthese::resa::ReservationAdmin::integrate();


	synthese::resa::ResaDBLog::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::resa::Reservation>();
	synthese::util::Env::Integrate<synthese::resa::ReservationTransaction>();
	synthese::util::Env::Integrate<synthese::resa::OnlineReservationRule>();
}
