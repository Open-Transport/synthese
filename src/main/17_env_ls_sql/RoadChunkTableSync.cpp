#include "RoadChunkTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteThreadExec.h"

#include "15_env/RoadChunk.h"
#include "15_env/Point.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>
#include <assert.h>



using synthese::util::Conversion;
using synthese::db::SQLiteResult;
using synthese::env::Environment;
using synthese::env::Point;
using synthese::env::RoadChunk;

namespace synthese
{
namespace envlssql
{



RoadChunkTableSync::RoadChunkTableSync (Environment::Registry& environments)
: ComponentTableSync (ROADCHUNKS_TABLE_NAME, environments, true, false)
{
    addTableColumn (ROADCHUNKS_TABLE_COL_FROMADDRESSID, "INTEGER", false);
    addTableColumn (ROADCHUNKS_TABLE_COL_RANKINPATH, "INTEGER", false);
    addTableColumn (ROADCHUNKS_TABLE_COL_VIAPOINTS, "TEXT", true);
}



RoadChunkTableSync::~RoadChunkTableSync ()
{

}

    


void 
RoadChunkTableSync::doAdd (const synthese::db::SQLiteResult& rows, int rowIndex,
			   synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));

    uid fromAddressId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_FROMADDRESSID)));

    int rankInRoad (
	Conversion::ToInt (rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_RANKINPATH)));
    
    std::string viaPointsStr (
	rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_VIAPOINTS));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    RoadChunk* rc = new synthese::env::RoadChunk (
	id, environment.getAddresses ().get (fromAddressId), rankInRoad);

    boost::char_separator<char> sep1 (",");
    boost::char_separator<char> sep2 (":");
    tokenizer viaPointsTokens (viaPointsStr, sep1);
    for (tokenizer::iterator viaPointIter = viaPointsTokens.begin();
	 viaPointIter != viaPointsTokens.end (); ++viaPointIter)
    {
	tokenizer valueTokens (*viaPointIter, sep2);
	tokenizer::iterator valueIter = valueTokens.begin();

	// X:Y
	rc->addViaPoint (synthese::env::Point (Conversion::ToDouble (*valueIter), 
					       Conversion::ToDouble (*(++valueIter))));
    }

    environment.getRoadChunks ().add (rc, false);
}






void 
RoadChunkTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    synthese::env::RoadChunk* rc = environment.getRoadChunks ().get (id);

    std::string viaPointsStr (
	rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_VIAPOINTS));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    
    rc->clearViaPoints ();

    boost::char_separator<char> sep1 (",");
    boost::char_separator<char> sep2 (":");
    tokenizer viaPointsTokens (viaPointsStr, sep1);
    for (tokenizer::iterator viaPointIter = viaPointsTokens.begin();
	 viaPointIter != viaPointsTokens.end (); ++viaPointIter)
    {
	tokenizer valueTokens (*viaPointIter, sep2);
	tokenizer::iterator valueIter = valueTokens.begin();

	// X:Y
	rc->addViaPoint (synthese::env::Point (Conversion::ToDouble (*valueIter), 
					       Conversion::ToDouble (*(++valueIter))));
    }

}





void 
RoadChunkTableSync::doRemove (const synthese::db::SQLiteResult& rows, int rowIndex,
			 synthese::env::Environment& environment)
{
    uid id = Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID));
    environment.getRoadChunks ().remove (id);
}















}

}
