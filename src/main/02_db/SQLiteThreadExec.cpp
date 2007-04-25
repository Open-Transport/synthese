#include "02_db/SQLiteThreadExec.h"

#include "02_db/SQLiteQueueThreadExec.h"
#include "02_db/SQLite.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"

#include <boost/iostreams/stream.hpp>



using synthese::util::Log;
using synthese::util::Conversion;



namespace synthese
{
	namespace db
	{

		SQLiteThreadExec::SQLiteThreadExec (synthese::tcp::TcpService* tcpService) 
			: _tcpService (tcpService)
		{
		}

	    
			
		void 
		SQLiteThreadExec::loop ()
		{
		    static const char ETB (23);

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
		    tcpStream << "Welcome to SYNTHESE SQLite embedded db server!" << std::endl;
		    tcpStream.flush ();

		    char buffer[1024*512]; // 512K buffer max
		    while (tcpStream.getline (buffer, 1024*512, ETB))
		    {
		    
			std::string requestString (buffer);
			    
			// Request string is pure SQL
			    
			try
			{
			    // First determine is SQL is a query or an update
			    if (SQLite::IsUpdateStatement (requestString))
			    {
				Log::GetInstance ().debug ("Received SQL update : " + 
							   requestString + " (" + 
							   Conversion::ToString (requestString.size ()) + 
							   " bytes)");
				    
				DBModule::GetSQLite ()->execUpdate (requestString);

				tcpStream << "00" << "Update successful." << ETB;
				    
			    }
			    else
			    {
				Log::GetInstance ().debug ("Received SQL query : " + 
							   requestString + " (" + 
							   Conversion::ToString (requestString.size ()) + 
							   " bytes)");

				SQLiteResult result = DBModule::GetSQLite ()->execQuery (requestString);
				    
				tcpStream << "00" << result << ETB;
				
			    }
			}
			catch (util::Exception& e)
			{
			    Log::GetInstance().debug("Exception", e);
			    tcpStream << "01" << e.what () << ETB;
			}

			    
			    
			tcpStream.flush();
		    }

		    _tcpService->closeConnection (serverSocket);
		}
	}
}

