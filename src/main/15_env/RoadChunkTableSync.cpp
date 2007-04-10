#include "RoadChunkTableSync.h"

#include "01_util/Conversion.h"
#include "02_db/SQLiteResult.h"
#include "02_db/SQLiteQueueThreadExec.h"

#include "15_env/Address.h"
#include "15_env/RoadChunk.h"
#include "15_env/Point.h"

#include <boost/tokenizer.hpp>
#include <sqlite/sqlite3.h>
#include <assert.h>


using namespace boost;
using synthese::util::Conversion;

namespace synthese
{
	using namespace db;

namespace env
{



RoadChunkTableSync::RoadChunkTableSync ()
: ComponentTableSync (ROADCHUNKS_TABLE_NAME, true, false, db::TRIGGERS_ENABLED_CLAUSE)
{
    addTableColumn (ROADCHUNKS_TABLE_COL_ADDRESSID, "INTEGER", false);
    addTableColumn (ROADCHUNKS_TABLE_COL_RANKINPATH, "INTEGER", false);
    addTableColumn (ROADCHUNKS_TABLE_COL_ISDEPARTURE, "BOOLEAN", false);
    addTableColumn (ROADCHUNKS_TABLE_COL_ISARRIVAL, "BOOLEAN", false);
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

    if (environment.getRoadChunks ().contains (id)) return;

    uid fromAddressId (
	Conversion::ToLongLong (rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_ADDRESSID)));

    bool isDeparture (Conversion::ToBool (
			  rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_ISDEPARTURE)));
    bool isArrival (Conversion::ToBool (
			rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_ISARRIVAL)));

    int rankInRoad (
	Conversion::ToInt (rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_RANKINPATH)));
    
    std::string viaPointsStr (
	rows.getColumn (rowIndex, ROADCHUNKS_TABLE_COL_VIAPOINTS));

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

    shared_ptr<const Address> fromAddress = environment.getAddresses ().get (fromAddressId);
    shared_ptr<RoadChunk> rc(new RoadChunk (id, fromAddress.get(), isDeparture, isArrival, rankInRoad));

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
    
    environment.getRoads ().getUpdateable(fromAddress->getRoad ()->getId ())->addEdge (rc.get());
    environment.getRoadChunks ().add (rc);
}






void 
RoadChunkTableSync::doReplace (const synthese::db::SQLiteResult& rows, int rowIndex,
			  synthese::env::Environment& environment)
{
    uid id (Conversion::ToLongLong (rows.getColumn (rowIndex, TABLE_COL_ID)));
    shared_ptr<RoadChunk> rc = environment.getRoadChunks ().getUpdateable (id);

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

