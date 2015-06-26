
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
#include "Log.h"
#include "Request.h"
#include "ScenarioSaveAction.h"
#include "ScenarioStopAction.h"
#include "ScenarioTableSync.h"
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
	using namespace messages;
	using namespace pt;
	using namespace util;
	using namespace server;
	
	namespace ineo_terminus
	{
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PORT = "ineo_terminus_port";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_NETWORK = "ineo_terminus_network";
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_TICK_INTERVAL = "ineo_terminus_tick_interval";

		boost::shared_ptr<IneoTerminusConnection> IneoTerminusConnection::_theConnection(new IneoTerminusConnection);
		int IneoTerminusConnection::_idRequest(0);
		

		IneoTerminusConnection::IneoTerminusConnection(
		)
		{
			_ineoNetworkID = 0;
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
				_theConnection->getIneoNetworkID()
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
			)   );
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
				string message(_getXMLHeader());
				message += char(10);
				message += bufStr;

				if (tagName == "CheckStatusRequest")
				{
					if (_checkStatusRequest(childNode))
					{
						replace_all(message, "CheckStatusRequest", "CheckStatusResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "PassengerCreateMessageRequest")
				{
					if (_createMessageRequest(childNode))
					{
						replace_all(message, "PassengerCreateMessageRequest", "PassengerCreateMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "PassengerDeleteMessageRequest")
				{
					if (_deleteMessageRequest(childNode))
					{
						replace_all(message, "PassengerDeleteMessageRequest", "PassengerDeleteMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "DriverCreateMessageRequest")
				{
					if (_createMessageRequest(childNode))
					{
						replace_all(message, "DriverCreateMessageRequest", "DriverCreateMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "DriverDeleteMessageRequest")
				{
					if (_deleteMessageRequest(childNode))
					{
						replace_all(message, "DriverDeleteMessageRequest", "DriverDeleteMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "PpdsCreateMessageRequest")
				{
					if (_createMessageRequest(childNode))
					{
						replace_all(message, "PpdsCreateMessageRequest", "PpdsCreateMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "PpdsDeleteMessageRequest")
				{
					if (_deleteMessageRequest(childNode))
					{
						replace_all(message, "PpdsDeleteMessageRequest", "PpdsDeleteMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "GirouetteCreateMessageRequest")
				{
					if (_createMessageRequest(childNode))
					{
						replace_all(message, "GirouetteCreateMessageRequest", "GirouetteCreateMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "GirouetteDeleteMessageRequest")
				{
					if (_deleteMessageRequest(childNode))
					{
						replace_all(message, "GirouetteDeleteMessageRequest", "GirouetteDeleteMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "SonoPassengerCreateMessageRequest")
				{
					if (_createMessageRequest(childNode))
					{
						replace_all(message, "SonoPassengerCreateMessageRequest", "SonoPassengerCreateMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else if (tagName == "SonoPassengerDeleteMessageRequest")
				{
					if (_deleteMessageRequest(childNode))
					{
						replace_all(message, "SonoPassengerDeleteMessageRequest", "SonoPassengerDeleteMessageResponse");
					}
					else
					{
						message = "";
					}
				}
				else
				{
					util::Log::GetInstance().warn("Ineo Terminus : Parser non codé pour " + tagName);
				}
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
			RegistryKeyType network_id
		):	_io_service(ioService),
			_network_id(network_id),
			_acceptor(ioService, tcp::endpoint(tcp::v4(), lexical_cast<int>(port)))
		{
			start_accept();
		}

		void IneoTerminusConnection::tcp_server::start_accept()
		{
			IneoTerminusConnection::tcp_connection* new_connection = new IneoTerminusConnection::tcp_connection(_io_service, _network_id);

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

		bool IneoTerminusConnection::tcp_connection::_checkStatusRequest(XMLNode node)
		{
			string tagName(node.getName());
			if (tagName != "CheckStatusRequest")
			{
				// tagName is not CheckStatusRequest, this method should not have been called
				return "";
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

			return true;
		}

		bool IneoTerminusConnection::tcp_connection::_createMessageRequest(XMLNode node)
		{
			string tagName(node.getName());
			string messagerieName;
			if (tagName != "PassengerCreateMessageRequest")
			{
				messagerieName = "Passenger";
			}
			else if (tagName != "DriverCreateMessageRequest")
			{
				messagerieName = "Driver";
			}
			else if (tagName != "PpdsCreateMessageRequest")
			{
				messagerieName = "Ppds";
			}
			else if (tagName != "GirouetteCreateMessageRequest")
			{
				messagerieName = "Girouette";
			}
			else if (tagName != "SonoPassengerCreateMessageRequest")
			{
				messagerieName = "SonoPassenger";
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
			ScenarioTableSync::Save(sscenario.get());

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

			return true;
		}

		bool IneoTerminusConnection::tcp_connection::_deleteMessageRequest(XMLNode node)
		{
			string tagName(node.getName());
			string messagerieName;
			if (tagName != "PassengerDeleteMessageRequest")
			{
				messagerieName = "Passenger";
			}
			else if (tagName != "DriverDeleteMessageRequest")
			{
				messagerieName = "Driver";
			}
			else if (tagName != "PpdsDeleteMessageRequest")
			{
				messagerieName = "Ppds";
			}
			else if (tagName != "GirouetteDeleteMessageRequest")
			{
				messagerieName = "Girouette";
			}
			else if (tagName != "SonoPassengerDeleteMessageRequest")
			{
				messagerieName = "SonoPassenger";
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

			return true;
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
				messagerieName == "SonoPassenger")
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
			message.startDate =  XmlToolkit::GetIneoDateTime(
				startDateStr + " " + startTimeStr
			);
			message.stopDate =  XmlToolkit::GetIneoDateTime(
				stopDateStr + " " + stopTimeStr
			);
			if (node.nChildNode("RepeatPeriod") > 0)
			{
				XMLNode repeatPeriodNode = node.getChildNode("RepeatPeriod", 0);
				message.repeatPeriod = lexical_cast<int>(repeatPeriodNode.getText());
			}
			if (messagerieName == "Driver" && message.dispatching == Immediat)
			{
				XMLNode confirmNode = node.getChildNode("Confirm", 0);
				message.confirm = ((string)(confirmNode.getText()) == "oui");
			}
			if (messagerieName == "Passenger" ||
				messagerieName == "SonoPassenger")
			{
				XMLNode inhibitionNode = node.getChildNode("Inhibition", 0);
				message.inhibition = ((string)(inhibitionNode.getText()) == "oui");
			}
			if (messagerieName == "Passenger")
			{
				XMLNode colorNode = node.getChildNode("Color", 0);
				message.color = colorNode.getText();
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
			if (node.nChildNode("EndStopPoint") > 0)
			{
				XMLNode endStopPointNode = node.getChildNode("EndStopPoint", 0);
				message.endStopPoint = endStopPointNode.getText();
			}
			if (messagerieName == "Girouette")
			{
				XMLNode codeNode = node.getChildNode("Code", 0);
				message.codeGirouette = lexical_cast<int>(codeNode.getText());
			}
			XMLNode textNode = node.getChildNode("Text", 0);
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
					BOOST_FOREACH(const CommercialLine::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<CommercialLine>())
					{
						if(it.second->getShortName() == recipient.name &&
							it.second->getNetwork()->getKey())
						{
							boost::shared_ptr<ParametersMap> lineRecipientPM(new ParametersMap);
							lineRecipientPM->insert("recipient_id", it.second->getKey());
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
				else
				{
					util::Log::GetInstance().warn("_addRecipientsPM : Recipient non codé : " + recipient.type);
				}
			}
			pm.insert("displayscreen_recipient", "");
			pm.insert("stoparea_recipient", "");
		}
}	}

