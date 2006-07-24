#include "LineStopTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/LineStop.h"
#include "15_env/Point.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;
using synthese::env::Point;
using synthese::env::LineStop;

namespace synthese
{
namespace envlssql
{



LineStopTableSync::LineStopTableSync (Environment::Registry& environments)
: ComponentTableSync (LINESTOPS_TABLE_NAME, environments)
{
    addTableColumn (LINESTOPS_TABLE_COL_FROMPHYSICALSTOPID, "INTEGER");
    addTableColumn (LINESTOPS_TABLE_COL_LINEID, "INTEGER");
    addTableColumn (LINESTOPS_TABLE_COL_RANKINPATH, "INTEGER");
    addTableColumn (LINESTOPS_TABLE_COL_METRICOFFSET, "DOUBLE");
    addTableColumn (LINESTOPS_TABLE_COL_VIAPOINTS, "TEXT");
}



LineStopTableSync::~LineStopTableSync ()
{

}

    


void 
LineStopTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
			   synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    uid fromPhysicalStopId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_FROMPHYSICALSTOPID)));

    uid lineId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_LINEID)));

    int rankInPath (
	Conversion::ToInt (rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_RANKINPATH)));

    double metricOffset (
	Conversion::ToDouble (rows.getColumn (rowIndex, LINESTOPS_TABLE_COL_METRICOFFSET)));
    
    std::string viaPointsStr (
	rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_VIAPOINTS));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    LineStop* ls = new synthese::env::LineStop (
	id, 
	environment.getLines ().get (lineId),
	rankInPath,
	metricOffset,
	environment.getPhysicalStops ().get (fromPhysicalStopId)
	);

    boost::char_separator<char> sep1 (",");
    boost::char_separator<char> sep2 (":");
    tokenizer viaPointsTokens (viaPointsStr, sep1);
    for (tokenizer::iterator viaPointIter = viaPointsTokens.begin();
	 viaPointIter != viaPointsTokens.end (); ++viaPointIter)
    {
	tokenizer valueTokens (*viaPointIter, sep2);
	tokenizer::iterator valueIter = valueTokens.begin();

	// X:Y
	ls->addViaPoint (synthese::env::Point (Conversion::ToDouble (*valueIter), 
					       Conversion::ToDouble (*(++valueIter))));
    }

    environment.getLineStops ().add (ls, false);
}






void 
LineStopTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
}





void 
LineStopTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
}















}

}
