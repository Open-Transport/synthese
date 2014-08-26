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

#include "Settable.h"

#include <string>
#include <vector>
#include <list>

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

		   A call on Start() launches a new thread and return immediately.
		   To stop listening, use the Stop() function.

		   This object uses the boost ASIO library to connect to the socket.

		   This object uses a simple state machine to go through the connection stages :
		   \li Resolve the server adress
		   \li Connect to the socket
		   \li Authenticate
		   \li Read
		   \li Close

		   On error, the socket is closed and the state machine restart at the first step.

		   On some of the states, a timeout can be set to be sure that the server will not hang.

		  */
		class SCOMSocketReader :
			settings::Settable
		{
		public:

			/** Creator

			  Note : the SCOMData is not deleted by SCOMSocket.

			  @param dataHandler The SCOMData object in which the data will be fed.
			*/
			SCOMSocketReader (SCOMData* dataHandler);

			// Unregister our settings
			~SCOMSocketReader ();

			/** Start the socket listener

			 This function will create its own thread so it will return immediatly.
			*/
			void Start ();

			/** Stop the listening

			 When called, this function will close the socket and stop the thread.
			 It can then be started again.
			*/
			void Stop ();

			//! Settings value updated, see Settable::ValueUpdated()
			virtual void ValueUpdated (
				const std::string& module,
				const std::string& name,
				const std::string& value,
				bool notify
			);

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

			// Retry delays (in seconds)
			int _resolveRetry;
			int _connectRetry;

			// Boost's ASIO service and socket
			boost::asio::io_service* _ios;
			boost::asio::io_service::work* _work;
			boost::asio::ip::tcp::socket* _socket;
			boost::asio::ip::tcp::endpoint _endpoint;

			// Generic timer used for async operations
			// Note for the use of this timer :
			// It is the responsibility of the completed operation
			// (the one not called by the timer) to stop and delete the timer.
			boost::asio::deadline_timer* _timer;

			// Enumeration of possible states
			enum States {
				RESOLVE,
				CONNECT,
				AUTHENTICATE,
				READ,
				CLOSE,
				STOP,
				STATE_NUMBER
			};

			// Name of each state
			std::string _stateName[STATE_NUMBER];

			// Current and next state of the state machine
			States _state, _next;

			// Timeouts for each state (in seconds)
			int _timeouts[STATE_NUMBER];

			// Internal settings
			static const std::string SETTING_RESOLVERETRY;
			static const std::string SETTING_CONNECTRETRY;
			static const std::string SETTING_SERVER;
			static const std::string SETTING_PORT;
			static const std::string SETTING_ID;
			static const std::string SETTING_BORNES;
			static const std::string SETTING_CONNECTTIMEOUT;
			static const std::string SETTING_READTIMEOUT;

			// Our module name for the settings
			static const std::string SETTINGS_MODULE;


			// Main loop for the state machine
			void _mainLoop (const std::string& error, const boost::system::error_code& ec);

			// Resolv the server address
			bool _resolv ();

			// Connect to the SCOM TCP socket
			void _connect ();

			// Authenticate on the SCOM server using the client ID
			void _authenticate ();

			// Data received
			// This function calls the main loop again on error
			void _dataReceived (const boost::system::error_code& error, std::size_t size);

			// Close the socket
			void _close ();

		};

	}
}

#endif // SYNTHESE_SCOMSocketReader_h__
