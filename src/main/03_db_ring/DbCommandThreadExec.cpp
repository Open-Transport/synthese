#include "03_db_ring/DbCommandThreadExec.h"
#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/Node.h"

#include "02_db/SQLite.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"
#include "00_tcp/Constants.h"

#include "01_util/Conversion.h"
#include "01_util/Compression.h"
#include "01_util/Log.h"
#include "01_util/Thread.h"

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>



using namespace synthese::tcp;
using namespace synthese::db;
using namespace synthese::dbring;
using namespace synthese::util;



namespace synthese
{
	namespace dbring
	{

		DbCommandThreadExec::DbCommandThreadExec (synthese::tcp::TcpService* tcpService) 
			: _tcpService (tcpService)
		{
		}

	    
			
		void 
		DbCommandThreadExec::loop ()
		{
		    // No need to lock, TcpService methods are thread-safe.
		    synthese::tcp::TcpServerSocket* serverSocket =
			_tcpService->acceptConnection ();
		    
		    // Non-blocking mode (default)
		    if (serverSocket == 0) return;

		    // A client can stay connected forever...
		    serverSocket->setTimeOut (0);

		    boost::iostreams::stream<synthese::tcp::TcpServerSocket> 
			tcpStream (*serverSocket);
		    
		    // Sends welcome message
		    tcpStream << "Welcome to SYNTHESE SQLite embedded db server!" << std::endl << std::flush;

		    while (tcpStream.good ()) 
		    {
			std::stringstream msg;
			Compression::ZlibDecompress (tcpStream, msg);

			const std::string& requestString = msg.str ();
			if (requestString.length () == 0) continue;

			std::string smallString (Conversion::ToTruncatedString (requestString));

			Log::GetInstance ().debug ("Received SQL statement : " + 
						   smallString + " (" + 
						   Conversion::ToString (requestString.length ()) + 
						   " bytes)");

			std::stringstream reply;
			try
			{

			    // First determine is SQL is a query or an update
			    if (SQLite::IsUpdateStatement (requestString))
			    {
				// With ring replication
				DbRingModule::GetNode ()->execUpdate (requestString);  
				// DBModule::GetSQLite ()->execUpdate (requestString);

				reply << "00" << "Update successful.";
				    
			    }
			    else
			    {

				// SQLiteResult result = DBModule::GetSQLite ()->execQuery (requestString);
				SQLiteResult result = DbRingModule::GetNode ()->execQuery (requestString);

				reply << "00" << result << std::flush;
			    }

			}
			catch (util::Exception& e)
			{
			    Log::GetInstance().debug("Exception", e);
			    reply << "01" << e.what () << std::flush;
			}
			
			Compression::ZlibCompress (reply, tcpStream);
			// tcpStream.flush();
		    }

		    _tcpService->closeConnection (serverSocket);
		}
	}
}

