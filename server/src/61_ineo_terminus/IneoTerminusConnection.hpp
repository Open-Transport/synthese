﻿
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
#include "XmlParser.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
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

			enum Status
			{
				offline,	// The client is not connected
				online,		// The client is connected
				connect		// The client must be connected
			};

		private:
			static boost::shared_ptr<IneoTerminusConnection> _theConnection;

			std::string _ineoPort;

			mutable Status _status;

			typedef std::pair<XMLResults, XMLNode> XMLParserResult;
			
			static XMLNode ParseInput(
				const std::string& xml
			);

			class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
			{
			public:
				tcp_connection(boost::asio::io_service& io_service) : _socket(io_service) {}

				boost::asio::ip::tcp::socket& socket();

				void start();

			private:
				void handle_write(
					const boost::system::error_code& error
				);

				void handle_read(
					const boost::system::error_code& error,
					size_t bytes_transferred
				);

				boost::asio::ip::tcp::socket _socket;
				boost::shared_ptr<boost::asio::streambuf> _buf;
			};

			class tcp_server
			{
			public:
				tcp_server(boost::asio::io_service& io_service, std::string port);

			private:
				void start_accept();

				void handle_accept(
					tcp_connection* new_connection,
					const boost::system::error_code& error
				);

				boost::asio::io_service& _io_service;
				boost::asio::ip::tcp::acceptor _acceptor;
			};


			
		public:
			IneoTerminusConnection();

			/// @name Setters
			//@{
				void setIneoPort(const std::string& value){ _ineoPort = value; }
				void setStatus(const Status& value){ _status = value; }
			//@}

			/// @name Getters
			//@{
				const std::string& getIneoPort() const { return _ineoPort; }
				const Status& getStatus() const { return _status; }
			//@}

//				static void test(const boost::system::error_code& error);
			static void RunThread();

			static void ParameterCallback(
				const std::string& name,
				const std::string& value
			);

			static boost::shared_ptr<IneoTerminusConnection> GetTheConnection(){ return _theConnection; }
		};
}	}

#endif // SYNTHESE_pt_IneoTerminusConnection_hpp__

