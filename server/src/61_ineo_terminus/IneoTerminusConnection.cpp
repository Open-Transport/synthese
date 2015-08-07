
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
#include "MessagesModule.h"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string/replace.hpp>

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
	
	namespace ineo_terminus
	{
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PORT = "ineo_terminus_port";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_NETWORK = "ineo_terminus_network";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_DATASOURCE = "ineo_terminus_datasource";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_TICK_INTERVAL = "ineo_terminus_tick_interval";
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
			boost::mutex::scoped_lock connectionLock(_connectionsMutex);
			_livingConnections.insert(new_connection);

			util::Log::GetInstance().info("New connection from Ineo SAE (count=" + boost::lexical_cast<std::string>(_livingConnections.size()) + ")");

			if(1 == _livingConnections.size())
			{
				// Upon first connection or reconnection from Ineo SAE, we need to synchronize the messages between the 2 systems
				_synchronizeMessages();
			}
		}


		void IneoTerminusConnection::removeConnection(IneoTerminusConnection::tcp_connection* connection_to_remove)
		{
			boost::mutex::scoped_lock lock(_connectionsMutex);
			_livingConnections.erase(connection_to_remove);
			delete connection_to_remove;
			util::Log::GetInstance().info("Connection to Ineo SAE closed (count=" + boost::lexical_cast<std::string>(_livingConnections.size()) + ")");
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
					(*(_livingConnections.begin()))->sendMessage(firstMessage);
					_messagesToSend.pop_front();

					util::Log::GetInstance().debug("Ineo Terminus : sending message " + firstMessage);
				}
			}
		}


		void IneoTerminusConnection::_synchronizeMessages()
		{
			util::Log::GetInstance().info("Synchronizing messages with Ineo SAE");

			// Delete existing Ineo SAE messages
			if(0 != _ineoDatasource)
			{
				db::DBTransaction transaction;
				boost::shared_ptr<DataSource> ineoDataSource = Env::GetOfficialEnv().getEditable<DataSource>(_ineoDatasource);
				DataSource::LinkedObjects ineoSaeScenarios(ineoDataSource->getLinkedObjects<SentScenario>());

				BOOST_FOREACH(const DataSource::LinkedObjects::value_type& ineoSaeScenario, ineoSaeScenarios)
				{
					SentScenarioTableSync::Remove(NULL, ineoSaeScenario.second->getKey(), transaction, false);
				}

				transaction.run();
			}

			// Lock the message queue and empty it
			boost::recursive_mutex::scoped_lock messagesLock(_messagesMutex);
			_messagesToSend.clear();

			// For each Ineo Notification provider :
			// * build a XXXGetStatesRequest to request active messages from Ineo SAE
			// * build a XXXCreateMessageRequest for each currently active message associated to this provider
			BOOST_FOREACH(const NotificationProvider::Registry::value_type& providerEntry, Env::GetOfficialEnv().getRegistry<NotificationProvider>())
			{
				boost::shared_ptr<NotificationProvider> provider = providerEntry.second;

				if(IneoNotificationChannel::FACTORY_KEY == provider->get<NotificationChannelKey>())
				{
					const ParametersMap& parameters = provider->get<Parameters>();
					MessagesModule::ActivatedMessages providerMessages = MessagesModule::GetActivatedMessages(*provider, ParametersMap());

					if(true == parameters.isDefined(IneoNotificationChannel::PARAMETER_INEO_MESSAGE_TYPE))
					{
						// Build XXXGetStatesRequest and queue it
						std::string ineoMessageType = parameters.getValue(IneoNotificationChannel::PARAMETER_INEO_MESSAGE_TYPE);
						std::string stateRequest = _buildGetStatesRequest(ineoMessageType);
						addMessage(stateRequest);

						util::Log::GetInstance().info("Ineo SAE NotificationProvider " + ineoMessageType + " has " +
													  boost::lexical_cast<std::string>(providerMessages.size()) + " active messages");
					}

					BOOST_FOREACH(const boost::shared_ptr<Alarm> providerMessage, providerMessages)
					{
						// NotificationProvider does not process Alarm, it requires a NotificationEvent
						// Since we don't want to add another NotificationEvent into the database, we create a fake one that is only used to generate the message
						const boost::shared_ptr<NotificationEvent> fakeEvent(new NotificationEvent(0, *providerMessage, *provider, BEGIN));
						provider->notify(fakeEvent);
					}
				}
			}

			util::Log::GetInstance().debug("Synchronization with Ineo SAE ended : " + boost::lexical_cast<std::string>(_messagesToSend.size()) +
										   " messages queued for sending");
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
			requestStream << "<" << requestTag << ">" << char(10);
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
					bool requestSuccessful = _checkStatusRequest(childNode, response);

					if(!requestSuccessful)
					{
						// TODO : log
					}
				}

				else if (_creationRequestTags.end() != _creationRequestTags.find(tagName))
				{
					bool requestSuccessful = _createMessageRequest(childNode, response);

					if(!requestSuccessful)
					{
						// TODO : log
					}
				}

				else if (_deletionRequestTags.end() != _deletionRequestTags.find(tagName))
				{
					bool requestSuccessful = _deleteMessageRequest(childNode, response);

					if(!requestSuccessful)
					{
						// TODO : log
					}
				}

				else if (_creationOrDeletionResponseTags.end() != _creationOrDeletionResponseTags.find(tagName))
				{
					// TODO : error handling
					// If message has 'ErrorType', 'ErrorMessage' and 'ErrorID' nodes then our request was rejected
				}

				else if (_getStatesResponseTags.end() != _getStatesResponseTags.find(tagName))
				{
					bool requestSuccessful = _getStatesResponse(childNode, response);

					if(!requestSuccessful)
					{
						// TODO : log
					}
				}

				else
				{
					util::Log::GetInstance().warn("Ineo Terminus unsupported message : " + tagName);
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
			_acceptor(ioService, tcp::endpoint(tcp::v4(), lexical_cast<int>(port)))
		{
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
			inhibition = false;
			color = "";
			codeGirouette = 0;
			activateHeadJingle = false;
			activateBackJingle = false;
			confirm = false;
			startStopPoint = "";
			endStopPoint = "";
			diodFlashing = false;
			alternance = false;
			multipleStop = false;
			terminusOrStop = false;
			way = "";
			stopPoint = "";
			numberShow = 0;
			ttsBroadcasting = false;
			jingle = false;
			chaining = "";
			priority = false;
			varying = false;
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
			BOOST_FOREACH(const Messaging& message, messages)
			{
				boost::shared_ptr<ParametersMap> periodPM(new ParametersMap);
				periodPM->insert("start_date", boost::gregorian::to_iso_extended_string(message.startDate.date()) +" "+ boost::posix_time::to_simple_string(message.startDate.time_of_day()));
				periodPM->insert("end_date", boost::gregorian::to_iso_extended_string(message.stopDate.date()) +" "+ boost::posix_time::to_simple_string(message.stopDate.time_of_day()));
				periodPM->insert("date", "");

				boost::shared_ptr<ParametersMap> messagePM(new ParametersMap);
				messagePM->insert("title", message.name);
				messagePM->insert("content", message.content);
				messagePM->insert("color", message.color);
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
				messagePM->insert("repeat_interval", lexical_cast<string>(message.repeatPeriod));
				messagePM->insert("inhibition", (message.inhibition ? "oui" : "non"));
				messagePM->insert("section", "");
				messagePM->insert("alternative", "");

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
				messagesAndCalendarsPM->insert("calendar", calendarPM);
			}

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
						scenarioStopAction.setScenario(sscenario);
						Request fakeRequest;
						scenarioStopAction.run(fakeRequest);
					}
					else
					{
						// SYNTHESE has no matching scenario, generate an error
						util::Log::GetInstance().warn("Ineo Terminus : no scenario matching this delete request");
						status = false;
						errorCode = ProgrammationInconnue;
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

			// Check for mandatory nodes
			int numIDNode = node.nChildNode("ID");
			int numRequestTimeStampNode = node.nChildNode("RequestTimeStamp");
			int numRequestorRefNode = node.nChildNode("RequestorRef");
			int numMessagingStatesNode = node.nChildNode("MessagingStates");

			status = ((1 == numIDNode) && (1 == numRequestTimeStampNode) && (1 == numRequestorRefNode) && (1 == numMessagingStatesNode));
			if(false == status)
			{
				// Message is ill-formed, reply to Ineo with an error
				util::Log::GetInstance().warn("Ineo Terminus : message misses mandatory nodes");
				return status;
			}

			// Extract Ineo message type from tag name
			std::string tagName(node.getName());
			std::string ineoMessageType = tagName.substr(0, tagName.find("GetStatesResponse"));
			RegistryKeyType fakeBroadCastPoint = _fakeBroadcastPoints.at(ineoMessageType);

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
			int numActiveMessages = 0;

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
					vector<Messaging> messages;
					messages.push_back(message);

					// Creation of a scenario and a message in SYNTHESE
					// Note : Ineo does not expect SYNTHESE to reply to a XXXGetStatesResponse, so we do not create an error response
					IneoApplicationError unused = AucuneErreur;
					_createMessages(messages, fakeBroadCastPoint, unused);

					numActiveMessages++;
				}
			}

			util::Log::GetInstance().debug(
				"Ineo Terminus processing message " + tagName + " : id " + idStr + " ; request id " + requestIdStr + " ; timestamp " +
				boost::posix_time::to_simple_string(responseTimeStamp) + " ; from " + responseRefStr + " with " + lexical_cast<string>(numActiveMessages) + " active message(s)"
			);

			return status;
		}


		std::string IneoTerminusConnection::tcp_connection::_generateResponse(XMLNode& requestNode, const IneoApplicationError& errorCode)
		{
			std::stringstream responseStream;

			string requestTag = requestNode.getName();
			string requestId = requestNode.getChildNode("ID", 0).getText();
			string requestTimestamp = requestNode.getChildNode("RequestTimeStamp", 0).getText();

			// Build the response tag
			std::string responseTag = requestTag;
			replace_all(responseTag, "Request", "Response");

			// Build the response header
			responseStream << INEO_TERMINUS_XML_HEADER << char(10);
			responseStream << "<" << responseTag << ">" << char(10);
			responseStream << "\t<ID>" << boost::lexical_cast<std::string>(IneoTerminusConnection::GetTheConnection()->getNextRequestID()) << "</ID>" << char(10);
			responseStream << "\t<RequestID>" << requestId << "</RequestID>" << char(10);
			// Note : according to interface description 'ResponseTimeStamp' = 'RequestTimeStamp'
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
			string startDateStr("01/01/1970");
			string stopDateStr("31/12/2037");
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
			string startTimeStr("00:00:00");
			string stopTimeStr("23:59:00");
			if (node.nChildNode("StartTime") > 0)
			{
				XMLNode startTimeNode = node.getChildNode("StartTime", 0);
				startTimeStr = startTimeNode.getText();
			}
			if (node.nChildNode("StopTime") > 0)
			{
				XMLNode stopTimeNode = node.getChildNode("StopTime", 0);
				stopTimeStr = stopTimeNode.getText();
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
			}
			XMLNode RecipientsNode = node.getChildNode("Recipients", 0);
			message.recipients = _readRecipients(RecipientsNode);

			return message;
		}


		bool IneoTerminusConnection::tcp_connection::_createMessages(std::vector<Messaging> messages, RegistryKeyType fakeBroadCastPoint, IneoApplicationError& errorCode)
		{
			bool status = true;
			boost::shared_ptr<ParametersMap> messagesAndCalendarsPM(new ParametersMap);

			// Fill the parameters map from the vector of messages
			BOOST_FOREACH(const Messaging& message, messages)
			{
				boost::shared_ptr<ParametersMap> periodPM(new ParametersMap);
				periodPM->insert("start_date", boost::gregorian::to_iso_extended_string(message.startDate.date()) +" "+ boost::posix_time::to_simple_string(message.startDate.time_of_day()));
				periodPM->insert("end_date", boost::gregorian::to_iso_extended_string(message.stopDate.date()) +" "+ boost::posix_time::to_simple_string(message.stopDate.time_of_day()));
				periodPM->insert("date", "");

				boost::shared_ptr<ParametersMap> messagePM(new ParametersMap);
				messagePM->insert("title", message.name);
				messagePM->insert("content", message.content);
				messagePM->insert("color", message.color);
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
				messagePM->insert("repeat_interval", lexical_cast<string>(message.repeatPeriod));
				messagePM->insert("inhibition", (message.inhibition ? "oui" : "non"));
				messagePM->insert("section", "");
				messagePM->insert("alternative", "");

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
				messagesAndCalendarsPM->insert("calendar", calendarPM);
			}

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
						scenario = static_pointer_cast<Scenario, SentScenario>(sscenario);
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
					util::Log::GetInstance().warn("Ineo Terminus : an error occurred while trying to create a message");
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
			recipientTypes.insert(make_pair("LinesWays", "LineWay"));
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

				// Generic case : recipients are listed by class
				else if (recipientTypes.end() != recipientTypes.find(recipientType))
				{
					const std::string expectedType = recipientTypes.at(recipientType);
					int nChildNodes = recipientNode.nChildNode();

					for (int cptChildNode = 0; cptChildNode < nChildNodes; cptChildNode++)
					{
						XMLNode childNode = recipientNode.getChildNode(cptChildNode);
						string childNodeType(childNode.getName());

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
				if (recipient.type == "AllNetwork")
				{
					boost::shared_ptr<ParametersMap> lineRecipientPM(new ParametersMap);
					lineRecipientPM->insert("recipient_id", _network_id);
					pm.insert("line_recipient", lineRecipientPM);
				}

				else if (recipient.type == "Line")
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

				else if (recipient.type == "StopPoint")
				{
					if(NULL == dataSource.get())
					{
						status = false;
						errorCode = ArretInconnu;
					}

					else
					{
						ImportableTableSync::ObjectBySource<StopPointTableSync> stopPoints(*dataSource, Env::GetOfficialEnv());
						set<StopPoint*> loadedStopPoints(stopPoints.get(recipient.name));

						BOOST_FOREACH(StopPoint* loadedStopPoint, loadedStopPoints)
						{
							boost::shared_ptr<ParametersMap> stopRecipientPM(new ParametersMap);
							stopRecipientPM->insert("recipient_id", loadedStopPoint->getKey());
							pm.insert("stoparea_recipient", stopRecipientPM);
						}

						if(true == loadedStopPoints.empty())
						{
							util::Log::GetInstance().warn("Ineo Terminus : stop not found " + recipient.name);
							status = false;
							errorCode = ArretInconnu;
						}
					}
				}

				// The following recipient types are not processed by SYNTHESE
				else if (
						  (recipient.type == "Vehicule") ||
						  (recipient.type == "Car") ||
						  (recipient.type == "CarService") ||
						  (recipient.type == "LineWay") ||
						  (recipient.type == "Biv") ||
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
}	}

