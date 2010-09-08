#ifndef SYNTHESE_DBRING_MODULE_H
#define SYNTHESE_DBRING_MODULE_H

#include "ModuleClassTemplate.h"

namespace synthese
{
	/** @defgroup m101 101 Database synchronization ring.
		@ingroup m1
		
		When this module will run correctly, integrate it into 10_db

		@todo To reactivate 101 db ring module, replace the time storage in iso format by standard SQL format

		C:\forge2\dev\synthese3\src\03_db_ring\NodeClientThreadExec.cpp(125):		tcpStream << _clientNodeInfo << ETB << to_iso_string (lastAcknowledgedTimestamp) << ETB << std::flush;
		C:\forge2\dev\synthese3\src\03_db_ring\NodeClientThreadExec.cpp(178):					      + to_iso_string (lastAcknowledgedTimestamp));
		C:\forge2\dev\synthese3\src\03_db_ring\NodeServerThreadExec.cpp(140):					      + to_iso_string (timestamp));
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecord.cpp(94):    os << op._key << " " << to_iso_string (op._timestamp) << " " << op._emitterNodeId 
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecord.cpp(121):    //std::cerr << "..got timestamp " << to_iso_string (op._timestamp) << std::endl;
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecordTableSync.cpp(205):		query << "((" << TABLE_COL_TIMESTAMP << " > " << Conversion::ToSQLiteString (to_iso_string (clientLastAcknowledgedTimestamp)) << ") OR ";
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecordTableSync.cpp(238):		      << TABLE_COL_TIMESTAMP << " > " << Conversion::ToSQLiteString (to_iso_string (clientLastAcknowledgedTimestamp)) << ") AND (";
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecordTableSync.cpp(295):		  << Conversion::ToSQLiteString (to_iso_string (lastAcknowledgedTimestamp))
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecordTableSync.cpp(349):				<< "," << TABLE_COL_TIMESTAMP << "=" << Conversion::ToSQLiteString (to_iso_string (ur->getTimestamp ()));
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecordTableSync.cpp(389):		  << "," << TABLE_COL_TIMESTAMP << "=" << Conversion::ToSQLiteString(to_iso_string (ur->getTimestamp ()));
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecordTableSync.cpp(404):	    query << "UPDATE " << TABLE.NAME << " SET " << TABLE_COL_TIMESTAMP << "=" << Conversion::ToSQLiteString (to_iso_string (now));
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecordTableSync.cpp(461):		  << TABLE_COL_TIMESTAMP << " > " << Conversion::ToSQLiteString (to_iso_string (startTimestamp)) << " AND "
		C:\forge2\dev\synthese3\src\03_db_ring\UpdateRecordTableSync.cpp(462):		  << TABLE_COL_TIMESTAMP << " < " << Conversion::ToSQLiteString (to_iso_string (endTimestamp)) ;


	@{
	*/


	/** 101 Database ring namespace.
	*/
	namespace dbring
	{

	    class Node;

		class DbRingModule:
			public server::ModuleClassTemplate<DbRingModule>
	    {
	    private:
		
			static Node* _Node;
		
	    public:
		
			static Node* GetNode ();
		
			/** Called whenever a parameter registered by this module is changed
			*/
			static void ParameterCallback (const std::string& name, 
							const std::string& value);
		};
	}
	
	/** @} */
	
}
#endif

