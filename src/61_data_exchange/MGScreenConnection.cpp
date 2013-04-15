
/** MGScreenConnection class implementation.
	@file MGScreenConnection.cpp

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

#include "MGScreenConnection.hpp"

#include "Env.h"
#include "Exception.h"
#include "Log.h"
#include "CurrentJourney.hpp"
#include "ServerModule.h"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::gregorian;
using namespace boost::lambda;
using namespace boost::posix_time;
using namespace std;


namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace pt;
	using namespace server;
	using namespace util;
	using namespace vehicle;


	namespace data_exchange
	{
		const string MGScreenConnection::MODULE_PARAM_MG_SCREEN_HOST = "mg_screen_host";
		const string MGScreenConnection::MODULE_PARAM_MG_SCREEN_PORT = "mg_screen_port";
		const string MGScreenConnection::MODULE_PARAM_MG_SCREEN_SPEED = "mg_screen_speed";
		const string MGScreenConnection::MODULE_PARAM_MG_SCREEN_VALUE = "mg_screen_value";

		boost::shared_ptr<MGScreenConnection> MGScreenConnection::_theConnection(new MGScreenConnection);


		MGScreenConnection::MGScreenConnection(
		):	_mgScreenSpeed(0), // Will be initialized by the --params
			_mgScreenValue(0),
			_status(offline),
			_io_service(),
			_deadline(_io_service),
			_socket(_io_service)
		{
			// No deadline is required until the first socket operation is started. We
			// set the deadline to positive infinity so that the actor takes no action
			// until a specific deadline is set.
			_deadline.expires_at(boost::posix_time::pos_infin);

			// Start the persistent actor that checks for deadline expiry.
			checkDeadline();

		}



		void MGScreenConnection::establishConnection()
		{
			// Attempt a connection
			// Get a list of endpoints corresponding to the server name.
			tcp::resolver resolver(_io_service);
			tcp::resolver::query query(_mgScreenAddress, _mgScreenPort);
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			tcp::resolver::iterator end;
			_buf.reset(new boost::asio::streambuf);

			// Try each endpoint until we successfully establish a connection.
			boost::system::error_code error = boost::asio::error::host_not_found;
			while (error && endpoint_iterator != end)
			{
				_socket.close();
				_socket.connect(*endpoint_iterator++, error);
			}
			if (error)
			{
				throw boost::system::system_error(error);
			}

			if(_status == connect)
			{
				_status = online;
			}
		}



		void MGScreenConnection::read()
		{
			if(!_buf.get())
			{
				throw Exception("Buffer is null");
			}

			// Set a deadline for the asynchronous operation. Since this function uses
			// a composed operation (async_read_until), the deadline applies to the
			// entire operation, rather than individual reads from the socket.
			_deadline.expires_from_now(minutes(1));

			// Set up the variable that receives the result of the asynchronous
			// operation. The error code is set to would_block to signal that the
			// operation is incomplete. Asio guarantees that its asynchronous
			// operations will never fail with would_block, so any other value in
			// ec indicates completion.
			boost::system::error_code ec = boost::asio::error::would_block;

			// Start the asynchronous operation itself. The boost::lambda function
			// object is used as a callback and will update the ec variable when the
			// operation completes. The blocking_udp_client.cpp example shows how you
			// can use boost::bind rather than boost::lambda.
			boost::asio::async_read_until(_socket, *_buf, "} }", var(ec) = lambda::_1);

			// Block until the asynchronous operation has completed.
			do _io_service.run_one(); while (ec == boost::asio::error::would_block);

			if (ec)
			{
				throw boost::system::system_error(ec);
			}
		}



		void MGScreenConnection::checkDeadline()
		{
			// Check whether the deadline has passed. We compare the deadline against
			// the current time since a new asynchronous operation may have moved the
			// deadline before this actor had a chance to run.
			if (_deadline.expires_at() <= deadline_timer::traits_type::now())
			{
				// The deadline has passed. The socket is closed so that any outstanding
				// asynchronous operations are cancelled. This allows the blocked
				// connect(), read_line() or write_line() functions to return.
				_socket.close();

				// There is no longer an active deadline. The expiry is set to positive
				// infinity so that the actor takes no action until a new deadline is set.
				_deadline.expires_at(boost::posix_time::pos_infin);
			}

			// Put the actor back to sleep.
			_deadline.async_wait(bind(&MGScreenConnection::checkDeadline, this));
		}


		void MGScreenConnection::RunThread()
		{
			// Main loop (never ends)
			while(true)
			{
				if(	_theConnection->_status == online ||
					_theConnection->_status == connect
				){
					try
					{
						ServerModule::SetCurrentThreadRunningAction();

						_theConnection->establishConnection();

						_theConnection->registerData();

						while(true)
						{
							if(_theConnection->_status == offline ||
								_theConnection->_status == connect
							){
								break;
							}

							// Read until eof
							_theConnection->read();

							_theConnection->handleData();
						}
					}
					catch(std::exception& e)
					{
						util::Log::GetInstance().info(
							e.what()
						);
					}
					catch(thread_interrupted)
					{
						throw thread_interrupted();
					}
					catch(...)
					{

					}
				}

				// TBD We lost the connection, what should we do?

				// Wait 30 s
				ServerModule::SetCurrentThreadWaiting();
				this_thread::sleep(seconds(30));
			}
		}


		void MGScreenConnection::ParameterCallback( const std::string& name, const std::string& value )
		{
			// Host and port
			bool changed(false);
			if(name == MODULE_PARAM_MG_SCREEN_HOST)
			{
				changed = (_theConnection->_mgScreenAddress != value);
				_theConnection->_mgScreenAddress = value;
			}
			else if(name == MODULE_PARAM_MG_SCREEN_PORT)
			{
				changed = (_theConnection->_mgScreenPort != value);
				_theConnection->_mgScreenPort = value;
			}

			if(	changed
			){
				if(	!_theConnection->_mgScreenAddress.empty() &&
					!_theConnection->_mgScreenPort.empty()
				){
					_theConnection->_status = connect;
				}
				else
				{
					_theConnection->_status = offline;
				}
			}

			if(name == MODULE_PARAM_MG_SCREEN_SPEED)
			{
				_theConnection->_mgScreenSpeed = lexical_cast<int>(value);
			}

			if(name == MODULE_PARAM_MG_SCREEN_VALUE)
			{
				_theConnection->_mgScreenValue = lexical_cast<int>(value);
			}
		}

		void MGScreenConnection::registerData(
		) const	{
			stringstream reply;
			reply << "{ \"jsonrpc\": \"2.0\", \"id\": \"1\", \"method\": "
					 "\"registerObject\", \"params\": [\"InfovisionDisplayState\" ] }\n";

			reply << "{ \"jsonrpc\": \"2.0\", \"id\": \"1\", \"method\": "
					 "\"registerObject\", \"params\": [ \"InfovisionSystemState\" ] }\n";

			reply << "{ \"jsonrpc\": \"2.0\", \"id\": \"1\", \"method\": "
					 "\"registerObject\", \"params\": [ \"InfovisionInputState\" ] }\n";

			cout << reply.str() << endl;
			boost::asio::write(_socket, boost::asio::buffer(reply.str()));
		}

		void MGScreenConnection::displaySetBacklightParams(int min, int max, int speed) const
		{
			BOOST_FOREACH(VehicleModule::VehicleScreensMap::value_type screen,
						  VehicleModule::GetVehicleScreens())
			{
				stringstream reply;
				reply << "{ \"jsonrpc\": \"2.0\", \"id\": \"1\", \"method\": "
						 "\"displaySetBacklightParams\", \"params\": { "
						 "\"Address\": " << screen.second.getName() << ", " <<
						 "\"PanelNo\": 0, "
						 "\"Minimum\": " << min << ", " <<
						 "\"Maximum\": " << max << ", " <<
						 "\"Speed\": " << speed << " " <<
						 "} }\n";
				cout << reply.str() << endl;
				boost::asio::write(_socket, boost::asio::buffer(reply.str()));
			}
		}

		void MGScreenConnection::displaySetBacklightValue(int value) const
		{
			BOOST_FOREACH(VehicleModule::VehicleScreensMap::value_type screen,
						  VehicleModule::GetVehicleScreens())
			{
				stringstream reply;
				reply << "{ \"jsonrpc\": \"2.0\", \"id\": \"1\", \"method\": "
						 "\"displaySetBacklightValue\", \"params\": { "
						 "\"Address\": " << screen.second.getName() << ", " <<
						 "\"PanelNo\": 0, "
						 "\"BacklightValue\": " << value << " " <<
						 "} }\n";
				cout << reply.str() << endl;
				boost::asio::write(_socket, boost::asio::buffer(reply.str()));
			}
		}

		void MGScreenConnection::handleData(
		) const	{
			static bool initialized(false);

			// Copy the content obtained into a string
			string bufStr;
			istream is(_buf.get());
			getline(is, bufStr, char(0));
			if(bufStr.empty())
			{
				return;
			}

			// the json string is not accepted by our json parser because
			// it contains \/ (backquote slash)
			// here we fix that.
			size_t pos = 0;
			string badChar("\\/");
			while ((pos = bufStr.find(badChar, pos)) != std::string::npos) {
				bufStr.replace(pos, badChar.length(), "/");
				pos += 1;
			}

			// Log the input
			if(Log::GetInstance().getLevel() <= Log::LEVEL_DEBUG)
			{
				util::Log::GetInstance().info(
					bufStr
				);
			}

			boost::property_tree::ptree pt;
			istringstream ss(bufStr);
			boost::property_tree::read_json(ss, pt);

			try
			{
				string responseType(pt.get<string>("params.objectName"));
				if(responseType == "InfovisionDisplayState")
				{
					BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("params.Display"))
					{
						VehicleScreen &screen(VehicleModule::GetVehicleScreen(v.second.get<string>("Address")));
						screen.setName(v.second.get<string>("Address"));
						screen.setConnected( v.second.get<string>("ConnectionState") == "connected" ? true : false);
						screen.setBacklight1_OK(v.second.get<bool>("BacklightInternal_1_OK"));
						screen.setBacklight2_OK(v.second.get<bool>("BacklightInternal_2_OK"));

						BOOST_FOREACH(boost::property_tree::ptree::value_type &panel, v.second.get_child("Panel"))
						{
							screen.setBacklightValue(panel.second.get<int>("BacklightValue"));
							screen.setBacklightMin(panel.second.get<int>("BacklightMin"));
							screen.setBacklightMax(panel.second.get<int>("BacklightMax"));
							screen.setBacklightSpeed(panel.second.get<int>("BacklightSpeed"));
						}
						screen.setBacklightAutomaticMode( screen.getBacklightValue() == -1 ? true : false);
					}

					if(!initialized)
					{
						displaySetBacklightParams(1, 255, _mgScreenSpeed);
						initialized = true;
					}

				}
				else if(responseType == "InfovisionSystemState")
				{
					VehicleModule::SetExtraParameter("InfovisionSystemState.Serial", pt.get<string>("params.Serial"));
					VehicleModule::SetExtraParameter("InfovisionSystemState.HWRef", pt.get<string>("params.HWRef"));
					VehicleModule::SetExtraParameter("InfovisionSystemState.At91Version", pt.get<string>("params.At91Version"));
					VehicleModule::SetExtraParameter("InfovisionSystemState.At91Rev", pt.get<string>("params.At91Rev"));
				}
				else if(responseType == "InfovisionInputState")
				{
					VehicleModule::setIgnition(pt.get<bool>("params.Ignition"));
					if(VehicleModule::getIgnition())
					{
						displaySetBacklightValue(-1);
					}
					else
					{
						displaySetBacklightValue(_mgScreenValue);
					}
				}
			}
			catch(std::exception& e)
			{

			}
		}
}	}


