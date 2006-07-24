#include "HandicappedComplianceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/HandicappedCompliance.h"

#include <sqlite/sqlite3.h>
#include <boost/logic/tribool.hpp>


using boost::logic::tribool;

using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;

namespace synthese
{
namespace envlssql
{



HandicappedComplianceTableSync::HandicappedComplianceTableSync (Environment::Registry& environments)
: ComponentTableSync (HANDICAPPEDCOMPLIANCES_TABLE_NAME, environments)
{
    addTableColumn (HANDICAPPEDCOMPLIANCES_TABLE_COL_STATUS, "INTEGER");
    addTableColumn (HANDICAPPEDCOMPLIANCES_TABLE_COL_CAPACITY, "INTEGER");
}



HandicappedComplianceTableSync::~HandicappedComplianceTableSync ()
{

}

    


void 
HandicappedComplianceTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    
    tribool status = true;
    int statusInt (
	Conversion::ToInt (rows.getColumn (rowIndex, HANDICAPPEDCOMPLIANCES_TABLE_COL_STATUS)));
    if (statusInt < 0)
    {
	status = boost::logic::indeterminate;
    }
    else if (statusInt == 0)
    {
	status = false;
    }
    
    int capacity (
	Conversion::ToInt (rows.getColumn (rowIndex, HANDICAPPEDCOMPLIANCES_TABLE_COL_CAPACITY)));
    
    synthese::env::HandicappedCompliance* cmp = new synthese::env::HandicappedCompliance (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	status, capacity);
    
    environment.getHandicappedCompliances ().add (cmp, false);
}



void 
HandicappedComplianceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
}



void 
HandicappedComplianceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
}













}

}
