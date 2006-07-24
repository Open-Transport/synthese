#include "BikeComplianceTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/BikeCompliance.h"

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



BikeComplianceTableSync::BikeComplianceTableSync (Environment::Registry& environments)
: ComponentTableSync (BIKECOMPLIANCES_TABLE_NAME, environments)
{
    addTableColumn (BIKECOMPLIANCES_TABLE_COL_STATUS, "INTEGER");
    addTableColumn (BIKECOMPLIANCES_TABLE_COL_CAPACITY, "INTEGER");
}



BikeComplianceTableSync::~BikeComplianceTableSync ()
{

}

    


void 
BikeComplianceTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
		      synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    
    tribool status = true;
    int statusInt (
	Conversion::ToInt (rows.getColumn (rowIndex, BIKECOMPLIANCES_TABLE_COL_STATUS)));
    if (statusInt < 0)
    {
	status = boost::logic::indeterminate;
    }
    else if (statusInt == 0)
    {
	status = false;
    }
    
    int capacity (
	Conversion::ToInt (rows.getColumn (rowIndex, BIKECOMPLIANCES_TABLE_COL_CAPACITY)));
    
    synthese::env::BikeCompliance* cmp = new synthese::env::BikeCompliance (
	Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)),
	status, capacity);
    
    environment.getBikeCompliances ().add (cmp, false);
}



void 
BikeComplianceTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    synthese::env::BikeCompliance* cmp = 
	environment.getBikeCompliances ().get (id);

    tribool status = true;
    int statusInt (
	Conversion::ToInt (rows.getColumn (rowIndex, BIKECOMPLIANCES_TABLE_COL_STATUS)));
    if (statusInt < 0)
    {
	status = boost::logic::indeterminate;
    }
    else if (statusInt == 0)
    {
	status = false;
    }
    
    int capacity (
	Conversion::ToInt (rows.getColumn (rowIndex, BIKECOMPLIANCES_TABLE_COL_CAPACITY)));
    
    cmp->setCompliant (status);
    cmp->setCapacity (capacity);
}



void 
BikeComplianceTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    assert (false);
}













}

}
