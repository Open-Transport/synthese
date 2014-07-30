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

#include "SCOMSocketReader.h"

#include "SCOMData.h"

#include "Log.h"
#include "ServerModule.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#include <sstream>

namespace synthese
{
	namespace scom
	{

		using namespace boost::asio;
		using namespace synthese::util;

		// Setup some settings
		// Every setting setup in hard here will be parameters in the end
		// (At least I hope... noba)
		SCOMSocketReader::SCOMSocketReader (SCOMData *dataHandler) :
			_dataHandler(dataHandler),
			_connectTimeout(5),
			_resolveRetry(10),
			_connectRetry(10),
			_socket(NULL),
			_timer(NULL)
		{
			_server = "127.0.0.1"; //10.4.20.23"; // saetr.t-l.ch
			_port = 3106;
			_id = "1";
			_bornes.push_back("517");
			_bornes.push_back("518");

			// Boost's input/output service
			_ios = new io_service();
		}

		// Start the thread
		void SCOMSocketReader::Start ()
		{
			_ios->reset();

			// Launch the first async event before the loop (or it will quit directly)
			// The timer to 0 makes the _resolv() called from the _ios loop, not the
			// main thread
			deadline_timer timer(*_ios,boost::posix_time::seconds(0));
			timer.async_wait( boost::bind(&synthese::scom::SCOMSocketReader::_resolv, this, true));

			// If the io_service is out-of-work, it will stops.
			// We need it to continue so we add this object in its queue
			_work = new boost::asio::io_service::work(*_ios);

			// Run the boost ASIO main loop (for async events)
			_thread = server::ServerModule::AddThread(
				boost::bind(&io_service::run,_ios),
				"SCOM Listener thread"
			);
		}

		void SCOMSocketReader::Stop ()
		{

		}

		/* Will try to resolv the server address
		   Loop until a resolution is made and then call _connect()
		   */
		void SCOMSocketReader::_resolv(bool first)
		{
			boost::system::error_code error;

			// If it is not the first time, it means a resolv have failed, and we wait a certain time before
			// trying again
			if (!first)
			{
				deadline_timer timer(*_ios,boost::posix_time::seconds(_resolveRetry));
				timer.wait(error);
			}

			// Resolve the server address (in case of a FQDN)
			Log::GetInstance().debug("SCOM : Resolving " + _server);
			ip::tcp::resolver resolver(*_ios);
			ip::tcp::resolver::query query(_server,"");
			ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, error);
			ip::tcp::resolver::iterator end;

			// If nothing is found, print an error and call ourselves (try again)
			if (endpoint_iterator == end || error)
			{
				Log::GetInstance().warn("SCOM : Error while connecting to the server : invalid server : " + _server);

				if (error)
				{
					Log::GetInstance().warn("SCOM : Specific error : " + error.message());
				}

				_resolv(false);
				return;
			}

			// Build the IP address from the first ipv4 endpoint
			// Use the first point by default if no IPV4 entry is found
			ip::tcp::endpoint endpoint = *endpoint_iterator;
			while (endpoint_iterator != end)
			{
				if ( endpoint_iterator->endpoint().address().is_v4() )
				{
					endpoint = *endpoint_iterator;
					break;
				}
				endpoint_iterator++;
			}

			// Set the port
			endpoint.port(_port);

			// Phase 2 : connection
			_connect(endpoint);
		}

		/*	Setup an async connection to the SCOM server and start a timeout handler
			If the timeout is reached, _connectionFailed is called
		*/
		void SCOMSocketReader::_connect(const ip::tcp::endpoint& endpoint)
		{
			// Async connection to _connectionComplete
			Log::GetInstance().debug("SCOM : Attempt to connect to " + endpoint.address().to_string() +
									 " port " + boost::lexical_cast<std::string>(endpoint.port()));
			_socket = new ip::tcp::socket(*_ios);
			_socket->async_connect(
				endpoint,
				boost::bind(&synthese::scom::SCOMSocketReader::_connectionComplete,
							this,
							"", // No error
							boost::asio::placeholders::error)
			);

			// Timeout to _connectionFailed
			_timer = new deadline_timer(*_ios);
			_timer->expires_from_now(boost::posix_time::seconds(_connectTimeout));
			_timer->async_wait( boost::bind(&synthese::scom::SCOMSocketReader::_connectionComplete,
										  this,
										  "Timeout",
										  boost::asio::placeholders::error)
							);
		}

		// If an error occured, restart the connection
		// If it worked, go to phase 3 : authenticate
		void SCOMSocketReader::_connectionComplete(const std::string& error, const boost::system::error_code& ec)
		{
			if ( ec == boost::asio::error::operation_aborted )
				Log::GetInstance().warn("Abort");
			if (ec)
				Log::GetInstance().warn("Error");

			// First, stop the timer
			if (_timer)
			{
				_timer->cancel();
				delete _timer;
				_timer = NULL;
				_ios->run_one();
			}

			// If there is an error, print it
			// If the error is a cancelled timer, ignore
			if ( ( ! error.empty() || ec ) && ec != boost::asio::error::operation_aborted )
			{
				Log::GetInstance().warn("SCOM : Error while authenticating to SCOM : " + error);
				if (ec)
				{
					Log::GetInstance().warn("SCOM : Specific error : " + ec.message());
				}
				_close(true); // Will restart the process
				return;
			}

			// Connected, on to phase 3
			Log::GetInstance().debug("SCOM : Connected");
			_authenticate();
		}

		// Close and delete the socket
		void SCOMSocketReader::_close(bool restart)
		{
			boost::system::error_code error;

			Log::GetInstance().debug("SCOM : Closing the SCOM socket");
			_socket->close(error);
			if (error)
			{
				Log::GetInstance().warn("SCOM : Error while closing socket : " + error.message());
			}

			if (_socket)
			{
				delete _socket;
				_socket = NULL;
			}

			// Restart the connection procedure if asked
			if (restart)
			{
				// Wait a moment
				boost::system::error_code ec2;
				deadline_timer timer(*_ios,boost::posix_time::seconds(_connectRetry));
				timer.wait(ec2);
				_resolv();
			}
		}

		// Send the authentication XML to SCOM
		// Redirect to _authicationComplete
		void SCOMSocketReader::_authenticate()
		{
			Log::GetInstance().debug("SCOM : Sending authentication with ID " + _id);

			// Generate the XML identification message
			// (Note : lf seems to be deleted by msg, so do not delete it yourself)
			std::ostringstream msg;
			boost::posix_time::time_facet* lf = new boost::posix_time::time_facet("DATE=\"%d/%m/%Y\" HEURE=\"%H:%M:%S\"");
			const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
			msg.imbue(std::locale(msg.getloc(),lf));
			msg << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>" << std::endl;
			msg << "<IDENT>" << std::endl;
			msg << "   <CLIENT ID=\"" << _id << "\" " << now << "/>" << std::endl;
			msg << "   <LISTE_BORNE>" << std::endl;

			// Add each borne
			BOOST_FOREACH( std::string borne, _bornes)
			{
				msg << "      <BORNE NUMERO=\"" << borne << "\" />" << std::endl;
			}

			msg << "   </LISTE_BORNE>" << std::endl;
			msg << "</IDENT>" << std::endl;
			msg << '\0';

			// Send the message
			async_write(
				*_socket,
				buffer(msg.str()),
				boost::bind(&synthese::scom::SCOMSocketReader::_authenticationComplete,
							this,
							"",
							boost::asio::placeholders::error)
			);

			// Timer on timeout
			_timer = new deadline_timer(*_ios);
			_timer->expires_from_now(boost::posix_time::seconds(_connectTimeout)); // Same as connect
			_timer->async_wait( boost::bind(&synthese::scom::SCOMSocketReader::_authenticationComplete,
										  this,
										  "Timeout",
										  boost::asio::placeholders::error)
							);
		}

		// An authentification is complete
		// The protocol specifies no return message, so we're happy as long
		// as the authentification has been send and the socket is not closed.
		// If an error occurs, we print it and launch the resolving again.
		void SCOMSocketReader::_authenticationComplete(const std::string& error, const boost::system::error_code& ec)
		{
			// Stop the timer
			if (_timer)
			{
				_timer->cancel();
				delete _timer;
				_timer = NULL;
				_ios->run_one();
			}

			// If there is an error, print it
			// If the error is a cancelled timer, ignore
			if ( ( ! error.empty() || ec ) && ec != boost::asio::error::operation_aborted )
			{
				Log::GetInstance().warn("SCOM : Error while authenticating to SCOM : " + error);
				if (ec)
				{
					Log::GetInstance().warn("SCOM : Specific error : " + ec.message());
				}
				_close(true); // Will restart the process
				return;
			}

			// Everything is OK, continue with phase 4 : listening
			Log::GetInstance().info("SCOM : Succesfully authenticated");
			_dataReceived(boost::system::error_code(), 0);

			// IDEA (noba) : A watchdog to restart the connection in case of prolonged inactivity might be a good idea
		}

		// Reads the data fetched from the socket and stores it in the SCOMData
		// On error, restarts the connection
		// On each read a timeout is created that will
		// Call itselfs with async_read
		void SCOMSocketReader::_dataReceived (const boost::system::error_code& error, std::size_t size)
		{
			// On error, display it and restart the connection
			if (error)
			{
				Log::GetInstance().warn("SCOM : Error while reading : " + error.message());
				_close(true);
				return;
			}

			// If the size is bigger than 0, read the message
			if (size > 0)
			{
				// Copy to string
				std::string chunk(_buffer.begin(), _buffer.begin()+size);

				// Search for one or multiple end of file flags (\0)
				size_t pos = 0, lastpos = 0;
				while ( (pos = chunk.find('\0',lastpos)) != std::string::npos )
				{
					// If found, add the part before the flag to the XML and send it to SCOMData
					_xml += chunk.substr(lastpos,pos-lastpos);
					_dataHandler->AddXML(_xml);
					_xml.clear();
					lastpos = pos+1;
				}

				// The rest is added
				_xml = chunk.substr(lastpos,std::string::npos);
			}

			// Start an async read on ourselves
			_socket->async_receive(buffer(_buffer),
								   boost::bind(&synthese::scom::SCOMSocketReader::_dataReceived,
											   this,
											   _1,
											   _2)
								   );
		}
	}
}
