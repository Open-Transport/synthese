
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

#include "Log.h"
#include "ServerModule.h"
#include "XmlToolkit.h"

#include <boost/algorithm/string.hpp>
#include <boost/bind/bind.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/lambda/lambda.hpp>

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
	using namespace util;
	using namespace server;
	
	namespace ineo_terminus
	{
		const string IneoTerminusConnection::MODULE_PARAM_INEO_TERMINUS_PORT = "ineo_terminus_port";

		boost::shared_ptr<IneoTerminusConnection> IneoTerminusConnection::_theConnection(new IneoTerminusConnection);
		

		IneoTerminusConnection::IneoTerminusConnection(
		)
		{
		}


		void IneoTerminusConnection::RunThread()
		{
			// Open the server socket
			boost::asio::io_service ioService;
			IneoTerminusConnection::tcp_server tcpServer(ioService, _theConnection->getIneoPort());
			ioService.run();
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

			if(	changed
			){
				if(	_theConnection->_ineoPort.empty()
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
				bufStr = bufStr.substr(43, bufStr.size() - 43); // Remove XML header (<?xml ... ?>)

				// Parsing
				XMLNode node(ParseInput(bufStr));
				if(node.isEmpty())
				{
					util::Log::GetInstance().warn("Ineo Terminus : Message XML vide");
					return;
				}
				XMLNode childNode(node.getChildNode(0));
				if(childNode.isEmpty())
				{
					util::Log::GetInstance().warn("Ineo Terminus : Message XML vide sans node 0");
					return;
				}
				string tagName(childNode.getName());
				string message("Message pour Ineo");

				if (tagName == "CheckStatusRequest")
				{
					message = _checkStatusRequest(childNode);
				}
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
				delete this;
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
				delete this;
			}
		}

		IneoTerminusConnection::tcp_server::tcp_server(
			boost::asio::io_service& ioService,
			string port
		): _io_service(ioService),
		   _acceptor(ioService, tcp::endpoint(tcp::v4(), lexical_cast<int>(port)))
		{
			start_accept();
		}

		void IneoTerminusConnection::tcp_server::start_accept()
		{
			IneoTerminusConnection::tcp_connection* new_connection = new IneoTerminusConnection::tcp_connection(_io_service);

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

			stringstream message(_getXMLHeader());
			message << "<CheckStatusResponse><ID>" <<
				idStr <<
				"</ID><RequestID>" <<
				idStr <<
				"</RequestID><ResponseTimeStamp>";
			XmlToolkit::ToXsdDateTime(message, requestTimeStamp); // page 12 de la spec INEO : Horodatage de la demande
			message << "</ResponseTimeStamp><ResponseRef>Terminus</ResponseRef></CheckStatusResponse>" <<
				char(0);

			return message.str();
		}
}	}

