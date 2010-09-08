
/** DbCommandThreadExec class implementation.
	@file DbCommandThreadExec.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "DbCommandThreadExec.h"
#include "DbRingModule.h"
#include "Node.h"

#include "DBModule.h"
#include "SQLiteHandle.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"
#include "00_tcp/Constants.h"

#include "Conversion.h"
#include "iostreams/Compression.h"
#include "Log.h"
#include "threads/Thread.h"

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

		    // Get info about connection mode
		    char replicationMode;
		    tcpStream.get (replicationMode);
		    bool withoutReplication (replicationMode == 'L');

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
			    if (SQLiteHandle::IsUpdateStatement (requestString))
			    {
				// With ring replication
				if (withoutReplication)
				{
				    DBModule::GetSQLite ()->execUpdate (requestString);
				}
				else
				{
				    DbRingModule::GetNode ()->execUpdate (requestString);  
				}

				reply << "00" << "Update successful.";
				    
			    }
			    else
			    {
				// We want a cached result so lazy parameter is false
				SQLiteResultSPtr result (DbRingModule::GetNode ()->execQuery (requestString, true));

				reply << "00" << (*result) << std::flush;
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

