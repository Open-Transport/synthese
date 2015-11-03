
/** IneoTerminusConnection class implementation.
	@file IneoTerminusConnection.cpp

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

#include "IneoTerminusConnection.hpp"

#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DataSourceTableSync.h"
#include "ImportableTableSync.hpp"
#include "IneoTerminusModule.hpp"
#include "Log.h"
#include "Request.h"
#include "ScenarioSaveAction.h"
#include "ScenarioStopAction.h"
#include "SentScenarioTableSync.h"
#include "ServerModule.h"
#include "TransportNetwork.h"
#include "XmlToolkit.h"
#include "NotificationProvider.hpp"
#include "IneoNotificationChannel.hpp"
#include "IneoTerminusLog.hpp"
#include "MessagesModule.h"
#include "IneoFileFormat.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <iomanip>

using namespace boost;
using namespace boost::algorithm;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace boost::system;
using namespace std;


namespace synthese
{
	using namespace impex;
	using namespace messages;
	using namespace pt;
	using namespace util;
	using namespace server;
	using namespace departure_boards;
	
	namespace ineo_terminus
	{
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PORT = "ineo_terminus_port";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_NETWORK = "ineo_terminus_network";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_DATASOURCE = "ineo_terminus_datasource";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_TICK_INTERVAL = "ineo_terminus_tick_interval";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_XSD_LOCATION = "ineo_terminus_xsd_location";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PASSENGER_FAKE_BROADCAST = "ineo_terminus_passenger_fake_broadcast";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_DRIVER_FAKE_BROADCAST = "ineo_terminus_driver_fake_broadcast";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PPDS_FAKE_BROADCAST = "ineo_terminus_ppds_fake_broadcast";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_GIROUETTE_FAKE_BROADCAST = "ineo_terminus_girouette_fake_broadcast";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_SONOPASSENGER_FAKE_BROADCAST = "ineo_terminus_sonopassenger_fake_broadcast";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_SONODRIVER_FAKE_BROADCAST = "ineo_terminus_sonodriver_fake_broadcast";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_SONOSTOPPOINT_FAKE_BROADCAST = "ineo_terminus_sonostoppoint_fake_broadcast";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_BIVGENERAL_FAKE_BROADCAST = "ineo_terminus_bivgeneral_fake_broadcast";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_BIVLINEAUTO_FAKE_BROADCAST = "ineo_terminus_bivlineauto_fake_broadcast";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_BIVLINEMAN_FAKE_BROADCAST = "ineo_terminus_bivlineman_fake_broadcast";
		const string IneoTerminusConnection::INEO_TERMINUS_XML_HEADER = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>";

		boost::shared_ptr<IneoTerminusConnection> IneoTerminusConnection::_theConnection(new IneoTerminusConnection);
		int IneoTerminusConnection::_idRequest(0);
		
		std::map<std::string, util::RegistryKeyType> IneoTerminusConnection::_fakeBroadcastPoints;
		std::set<std::string> IneoTerminusConnection::_creationRequestTags;
		std::set<std::string> IneoTerminusConnection::_deletionRequestTags;
		std::set<std::string> IneoTerminusConnection::_creationOrDeletionResponseTags;
		std::set<std::string> IneoTerminusConnection::_getStatesResponseTags;

		IneoTerminusConnection::IneoTerminusConnection(
		)
		{
			_ineoNetworkID = 0;
			_ineoDatasource = 0;
			_ineoTickInterval = 0;
			_ineoPort = "";
		}


		void IneoTerminusConnection::RunThread()
		{
			// Open the server socket
			boost::asio::io_service ioService;
			IneoTerminusConnection::tcp_server tcpServer(
				ioService,
				_theConnection->getIneoPort(),
				_theConnection->getIneoNetworkID(),
				_theConnection->getIneoDatasource()
			);
			ioService.run();
		}


		void IneoTerminusConnection::Initialize()
		{
			std::list<std::string> ineoMessageTypes;
			ineoMessageTypes.push_back("Passenger");
			ineoMessageTypes.push_back("Driver");
			ineoMessageTypes.push_back("Ppds");
			ineoMessageTypes.push_back("Girouette");
			ineoMessageTypes.push_back("SonoPassenger");
			ineoMessageTypes.push_back("SonoDriver");
			ineoMessageTypes.push_back("SonoStopPoint");
			ineoMessageTypes.push_back("BivGeneral");
			ineoMessageTypes.push_back("BivLineMan");
			ineoMessageTypes.push_back("BivLineAuto");

			BOOST_FOREACH(const std::string& ineoMessageType, ineoMessageTypes)
			{
				std::string creationRequestTag   = ineoMessageType + "CreateMessageRequest";
				std::string deletionRequestTag   = ineoMessageType + "DeleteMessageRequest";
				std::string creationResponseTag  = ineoMessageType + "CreateMessageResponse";
				std::string deletionResponseTag  = ineoMessageType + "DeleteMessageResponse";
				std::string getStatesResponseTag = ineoMessageType + "GetStatesResponse";

				// Those sets are used to decode the messages received from Ineo SAE
				_creationRequestTags.insert(creationRequestTag);
				_deletionRequestTags.insert(deletionRequestTag);
				_creationOrDeletionResponseTags.insert(creationResponseTag);
				_creationOrDeletionResponseTags.insert(deletionResponseTag);
				_getStatesResponseTags.insert(getStatesResponseTag);
			}
		}


		void IneoTerminusConnection::addConnection(IneoTerminusConnection::tcp_connection* new_connection)
		{
			boost::mutex::scoped_lock connectionsLock(_connectionsMutex);
			_livingConnections.push_back(new_connection);

			util::Log::GetInstance().info("New connection from Ineo SAE (count=" + boost::lexical_cast<std::string>(_livingConnections.size()) + ")");

			if(1 == _livingConnections.size())
			{
				// Upon first connection or reconnection from Ineo SAE, we need to synchronize the messages between the 2 systems
				synchronizeMessages();
			}
		}


		void IneoTerminusConnection::removeConnection(IneoTerminusConnection::tcp_connection* connection_to_remove)
		{
			boost::mutex::scoped_lock connectionsLock(_connectionsMutex);
			std::deque<IneoTerminusConnection::tcp_connection*>::iterator itConnections = _livingConnections.begin();

			for(; itConnections != _livingConnections.end(); itConnections++)
			{
				// Search for connection_to_remove and stop iterating when it is found
				if(*itConnections == connection_to_remove) break;
			}

			if(itConnections != _livingConnections.end())
			{
				// Remove connection_to_remove from list
				_livingConnections.erase(itConnections);
			}

			delete connection_to_remove;
			util::Log::GetInstance().info("Connection to Ineo SAE closed (count=" + boost::lexical_cast<std::string>(_livingConnections.size()) + ")");
		}


		void IneoTerminusConnection::setActiveConnection(tcp_connection* active_connection)
		{
			// Push active_connection as the first connection of _livingConnections, so that it will be used to send new messages to Ineo SAE
			boost::mutex::scoped_lock connectionsLock(_connectionsMutex);

			if(_livingConnections.front() != active_connection)
			{
				std::deque<IneoTerminusConnection::tcp_connection*>::iterator itConnections = _livingConnections.begin();

				for(; itConnections != _livingConnections.end(); itConnections++)
				{
					// Search for connection_to_remove and stop iterating when it is found
					if(*itConnections == active_connection) break;
				}

				if(itConnections != _livingConnections.end())
				{
					// Remove connection_to_remove from list
					_livingConnections.erase(itConnections);
				}

				_livingConnections.push_front(active_connection);
			}
		}


		void IneoTerminusConnection::addMessage(string new_message)
		{
			boost::recursive_mutex::scoped_lock lock(_messagesMutex);
			_messagesToSend.push_back(new_message);
		}


		void IneoTerminusConnection::MessageSender()
		{
			// On consomme ici les messages PUSH vers Ineo SAE
			// Main loop
			while (true)
			{
				GetTheConnection()->_sendMessage();
				// Sleep
				boost::this_thread::sleep(boost::posix_time::seconds(_theConnection->getIneoTickInterval()));
			}
		}


		void IneoTerminusConnection::_sendMessage()
		{
			boost::mutex::scoped_lock connectionsLock(_connectionsMutex);
			// If we have an active connection, we send the first message from the queue
			if (!_livingConnections.empty())
			{
				boost::recursive_mutex::scoped_lock messagesLock(_messagesMutex);
				if (!_messagesToSend.empty())
				{
					std::string firstMessage = _messagesToSend.front();
					(_livingConnections.front())->sendMessage(firstMessage);
					_messagesToSend.pop_front();

					util::Log::GetInstance().debug("Ineo Terminus : sending message " + firstMessage);
				}
			}
		}


		void IneoTerminusConnection::synchronizeMessages()
		{
			util::Log::GetInstance().info("Synchronizing messages with Ineo SAE");

			// Delete existing Ineo SAE messages
			if(0 != _ineoDatasource)
			{
				db::DBTransaction transaction;
				boost::shared_ptr<DataSource> ineoDataSource = Env::GetOfficialEnv().getEditable<DataSource>(_ineoDatasource);
				DataSource::LinkedObjects ineoSaeScenarios(ineoDataSource->getLinkedObjects<SentScenario>());

				util::Log::GetInstance().info("Deleting " + boost::lexical_cast<std::string>(ineoSaeScenarios.size()) + " Ineo SAE messages");

				BOOST_FOREACH(const DataSource::LinkedObjects::value_type& ineoSaeScenario, ineoSaeScenarios)
				{
					SentScenarioTableSync::Remove(NULL, ineoSaeScenario.second->getKey(), transaction, false);
				}

				transaction.run();
			}

			// Lock the message queue and empty it
			boost::recursive_mutex::scoped_lock messagesLock(_messagesMutex);
			_messagesToSend.clear();

			// For each Ineo Notification provider build a XXXGetStatesRequest to request messages from Ineo SAE
			BOOST_FOREACH(const NotificationProvider::Registry::value_type& providerEntry, Env::GetOfficialEnv().getRegistry<NotificationProvider>())
			{
				boost::shared_ptr<NotificationProvider> provider = providerEntry.second;

				if(IneoNotificationChannel::FACTORY_KEY == provider->get<NotificationChannelKey>())
				{
					const ParametersMap& parameters = provider->get<Parameters>();

					if(true == parameters.isDefined(IneoNotificationChannel::PARAMETER_INEO_MESSAGE_TYPE))
					{
						// Build XXXGetStatesRequest and queue it
						std::string ineoMessageType = parameters.getValue(IneoNotificationChannel::PARAMETER_INEO_MESSAGE_TYPE);
						std::string stateRequest = _buildGetStatesRequest(ineoMessageType);
						addMessage(stateRequest);
					}
				}
			}
		}


		const std::string IneoTerminusConnection::_buildGetStatesRequest(const std::string& ineoMessageType)
		{
			std::stringstream requestStream;

			// Build the request tag
			std::string requestTag = ineoMessageType + "GetStatesRequest";

			// Set the format of datetime objects to the format expected by Ineo
			boost::posix_time::ptime now = second_clock::local_time();
			std::stringstream timestampStream;
			timestampStream << setfill('0') << setw(2) << now.date().day() << "/"
							<< setfill('0') << setw(2) << int(now.date().month()) << "/"
							<< setfill('0') << setw(4) << now.date().year() << " "
							<< setfill('0') << setw(2) << now.time_of_day().hours() << ":"
							<< setfill('0') << setw(2) << now.time_of_day().minutes() << ":"
							<< setfill('0') << setw(2) << now.time_of_day().seconds();

			// Build the request header
			requestStream << INEO_TERMINUS_XML_HEADER << char(10);

			std::string xsdLocation = IneoTerminusConnection::GetTheConnection()->getIneoXSDLocation();
			requestStream << "<" << requestTag;
			if(false == xsdLocation.empty())
			{
				requestStream << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"" << xsdLocation << "\"";
			}
			requestStream << ">" << char(10);

			requestStream << "\t<ID>" << boost::lexical_cast<std::string>(getNextRequestID()) << "</ID>" << char(10);
			requestStream << "\t<RequestTimeStamp>" << timestampStream.str() << "</RequestTimeStamp>" << char(10);
			requestStream << "\t<RequestorRef>Terminus</RequestorRef>" << char(10);
			requestStream << "</" << requestTag << ">" << char(10);

			return requestStream.str();
		}


		XMLNode IneoTerminusConnection::ParseInput(
			const std::string& xml
		){
			XMLResults results;
			XMLNode allNode = XMLNode::parseString(xml.c_str(), NULL, &results);
			if (results.error != eXMLErrorNone)
			{
				throw Exception("IneoTerminusConnection : Invalid XML");
			}
			return allNode;
		}


		void IneoTerminusConnection::ParameterCallback( const std::string& name, const std::string& value )
		{
			// Port
			bool changed(false);
			if(name == MODULE_PARAM_INEO_TERMINUS_PORT)
			{
				changed = (_theConnection->_ineoPort != value);
				_theConnection->_ineoPort = value;
			}
			if(name == MODULE_PARAM_INEO_TERMINUS_NETWORK &&
				!value.empty())
			{
				changed = (_theConnection->_ineoNetworkID != lexical_cast<RegistryKeyType>(value));
				_theConnection->_ineoNetworkID = lexical_cast<RegistryKeyType>(value);
			}
			if(name == MODULE_PARAM_INEO_TERMINUS_DATASOURCE &&
				!value.empty())
			{
				changed = (_theConnection->_ineoDatasource != lexical_cast<RegistryKeyType>(value));
				_theConnection->_ineoDatasource = lexical_cast<RegistryKeyType>(value);
			}
			if (name == MODULE_PARAM_INEO_TERMINUS_TICK_INTERVAL &&
				!value.empty())
			{
				changed = (_theConnection->_ineoTickInterval != lexical_cast<int>(value));
				_theConnection ->_ineoTickInterval = lexical_cast<int>(value);
			}

			if(	changed
			){
				if(	_theConnection->_ineoPort.empty() ||
					_theConnection->_ineoNetworkID == 0 ||
					_theConnection->_ineoDatasource == 0 ||
					_theConnection->_ineoTickInterval == 0
				){
					_theConnection->_status = connect;
				}
				else
				{
					_theConnection->_status = offline;
				}
			}

			if (name == MODULE_PARAM_INEO_TERMINUS_XSD_LOCATION)
			{
				_theConnection ->_ineoXsdLocation = value;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_PASSENGER_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "Passenger";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_DRIVER_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "Driver";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_PPDS_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "Ppds";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_GIROUETTE_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "Girouette";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_SONOPASSENGER_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "SonoPassenger";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_SONODRIVER_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "SonoDriver";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_SONOSTOPPOINT_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "SonoStopPoint";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_BIVGENERAL_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "BivGeneral";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_BIVLINEMAN_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "BivLineMan";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}

			if(name == MODULE_PARAM_INEO_TERMINUS_BIVLINEAUTO_FAKE_BROADCAST)
			{
				std::string ineoMessageType = "BivLineAuto";
				RegistryKeyType registryKey = boost::lexical_cast<RegistryKeyType>(value);
				_fakeBroadcastPoints[ineoMessageType] = registryKey;
			}
		}


		int IneoTerminusConnection::getNextRequestID()
		{
			boost::mutex::scoped_lock lock (_requestIdsMutex);
			// Request identifiers are comprised between 1 and 65535
			_idRequest = (_idRequest % 65534) + 1;
			return _idRequest;
		}


		IneoTerminusConnection::tcp_connection::~tcp_connection()
		{
			boost::system::error_code errorCode;

			// Upon destruction of this tcp_connection, close its socket properly
			_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, errorCode);
			_socket.close(errorCode);
		}


		tcp::socket& IneoTerminusConnection::tcp_connection::socket()
		{
			return _socket;
		}


		void IneoTerminusConnection::tcp_connection::start()
		{
			_buf.reset(new boost::asio::streambuf);

			boost::asio::async_read_until(
				_socket,
				*_buf,
				char(0),
				boost::bind(
					&tcp_connection::handle_read,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred
			)	);
		}


		void IneoTerminusConnection::tcp_connection::handle_read(
			const boost::system::error_code& error,
			size_t bytes_transferred
		)
		{
			if (!error)
			{
				// The last connection which receives data is set as the currently "active" connection, and will be used to send messages to Ineo SAE
				GetTheConnection()->setActiveConnection(this);

				// Read data until '\0' is reached
				string bufStr;
				istream is(_buf.get());
				getline(is, bufStr, char(0));
				trim(bufStr);
				if(bufStr.empty())
				{
					return;
				}

				// Log for debug
				util::Log::GetInstance().debug("Ineo Terminus received : " + bufStr);

				size_t messageHeaderPosition = bufStr.find("?>");
				string messageHeader = (string::npos != messageHeaderPosition) ? bufStr.substr(0, messageHeaderPosition + 2) : "";
				boost::replace_all(messageHeader, "'", "\"");

				if (INEO_TERMINUS_XML_HEADER != messageHeader)
				{
					util::Log::GetInstance().warn("Ineo Terminus received a XML message with wrong header");
					string message("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><ResponseRef>Terminus</ResponseRef><ErrorType>ProtocolError</ErrorType><ErrorMessage>Syntaxe incorrecte</ErrorMessage><ErrorID>1</ErrorID>");
					util::IConv iconv("UTF-8","ISO-8859-1");
					message = iconv.convert(message);
					message += char(0);
					boost::asio::async_write(
						_socket,
						boost::asio::buffer(message),
						boost::bind(
							&tcp_connection::handle_write,
							this,
							boost::asio::placeholders::error
						)
					);
					boost::asio::async_read_until(
						_socket,
						*_buf,
						char(0),
						boost::bind(
							&tcp_connection::handle_read,
							this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
					)	);
					return;
				}
				bufStr = bufStr.substr(43, bufStr.size() - 43); // Remove XML header (<?xml ... ?>)

				// Parsing
				XMLNode node;

				try
				{
					node = ParseInput(bufStr);
				}

				catch(synthese::Exception&)
				{
					util::Log::GetInstance().warn("Ineo Terminus : received message is ill-formed");
					string message("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><ResponseRef>Terminus</ResponseRef><ErrorType>ProtocolError</ErrorType><ErrorMessage>Syntaxe incorrecte</ErrorMessage><ErrorID>1</ErrorID>");
					util::IConv iconv("UTF-8","ISO-8859-1");
					message = iconv.convert(message);
					message += char(0);
					boost::asio::async_write(
						_socket,
						boost::asio::buffer(message),
						boost::bind(
							&tcp_connection::handle_write,
							this,
							boost::asio::placeholders::error
						)
					);
					boost::asio::async_read_until(
						_socket,
						*_buf,
						char(0),
						boost::bind(
							&tcp_connection::handle_read,
							this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
					)	);
					return;
				}

				XMLNode childNode(node.getChildNode(0));
				if(childNode.isEmpty())
				{
					util::Log::GetInstance().warn("Ineo Terminus : Message XML vide sans node 0");
					string message("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><ResponseRef>Terminus</ResponseRef><ErrorType>ProtocolError</ErrorType><ErrorMessage>Syntaxe incorrecte</ErrorMessage><ErrorID>1</ErrorID>");
					util::IConv iconv("UTF-8","ISO-8859-1");
					message = iconv.convert(message);
					message += char(0);
					boost::asio::async_write(
						_socket,
						boost::asio::buffer(message),
						boost::bind(
							&tcp_connection::handle_write,
							this,
							boost::asio::placeholders::error
						)
					);
					boost::asio::async_read_until(
						_socket,
						*_buf,
						char(0),
						boost::bind(
							&tcp_connection::handle_read,
							this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred
					)	);
					return;
				}

				string tagName(childNode.getName());
				string response = "";

				if (tagName == "CheckStatusRequest")
				{
					_checkStatusRequest(childNode, response);
				}

				else if (_creationRequestTags.end() != _creationRequestTags.find(tagName))
				{
					_createMessageRequest(childNode, response);
				}

				else if (_deletionRequestTags.end() != _deletionRequestTags.find(tagName))
				{
					_deleteMessageRequest(childNode, response);
				}

				else if (_creationOrDeletionResponseTags.end() != _creationOrDeletionResponseTags.find(tagName))
				{
					// If message has 'ErrorType', 'ErrorMessage' and 'ErrorID' nodes then our request was rejected
					if (childNode.nChildNode("ErrorType") == 1)
					{
						util::Log::GetInstance().warn("Ineo Terminus error received in response : " + tagName);
						IneoTerminusLog::AddIneoTerminusErrorMessageEntry(childNode);
					}
				}

				else if (_getStatesResponseTags.end() != _getStatesResponseTags.find(tagName))
				{
					_getStatesResponse(childNode, response);
				}

				else
				{
					util::Log::GetInstance().warn("Ineo Terminus unsupported message : " + tagName);
					IneoTerminusLog::AddIneoTerminusErrorMessageEntry(childNode);
				}

				if(false == response.empty())
				{
					util::Log::GetInstance().debug("Ineo Terminus response : " + response);
					// This message requires a response, add the message delimiter and send the buffer
					response += char(0);
					boost::asio::async_write(
						_socket,
						boost::asio::buffer(response),
						boost::bind(
							&tcp_connection::handle_write,
							this,
							boost::asio::placeholders::error
						)
					);
				}

				boost::asio::async_read_until(
					_socket,
					*_buf,
					char(0),
					boost::bind(
						&tcp_connection::handle_read,
						this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred
				)	);
			}
			else
			{
				util::Log::GetInstance().debug("Ineo Terminus read error : " + error.message());
				IneoTerminusConnection::GetTheConnection()->removeConnection(this);
			}
		}


		void IneoTerminusConnection::tcp_connection::handle_write
		(
			const boost::system::error_code& error
		)
		{
			if (error)
			{
				util::Log::GetInstance().debug("Ineo Terminus write error : " + error.message());
				IneoTerminusConnection::GetTheConnection()->removeConnection(this);
			}
		}


		void IneoTerminusConnection::tcp_connection::sendMessage(
			string message
		)
		{
			// message is supposed to be in iso-8859-1
			message += char(0);
			boost::asio::async_write(
				_socket,
				boost::asio::buffer(message),
				boost::bind(
					&tcp_connection::handle_write,
					this,
					boost::asio::placeholders::error
			)	);
		}


		IneoTerminusConnection::tcp_server::tcp_server(
			boost::asio::io_service& ioService,
			string port,
			RegistryKeyType network_id,
			RegistryKeyType datasource_id
		):	_io_service(ioService),
			_network_id(network_id),
			_datasource_id(datasource_id),
			_acceptor(ioService)
		{
			// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
			std::string address("0.0.0.0");
			asio::ip::tcp::resolver resolver(_io_service);
			asio::ip::tcp::resolver::query query(address, port);
			asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

			_acceptor.open(endpoint.protocol());
			_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
			_acceptor.bind(endpoint);
			_acceptor.listen();

			start_accept();
		}


		void IneoTerminusConnection::tcp_server::start_accept()
		{
			IneoTerminusConnection::tcp_connection* new_connection = new IneoTerminusConnection::tcp_connection(_io_service, _network_id, _datasource_id);

			_acceptor.async_accept(
				new_connection->socket(),
				boost::bind(
					&IneoTerminusConnection::tcp_server::handle_accept,
					this,
					new_connection,
					boost::asio::placeholders::error
				)
			);
		}


		void IneoTerminusConnection::tcp_server::handle_accept(
			tcp_connection* new_connection,
			const boost::system::error_code& error
		)
		{
			if (!error)
			{
				new_connection->start();
				IneoTerminusConnection::GetTheConnection()->addConnection(new_connection);
			}
			else
			{
				delete new_connection;
			}

			start_accept();
		}


		IneoTerminusConnection::tcp_connection::Messaging::Messaging()
		{
			name = "";
			dispatching = Repete;
			startDate = boost::posix_time::not_a_date_time;
			stopDate = boost::posix_time::not_a_date_time;
			repeatPeriod = 0;
			inhibition = boost::logic::indeterminate;
			color = "";
			codeGirouette = 0;
			activateHeadJingle = boost::logic::indeterminate;
			activateBackJingle = boost::logic::indeterminate;
			confirm = boost::logic::indeterminate;
			startStopPoint = "";
			endStopPoint = "";
			diodFlashing = boost::logic::indeterminate;
			alternance = boost::logic::indeterminate;
			multipleStop = boost::logic::indeterminate;
			terminusOrStop = boost::logic::indeterminate;
			way = "";
			stopPoint = "";
			numberShow = 0;
			ttsBroadcasting = boost::logic::indeterminate;
			jingle = boost::logic::indeterminate;
			chaining = "";
			priority = boost::logic::indeterminate;
			varying = boost::logic::indeterminate;
			duration = 0;
			content = "";
			contentTts = "";
			contentScrolling = "";
		}


		bool IneoTerminusConnection::tcp_connection::_checkStatusRequest(XMLNode& node, std::string& response)
		{
			bool status = true;

			// Check for mandatory nodes
			int numIDNode = node.nChildNode("ID");
			int numRequestTimeStampNode = node.nChildNode("RequestTimeStamp");
			int numRequestorRefNode = node.nChildNode("RequestorRef");

			status = ((1 == numIDNode) && (1 == numRequestTimeStampNode) && (1 == numRequestorRefNode));
			if(false == status)
			{
				// Message is ill-formed, reply to Ineo with an error
				util::Log::GetInstance().warn("Ineo Terminus : message misses mandatory nodes");
				response = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><ResponseRef>Terminus</ResponseRef><ErrorType>ProtocolError</ErrorType><ErrorMessage>Syntaxe incorrecte</ErrorMessage><ErrorID>1</ErrorID>";
				return status;
			}

			XMLNode IDNode = node.getChildNode("ID", 0);
			string idStr = IDNode.getText();
			XMLNode RequestTimeStampNode = node.getChildNode("RequestTimeStamp", 0);
			ptime requestTimeStamp(
				XmlToolkit::GetIneoDateTime(
					RequestTimeStampNode.getText()
			)	);
			XMLNode RequestorRefNode = node.getChildNode("RequestorRef", 0);
			string requestorRefStr = RequestorRefNode.getText();

			util::Log::GetInstance().debug("Ineo Terminus : check status request id " +
				idStr +
				" ; timestamp " +
				boost::posix_time::to_simple_string(requestTimeStamp) +
				" ; from " +
				requestorRefStr
			);

			// Generate the XML response to this request
			response = _generateResponse(node, AucuneErreur);

			return status;
		}


		bool IneoTerminusConnection::tcp_connection::_createMessageRequest(XMLNode& node, std::string& response)
		{
			bool status = true;
			IneoApplicationError errorCode = AucuneErreur;

			// Check for mandatory nodes
			int numIDNode = node.nChildNode("ID");
			int numRequestTimeStampNode = node.nChildNode("RequestTimeStamp");
			int numRequestorRefNode = node.nChildNode("RequestorRef");
			int numMessagingNode = node.nChildNode("Messaging");

			status = ((1 == numIDNode) && (1 == numRequestTimeStampNode) && (1 == numRequestorRefNode) && (0 < numMessagingNode));
			if(false == status)
			{
				// Message is ill-formed, reply to Ineo with an error
				util::Log::GetInstance().warn("Ineo Terminus : message misses mandatory nodes");
				response = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><ResponseRef>Terminus</ResponseRef><ErrorType>ProtocolError</ErrorType><ErrorMessage>Syntaxe incorrecte</ErrorMessage><ErrorID>1</ErrorID>";
				return status;
			}

			// Extract Ineo message type from tag name
			std::string tagName(node.getName());
			std::string ineoMessageType = tagName.substr(0, tagName.find("CreateMessageRequest"));
			RegistryKeyType fakeBroadCastPoint = _fakeBroadcastPoints.at(ineoMessageType);

			XMLNode IDNode = node.getChildNode("ID", 0);
			string idStr = IDNode.getText();
			XMLNode RequestTimeStampNode = node.getChildNode("RequestTimeStamp", 0);
			ptime requestTimeStamp(
				XmlToolkit::GetIneoDateTime(
					RequestTimeStampNode.getText()
			)	);
			XMLNode RequestorRefNode = node.getChildNode("RequestorRef", 0);
			string requestorRefStr = RequestorRefNode.getText();

			vector<Messaging> messages;
			for (int cptMessagingNode = 0;cptMessagingNode<numMessagingNode;cptMessagingNode++)
			{
				XMLNode MessagingNode = node.getChildNode("Messaging", cptMessagingNode);
				Messaging message = _readMessagingNode(MessagingNode, ineoMessageType);
				messages.push_back(message);
			}

			util::Log::GetInstance().debug("Ineo Terminus : create message request : id " +
				idStr +
				" ; timestamp " +
				boost::posix_time::to_simple_string(requestTimeStamp) +
				" ; from " +
				requestorRefStr +
				" with " +
				lexical_cast<string>(messages.size()) +
				" message(s)"
			);

			// Creation of a scenario and a message using ScenarioSaveAction
			status = _createMessages(messages, fakeBroadCastPoint, errorCode);

			// Generate the XML response to this request
			response = _generateResponse(node, errorCode);

			return status;
		}


		bool IneoTerminusConnection::tcp_connection::_deleteMessageRequest(XMLNode& node, std::string& response)
		{
			bool status = true;
			IneoApplicationError errorCode = AucuneErreur;

			// Check for mandatory nodes
			int numIDNode = node.nChildNode("ID");
			int numRequestTimeStampNode = node.nChildNode("RequestTimeStamp");
			int numRequestorRefNode = node.nChildNode("RequestorRef");
			int numMessagingNode = node.nChildNode("Messaging");

			status = ((1 == numIDNode) && (1 == numRequestTimeStampNode) && (1 == numRequestorRefNode) && (0 < numMessagingNode));
			if(false == status)
			{
				// Message is ill-formed, reply to Ineo with an error
				util::Log::GetInstance().warn("Ineo Terminus : message misses mandatory nodes");
				response = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><ResponseRef>Terminus</ResponseRef><ErrorType>ProtocolError</ErrorType><ErrorMessage>Syntaxe incorrecte</ErrorMessage><ErrorID>1</ErrorID>";
				return status;
			}

			// Extract Ineo message type from tag name
			std::string tagName(node.getName());
			std::string ineoMessageType = tagName.substr(0, tagName.find("DeleteMessageRequest"));
			RegistryKeyType fakeBroadCastPoint = _fakeBroadcastPoints.at(ineoMessageType);

			XMLNode IDNode = node.getChildNode("ID", 0);
			string idStr = IDNode.getText();
			XMLNode RequestTimeStampNode = node.getChildNode("RequestTimeStamp", 0);
			ptime requestTimeStamp(
				XmlToolkit::GetIneoDateTime(
					RequestTimeStampNode.getText()
			)	);
			XMLNode RequestorRefNode = node.getChildNode("RequestorRef", 0);
			string requestorRefStr = RequestorRefNode.getText();

			vector<Messaging> messages;
			for (int cptMessagingNode = 0;cptMessagingNode<numMessagingNode;cptMessagingNode++)
			{
				XMLNode MessagingNode = node.getChildNode("Messaging", cptMessagingNode);
				Messaging message = _readMessagingNode(MessagingNode, ineoMessageType);
				messages.push_back(message);
			}

			util::Log::GetInstance().debug("Ineo Terminus : delete message request id " +
				idStr +
				" ; timestamp " +
				boost::posix_time::to_simple_string(requestTimeStamp) +
				" ; from " +
				requestorRefStr +
				" with " +
				lexical_cast<string>(messages.size()) +
				" message(s)"
			);

			// Build a parameters map from the data of the request
			boost::shared_ptr<ParametersMap> messagesAndCalendarsPM(new ParametersMap);
			status = _buildMessagingParametersMap(messages, fakeBroadCastPoint, messagesAndCalendarsPM, errorCode);

			if(true == status)
			{
				// ScenarioStopAction requires a boost::property_tree so we need to tranform the parameters map into JSON and then parse it with Boost
				stringstream stream;
				messagesAndCalendarsPM->outputJSON(stream, "");
				boost::property_tree::ptree messagesAndCalendars;
				boost::property_tree::json_parser::read_json(stream, messagesAndCalendars);
				ScenarioStopAction scenarioStopAction;
				SentScenario* sscenario = NULL;

				try
				{
					sscenario = scenarioStopAction.findScenarioByMessagesAndCalendars(boost::optional<boost::property_tree::ptree>(messagesAndCalendars));

					if (sscenario)
					{
						// SYNTHESE has a matching scenario, try to stop it
						util::Log::GetInstance().debug("Ineo Terminus : scenario " + sscenario->getName() + " deleted due to Ineo delete request");
						scenarioStopAction.setScenario(sscenario);
						scenarioStopAction.setArchive(true);
						Request fakeRequest;
						scenarioStopAction.run(fakeRequest);
					}
					else
					{
						// Search a matching scenario with different recipients
						sscenario = scenarioStopAction.findScenarioByMessagesAndCalendars(boost::optional<boost::property_tree::ptree>(messagesAndCalendars), false);
						if (sscenario)
						{
							util::Log::GetInstance().debug("Ineo Terminus : scenario " + sscenario->getName() + " updated due to Ineo delete request");
							// The delete requests only concerns recipients
							scenarioStopAction.setScenario(sscenario);
							Request fakeRequest;
							scenarioStopAction.deleteAlarmObjectLinks(boost::optional<boost::property_tree::ptree>(messagesAndCalendars),fakeRequest);
						}
						else
						{
							// SYNTHESE has no matching scenario, generate an error
							util::Log::GetInstance().warn("Ineo Terminus : no scenario matching this delete request");
							status = false;
							errorCode = ProgrammationInconnue;
						}
					}
				}

				catch(const std::exception& e)
				{
					// Catching std::exception is considered bad practice, but it is needed here because the implementation
					// of ScenarioStopAction is not robust : in particular it does not check the structure of the ptree
					// prior to accessing it, so it may trigger exceptions if it is ill-formed
					// TODO : rewrite ScenarioStopAction
					util::Log::GetInstance().warn("Ineo Terminus : an error occurred while processing this delete request");
					status = false;
					errorCode = AutreErreur;
				}
			}

			// Generate the XML response to this request
			response = _generateResponse(node, errorCode);

			return status;
		}


		bool IneoTerminusConnection::tcp_connection::_getStatesResponse(XMLNode& node, std::string& response)
		{
			bool status = true;

			// Extract Ineo message type from tag name
			std::string tagName(node.getName());
			std::string ineoMessageType = tagName.substr(0, tagName.find("GetStatesResponse"));

			// Check for mandatory nodes
			int numIDNode = node.nChildNode("ID");
			int numRequestIDNode = node.nChildNode("RequestID");
			int numResponseTimeStampNode = node.nChildNode("ResponseTimeStamp");
			int numResponseRefNode = node.nChildNode("ResponseRef");
			int numMessagingStatesNode = node.nChildNode("MessagingStates");
			int numErrorTypeNode = node.nChildNode("ErrorType");

			status = ((1 == numIDNode) && (1 == numRequestIDNode) && (1 == numResponseTimeStampNode) && (1 == numResponseRefNode));
			if(false == status)
			{
				// Message is ill-formed, reply to Ineo with an error
				util::Log::GetInstance().warn("Ineo Terminus : " + tagName + " misses mandatory nodes");
				return status;
			}

			if(0 < numErrorTypeNode)
			{
				std::string errorType(node.getChildNode("ErrorType").getText());
				std::string errorId(node.getChildNode("ErrorID").getText());

				// An error occurred, resend the XXXGetStatesRequest until a proper response is received
				std::string stateRequest = IneoTerminusConnection::GetTheConnection()->_buildGetStatesRequest(ineoMessageType);
				IneoTerminusConnection::GetTheConnection()->addMessage(stateRequest);

				// Do not log ProtocolError #3 ("SAETR absent") because it happens on a regular basis and would flood the log databases
				if(false == (("ProtocolError" == errorType) && ("3" == errorId)))
				{
					util::Log::GetInstance().warn("Ineo Terminus : " + tagName + " has error " + std::string(node.getChildNode("ErrorMessage").getText()));
					IneoTerminusLog::AddIneoTerminusErrorMessageEntry(node);
				}
			}

			if(1 != numMessagingStatesNode)
			{
				return status;
			}

			RegistryKeyType fakeBroadCastPoint = _fakeBroadcastPoints.at(ineoMessageType);
			boost::shared_ptr<NotificationProvider> provider;
			MessagesModule::SortedActivatedMessages activatedMessages;
			typedef std::map< std::string, boost::shared_ptr<Alarm> > MessageMap;
			MessageMap mapActivatedMessages;
			MessageMap mapMessagesUnknownFromIneo;

			// Find the NotificationProvider matching this response and query its activated messages
			BOOST_FOREACH(const NotificationProvider::Registry::value_type& providerEntry, Env::GetOfficialEnv().getRegistry<NotificationProvider>())
			{
				if(IneoNotificationChannel::FACTORY_KEY == providerEntry.second->get<NotificationChannelKey>())
				{
					const ParametersMap& parameters = providerEntry.second->get<Parameters>();

					if(ineoMessageType == parameters.getDefault<std::string>(IneoNotificationChannel::PARAMETER_INEO_MESSAGE_TYPE, ""))
					{
						provider = providerEntry.second;
						activatedMessages = MessagesModule::GetActivatedMessages(*provider, ParametersMap());
						break;
					}
				}
			}

			util::Log::GetInstance().info("Ineo Terminus : SYNTHESE has " + boost::lexical_cast<std::string>(activatedMessages.size()) + " active messages for " + ineoMessageType);

			// Compute the "Ineo title" of each activated message and create a map based on that key for fast lookup
			BOOST_FOREACH(MessagesModule::SortedActivatedMessages::value_type message, activatedMessages)
			{
				std::string messageTitle = message->get<ShortMessage>();
				RegistryKeyType messageId = message->getKey();
				std::stringstream messageKeyStream;
				messageKeyStream << std::setw(4) << std::setfill('0') << (messageId % 10000);
				messageKeyStream << std::setw(0) << " " << messageTitle.substr(0, 27);
				mapActivatedMessages.insert(std::make_pair(messageKeyStream.str(), message));
			}

			// Perform a copy of the map of currently activated messages that will be used to determine which messages must be sent to Ineo
			mapMessagesUnknownFromIneo = mapActivatedMessages;

			XMLNode idNode = node.getChildNode("ID", 0);
			string idStr = idNode.getText();

			XMLNode requestIdNode = node.getChildNode("RequestID", 0);
			string requestIdStr = requestIdNode.getText();

			XMLNode responseTimeStampNode = node.getChildNode("ResponseTimeStamp", 0);
			ptime responseTimeStamp(
				XmlToolkit::GetIneoDateTime(
					responseTimeStampNode.getText()
			)	);

			XMLNode responseRefNode = node.getChildNode("ResponseRef", 0);
			string responseRefStr = responseRefNode.getText();

			XMLNode messagingStatesNode = node.getChildNode("MessagingStates", 0);
			int numMessagingStateNode = messagingStatesNode.nChildNode("MessagingState");
			int numCreatedMessages = 0;
			int numSentMessages = 0;

			for (int cptMessagingStateNode = 0; cptMessagingStateNode < numMessagingStateNode; cptMessagingStateNode++)
			{
				XMLNode messagingStateNode = messagingStatesNode.getChildNode("MessagingState", cptMessagingStateNode);
				XMLNode messagingNode = messagingStateNode.getChildNode("Messaging");
				XMLNode isActiveNode = messagingStateNode.getChildNode("IsActive");
				std::string isActiveText(isActiveNode.getText());

				// Process this message only if it is active
				if("oui" == isActiveText)
				{
					Messaging message = _readMessagingNode(messagingNode, ineoMessageType);
					MessageMap::iterator it = mapActivatedMessages.find(message.name);

					if(mapActivatedMessages.end() == it)
					{
						// Ineo specified that SYNTHESE messages must comply to a specific naming convention
						static const boost::regex isFromSyntheseRegexp("[0-9]{4} ");

						if(true == boost::regex_match(message.name, isFromSyntheseRegexp))
						{
							// This message matches SYNTHESE naming convention but is unknown
							// Log a warning and send a delete request to Ineo
							util::Log::GetInstance().warn("Ineo Terminus : message " + message.name + " from Ineo seems to be a SYNTHESE message, but could not be matched");
							std::string deleteRequest = _generateDeleteRequest(ineoMessageType, messagingNode);
							IneoTerminusConnection::GetTheConnection()->addMessage(deleteRequest);
						}

						else
						{
							// This message is not a currently activated message of SYNTHESE, create it with origin = Ineo
							vector<Messaging> messages;
							messages.push_back(message);

							// Creation of a scenario and a message in SYNTHESE
							// Note : Ineo does not expect SYNTHESE to reply to a XXXGetStatesResponse, so we do not create an error response
							IneoApplicationError unused = AucuneErreur;
							_createMessages(messages, fakeBroadCastPoint, unused);

							numCreatedMessages++;
						}
					}

					else
					{
						// This message is a currently activated message of SYNTHESE, remove it from the list of messages unknown from Ineo
						mapMessagesUnknownFromIneo.erase(message.name);
						util::Log::GetInstance().debug("Ineo Terminus : message " + message.name + " from Ineo matches a SYNTHESE message, do not recreate");
					}
				}
			}

			// Loop through the list of currently activated messages that Ineo does not know, and send them to Ineo
			BOOST_FOREACH(MessageMap::value_type messageEntry, mapMessagesUnknownFromIneo)
			{
				// NotificationProvider does not process Alarm, it requires a NotificationEvent
				// Since we don't want to add another NotificationEvent into the database, we create a fake one that is only used to generate the message
				const boost::shared_ptr<NotificationEvent> fakeEvent(new NotificationEvent(0, *(messageEntry.second.get()), *provider, BEGIN));
				provider->notify(fakeEvent);
				numSentMessages++;
			}

			util::Log::GetInstance().debug(
				"Ineo Terminus processing message " + tagName + " : id " + idStr + " ; request id " + requestIdStr + " ; timestamp " +
				boost::posix_time::to_simple_string(responseTimeStamp) + " ; from " + responseRefStr + " with " + lexical_cast<string>(numCreatedMessages) +
				" created message(s) and " + lexical_cast<string>(numSentMessages) + " sent message(s)"
			);

			return status;
		}


		std::string IneoTerminusConnection::tcp_connection::_generateResponse(XMLNode& requestNode, const IneoApplicationError& errorCode)
		{
			std::stringstream responseStream;

			std::string requestTag = requestNode.getName();
			std::string requestId = requestNode.getChildNode("ID", 0).getText();
			std::string requestTimestamp = requestNode.getChildNode("RequestTimeStamp", 0).getText();

			// Build the response tag
			std::string responseTag = requestTag;
			replace_all(responseTag, "Request", "Response");

			// Build the response header
			responseStream << INEO_TERMINUS_XML_HEADER << char(10);

			std::string xsdLocation = IneoTerminusConnection::GetTheConnection()->getIneoXSDLocation();
			responseStream << "<" << responseTag;
			if(false == xsdLocation.empty())
			{
				responseStream << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"" << xsdLocation << "\"";
			}
			responseStream << ">" << char(10);

			responseStream << "\t<ID>" << boost::lexical_cast<std::string>(IneoTerminusConnection::GetTheConnection()->getNextRequestID()) << "</ID>" << char(10);
			responseStream << "\t<RequestID>" << requestId << "</RequestID>" << char(10);
			// Note : according to interface description 'ResponseTimeStamp' = 'RequestTimeStamp' but I suspect this is a typo
			responseStream << "\t<ResponseTimeStamp>" << requestTimestamp << "</ResponseTimeStamp>" << char(10);
			responseStream << "\t<ResponseRef>Terminus</ResponseRef>" << char(10);

			// If an error occurred while processing the request, notify Ineo
			if(AucuneErreur != errorCode)
			{
				std::string errorMessage = "Erreur non documentée";

				switch(errorCode)
				{
					case LigneInconnue:
						errorMessage = "Ligne inconnue";
						break;

					case ArretInconnu:
						errorMessage = "Arrêt inconnu";
						break;

					case ProgrammationInconnue:
						errorMessage = "Programmation inconnue";
						break;

					default:
						// no default case because errorMessage is already initialized a default value
						break;
				}

				responseStream << "\t<ErrorType>ApplicationError</ErrorType>" << char(10);
				responseStream << "\t<ErrorMessage>" << errorMessage << "</ErrorMessage>" << char(10);
				responseStream << "\t<ErrorID>" << errorCode << "</ErrorID>" << char(10);
			}

			// Copy the content of 'Messaging' nodes
			int messagingCount = requestNode.nChildNode("Messaging");
			for (int cptMessaging = 0; cptMessaging < messagingCount; cptMessaging++)
			{
				XMLNode messagingNode = requestNode.getChildNode("Messaging", cptMessaging);
				int tabDepth = 2;

				responseStream << "\t<Messaging>" << char(10);
				_copyXMLNode(messagingNode, tabDepth, responseStream);
				responseStream << "\t</Messaging>" << char(10);
			}

			responseStream << "</" << responseTag << ">" << char(10);

			return responseStream.str();
		}


		std::string IneoTerminusConnection::tcp_connection::_generateDeleteRequest(const std::string& messageType, XMLNode& messageNode)
		{
			std::stringstream responseStream;

			// TODO : factor
			boost::posix_time::ptime now = second_clock::local_time();
			std::stringstream timestampStream;
			timestampStream << setfill('0') << setw(2) << now.date().day() << "/"
							<< setfill('0') << setw(2) << int(now.date().month()) << "/"
							<< setfill('0') << setw(4) << now.date().year() << " "
							<< setfill('0') << setw(2) << now.time_of_day().hours() << ":"
							<< setfill('0') << setw(2) << now.time_of_day().minutes() << ":"
							<< setfill('0') << setw(2) << now.time_of_day().seconds();

			// Build the response tag
			std::string responseTag = messageType + "DeleteRequest";

			// Build the response header
			responseStream << INEO_TERMINUS_XML_HEADER << char(10);

			std::string xsdLocation = IneoTerminusConnection::GetTheConnection()->getIneoXSDLocation();
			responseStream << "<" << responseTag;
			if(false == xsdLocation.empty())
			{
				responseStream << " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"" << xsdLocation << "\"";
			}
			responseStream << ">" << char(10);

			responseStream << "\t<ID>" << boost::lexical_cast<std::string>(IneoTerminusConnection::GetTheConnection()->getNextRequestID()) << "</ID>" << char(10);
			responseStream << "\t<RequestTimeStamp>" << timestampStream.str() << "</RequestTimeStamp>" << char(10);
			responseStream << "\t<RequestorRef>Terminus</RequestorRef>" << char(10);

			// Copy the content of 'Messaging' node
			responseStream << "\t<Messaging>" << char(10);
			int tabDepth = 2;
			_copyXMLNode(messageNode, tabDepth, responseStream);
			responseStream << "\t</Messaging>" << char(10);

			responseStream << "</" << responseTag << ">" << char(10);

			return responseStream.str();
		}


		void IneoTerminusConnection::tcp_connection::_copyXMLNode(XMLNode& node, const int tabDepth, std::stringstream& outputStream)
		{
			int childCount = node.nChildNode();
			std::string tabs(tabDepth, '\t');

			for (int cptChild = 0; cptChild < childCount; cptChild++)
			{
				XMLNode childNode = node.getChildNode(cptChild);
				std::string childName = childNode.getName();

				outputStream << tabs << "<" << childName << ">";

				if(0 < childNode.nChildNode())
				{
					// This child has children, call _copyXMLNode recursively
					outputStream << char(10);
					_copyXMLNode(childNode, tabDepth + 1, outputStream);
					outputStream << tabs << "</" << childName << ">" << char(10);
				}

				else
				{
					// This child is a simple text node, copy its value
					outputStream << childNode.getText() << "</" << childName << ">" << char(10);
				}
			}
		}


		IneoTerminusConnection::tcp_connection::Messaging IneoTerminusConnection::tcp_connection::_readMessagingNode(XMLNode node, string messagerieName)
		{
			Messaging message;
			if (node.nChildNode("Name") > 0)
			{
				XMLNode nameNode = node.getChildNode("Name", 0);
				message.name = _iconv.convert(nameNode.getText());
			}
			if (messagerieName == "Passenger" ||
				messagerieName == "Driver" ||
				messagerieName == "SonoPassenger" ||
				messagerieName == "SonoDriver" ||
				messagerieName == "SonoStopPoint")
			{
				XMLNode dispatchingNode = node.getChildNode("Dispatching", 0);
				if ((string)(dispatchingNode.getText()) == "Immediat")
				{
					message.dispatching = Immediat;
				}
				else if ((string)(dispatchingNode.getText()) == "Differe")
				{
					message.dispatching = Differe;
				}
				else if ((string)(dispatchingNode.getText()) == "Repete")
				{
					message.dispatching = Repete;
				}
				else
				{
					util::Log::GetInstance().warn("IneoTerminusConnection : Message avec Dispatching inconnu : ");
				}
			}

			// Define default time period : start time and stop time match Ineo exploitation period
			// TODO : change start time and stop time constants to be configuration parameters
			string startDateStr("01/01/1970");
			string stopDateStr("31/12/2037");
			string startTimeStr("03:00:00");
			string stopTimeStr("27:00:00");
			message.startHour = boost::posix_time::not_a_date_time;
			message.stopHour = boost::posix_time::not_a_date_time;
			if (node.nChildNode("StartDate") > 0)
			{
				XMLNode startDateNode = node.getChildNode("StartDate", 0);
				startDateStr = startDateNode.getText();
			}
			if (node.nChildNode("StopDate") > 0)
			{
				XMLNode stopDateNode = node.getChildNode("StopDate", 0);
				stopDateStr = stopDateNode.getText();
			}
			if (node.nChildNode("StartTime") > 0)
			{
				XMLNode startTimeNode = node.getChildNode("StartTime", 0);
				startTimeStr = startTimeNode.getText();
				message.startHour = boost::posix_time::duration_from_string(startTimeStr);
			}
			if (node.nChildNode("StopTime") > 0)
			{
				XMLNode stopTimeNode = node.getChildNode("StopTime", 0);
				stopTimeStr = stopTimeNode.getText();
				message.stopHour = boost::posix_time::duration_from_string(stopTimeNode.getText());
			}
			message.startDate =	 XmlToolkit::GetIneoDateTime(
				startDateStr + " " + startTimeStr
			);
			message.stopDate =	XmlToolkit::GetIneoDateTime(
				stopDateStr + " " + stopTimeStr
			);
			if (node.nChildNode("MultipleStop") > 0)
			{
				XMLNode multipleStopNode = node.getChildNode("MultipleStop", 0);
				message.multipleStop = ((string)(multipleStopNode.getText()) == "oui");
			}
			if (messagerieName == "Driver" &&
				node.nChildNode("EndStopPoint") > 0)
			{
				XMLNode endStopPointNode = node.getChildNode("EndStopPoint", 0);
				message.terminusOrStop = ((string)(endStopPointNode.getText()) == "oui");
			}
			if (node.nChildNode("Way") > 0)
			{
				XMLNode wayNode = node.getChildNode("way", 0);
				message.way = wayNode.getText();
			}
			if (node.nChildNode("StopPoint") > 0)
			{
				XMLNode stopPointNode = node.getChildNode("StopPoint", 0);
				message.stopPoint = stopPointNode.getText();
			}
			if (node.nChildNode("NumberShow") > 0)
			{
				XMLNode numberShowNode = node.getChildNode("NumberShow", 0);
				message.numberShow = lexical_cast<int>(numberShowNode.getText());
			}
			if (node.nChildNode("RepeatPeriod") > 0)
			{
				XMLNode repeatPeriodNode = node.getChildNode("RepeatPeriod", 0);
				message.repeatPeriod = lexical_cast<int>(repeatPeriodNode.getText());
			}
			if (node.nChildNode("Confirm") > 0)
			{
				XMLNode confirmNode = node.getChildNode("Confirm", 0);
				message.confirm = ((string)(confirmNode.getText()) == "oui");
			}
			if (node.nChildNode("Inhibition") > 0)
			{
				XMLNode inhibitionNode = node.getChildNode("Inhibition", 0);
				message.inhibition = ((string)(inhibitionNode.getText()) == "oui");
			}
			if (node.nChildNode("Color") > 0)
			{
				XMLNode colorNode = node.getChildNode("Color", 0);
				message.color = colorNode.getText();
			}
			if (node.nChildNode("TtsBroadcasting") > 0)
			{
				XMLNode ttsBroadcastingNode = node.getChildNode("TtsBroadcasting", 0);
				message.ttsBroadcasting = ((string)(ttsBroadcastingNode.getText()) == "oui");
			}
			if (node.nChildNode("Jingle") > 0)
			{
				XMLNode jingleNode = node.getChildNode("Jingle", 0);
				message.jingle = ((string)(jingleNode.getText()) == "oui");
			}
			if (messagerieName == "SonoPassenger")
			{
				XMLNode activateHeadJingle = node.getChildNode("ActivateHeadJingle", 0);
				message.activateHeadJingle = ((string)(activateHeadJingle.getText()) == "oui");
				XMLNode activateBackJingle = node.getChildNode("ActivateBackJingle", 0);
				message.activateBackJingle = ((string)(activateBackJingle.getText()) == "oui");
			}
			if (node.nChildNode("StartStopPoint") > 0)
			{
				XMLNode startStopPointNode = node.getChildNode("StartStopPoint", 0);
				message.startStopPoint = startStopPointNode.getText();
			}
			if (messagerieName != "Driver" &&
				node.nChildNode("EndStopPoint") > 0)
			{
				XMLNode endStopPointNode = node.getChildNode("EndStopPoint", 0);
				message.endStopPoint = endStopPointNode.getText();
			}
			if (node.nChildNode("Chaining") > 0)
			{
				XMLNode chainingNode = node.getChildNode("Chaining", 0);
				message.chaining = chainingNode.getText();
			}
			if (node.nChildNode("Priority") > 0)
			{
				XMLNode priorityNode = node.getChildNode("Priority", 0);
				message.priority = ((string)(priorityNode.getText()) == "oui");
			}
			if (messagerieName == "Girouette")
			{
				XMLNode codeNode = node.getChildNode("Code", 0);
				message.codeGirouette = lexical_cast<int>(codeNode.getText());
			}
			if (node.nChildNode("Varying") > 0)
			{
				XMLNode varyingNode = node.getChildNode("Varying", 0);
				message.varying = ((string)(varyingNode.getText()) == "oui");
			}
			if (node.nChildNode("Duration") > 0)
			{
				XMLNode durationNode = node.getChildNode("Duration", 0);
				message.duration = lexical_cast<int>(durationNode.getText());
			}
			if (node.nChildNode("DiodFlashing") > 0)
			{
				XMLNode diodFlashingNode = node.getChildNode("DiodFlashing", 0);
				message.diodFlashing = ((string)(diodFlashingNode.getText()) == "oui");
			}
			if (node.nChildNode("Alternance") > 0)
			{
				XMLNode alternanceNode = node.getChildNode("Alternance", 0);
				message.alternance = ((string)(alternanceNode.getText()) == "oui");
			}
			int numTextNode = node.nChildNode("Text");
			for (int cptTextNode = 0;cptTextNode<numTextNode;cptTextNode++)
			{
				if (cptTextNode > 0)
				{
					message.content += "<br /><br />";
				}
				XMLNode textNode = node.getChildNode("Text", cptTextNode);
				int numLineNode = textNode.nChildNode("Line");
				for (int cptLineNode = 0;cptLineNode<numLineNode;cptLineNode++)
				{
					XMLNode LineNode = textNode.getChildNode("Line", cptLineNode);
					if (cptLineNode > 0)
					{
						message.content += "<br />";
					}
					message.content += _iconv.convert(LineNode.getText());
				}
				if (textNode.nChildNode("TypeBIV") > 0)
				{
					// We are in a BIVGENERALMESSAGINGTEXTTYPE
					XMLNode fixedTextNode = textNode.getChildNode("FixedText", 0);
					numLineNode = fixedTextNode.nChildNode("Line");
					for (int cptLineNode = 0;cptLineNode<numLineNode;cptLineNode++)
					{
						XMLNode LineNode = fixedTextNode.getChildNode("Line", cptLineNode);
						if (cptLineNode > 0)
						{
							message.content += "<br />";
						}
						message.content += _iconv.convert(LineNode.getText());
					}
					XMLNode scrollingTextNode = textNode.getChildNode("ScrollingText", 0);
					numLineNode = scrollingTextNode.nChildNode("Line");
					for (int cptLineNode = 0;cptLineNode<numLineNode;cptLineNode++)
					{
						XMLNode LineNode = scrollingTextNode.getChildNode("Line", cptLineNode);
						if (cptLineNode > 0)
						{
							message.contentScrolling += "<br />";
						}
						message.contentScrolling += _iconv.convert(LineNode.getText());
					}
					XMLNode ttsTextNode = textNode.getChildNode("TtsText", 0);
					numLineNode = ttsTextNode.nChildNode("Line");
					for (int cptLineNode = 0;cptLineNode<numLineNode;cptLineNode++)
					{
						XMLNode LineNode = ttsTextNode.getChildNode("Tts", cptLineNode);
						if (cptLineNode > 0)
						{
							message.contentTts += "<br />";
						}
						message.contentTts += _iconv.convert(LineNode.getText());
					}
				}
			}
			if (node.nChildNode("Tts") > 0)
			{
				XMLNode ttsNode = node.getChildNode("Tts", 0);
				int numLineNode = ttsNode.nChildNode("Line");
				for (int cptLineNode = 0;cptLineNode<numLineNode;cptLineNode++)
				{
					XMLNode LineNode = ttsNode.getChildNode("Line", cptLineNode);
					if (cptLineNode > 0)
					{
						message.contentTts += "<br />";
					}
					message.contentTts += _iconv.convert(LineNode.getText());
				}

				if("BivLineMan" == messagerieName)
				{
					message.ttsBroadcasting = true;
				}
			}
			XMLNode RecipientsNode = node.getChildNode("Recipients", 0);
			message.recipients = _readRecipients(RecipientsNode);

			if(false == message.stopPoint.empty())
			{
				// Driver.StopPoint is processed as a StopPoint recipient
				IneoTerminusConnection::Recipient new_recipient;
				new_recipient.type = "StopPoint";
				new_recipient.name = message.stopPoint;
				message.recipients.push_back(new_recipient);
			}

			return message;
		}


		bool IneoTerminusConnection::tcp_connection::_buildMessagingParametersMap(
			std::vector<Messaging> messages,
			RegistryKeyType fakeBroadCastPoint,
			boost::shared_ptr<ParametersMap> parametersMap,
			IneoApplicationError& errorCode
		)
		{
			bool status = true;

			// Fill the parameters map from the vector of messages
			BOOST_FOREACH(const Messaging& message, messages)
			{
				boost::shared_ptr<ParametersMap> periodPM(new ParametersMap);
				periodPM->insert("start_date", boost::gregorian::to_iso_extended_string(message.startDate.date()) + " " + boost::posix_time::to_simple_string(message.startDate.time_of_day()));
				periodPM->insert("end_date", boost::gregorian::to_iso_extended_string(message.stopDate.date()) + " " + boost::posix_time::to_simple_string(message.stopDate.time_of_day()));
				periodPM->insert("start_hour", message.startHour.is_not_a_date_time() ? "" : boost::posix_time::to_simple_string(message.startHour));
				periodPM->insert("end_hour", message.stopHour.is_not_a_date_time() ? "" : boost::posix_time::to_simple_string(message.stopHour));
				periodPM->insert("date", "");

				boost::shared_ptr<ParametersMap> messagePM(new ParametersMap);
				messagePM->insert("title", message.name);

				std::string messageContent = message.content;
				if (false == message.color.empty())
				{
					std::string htmlColor = "green";
					if("Orange" == message.color)
					{
						htmlColor = "orange";
					}
					if("Rouge" == message.color)
					{
						htmlColor = "red";
					}
					messageContent = "<font color=\\\"" + htmlColor + "\\\">" + message.content + "</font>";
				}
				messagePM->insert("content", messageContent);

				if (message.dispatching == Immediat)
				{
					messagePM->insert("dispatching", "Immediat");
				}
				else if (message.dispatching == Differe)
				{
					messagePM->insert("dispatching", "Differe");
				}
				else if (message.dispatching == Repete)
				{
					messagePM->insert("dispatching", "Repete");
				}

				// Alarm level is set by different attributes depending on the type of the message
				AlarmLevel level = ALARM_LEVEL_INFO;
				if(false == boost::logic::indeterminate(message.alternance))
				{
					level = (message.alternance ? ALARM_LEVEL_ALTERNATE : ALARM_LEVEL_WARNING);
				}
				if(false == boost::logic::indeterminate(message.priority))
				{
					level = (message.priority ? ALARM_LEVEL_WARNING : ALARM_LEVEL_ALTERNATE);
				}
				if(false == boost::logic::indeterminate(message.varying))
				{
					level = (message.varying ? ALARM_LEVEL_ALTERNATE : ALARM_LEVEL_WARNING);
				}
				if(false == boost::logic::indeterminate(message.inhibition))
				{
					level = (message.inhibition ? ALARM_LEVEL_WARNING : ALARM_LEVEL_ALTERNATE);
				}
				messagePM->insert("level", level);

				if(0 < message.duration)
				{
					messagePM->insert("displayDuration", message.duration);
				}
				messagePM->insert("repeat_interval", lexical_cast<string>(message.repeatPeriod * 60));
				messagePM->insert("section", "");
				messagePM->insert("alternative", "");
				messagePM->insert("with_ack", (bool) message.confirm);
				messagePM->insert("multiple_stops", (bool) message.multipleStop);
				messagePM->insert("play_tts", (bool) message.ttsBroadcasting);
				messagePM->insert("light", (bool) message.diodFlashing);
				messagePM->insert("direction_sign_code", message.codeGirouette);

				if(false == message.startStopPoint.empty())
				{
					std::set<StopPoint*> startStopPoints = _findIneoStopPoint(message.startStopPoint);

					if(false == startStopPoints.empty())
					{
						std::set<StopPoint*>::iterator firstStopPoint = startStopPoints.begin();
						util::RegistryKeyType firstStopPointKey = (*firstStopPoint)->getKey();
						messagePM->insert("start_stop_point", firstStopPointKey);
					}

					else
					{
						util::Log::GetInstance().warn("Ineo Terminus : start stop point " + message.startStopPoint + " could not be found");
					}
				}

				if(false == message.endStopPoint.empty())
				{
					std::set<StopPoint*> endStopPoints = _findIneoStopPoint(message.endStopPoint);

					if(false == endStopPoints.empty())
					{
						std::set<StopPoint*>::iterator firstStopPoint = endStopPoints.begin();
						util::RegistryKeyType firstStopPointKey = (*firstStopPoint)->getKey();
						messagePM->insert("end_stop_point", firstStopPointKey);
					}

					else
					{
						util::Log::GetInstance().warn("Ineo Terminus : end stop point " + message.endStopPoint + " could not be found");
					}
				}

				IneoApplicationError recipientsErrorCode = AucuneErreur;
				bool recipientsFound = _addRecipientsPM(*messagePM, message.recipients, recipientsErrorCode);

				if(false == recipientsFound)
				{
					// At least one recipient could not be found, reply to Ineo with an error
					errorCode = recipientsErrorCode;
					status = false;
				}

				if(0 != fakeBroadCastPoint)
				{
					// If a fake broadcast point was configured for this message type, add it as a recipient
					boost::shared_ptr<ParametersMap> displayRecipientPM(new ParametersMap);
					displayRecipientPM->insert("recipient_id", fakeBroadCastPoint);
					messagePM->insert("displayscreen_recipient", displayRecipientPM);
				}

				boost::shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				calendarPM->insert("period", periodPM);
				calendarPM->insert("message", messagePM);
				parametersMap->insert("calendar", calendarPM);
			}

			return status;
		}


		bool IneoTerminusConnection::tcp_connection::_createMessages(std::vector<Messaging> messages, RegistryKeyType fakeBroadCastPoint, IneoApplicationError& errorCode)
		{
			boost::shared_ptr<ParametersMap> messagesAndCalendarsPM(new ParametersMap);
			bool status = _buildMessagingParametersMap(messages, fakeBroadCastPoint, messagesAndCalendarsPM, errorCode);

			if(true == status)
			{
				// Convert the parameters map into a Boost property tree
				stringstream stream;
				messagesAndCalendarsPM->outputJSON(stream, "");
				boost::property_tree::ptree messagesAndCalendars;
				boost::property_tree::json_parser::read_json(stream, messagesAndCalendars);

				// Check for the existence of an identical event
				ScenarioStopAction scenarioStopAction;

				try
				{
					SentScenario* identicalScenario = scenarioStopAction.findScenarioByMessagesAndCalendars(boost::optional<boost::property_tree::ptree>(messagesAndCalendars));

					if (!identicalScenario)
					{
						// This event does not exist, create it
						ScenarioSaveAction scenarioSaveAction;
						scenarioSaveAction.setMessagesAndCalendars(boost::optional<boost::property_tree::ptree>(messagesAndCalendars));
						boost::shared_ptr<Scenario>	scenario;
						boost::shared_ptr<SentScenario> sscenario;
						sscenario.reset(new SentScenario);
						sscenario->set<Name>(messages[0].name);

						scenario = static_pointer_cast<Scenario, SentScenario>(sscenario);
						scenarioSaveAction.setCreation(true);
						scenarioSaveAction.setSScenario(sscenario);
						scenarioSaveAction.setScenario(scenario);
						Request fakeRequest;
						scenarioSaveAction.run(fakeRequest);

						// Enable the scenario
						sscenario->setIsEnabled(true);

						// If the Ineo Terminus data source is configured, set it as the source of the scenario
						std::string ineoDataSourceStr = IneoTerminusModule::GetParameter(IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_DATASOURCE);
						RegistryKeyType ineoDataSourceId = (false == ineoDataSourceStr.empty()) ? boost::lexical_cast<RegistryKeyType>(ineoDataSourceStr) : 0;

						if(0 != ineoDataSourceId) try
						{
							boost::shared_ptr<const DataSource> ineoDataSource = Env::GetOfficialEnv().getRegistry<DataSource>().get(ineoDataSourceId);
							sscenario->addCodeBySource(*ineoDataSource, "");
						}

						catch(synthese::util::ObjectNotFoundException<impex::DataSource>&)
						{
							util::Log::GetInstance().warn("Ineo Terminus : data source " + boost::lexical_cast<string>(_datasource_id) + " does not exist");
						}

						SentScenarioTableSync::Save(sscenario.get());
					}

					else
					{
						// This event already exists, log a message
						util::Log::GetInstance().debug("Ineo Terminus : cannot create message because it already exists");
						// TODO : generate a specific Ineo error ???
					}
				}

				catch(const std::exception& e)
				{
					// Catching std::exception is considered bad practice, but it is needed here because the implementation
					// of ScenarioStopAction/ScenarioSaveAction are not robust : in particular they do not check the structure
					// of the ptree prior to accessing it, so they may trigger exceptions if it is ill-formed
					// TODO : rewrite ScenarioStopAction and ScenarioSaveAction
					util::Log::GetInstance().warn("Ineo Terminus : an error occurred while trying to create a message : " + std::string(e.what()));
					status = false;
					errorCode = AutreErreur;
				}
			}

			return status;
		}


		vector<IneoTerminusConnection::Recipient> IneoTerminusConnection::tcp_connection::_readRecipients(XMLNode node)
		{
			// This map stores the pair 'list_node', 'element_node' for recipient lists
			std::map<std::string, std::string> recipientTypes;
			recipientTypes.insert(make_pair("Lines", "Line"));
			recipientTypes.insert(make_pair("StopPoints", "StopPoint"));
			recipientTypes.insert(make_pair("Vehicules", "Vehicule"));
			recipientTypes.insert(make_pair("Cars", "Car"));
			recipientTypes.insert(make_pair("CarServices", "CarService"));
			recipientTypes.insert(make_pair("Bivs", "Biv"));
			recipientTypes.insert(make_pair("Groups", "Group"));

			vector<IneoTerminusConnection::Recipient> recipients;
			int nChildNode = node.nChildNode();
			for (int cptChildNode = 0;cptChildNode<nChildNode;cptChildNode++)
			{
				XMLNode recipientNode = node.getChildNode(cptChildNode);
				std::string recipientType(recipientNode.getName());

				// Special case #1 : 'AllNetwork'
				if ("AllNetwork" == recipientType)
				{
					IneoTerminusConnection::Recipient new_recipient;
					new_recipient.type = "AllNetwork";
					new_recipient.name = "AllNetwork";
					recipients.push_back(new_recipient);
				}

				// Special case #2 : 'Line' node without 'Lines' (found in BivLineManMessagingStructure)
				else if ("Line" == recipientType)
				{
					std::string lineCode = recipientNode.getText();
					IneoTerminusConnection::Recipient new_recipient;
					new_recipient.type = "Line";
					new_recipient.name = lineCode;
					recipients.push_back(new_recipient);
				}

				// Special case #3 : 'LinesWays'
				else if ("LinesWays" == recipientType)
				{
					int nChildNodes = recipientNode.nChildNode();

					for (int cptChildNode = 0; cptChildNode < nChildNodes; cptChildNode++)
					{
						XMLNode childNode = recipientNode.getChildNode(cptChildNode);
						std::string childNodeType(childNode.getName());

						if ("LineWay" == childNodeType)
						{
							XMLNode lineNode = childNode.getChildNode("Line", 0);
							XMLNode inWardNode = childNode.getChildNode("InWard", 0);
							XMLNode outWardNode = childNode.getChildNode("OutWard", 0);
							std::string lineCode = lineNode.getText();
							std::string inWard(inWardNode.getText());
							std::string outWard(outWardNode.getText());
							IneoTerminusConnection::Recipient new_recipient;

							new_recipient.type = "Line";
							new_recipient.name = lineCode;
							new_recipient.parameter = "";

							if("oui" == inWard && "non" == outWard)
							{
								new_recipient.parameter = "0";
							}

							if("non" == inWard && "oui" == outWard)
							{
								new_recipient.parameter = "1";
							}

							recipients.push_back(new_recipient);
						}

						else
						{
							util::Log::GetInstance().warn("Ineo Terminus : unexpected recipient " + childNodeType + " as child of LinesWays");
						}
					}
				}

				// Generic case : recipients are listed by class
				else if (recipientTypes.end() != recipientTypes.find(recipientType))
				{
					const std::string expectedType = recipientTypes.at(recipientType);
					int nChildNodes = recipientNode.nChildNode();

					for (int cptChildNode = 0; cptChildNode < nChildNodes; cptChildNode++)
					{
						XMLNode childNode = recipientNode.getChildNode(cptChildNode);
						std::string childNodeType(childNode.getName());

						if (expectedType == childNodeType)
						{
							std::string recipientCode = childNode.getText();
							IneoTerminusConnection::Recipient new_recipient;
							new_recipient.type = expectedType;
							new_recipient.name = recipientCode;
							recipients.push_back(new_recipient);
						}

						else
						{
							util::Log::GetInstance().warn("Ineo Terminus : unexpected recipient " + childNodeType + " as child of " + recipientType);
						}
					}
				}

				else
				{
					util::Log::GetInstance().warn("Ineo Terminus : unsupported recipient " + recipientType);
				}
			}

			return recipients;
		}


		bool IneoTerminusConnection::tcp_connection::_addRecipientsPM(ParametersMap& pm, vector<IneoTerminusConnection::Recipient> recipients, IneoApplicationError& errorCode)
		{
			boost::shared_ptr<const impex::DataSource> dataSource;
			bool status = true;
			errorCode = AucuneErreur;

			// Get the DataSource object that will be used to query lines and stop points by their Ineo code
			try
			{
				dataSource = DataSourceTableSync::Get(_datasource_id, Env::GetOfficialEnv());
			}

			catch(synthese::util::ObjectNotFoundException<impex::DataSource>&)
			{
				util::Log::GetInstance().warn("Ineo Terminus : data source " + boost::lexical_cast<string>(_datasource_id) + " does not exist");
			}

			// Iterate through recipients
			BOOST_FOREACH(const IneoTerminusConnection::Recipient& recipient, recipients)
			{
				if ("AllNetwork" == recipient.type)
				{
					boost::shared_ptr<ParametersMap> lineRecipientPM(new ParametersMap);
					lineRecipientPM->insert("recipient_id", _network_id);
					pm.insert("line_recipient", lineRecipientPM);
				}

				else if ("Line" == recipient.type)
				{
					if(NULL == dataSource.get())
					{
						status = false;
						errorCode = LigneInconnue;
					}

					else
					{
						ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(*dataSource, Env::GetOfficialEnv());
						set<CommercialLine*> loadedLines(lines.get(recipient.name));
						bool found(false);

						BOOST_FOREACH(CommercialLine* loadedLine, loadedLines)
						{
							if(loadedLine->getNetwork()->getKey() == _network_id)
							{
								boost::shared_ptr<ParametersMap> lineRecipientPM(new ParametersMap);
								lineRecipientPM->insert("recipient_id", loadedLine->getKey());
								lineRecipientPM->insert("parameter", recipient.parameter);
								pm.insert("line_recipient", lineRecipientPM);
								found = true;
							}
						}

						if (!found)
						{
							util::Log::GetInstance().warn("Ineo Terminus : line not found " + recipient.name);
							status = false;
							errorCode = LigneInconnue;
						}
					}
				}

				else if ("StopPoint" == recipient.type)
				{
					if(NULL == dataSource.get())
					{
						status = false;
						errorCode = ArretInconnu;
					}

					else
					{
						ImportableTableSync::ObjectBySource<StopPointTableSync> stopPoints(*dataSource, Env::GetOfficialEnv());
						std::string stopPointCode1 = recipient.name;
						std::string stopPointCode2 = synthese::data_exchange::IneoFileFormat::Importer_::MNLP_PREFIX + recipient.name;
						set<StopPoint*> loadedStopPoints = stopPoints.get(stopPointCode1);

						if(true == loadedStopPoints.empty())
						{
							// IneoFileFormat prefixes Ineo stop code identifiers with the string MNLP_**_
							// So if no direct match is found, add the prefix and search again
							loadedStopPoints = stopPoints.get(stopPointCode2);
						}

						BOOST_FOREACH(StopPoint* loadedStopPoint, loadedStopPoints)
						{
							boost::shared_ptr<ParametersMap> stopRecipientPM(new ParametersMap);
							stopRecipientPM->insert("recipient_id", loadedStopPoint->getKey());
							pm.insert("stoparea_recipient", stopRecipientPM);
						}

						if(true == loadedStopPoints.empty())
						{
							util::Log::GetInstance().warn("Ineo Terminus : stop not found " + stopPointCode1 + " / " + stopPointCode2);
							status = false;
							errorCode = ArretInconnu;
						}
					}
				}

				else if ("Biv" == recipient.type)
				{
					if(NULL == dataSource.get())
					{
						status = false;
						errorCode = BorneInconnue;
					}

					else
					{
						ImportableTableSync::ObjectBySource<DisplayScreenTableSync> screens(*dataSource, Env::GetOfficialEnv());
						set<DisplayScreen*> loadedScreens = screens.get(recipient.name);

						BOOST_FOREACH(DisplayScreen* loadedScreen, loadedScreens)
						{
							boost::shared_ptr<ParametersMap> screenRecipientPM(new ParametersMap);
							screenRecipientPM->insert("recipient_id", loadedScreen->getKey());
							pm.insert("displayscreen_recipient", screenRecipientPM);
						}

						if(true == loadedScreens.empty())
						{
							util::Log::GetInstance().warn("Ineo Terminus : display screen not found " + recipient.name);
							status = false;
							errorCode = BorneInconnue;
						}
					}
				}


				// The following recipient types are not processed by SYNTHESE
				else if (
						  (recipient.type == "Vehicule") ||
						  (recipient.type == "Car") ||
						  (recipient.type == "CarService") ||
						  (recipient.type == "Group")
						)
				{
					util::Log::GetInstance().debug("Ineo Terminus : skipped recipient type " + recipient.type);
				}

				else
				{
					util::Log::GetInstance().warn("Ineo Terminus : unhandled recipient type " + recipient.type);
				}
			}

			return status;
		}


		std::set<StopPoint*> IneoTerminusConnection::tcp_connection::_findIneoStopPoint(const std::string& ineoStopPointCode)
		{
			boost::shared_ptr<const impex::DataSource> dataSource;
			std::set<StopPoint*> loadedStopPoints;

			// Get the DataSource object that will be used to query lines and stop points by their Ineo code
			try
			{
				dataSource = DataSourceTableSync::Get(_datasource_id, Env::GetOfficialEnv());
			}

			catch(synthese::util::ObjectNotFoundException<impex::DataSource>&)
			{
				util::Log::GetInstance().warn("Ineo Terminus : data source " + boost::lexical_cast<string>(_datasource_id) + " does not exist");
			}

			if(NULL != dataSource.get())
			{
				ImportableTableSync::ObjectBySource<StopPointTableSync> stopPoints(*dataSource, Env::GetOfficialEnv());
				std::string stopPointCode1 = ineoStopPointCode;
				std::string stopPointCode2 = synthese::data_exchange::IneoFileFormat::Importer_::MNLP_PREFIX + ineoStopPointCode;
				loadedStopPoints = stopPoints.get(stopPointCode1);

				if(true == loadedStopPoints.empty())
				{
					// IneoFileFormat prefixes Ineo stop code identifiers with the string MNLP_**_
					// So if no direct match is found, add the prefix and search again
					loadedStopPoints = stopPoints.get(stopPointCode2);
				}
			}

			return loadedStopPoints;
		}
}	}

