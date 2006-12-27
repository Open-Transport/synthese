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
			// No need to lock, TcpService methods are thread-safe.
			synthese::tcp::TcpServerSocket& serverSocket =
			_tcpService->acceptConnection ();
		    

			boost::iostreams::stream<synthese::tcp::TcpServerSocket> 
			tcpStream (serverSocket);
		    
		    
			char buffer[1024*512]; // 512K buffer max
			tcpStream.getline (buffer, 1024*512);
		    
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
				
				SQLiteResult result = DBModule::GetSQLite ()->execQuery (requestString);
				
				// Sends the answer with the following format :
				// TODO implement SQLiteResult ostream << ;
				
				
			    }
			    else
			    {
				Log::GetInstance ().debug ("Received SQL query : " + 
							   requestString + " (" + 
							   Conversion::ToString (requestString.size ()) + 
							   " bytes)");
				DBModule::GetSQLite ()->execUpdate (requestString);
			    }
			}
			catch (util::Exception e)
			{
			    Log::GetInstance().debug("Exception", e);
			}

			
			tcpStream.flush();
			_tcpService->closeConnection (serverSocket);
		}
	}
}

