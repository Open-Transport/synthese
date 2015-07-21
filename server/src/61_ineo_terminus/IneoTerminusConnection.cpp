
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
#include "DataSourceTableSync.h"
#include "ImportableTableSync.hpp"
#include "Log.h"
#include "Request.h"
#include "ScenarioSaveAction.h"
#include "ScenarioStopAction.h"
#include "SentScenarioTableSync.h"
#include "ServerModule.h"
#include "TransportNetwork.h"
#include "XmlToolkit.h"

#include <boost/algorithm/string.hpp>
#include <boost/bind/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace boost;
using namespace boost::algorithm;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::gregorian;
using namespace boost::lambda;
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

		boost::shared_ptr<IneoTerminusConnection> IneoTerminusConnection::_theConnection(new IneoTerminusConnection);
		int IneoTerminusConnection::_idRequest(0);
		

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

		void IneoTerminusConnection::addConnection(IneoTerminusConnection::tcp_connection* new_connection)
		{
			_livingConnections.insert(new_connection);
		}

		void IneoTerminusConnection::removeConnection(IneoTerminusConnection::tcp_connection* connection_to_remove)
		{
			_livingConnections.erase(connection_to_remove);
		}

		void IneoTerminusConnection::addMessage(string new_message)
		{
			_messagesToSend.insert(new_message);
		}

		void IneoTerminusConnection::MessageSender()
		{
			// On consomme ici les messages PUSH vers Ineo SAE
			// Main loop
			while (true)
			{
				// If we have an active connection, we send the first message by this way
				if (!_theConnection->_livingConnections.empty())
				{
					if (!_theConnection->_messagesToSend.empty())
					{
						(*(_theConnection->_livingConnections.begin()))->sendMessage(*(_theConnection->_messagesToSend.begin()));
						_theConnection->_messagesToSend.erase(_theConnection->_messagesToSend.begin());
					}
				}

				// Sleep
				boost::this_thread::sleep(boost::posix_time::seconds(_theConnection->getIneoTickInterval()));
			}
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
				string bufStr;
				istream is(_buf.get());
				getline(is, bufStr, char(0));
				trim(bufStr);
				if(bufStr.empty())
				{
					return;
				}

				string singleLine(bufStr);
				// Log for debug
				util::Log::GetInstance().debug("Ineo Terminus : on a recu ca : " + singleLine);
				if (bufStr.substr(0,43) != "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>")
				{
					util::Log::GetInstance().warn("Ineo Terminus : message XML sans entete");
					// TO-DO : insert char(10) (line feed) in error responses
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
					return;
				}
				bufStr = bufStr.substr(43, bufStr.size() - 43); // Remove XML header (<?xml ... ?>)

				// Parsing
				XMLNode node(ParseInput(bufStr));
				if(node.isEmpty())
				{
					util::Log::GetInstance().warn("Ineo Terminus : Message XML vide");
					// TO-DO : insert char(10) (line feed) in error responses
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
					return;
				}
				XMLNode childNode(node.getChildNode(0));
				if(childNode.isEmpty())
				{
					util::Log::GetInstance().warn("Ineo Terminus : Message XML vide sans node 0");
					// TO-DO : insert char(10) (line feed) in error responses
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
					return;
				}

				string tagName(childNode.getName());
				string response = "";

				std::set<std::string> creationRequestTags;
				std::set<std::string> deletionRequestTags;

				creationRequestTags.insert("PassengerCreateMessageRequest");
				creationRequestTags.insert("DriverCreateMessageRequest");
				creationRequestTags.insert("PpdsCreateMessageRequest");
				creationRequestTags.insert("GirouetteCreateMessageRequest");
				creationRequestTags.insert("SonoPassengerCreateMessageRequest");
				creationRequestTags.insert("SonoDriverCreateMessageRequest");
				creationRequestTags.insert("SonoStopPointCreateMessageRequest");
				creationRequestTags.insert("BivGeneralCreateMessageRequest");
				creationRequestTags.insert("BivLineManCreateMessageRequest");
				creationRequestTags.insert("BivLineAutoCreateMessageRequest");

				deletionRequestTags.insert("PassengerDeleteMessageRequest");
				deletionRequestTags.insert("DriverDeleteMessageRequest");
				deletionRequestTags.insert("PpdsDeleteMessageRequest");
				deletionRequestTags.insert("GirouetteDeleteMessageRequest");
				deletionRequestTags.insert("SonoPassengerDeleteMessageRequest");
				deletionRequestTags.insert("SonoDriverDeleteMessageRequest");
				deletionRequestTags.insert("SonoStopPointDeleteMessageRequest");
				deletionRequestTags.insert("BivGeneralDeleteMessageRequest");
				deletionRequestTags.insert("BivLineManDeleteMessageRequest");
				deletionRequestTags.insert("BivLineAutoDeleteMessageRequest");

				if (tagName == "CheckStatusRequest")
				{
					bool requestSuccessful = _checkStatusRequest(childNode, response);

					if(!requestSuccessful)
					{
						// TODO : log
					}
				}

				else if (creationRequestTags.end() != creationRequestTags.find(tagName))
				{
					bool requestSuccessful = _createMessageRequest(childNode, response);

					if(!requestSuccessful)
					{
						// TODO : log
					}
				}

				else if (deletionRequestTags.end() != deletionRequestTags.find(tagName))
				{
					bool requestSuccessful = _deleteMessageRequest(childNode, response);

					if(!requestSuccessful)
					{
						// TODO : log
					}
				}

				else
				{
					util::Log::GetInstance().warn("Ineo Terminus : Parser non codé pour " + tagName);
				}

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
			else
			{
				IneoTerminusConnection::GetTheConnection()->removeConnection(this);
			}
		}

		void IneoTerminusConnection::tcp_connection::handle_write
		(
			const boost::system::error_code& error
		)
		{
			if (!error)
			{
				boost::asio::async_read_until(
					_socket,
					*_buf,
					char(0),
					boost::bind(
						&tcp_connection::handle_read,
						this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred
					)
				);
			}
			else
			{
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

		// Response generators
		string IneoTerminusConnection::tcp_connection::_getXMLHeader()
		{
			return "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>";
		}

		bool IneoTerminusConnection::tcp_connection::_checkStatusRequest(XMLNode& node, std::string& response)
		{
			string tagName(node.getName());
			if (tagName != "CheckStatusRequest")
			{
				// tagName is not CheckStatusRequest, this method should not have been called
				return false;
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

			util::Log::GetInstance().debug("IneoTerminusConnection::_checkStatusRequest : id " +
				idStr +
				" ; timestamp " +
				RequestTimeStampNode.getText() +
				" ; de " +
				requestorRefStr
			);

			// Generate the XML response to this request
			response = _generateResponse(node);

			return true;
		}

		bool IneoTerminusConnection::tcp_connection::_createMessageRequest(XMLNode& node, std::string& response)
		{
			string tagName(node.getName());
			string messagerieName;
			if (tagName == "PassengerCreateMessageRequest")
			{
				messagerieName = "Passenger";
			}
			else if (tagName == "DriverCreateMessageRequest")
			{
				messagerieName = "Driver";
			}
			else if (tagName == "PpdsCreateMessageRequest")
			{
				messagerieName = "Ppds";
			}
			else if (tagName == "GirouetteCreateMessageRequest")
			{
				messagerieName = "Girouette";
			}
			else if (tagName == "SonoPassengerCreateMessageRequest")
			{
				messagerieName = "SonoPassenger";
			}
			else if (tagName == "SonoDriverCreateMessageRequest")
			{
				messagerieName = "SonoDriver";
			}
			else if (tagName == "SonoStopPointCreateMessageRequest")
			{
				messagerieName = "SonoStopPoint";
			}
			else if (tagName == "BivGeneralCreateMessageRequest")
			{
				messagerieName = "BivGeneral";
			}
			else if (tagName == "BivLineManCreateMessageRequest")
			{
				messagerieName = "BivLineMan";
			}
			else if (tagName == "BivLineAutoCreateMessageRequest")
			{
				messagerieName = "BivLineAuto";
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

			int numMessagingNode = node.nChildNode("Messaging");
			vector<Messaging> messages;
			for (int cptMessagingNode = 0;cptMessagingNode<numMessagingNode;cptMessagingNode++)
			{
				XMLNode MessagingNode = node.getChildNode("Messaging", cptMessagingNode);
				Messaging message = _readMessagingNode(MessagingNode, messagerieName);
				messages.push_back(message);
			}

			// Creation of a scenario and a message using ScenarioSaveAction
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
				_addRecipientsPM(*messagePM, message.recipients);
				boost::shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				calendarPM->insert("period", periodPM);
				calendarPM->insert("message", messagePM);
				messagesAndCalendarsPM->insert("calendar", calendarPM);
			}

			stringstream stream;
			messagesAndCalendarsPM->outputJSON(stream, "");
			boost::property_tree::ptree messagesAndCalendars;
			boost::property_tree::json_parser::read_json(stream, messagesAndCalendars);
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
			SentScenarioTableSync::Save(sscenario.get());

			util::Log::GetInstance().debug("IneoTerminusConnection::_createMessageRequest : id " +
				idStr +
				" ; timestamp " +
				RequestTimeStampNode.getText() +
				" ; de " +
				requestorRefStr +
				" avec " +
				lexical_cast<string>(messages.size()) +
				" message(s)"
			);

			// Generate the XML response to this request
			response = _generateResponse(node);

			return true;
		}

		bool IneoTerminusConnection::tcp_connection::_deleteMessageRequest(XMLNode& node, std::string& response)
		{
			string tagName(node.getName());
			string messagerieName;
			if (tagName == "PassengerDeleteMessageRequest")
			{
				messagerieName = "Passenger";
			}
			else if (tagName == "DriverDeleteMessageRequest")
			{
				messagerieName = "Driver";
			}
			else if (tagName == "PpdsDeleteMessageRequest")
			{
				messagerieName = "Ppds";
			}
			else if (tagName == "GirouetteDeleteMessageRequest")
			{
				messagerieName = "Girouette";
			}
			else if (tagName == "SonoPassengerDeleteMessageRequest")
			{
				messagerieName = "SonoPassenger";
			}
			else if (tagName == "SonoDriverDeleteMessageRequest")
			{
				messagerieName = "SonoDriver";
			}
			else if (tagName == "SonoStopPointDeleteMessageRequest")
			{
				messagerieName = "SonoStopPoint";
			}
			else if (tagName == "BivGeneralDeleteMessageRequest")
			{
				messagerieName = "BivGeneral";
			}
			else if (tagName == "BivLineManDeleteMessageRequest")
			{
				messagerieName = "BivLineMan";
			}
			else if (tagName == "BivLineAutoDeleteMessageRequest")
			{
				messagerieName = "BivLineAuto";
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

			int numMessagingNode = node.nChildNode("Messaging");
			vector<Messaging> messages;
			for (int cptMessagingNode = 0;cptMessagingNode<numMessagingNode;cptMessagingNode++)
			{
				XMLNode MessagingNode = node.getChildNode("Messaging", cptMessagingNode);
				Messaging message = _readMessagingNode(MessagingNode, messagerieName);
				messages.push_back(message);
			}

			// Find the scenario ScenarioStopAction
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
				_addRecipientsPM(*messagePM, message.recipients);
				boost::shared_ptr<ParametersMap> calendarPM(new ParametersMap);
				calendarPM->insert("period", periodPM);
				calendarPM->insert("message", messagePM);
				messagesAndCalendarsPM->insert("calendar", calendarPM);
			}

			stringstream stream;
			messagesAndCalendarsPM->outputJSON(stream, "");
			boost::property_tree::ptree messagesAndCalendars;
			boost::property_tree::json_parser::read_json(stream, messagesAndCalendars);
			ScenarioStopAction scenarioStopAction;
			SentScenario* sscenario = scenarioStopAction.findScenarioByMessagesAndCalendars(boost::optional<boost::property_tree::ptree>(messagesAndCalendars));
			if (sscenario)
			{
				scenarioStopAction.setScenario(sscenario);
				Request fakeRequest;
				scenarioStopAction.run(fakeRequest);
			}
			else
			{
				util::Log::GetInstance().warn("IneoTerminusConnection : requete Delete non prise en compte car evenement non trouvé dans Terminus");
			}

			// Generate the XML response to this request
			response = _generateResponse(node);

			return true;
		}

		std::string IneoTerminusConnection::tcp_connection::_generateResponse(XMLNode& requestNode)
		{
			std::stringstream responseStream;

			string requestTag = requestNode.getName();
			string requestId = requestNode.getChildNode("ID", 0).getText();
			string requestTimestamp = requestNode.getChildNode("RequestTimeStamp", 0).getText();

			// Build the response tag
			std::string responseTag = requestTag;
			replace_all(responseTag, "Request", "Response");

			// Build the response header
			responseStream << _getXMLHeader() << char(10);
			responseStream << "<" << responseTag << ">" << char(10);
			responseStream << "\t<ID>" << boost::lexical_cast<std::string>(GetNextRequestID()) << "</ID>" << char(10);
			responseStream << "\t<RequestID>" << requestId << "</RequestID>" << char(10);
			// Note : according to interface description 'ResponseTimeStamp' = 'RequestTimeStamp'
			responseStream << "\t<ResponseTimeStamp>" << requestTimestamp << "</ResponseTimeStamp>" << char(10);
			responseStream << "\t<ResponseRef>Terminus</ResponseRef>" << char(10);

			// Copy the content of 'Messaging' nodes
			int messagingCount = requestNode.nChildNode("Messaging");
			for (int cptMessaging = 0; cptMessaging < messagingCount; cptMessaging++)
			{
				XMLNode messagingNode = requestNode.getChildNode("Messaging", cptMessaging);
				int tabDepth = 2;
				_copyXMLNode(messagingNode, tabDepth, responseStream);
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

		vector<IneoTerminusConnection::Recipient> IneoTerminusConnection::tcp_connection::_readRecipients(XMLNode node)
		{
			vector<IneoTerminusConnection::Recipient> recipients;
			int nChildNode = node.nChildNode();
			for (int cptChildNode = 0;cptChildNode<nChildNode;cptChildNode++)
			{
				XMLNode recipientNode = node.getChildNode(cptChildNode);
				string recipientType(recipientNode.getName());
				if (recipientType == "AllNetwork")
				{
					IneoTerminusConnection::Recipient new_recipient;
					new_recipient.type = "AllNetwork";
					new_recipient.name = "AllNetwork";
					recipients.push_back(new_recipient);
				}
				else if (recipientType == "Lines")
				{
					int nLineNode = recipientNode.nChildNode();
					for (int cptLineNode = 0;cptLineNode<nLineNode;cptLineNode++)
					{
						XMLNode lineNode = recipientNode.getChildNode(cptLineNode);
						string recipientLineType(lineNode.getName());
						if (recipientLineType == "Line")
						{
							string lineShortName = lineNode.getText();
							IneoTerminusConnection::Recipient new_recipient;
							new_recipient.type = "Line";
							new_recipient.name = lineShortName;
							recipients.push_back(new_recipient);
						}
						else
						{
							util::Log::GetInstance().warn("IneoTerminusConnection : Un noeud " + recipientLineType + " est fils d'un noeud Lines");
						}
					}
				}
				else if (recipientType == "Vehicles")
				{
					int nVehicleNode = recipientNode.nChildNode();
					for (int cptVehicleNode = 0;cptVehicleNode<nVehicleNode;cptVehicleNode++)
					{
						XMLNode vehicleNode = recipientNode.getChildNode(cptVehicleNode);
						string recipientVehicleType(vehicleNode.getName());
						if (recipientVehicleType == "Vehicle")
						{
							string vehicleNumber = vehicleNode.getText();
							IneoTerminusConnection::Recipient new_recipient;
							new_recipient.type = "Vehicle";
							new_recipient.name = vehicleNumber;
							recipients.push_back(new_recipient);
						}
						else
						{
							util::Log::GetInstance().warn("IneoTerminusConnection : Un noeud " + recipientVehicleType + " est fils d'un noeud Vehicles");
						}
					}
				}
				else if (recipientType == "Cars")
				{
					int nCarNode = recipientNode.nChildNode();
					for (int cptCarNode = 0;cptCarNode<nCarNode;cptCarNode++)
					{
						XMLNode carNode = recipientNode.getChildNode(cptCarNode);
						string recipientCarType(carNode.getName());
						if (recipientCarType == "Car")
						{
							string carNumber = carNode.getText();
							IneoTerminusConnection::Recipient new_recipient;
							new_recipient.type = "Car";
							new_recipient.name = carNumber;
							recipients.push_back(new_recipient);
						}
						else
						{
							util::Log::GetInstance().warn("IneoTerminusConnection : Un noeud " + recipientCarType + " est fils d'un noeud Cars");
						}
					}
				}
				else if (recipientType == "CarServices")
				{
					int nCarServiceNode = recipientNode.nChildNode();
					for (int cptCarServiceNode = 0;cptCarServiceNode<nCarServiceNode;cptCarServiceNode++)
					{
						XMLNode carServiceNode = recipientNode.getChildNode(cptCarServiceNode);
						string recipientCarServiceType(carServiceNode.getName());
						if (recipientCarServiceType == "CarService")
						{
							string carServiceNumber = carServiceNode.getText();
							IneoTerminusConnection::Recipient new_recipient;
							new_recipient.type = "CarService";
							new_recipient.name = carServiceNumber;
							recipients.push_back(new_recipient);
						}
						else
						{
							util::Log::GetInstance().warn("IneoTerminusConnection : Un noeud " + recipientCarServiceType + " est fils d'un noeud CarServices");
						}
					}
				}
				else if (recipientType == "LinesWays")
				{
					int nLineWayNode = recipientNode.nChildNode();
					for (int cptLineWayNode = 0;cptLineWayNode<nLineWayNode;cptLineWayNode++)
					{
						XMLNode lineWayNode = recipientNode.getChildNode(cptLineWayNode);
						string recipientLineWayType(lineWayNode.getName());
						if (recipientLineWayType == "LineWay")
						{
							string lineWayNumber = lineWayNode.getText();
							IneoTerminusConnection::Recipient new_recipient;
							new_recipient.type = "LineWay";
							new_recipient.name = lineWayNumber;
							recipients.push_back(new_recipient);
						}
						else
						{
							util::Log::GetInstance().warn("IneoTerminusConnection : Un noeud " + recipientLineWayType + " est fils d'un noeud LinesWays");
						}
					}
				}
				else if (recipientType == "Bivs")
				{
					int nBivNode = recipientNode.nChildNode();
					for (int cptBivNode = 0;cptBivNode<nBivNode;cptBivNode++)
					{
						XMLNode bivNode = recipientNode.getChildNode(cptBivNode);
						string recipientBivType(bivNode.getName());
						if (recipientBivType == "Biv")
						{
							string bivNumber = bivNode.getText();
							IneoTerminusConnection::Recipient new_recipient;
							new_recipient.type = "Biv";
							new_recipient.name = bivNumber;
							recipients.push_back(new_recipient);
						}
						else
						{
							util::Log::GetInstance().warn("IneoTerminusConnection : Un noeud " + recipientBivType + " est fils d'un noeud Bivs");
						}
					}
				}
				else if (recipientType == "Groups")
				{
					int nGroupNode = recipientNode.nChildNode();
					for (int cptGroupNode = 0;cptGroupNode<nGroupNode;cptGroupNode++)
					{
						XMLNode groupNode = recipientNode.getChildNode(cptGroupNode);
						string recipientGroupType(groupNode.getName());
						if (recipientGroupType == "Group")
						{
							string groupNumber = groupNode.getText();
							IneoTerminusConnection::Recipient new_recipient;
							new_recipient.type = "Group";
							new_recipient.name = groupNumber;
							recipients.push_back(new_recipient);
						}
						else
						{
							util::Log::GetInstance().warn("IneoTerminusConnection : Un noeud " + recipientGroupType + " est fils d'un noeud Groups");
						}
					}
				}
				else
				{
					util::Log::GetInstance().warn("_readRecipients : Recipient non codé : " + recipientType);
				}
			}

			return recipients;
		}

		void IneoTerminusConnection::tcp_connection::_addRecipientsPM(ParametersMap& pm, vector<IneoTerminusConnection::Recipient> recipients)
		{
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
					bool found(false);
					boost::shared_ptr<const impex::DataSource> dataSource = DataSourceTableSync::Get(
						_datasource_id,
						Env::GetOfficialEnv()
					);
					ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(*dataSource, Env::GetOfficialEnv());
					set<CommercialLine*> loadedLines(lines.get(recipient.name));
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
						util::Log::GetInstance().warn("Ineo Terminus : Ligne non trouvée " + recipient.name);
						pm.insert("line_recipient", "");
					}
				}
				else if (recipient.type == "Vehicle")
				{
					// A priori on ne fait rien pour les recipient Vehicle dans Synthese
					util::Log::GetInstance().debug("_addRecipientsPM : Message concerne recipient vehicle");
				}
				else if (recipient.type == "Car")
				{
					// A priori on ne fait rien pour les recipient Car dans Synthese
					util::Log::GetInstance().debug("_addRecipientsPM : Message concerne recipient car");
				}
				else if (recipient.type == "CarService")
				{
					// A priori on ne fait rien pour les recipient CarService dans Synthese
					util::Log::GetInstance().debug("_addRecipientsPM : Message concerne recipient carService");
				}
				else if (recipient.type == "LineWay")
				{
					// A priori on ne fait rien pour les recipient LineWay dans Synthese
					util::Log::GetInstance().debug("_addRecipientsPM : Message concerne recipient LineWay");
				}
				else if (recipient.type == "Biv")
				{
					// A priori on ne fait rien pour les recipient Biv dans Synthese
					util::Log::GetInstance().debug("_addRecipientsPM : Message concerne recipient Biv");
				}
				else if (recipient.type == "Group")
				{
					// A priori on ne fait rien pour les recipient Group dans Synthese
					util::Log::GetInstance().debug("_addRecipientsPM : Message concerne recipient Group");
				}
				else
				{
					util::Log::GetInstance().warn("_addRecipientsPM : Recipient non codé : " + recipient.type);
				}
			}
			pm.insert("displayscreen_recipient", "");
			pm.insert("stoparea_recipient", "");
		}
}	}

