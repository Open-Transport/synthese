
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
