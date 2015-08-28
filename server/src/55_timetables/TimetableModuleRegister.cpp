
#include "TimetableModule.h"

#include "TimetableRowTableSync.h"
#include "TimetableRowGroupItemTableSync.hpp"
#include "TimetableRowGroupTableSync.hpp"
#include "TimetableTableSync.h"

#include "TimetableAdmin.h"

#include "TimetableAddAction.h"
#include "TimetableRowAddAction.h"
#include "TimetableRowGroupItemAddAction.hpp"
#include "TimetableUpdateAction.h"
#include "TimetableSetLineAction.h"
#include "TimetableSetPhysicalStopAction.h"
#include "TimetableTransferUpdateAction.hpp"

#include "TimetableBuildService.hpp"
#include "TimetableGenerateFunction.h"
#include "MultipleTimetableGenerateFunction.hpp"

#include "TimetableRight.h"

#include "Timetable.h"
#include "TimetableRow.h"
#include "TimetableRowGroup.hpp"
#include "TimetableRowGroupItem.hpp"


#include "TimetableModule.inc.cpp"

void synthese::timetables::moduleRegister()
{

	// TIMETABLES 55

	// Factories

	synthese::timetables::TimetableModule::integrate();

	synthese::timetables::TimetableRowTableSync::integrate();
	synthese::timetables::TimetableTableSync::integrate();
	synthese::timetables::TimetableRowGroupTableSync::integrate();
	synthese::timetables::TimetableRowGroupItemTableSync::integrate();

	synthese::timetables::TimetableAdmin::integrate();

	synthese::timetables::TimetableAddAction::integrate();
	synthese::timetables::TimetableRowAddAction::integrate();
	synthese::timetables::TimetableRowGroupItemAddAction::integrate();
	synthese::timetables::TimetableUpdateAction::integrate();
	synthese::timetables::TimetableSetLineAction::integrate();
	synthese::timetables::TimetableSetPhysicalStopAction::integrate();
	synthese::timetables::TimetableTransferUpdateAction::integrate();

	synthese::timetables::TimetableRight::integrate();

	synthese::timetables::TimetableBuildService::integrate();
	synthese::timetables::TimetableGenerateFunction::integrate();
	synthese::timetables::MultipleTimetableGenerateFunction::integrate();

	// Registries
	INTEGRATE(synthese::timetables::TimetableRowGroupItem);
	INTEGRATE(synthese::timetables::TimetableRowGroup);
	synthese::util::Env::Integrate<synthese::timetables::TimetableRow>();
	INTEGRATE(synthese::timetables::Timetable);

}
