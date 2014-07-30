/** SCOMSocketReader class header.
	@file SCOMSocketReader.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_SCOMSocketReader_h__
#define SYNTHESE_SCOMSocketReader_h__

#include <string>
#include <vector>

#include <boost/system/error_code.hpp>
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace scom
	{
		class SCOMData;

		/** This class reads an SCOM TCP socket

		   Once this object is started using the start() fonction, it reads the TCP socket and insert
		   all the values in the SCOMData object given on creation.

		   A call on start() launches a new thread and return immediately.

		   TODO : Process

		   This object uses the boost ASIO library to connect to the socket and the
		   boost thread library for the thread.
		  */
		class SCOMSocketReader
		{
		public:

			/** Creator

			  Note : the SCOMData is not deleted by SCOMSocket.

			  @param dataHandler The SCOMData object in which the data will be fed.
			*/
			SCOMSocketReader (SCOMData* dataHandler);

			/** Start the socket listener

			 This function will create its own thread so it will return immediatly.
			*/
			void Start ();

			/** Stop the listening

			 When called, this function will close the socket and stop the thread.
			 It can then be started again.
			*/
			void Stop ();

		private:
			// SCOM data handler to use
			SCOMData* _dataHandler;

			// Internal thread used for the async loop
			boost::shared_ptr<boost::thread> _thread;

			// The server IP/FQDN and port
			std::string _server;
			unsigned short _port;

			// ID and bornes to listen to
			std::string _id;
			std::vector<std::string> _bornes;

			// Buffer and internal XML file (as a string)
			boost::array<char,4096> _buffer;
			std::string _xml;

			// Timeouts (in seconds)
			int _connectTimeout;
			int _resolveRetry;
			int _connectRetry;

			// Boost's ASIO service and socket
			boost::asio::io_service* _ios;
			boost::asio::io_service::work* _work;
			boost::asio::ip::tcp::socket* _socket;

			// Generic timer used for async operations
			// Note for the use of this timer :
			// It is the responsibility of the completed operation
			// (the one not called by the timer) to stop and delete the timer.
			boost::asio::deadline_timer* _timer;

			// Resolv the server address
			void _resolv (bool first = true);

			// Connect to the SCOM TCP socket
			void _connect (const boost::asio::ip::tcp::endpoint& endpoint);

			// Close the socket
			// If restart is at true, the connection will be started again
			// after a wait of _connectRetry
			void _close (bool restart = false);

			// A connection is completed
			void _connectionComplete (const std::string& error, const boost::system::error_code& ec);

			// Authenticate on the SCOM server using the client ID
			void _authenticate ();

			// Authentication complete
			// If the error parameters isn't empty,
			void _authenticationComplete (const std::string& error, const boost::system::error_code& ec);

			// Data received
			void _dataReceived (const boost::system::error_code& error, std::size_t size);
		};

	}
}

#endif // SYNTHESE_SCOMSocketReader_h__
