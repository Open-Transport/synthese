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
#include "Settings.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <sstream>

namespace synthese
{
	namespace scom
	{

		using namespace boost::asio;
		using namespace synthese::util;
		using namespace synthese::settings;

		// Settings names
		const std::string SCOMSocketReader::SETTING_ENABLED = "enabled";
		const std::string SCOMSocketReader::SETTING_RESOLVERETRY = "resolveretry";
		const std::string SCOMSocketReader::SETTING_CONNECTRETRY = "connectretry";
		const std::string SCOMSocketReader::SETTING_SERVER = "serverip";
		const std::string SCOMSocketReader::SETTING_PORT = "serverport";
		const std::string SCOMSocketReader::SETTING_ID = "serverid";
		const std::string SCOMSocketReader::SETTING_BORNES = "bornes";
		const std::string SCOMSocketReader::SETTING_CONNECTTIMEOUT = "connecttimeout";
		const std::string SCOMSocketReader::SETTING_READTIMEOUT = "readtimeout";
		const std::string SCOMSocketReader::SETTING_FOLLOWPORT = "followport";

		// Module name for the settings
		const std::string SCOMSocketReader::SETTINGS_MODULE = "SCOMSocketReader";

		// Setup some settings
		// Every setting setup in hard here will be parameters in the end
		// (At least I hope... noba)
		SCOMSocketReader::SCOMSocketReader (SCOMData *dataHandler) :
			_dataHandler(dataHandler),
			_enabled(false),
			_followSocket(NULL),
			_followAcceptor(NULL),
			_followHasClient(false),
			_socket(NULL),
			_timer(NULL),
			_state(RESOLVE),
			_next(RESOLVE),
			_idpos(0)
		{
			// Fetch the settings
			_enabled = Settings::GetInstance().Init<bool>(SETTINGS_MODULE, SETTING_ENABLED,false);
			_resolveRetry = Settings::GetInstance().Init<int>(SETTINGS_MODULE, SETTING_RESOLVERETRY,20);
			_connectRetry = Settings::GetInstance().Init<int>(SETTINGS_MODULE, SETTING_CONNECTRETRY,10);
			_server = Settings::GetInstance().Init<std::string>(SETTINGS_MODULE, SETTING_SERVER,"127.0.0.1");
			_port = Settings::GetInstance().Init<int>(SETTINGS_MODULE, SETTING_PORT,3106);
			_followPort = Settings::GetInstance().Init<int>(SETTINGS_MODULE, SETTING_FOLLOWPORT,5555);
			
			// Timeouts
			// Needs to be set for each state
			_timeouts[RESOLVE] = 0;
			_timeouts[CONNECT] = Settings::GetInstance().Init<int>(SETTINGS_MODULE, SETTING_CONNECTTIMEOUT,5);
			_timeouts[AUTHENTICATE] = 0;
			_timeouts[READ] = Settings::GetInstance().Init<int>(SETTINGS_MODULE, SETTING_READTIMEOUT,120);
			_timeouts[CLOSE] = 0;
			
			// Bornes
			std::string bornes = Settings::GetInstance().Init<std::string>(SETTINGS_MODULE, SETTING_BORNES);
			boost::algorithm::split(_bornes,bornes,boost::algorithm::is_any_of(","));

			// IDs
			std::string ids = Settings::GetInstance().Init<std::string>(SETTINGS_MODULE, SETTING_ID,"1");
			boost::algorithm::split(_ids,ids,boost::algorithm::is_any_of(","));
			
			// Register for the settings changes
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_ENABLED, this);
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_RESOLVERETRY, this);
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_CONNECTRETRY, this);
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_SERVER, this);
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_PORT, this);
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_ID, this);
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_BORNES, this);
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_CONNECTTIMEOUT, this);
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_READTIMEOUT, this);
			Settings::GetInstance().Register(SETTINGS_MODULE, SETTING_FOLLOWPORT, this);

			// State names, used for errors
			_stateName[RESOLVE] = "resolving";
			_stateName[CONNECT] = "connecting";
			_stateName[AUTHENTICATE] = "authenticating";
			_stateName[READ] = "reading";
			_stateName[CLOSE] = "closing";

			// Boost's input/output service
			_ios = new io_service();
			_followIos = new io_service();

			// By default this one should be locked
			_mutexDisable.lock();
		}


		SCOMSocketReader::~SCOMSocketReader ()
		{
			// Unregister us from the settings
			Settings::GetInstance().Unregister(this,SETTINGS_MODULE, SETTING_RESOLVERETRY);
			Settings::GetInstance().Unregister(this,SETTINGS_MODULE, SETTING_CONNECTRETRY);
			Settings::GetInstance().Unregister(this,SETTINGS_MODULE, SETTING_SERVER);
			Settings::GetInstance().Unregister(this,SETTINGS_MODULE, SETTING_PORT);
			Settings::GetInstance().Unregister(this,SETTINGS_MODULE, SETTING_ID);
			Settings::GetInstance().Unregister(this,SETTINGS_MODULE, SETTING_BORNES);
			Settings::GetInstance().Unregister(this,SETTINGS_MODULE, SETTING_CONNECTTIMEOUT);
			Settings::GetInstance().Unregister(this,SETTINGS_MODULE, SETTING_READTIMEOUT);
		}


		// Start the thread
		void SCOMSocketReader::Start ()
		{
			_ios->reset();
			_followIos->reset();

			// Launch the first async event before the loop (or it will quit directly)
			// The timer to 0 makes the _mainLoop() called from the _ios loop, not the
			// main thread
			_next = RESOLVE;
			deadline_timer timer(*_ios,boost::posix_time::seconds(0));
			timer.async_wait( boost::bind(&synthese::scom::SCOMSocketReader::_mainLoop, this, "", boost::system::error_code()));

			// If the io_service is out-of-work, it will stops.
			// We need it to continue so we add this object in its queue
			_work = new boost::asio::io_service::work(*_ios);
			_followWork = new boost::asio::io_service::work(*_followIos);

			// Run the boost ASIO main loop (for async events)
			_thread = server::ServerModule::AddThread(
				boost::bind(&io_service::run,_ios),
				"SCOM Listener thread"
			);

			// Run the boost ASIO main loop for the follow socket (for async events)
			_followThread = server::ServerModule::AddThread(
				boost::bind(&io_service::run,_followIos),
				"SCOM Follower thread"
			);
		}

		// Close the socket, stop the main loop and stop the background thread
		void SCOMSocketReader::Stop ()
		{
			_next = STOP;
            Log::GetInstance().debug("SCOM : STOP ");
			_close();
            Log::GetInstance().debug("SCOM : Close ");
            _ios->stop();
            Log::GetInstance().debug("SCOM : ios stop ");
            _followIos->stop();
            Log::GetInstance().debug("SCOM : follow join ");
			_thread->interrupt();
            Log::GetInstance().debug("SCOM : interrupt ");
			_followThread->interrupt();
            Log::GetInstance().debug("SCOM : follow interrupt ");
			// Isn't there a function from server::ServerModule to remove the thread?
		}

		// Will try to resolv the server address
		// Should work with IP adresses too
		bool SCOMSocketReader::_resolv()
		{
			boost::system::error_code error;

			// Resolve the server address (in case of a FQDN)
            Log::GetInstance().debug("SCOM : Resolving " + _server);
			ip::tcp::resolver resolver(*_ios);
			ip::tcp::resolver::query query(_server,"");
			ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, error);
			ip::tcp::resolver::iterator end;

			// If nothing is found, print an error and return an invalid endpoint (empty)
			if (endpoint_iterator == end || error)
			{
				Log::GetInstance().warn("SCOM : Error while connecting to the server : invalid server : " + _server);

				if (error)
				{
					Log::GetInstance().warn("SCOM : Specific error : " + error.message());
				}

				return false;
			}

			// Build the IP address from the first ipv4 endpoint
			// Use the first point by default if no IPV4 entry is found
			_endpoint = *endpoint_iterator;
			while (endpoint_iterator != end)
			{
				if ( endpoint_iterator->endpoint().address().is_v4() )
				{
					_endpoint = *endpoint_iterator;
					break;
				}
				endpoint_iterator++;
			}

			// Set the port
			_endpoint.port(_port);

			return true;
		}

		// Setup an async connection to the SCOM server
		void SCOMSocketReader::_connect()
		{
			// Async connection to _connectionComplete
			Log::GetInstance().debug("SCOM : Attempt to connect to " + _endpoint.address().to_string() +
									 " port " + boost::lexical_cast<std::string>(_endpoint.port()));
			_socket = new ip::tcp::socket(*_ios);
			_socket->async_connect(
				_endpoint,
				boost::bind(&synthese::scom::SCOMSocketReader::_mainLoop,
							this,
							"", // No error
							boost::asio::placeholders::error)
			);

			// If the follow socket port is specified, we open it
			if (_followPort)
			{
				Log::GetInstance().debug("SCOM : Opening the follow socket on port " + boost::lexical_cast<std::string>(_followPort));
				_followSocket = new ip::tcp::socket(*_followIos);
				_followAcceptor = new ip::tcp::acceptor(*_followIos, ip::tcp::endpoint(ip::tcp::v4(), _followPort));
				_followAcceptor->async_accept(
					*_followSocket,
					boost::bind(&synthese::scom::SCOMSocketReader::_followAccept,
								this,
								boost::asio::placeholders::error)
				);
			}
		}

		// Close and delete the socket
		void SCOMSocketReader::_close()
		{
			boost::system::error_code error;

			// SCOM socket
			if (_socket)
			{
				Log::GetInstance().debug("SCOM : Closing the SCOM socket");
				_socket->close(error);
				if (error)
				{
					Log::GetInstance().warn("SCOM : Error while closing socket : " + error.message());
				}

				delete _socket;
				_socket = NULL;
			}

			// Follow socket
			if (_followSocket)
			{
				Log::GetInstance().debug("SCOM : Closing the follow socket");
				_followSocket->close(error);
				_followAcceptor->close();
				_followIos->reset();
				if (error)
				{
					Log::GetInstance().warn("SCOM : Error while closing the follow socket : " + error.message());
				}

				delete _followAcceptor;
				_followAcceptor = NULL;

				delete _followSocket;
				_followSocket = NULL;
			}
		}

		// Send the authentication XML to the SCOM server
		// The protocol specifies no return message, so we're happy as long
		// as the authentification has been send and the socket is not closed.
		// The id used will loop through the possibilities offered by _ids (each call use a new one)
		void SCOMSocketReader::_authenticate()
		{
			// Fetch the ID to use, and skip to the next one (loop)
			std::string id = _ids.at(_idpos);
			_idpos = (_idpos + 1) % _ids.size();

			Log::GetInstance().debug("SCOM : Sending authentication with ID " + id);

			// Generate the XML identification message
			// (Note : lf seems to be deleted by msg, so do not delete it yourself)
			std::ostringstream msg;
			boost::posix_time::time_facet* lf = new boost::posix_time::time_facet("DATE=\"%d/%m/%Y\" HEURE=\"%H:%M:%S\"");
			const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
			msg.imbue(std::locale(msg.getloc(),lf));
			msg << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>" << std::endl;
			msg << "<IDENT>" << std::endl;
			msg << "   <CLIENT ID=\"" << id << "\" " << now << "/>" << std::endl;
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
				boost::bind(&synthese::scom::SCOMSocketReader::_mainLoop,
							this,
							"",
							boost::asio::placeholders::error)
			);
		}

		// Reads the data fetched from the socket and stores it in the SCOMData
		// Call the main loop with the error in parameter
		void SCOMSocketReader::_dataReceived (const boost::system::error_code& error, std::size_t size)
		{
			// If the size is bigger than 0 and there is no error, read the message
			if (size > 0 && ! error)
			{
				// Copy to string
				std::string chunk(_buffer.begin(), _buffer.begin()+size);

				// If the follow socket is open, forward the data
				if (_followSocket && _followHasClient)
				{
					async_write(
						*_followSocket,
						buffer(chunk),
						boost::bind(&synthese::scom::SCOMSocketReader::_followDataSent,
									this,
									_1,
									_2)
					);
				}

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

			// Call the main loop
			_mainLoop("",error);
		}

		// Main loop for the state machine
		// For each state, execute some action then go to the next step and start again
		void SCOMSocketReader::_mainLoop(const std::string& error, const boost::system::error_code& ec)
		{
			// If the error is an operation aborted, ignore and quit
			if (ec && ec == boost::asio::error::operation_aborted)
			{
				//Log::GetInstance().debug("Aborted timeout for " + _stateName[_state]);
				return;
			}

			// If an error occured, log it and go to the closing state
			if ( ! error.empty() || ec )
			{
				Log::GetInstance().warn("SCOM : Error while in " + _stateName[_state] + " state : " + error + " " + ec.message());
				_next = CLOSE;
			}

			// Go to next step
			_state = _next;

			// Remove the timer (if any)
			if (_timer)
			{
				_timer->cancel();
				delete _timer;
				_timer = NULL;
			}

			// Timeout for this specific operation
			int timeout = _timeouts[_state];

			// If the timeout is bigger than 0 and we are enabled, setup a timer
			if (timeout > 0 && _enabled)
			{
				//Log::GetInstance().debug("New timeout for " + _stateName[_state]);
				_timer = new deadline_timer(*_ios);
				_timer->expires_from_now(boost::posix_time::seconds(timeout));
				_timer->async_wait( boost::bind(&synthese::scom::SCOMSocketReader::_mainLoop,
												this,
												"Timeout",
												boost::asio::placeholders::error)
									);
			}

			// State machine
			_mutex.lock();
			switch (_state)
			{
				// Try to resolve the SCOM server IP or FQDN
				// If we are disabled, wait for us to be re-enabled
				// On fail, start again
				// On success, go to CONNECT state
				case RESOLVE :
				{
					// If we are disabled, wait on re-enabling
					if ( ! _enabled )
					{
						Log::GetInstance().info("SCOM : Service disabled");
						_mutexDisable.lock();
						Log::GetInstance().info("SCOM : Service enabled");
					}

					// If the address is invalid, wait a moment and try again (no state change)
					if ( ! _resolv() )
					{
						deadline_timer timer(*_ios,boost::posix_time::seconds(_resolveRetry));
						timer.wait();
						_next = RESOLVE;
					}
					else // Go to the connected state
					{
						_next = CONNECT;
					}

					_mutex.unlock();
					_mainLoop("",boost::system::error_code());

					break;
				}

				// Connect to the socket
				// Go to AUTHENTICATE state in any case.
				// If an error or timeout occurs, it will be handled at the beginning of the main loop
				case CONNECT :
				{
					_connect();
					_next = AUTHENTICATE;
					break;
				}

				// Authenticate to the server
				// Go to READ state in any case, except for a socket error, nothing is waited in return
				case AUTHENTICATE :
				{
					_authenticate();
					_next = READ;
					break;
				}

				// Start an async read of the socket
				// If an error occurs, it will be handled at the beginning of the main loop
				// The _dataReceived function calls itself the main loop with the error (if any)
				case READ :
				{
					_socket->async_receive(buffer(_buffer),
						boost::bind(&synthese::scom::SCOMSocketReader::_dataReceived,
									this,
									_1,
									_2)
					);
					break;
				}

				// Close the socket and start the connection again
				case CLOSE :
				{
					_close();
					deadline_timer timerClose(*_ios,boost::posix_time::seconds(_connectRetry));
					timerClose.wait();
					_next = RESOLVE;
					_mutex.unlock();

					_mainLoop("", boost::system::error_code());

					break;
				}

				// Do nothing
				case STOP :
				{
					_next = STOP;
					break;
				}

				default :
				{
					break;
				}
			}
			_mutex.unlock();
		}


		// For some of the settings (server, port, id, bornes) we must restart
		// For the rest, just save them
		void SCOMSocketReader::ValueUpdated (
			const std::string& module,
			const std::string& name,
			const std::string& value,
			bool notify
		)
		{
			// If not from us, goodbye
			if (module == SETTINGS_MODULE)
			{
				// Save the values
				if (name == SETTING_SERVER)
				{
					_server = Settings::GetInstance().Get<std::string>(SETTINGS_MODULE, SETTING_SERVER,_server);
				}
				else if (name == SETTING_BORNES)
				{
					_bornes.clear();
					std::string bornes = Settings::GetInstance().Get<std::string>(SETTINGS_MODULE, SETTING_BORNES,"");
					boost::algorithm::split(_bornes,bornes,boost::algorithm::is_any_of(","));
				}
				else if (name == SETTING_CONNECTRETRY)
				{
					_connectRetry = Settings::GetInstance().Get<int>(SETTINGS_MODULE, SETTING_CONNECTRETRY,_connectRetry);
				}
				else if (name == SETTING_CONNECTTIMEOUT)
				{
					_timeouts[CONNECT] = Settings::GetInstance().Get<int>(SETTINGS_MODULE, SETTING_CONNECTTIMEOUT,_timeouts[CONNECT]);
				}
				else if (name == SETTING_ID)
				{
					_ids.clear();
					std::string ids = Settings::GetInstance().Get<std::string>(SETTINGS_MODULE, SETTING_ID,"");
					boost::algorithm::split(_ids,ids,boost::algorithm::is_any_of(","));
				}
				else if (name == SETTING_PORT)
				{
					_port = Settings::GetInstance().Get<int>(SETTINGS_MODULE, SETTING_PORT,_port);
				}
				else if (name == SETTING_READTIMEOUT)
				{
					_timeouts[READ] = Settings::GetInstance().Get<int>(SETTINGS_MODULE, SETTING_READTIMEOUT,_timeouts[READ]);
				}
				else if (name == SETTING_RESOLVERETRY)
				{
					_resolveRetry = Settings::GetInstance().Get<int>(SETTINGS_MODULE, SETTING_RESOLVERETRY,_resolveRetry);
				}
				else if (name == SETTING_FOLLOWPORT)
				{
					_followPort = Settings::GetInstance().Get<int>(SETTINGS_MODULE, SETTING_FOLLOWPORT,_followPort);
				}
				else if (name == SETTING_ENABLED)
				{
					_enabled = Settings::GetInstance().Get<bool>(SETTINGS_MODULE, SETTING_ENABLED,_enabled);

					// If enabled, unlock the main loop
					if (_enabled)
					{
						_mutexDisable.unlock();
					}
				}
				else
				{
					Log::GetInstance().error("SCOMSocketReader : invalid setting : " + name);
					return;
				}

				Log::GetInstance().debug("SCOMSocketReader : setting " + name + " to " + value);

				// If the value changed is one of these and the notify flag is true, restart the server
				if (
					(name == SETTING_SERVER ||
					 name == SETTING_PORT ||
					 name == SETTING_ID ||
					 name == SETTING_BORNES ||
					 name == SETTING_FOLLOWPORT ||
					 name == SETTING_ENABLED ) &&
					notify
				)
				{
					Log::GetInstance().debug("SCOMSocketReader : reloading the service");

					_mutex.lock();

					_next = CLOSE;

					_mutex.unlock();
				}
			}
		}


		// If an error occurs, print it and start accepting again
		void SCOMSocketReader::_followAccept(const boost::system::error_code& code)
		{
			if (code)
			{
				_followHasClient = false;

				// If the code is "operation canceled", we ignore it
				if (code != boost::asio::error::operation_aborted)
				{
					Log::GetInstance().debug("SCOM : follow socket error on accept : " + code.message());

					_followSocket->close();

					_followAcceptor->async_accept(
						*_followSocket,
						boost::bind(&synthese::scom::SCOMSocketReader::_followAccept,
									this,
									boost::asio::placeholders::error)
					);
				}
			}
			else
			{
				Log::GetInstance().debug("SCOM : accepted client " + _followSocket->remote_endpoint().address().to_string() + " on follow socket");

				_followHasClient = true;
			}
		}


		// If an error has occured, print it and reset the follow socket
		void SCOMSocketReader::_followDataSent(const boost::system::error_code& code, std::size_t)
		{
			if (code)
			{
				_followHasClient = false;

				// If the code is "operation canceled", we ignore it
				if (code != boost::asio::error::operation_aborted)
				{
					Log::GetInstance().debug("SCOM : follow socket error on send : " + code.message());

					// Close the socket
					_followSocket->close();

					// Reset the queue
					_followIos->reset();

					// Accept a new client
					_followAcceptor->async_accept(
						*_followSocket,
						boost::bind(&synthese::scom::SCOMSocketReader::_followAccept,
									this,
									boost::asio::placeholders::error)
					);
				}
			}
			else
			{
				Log::GetInstance().debug("SCOM : follow sent");
			}
		}
	}
}
