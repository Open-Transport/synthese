#include "PedestrianComplianceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/PedestrianCompliance.h"

#include <sqlite/sqlite3.h>
#include <boost/logic/tribool.hpp>

#include <assert.h>


using boost::logic::tribool;

using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



PedestrianComplianceTableSync::PedestrianComplianceTableSync (Environment::Registry& environments)
: ComponentTableSync (PEDESTRIANCOMPLIANCES_TABLE_NAME, environments)
{
    addTableColumn (PEDESTRIANCOMPLIANCES_TABLE_COL_STATUS, "INTEGER");
    addTableColumn (PEDESTRIANCOMPLIANCES_TABLE_COL_CAPACITY, "INTEGER");
}



PedestrianComplianceTableSync::~PedestrianComplianceTableSync ()
{

}

    


void 
PedestrianComplianceTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    
    tribool status = true;
    int statusInt (
	Conversion::ToInt (rows.getColumn (rowIndex, PEDESTRIANCOMPLIANCES_TABLE_COL_STATUS)));
    if (statusInt < 0)
    {
	status = boost::logic::indeterminate;
    }
    else if (statusInt == 0)
    {
	status = false;
    }
    
    int capacity (
	Conversion::ToInt (rows.getColumn (rowIndex, PEDESTRIANCOMPLIANCES_TABLE_COL_CAPACITY)));
    
    synthese::env::PedestrianCompliance* cmp = new synthese::env::PedestrianCompliance (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	status, capacity);
    
    environment.getPedestrianCompliances ().add (cmp, false);
}



void 
PedestrianComplianceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    synthese::env::PedestrianCompliance* cmp = 
	environment.getPedestrianCompliances ().get (id);

    tribool status = true;
    int statusInt (
	Conversion::ToInt (rows.getColumn (rowIndex, PEDESTRIANCOMPLIANCES_TABLE_COL_STATUS)));
    if (statusInt < 0)
    {
	status = boost::logic::indeterminate;
    }
    else if (statusInt == 0)
    {
	status = false;
    }
    
    int capacity (
	Conversion::ToInt (rows.getColumn (rowIndex, PEDESTRIANCOMPLIANCES_TABLE_COL_CAPACITY)));
    
    cmp->setCompliant (status);
    cmp->setCapacity (capacity);
}



void 
PedestrianComplianceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getPedestrianCompliances ().remove (id);
}













}

}
