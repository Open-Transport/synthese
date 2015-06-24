
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
				string message("Message pour Ineo");

				if (tagName == "CheckStatusRequest")
				{
					message = _checkStatusRequest(childNode);
				}
				else if (tagName == "PassengerCreateMessageRequest")
				{
					message = _createMessageRequest(childNode);
				}
				else if (tagName == "DriverCreateMessageRequest")
				{
					message = _createMessageRequest(childNode);
				}
				else
				{
					util::Log::GetInstance().warn("Ineo Terminus : Parser non codé pour " + tagName);
				}
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

		string IneoTerminusConnection::tcp_connection::_checkStatusRequest(XMLNode node)
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

			// TO-DO : insert line feeds
			stringstream response(_getXMLHeader());
			response << char(10) <<
				"<CheckStatusResponse>" << char(10) <<
				"  <ID>" << idStr << "</ID>" << char(10) <<
				"  <RequestID>" << idStr << "</RequestID>" << char(10) <<
				"  <ResponseTimeStamp>" << _writeIneoDate(requestTimeStamp) << " " << _writeIneoTime(requestTimeStamp) << "</ResponseTimeStamp>" << char(10) <<
				"  <ResponseRef>Terminus</ResponseRef>" << char(10) <<
				"</CheckStatusResponse>";

			return response.str();
		}

		string IneoTerminusConnection::tcp_connection::_createMessageRequest(XMLNode node)
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
				Messaging message;
				XMLNode MessagingNode = node.getChildNode("Messaging", cptMessagingNode);
				XMLNode nameNode = MessagingNode.getChildNode("Name", 0);
				message.name = _iconv.convert(nameNode.getText());
				XMLNode dispatchingNode = MessagingNode.getChildNode("Dispatching", 0);
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
				XMLNode startDateNode = MessagingNode.getChildNode("StartDate", 0);
				string startDateStr = startDateNode.getText();
				XMLNode stopDateNode = MessagingNode.getChildNode("StopDate", 0);
				string stopDateStr = stopDateNode.getText();
				XMLNode startTimeNode = MessagingNode.getChildNode("StartTime", 0);
				string startTimeStr = startTimeNode.getText();
				XMLNode stopTimeNode = MessagingNode.getChildNode("StopTime", 0);
				string stopTimeStr = stopTimeNode.getText();
				message.startDate =  XmlToolkit::GetIneoDateTime(
					startDateStr + " " + startTimeStr
				);
				message.stopDate =  XmlToolkit::GetIneoDateTime(
					stopDateStr + " " + stopTimeStr
				);
				XMLNode repeatPeriodNode = MessagingNode.getChildNode("RepeatPeriod", 0);
				message.repeatPeriod = lexical_cast<int>(repeatPeriodNode.getText());
				if (messagerieName == "Passenger")
				{
					XMLNode inhibitionNode = MessagingNode.getChildNode("Inhibition", 0);
					message.inhibition = ((string)(inhibitionNode.getText()) == "oui");
					XMLNode colorNode = MessagingNode.getChildNode("Color", 0);
					message.color = colorNode.getText();
				}
				else
				{
					message.inhibition = false;
					message.color = "";
				}
				XMLNode textNode = MessagingNode.getChildNode("Text", 0);
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
				XMLNode RecipientsNode = MessagingNode.getChildNode("Recipients", 0);
				message.recipients = _readRecipients(RecipientsNode);

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

			util::Log::GetInstance().debug("IneoTerminusConnection::_passengerCreateMessageRequest : id " +
				idStr +
				" ; timestamp " +
				RequestTimeStampNode.getText() +
				" ; de " +
				requestorRefStr +
				" avec " +
				lexical_cast<string>(messages.size()) +
				" message(s)"
			);

			stringstream response(_getXMLHeader());
			string responseName = messagerieName + "CreateMessageResponse";
			response << char(10) << responseName << char(10) <<
				"  <ID>" << idStr << "</ID>" << char(10) <<
				"  <RequestID>" << idStr << "</RequestID>" << char(10) <<
				"  <ResponseTimeStamp>" << _writeIneoDate(requestTimeStamp) << " " << _writeIneoTime(requestTimeStamp) << "</ResponseTimeStamp>" << char(10) <<
				"  <ResponseRef>Terminus</ResponseRef>" << char(10);
			BOOST_FOREACH(const Messaging& message, messages)
			{
				response << "  <Messaging>" << char(10) <<
					"    <Name>" << message.name << "</Name>" << char(10) <<
					"    <Dispatching>";
				if (message.dispatching == Immediat)
				{
					response << "Immediat";
				}
				else if (message.dispatching == Differe)
				{
					response << "Differe";
				}
				else if (message.dispatching == Repete)
				{
					response << "Repete";
				}
				response << "</Dispatching>" << char(10) <<
					"    <StartDate>" << _writeIneoDate(message.startDate) << "</StartDate>" << char(10) <<
					"    <StopDate>" << _writeIneoDate(message.stopDate) << "</StopDate>" << char(10) <<
					"    <StartTime>" << _writeIneoTime(message.startDate) << "</StartTime>" << char(10) <<
					"    <StopTime>" << _writeIneoTime(message.stopDate) << "</StopTime>" << char(10) <<
					"    <RepeatPeriod>" << lexical_cast<string>(message.repeatPeriod) << "</RepeatPeriod>" << char(10);
				if (messagerieName == "Passenger")
				{
					response <<
						"    <Inhibition>" << (message.inhibition ? "oui" : "non") << "</Inhibition>" << char(10) <<
						"    <Color>" << message.color << "</Color>" << char(10);
				}
				response <<
					"    <Text>" << char(10);
				string longMessage(message.content);
				stringstream lineSeparator;
				lineSeparator << "</Line>" << char(10) << "      <Line>";
				replace_all(longMessage, "<br />", lineSeparator.str());
				response << "      <Line>" << longMessage << "</Line>" << char(10) <<
					"    </Text>" << char(10) <<
					"    <Recipients>" << char(10) << _writeIneoRecipients(message.recipients) << char(10) << "    </Recipients>" << char(10) <<
					"  </Messaging>" << char(10);
			}
			response << responseName;

			return response.str();
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
				else
				{
					util::Log::GetInstance().warn("_readRecipients : Recipient non codé : " + recipientType);
				}
			}

			return recipients;
		}

		string IneoTerminusConnection::tcp_connection::_writeIneoRecipients(vector<IneoTerminusConnection::Recipient> recipients)
		{
			stringstream strRecipients;
			// All Network
			BOOST_FOREACH(const IneoTerminusConnection::Recipient& recipient, recipients)
			{
				if (recipient.type == "AllNetwork")
				{
					strRecipients << "      <AllNetwork />";
				}
			}

			// Lines
			bool first(true);
			BOOST_FOREACH(const IneoTerminusConnection::Recipient& recipient, recipients)
			{
				if (recipient.type == "Line" && first)
				{
					strRecipients << "      <Lines>" << char(10) <<
						"        <Line>" << recipient.name << "</Line>" << char(10);
					first = false;
				}
				else if (recipient.type == "Line")
				{
					strRecipients << "        <Line>" << recipient.name << "</Line>" << char(10);
				}
			}
			if (!first)
			{
				strRecipients << "      </Lines>";
			}

			// Write warn for non coded recipients
			BOOST_FOREACH(const IneoTerminusConnection::Recipient& recipient, recipients)
			{
				if (recipient.type != "AllNetwork" &&
					recipient.type != "Line")
				{
					util::Log::GetInstance().warn("_writeIneoRecipients : Recipient non codé : " + recipient.type);
				}
			}

			return strRecipients.str();
		}

		string IneoTerminusConnection::tcp_connection::_writeIneoDate(ptime date)
		{
			stringstream str;
			str << setw( 2 ) << setfill ( '0' ) <<
				date.date().day() << "/" <<
				setw( 2 ) << setfill ( '0' ) <<
				static_cast<long>(date.date().month()) << "/" <<
				setw( 2 ) << setfill ( '0' ) <<
				date.date().year();
			return str.str();
		}

		string IneoTerminusConnection::tcp_connection::_writeIneoTime(ptime date)
		{
			stringstream str;
			str << setw( 2 ) << setfill ( '0' ) <<
				date.time_of_day().hours() << ":" <<
				setw( 2 ) << setfill ( '0' ) <<
				date.time_of_day().minutes () << ":" <<
				setw( 2 ) << setfill ( '0' ) <<
				date.time_of_day().seconds();
			return str.str();
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
				else
				{
					util::Log::GetInstance().warn("_addRecipientsPM : Recipient non codé : " + recipient.type);
				}
			}
			pm.insert("displayscreen_recipient", "");
			pm.insert("stoparea_recipient", "");
		}
}	}

