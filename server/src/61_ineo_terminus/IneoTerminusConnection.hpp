
/** IneoTerminusConnection class header.
	@file IneoTerminusConnection.hpp

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

#ifndef SYNTHESE_pt_IneoTerminusConnection_hpp__
#define SYNTHESE_pt_IneoTerminusConnection_hpp__

#include "IConv.hpp"
#include "ParametersMap.h"
#include "UtilTypes.h"
#include "XmlParser.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <set>
#include <string>

namespace synthese
{

	namespace ineo_terminus
	{
		/** IneoTerminusConnection class.
			@ingroup m35
		*/
		class IneoTerminusConnection
		{
		public:
			static const std::string MODULE_PARAM_INEO_TERMINUS_PORT;
			static const std::string MODULE_PARAM_INEO_TERMINUS_NETWORK;
			static const std::string MODULE_PARAM_INEO_TERMINUS_TICK_INTERVAL;

			enum Status
			{
				offline,	// The client is not connected
				online,		// The client is connected
				connect		// The client must be connected
			};

		private:
			static boost::shared_ptr<IneoTerminusConnection> _theConnection;
			static int _idRequest;

			std::string _ineoPort;
			util::RegistryKeyType _ineoNetworkID;
			int _ineoTickInterval;

			mutable Status _status;

			typedef std::pair<XMLResults, XMLNode> XMLParserResult;

			struct Recipient
			{
				std::string type;
				std::string name;
			};
			
			static XMLNode ParseInput(
				const std::string& xml
			);

			class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
			{
			public:
				tcp_connection(
					boost::asio::io_service& io_service,
					util::RegistryKeyType network_id
				) :	_socket(io_service),
					_network_id(network_id),
					_iconv("ISO-8859-1","UTF-8") {}

				boost::asio::ip::tcp::socket& socket();

				void start();

				void sendMessage(
					std::string message
				);

			private:
				void handle_write(
					const boost::system::error_code& error
				);

				void handle_read(
					const boost::system::error_code& error,
					size_t bytes_transferred
				);

				boost::asio::ip::tcp::socket _socket;
				util::RegistryKeyType _network_id;
				util::IConv _iconv;
				boost::shared_ptr<boost::asio::streambuf> _buf;

				// Response generators
				std::string _checkStatusRequest(
					XMLNode node
				);
				std::string _passengerCreateMessageRequest(
					XMLNode node
				);

				// generic parsers
				std::string _getXMLHeader();
				std::vector<IneoTerminusConnection::Recipient> _readRecipients(XMLNode node);

				//generic writers
				std::string _writeIneoRecipients(std::vector<IneoTerminusConnection::Recipient>);
				std::string _writeIneoDate(boost::posix_time::ptime date);
				std::string _writeIneoTime(boost::posix_time::ptime date);
				void _addRecipientsPM(util::ParametersMap& pm, std::vector<IneoTerminusConnection::Recipient>);

				//generic enums
				typedef enum
				{
					Immediat = 0,
					Differe = 1,
					Repete = 2
				} Dispatching;

				// generic structs
				struct Messaging
				{
					std::string name;
					Dispatching dispatching;
					boost::posix_time::ptime startDate;
					boost::posix_time::ptime stopDate;
					int repeatPeriod;
					bool inhibition;
					std::string color;
					std::string content;
					std::vector<IneoTerminusConnection::Recipient> recipients;
				};
			};

			class tcp_server
			{
			public:
				tcp_server(
					boost::asio::io_service& io_service,
					std::string port,
					util::RegistryKeyType network_id
				);

			private:
				void start_accept();

				void handle_accept(
					tcp_connection* new_connection,
					const boost::system::error_code& error
				);

				boost::asio::io_service& _io_service;
				util::RegistryKeyType _network_id;
				boost::asio::ip::tcp::acceptor _acceptor;
			};

			std::set<IneoTerminusConnection::tcp_connection*> _livingConnections;
			std::set<std::string> _messagesToSend;
			
		public:
			IneoTerminusConnection();

			/// @name Setters
			//@{
				void setIneoPort(const std::string& value){ _ineoPort = value; }
				void setIneoNetworkID(const util::RegistryKeyType& value){ _ineoNetworkID = value; }
				void setIneoTickInterval(const int value) { _ineoTickInterval = value; }
				void setStatus(const Status& value){ _status = value; }
			//@}

			/// @name Getters
			//@{
				const std::string& getIneoPort() const { return _ineoPort; }
				const util::RegistryKeyType& getIneoNetworkID() const { return _ineoNetworkID; }
				int getIneoTickInterval() const { return _ineoTickInterval; }
				const Status& getStatus() const { return _status; }
			//@}
			
			void addConnection(tcp_connection* new_connection);
			void removeConnection(tcp_connection* connection_to_remove);

			void addMessage(std::string new_message);

			static void RunThread();

			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);

			static void MessageSender();

			static boost::shared_ptr<IneoTerminusConnection> GetTheConnection(){ return _theConnection; }
			static int GetNextRequestID(){ _idRequest++; return _idRequest; }
		};
}	}

#endif // SYNTHESE_pt_IneoTerminusConnection_hpp__

