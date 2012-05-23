//
// connection.hpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
// Integrated in SYNTHESE project by Hugues Romain 2009.

#ifndef HTTP_SERVER3_CONNECTION_HPP
#define HTTP_SERVER3_CONNECTION_HPP

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "HTTPReply.hpp"
#include "HTTPRequest.hpp"
#include "HTTPRequestParser.hpp"

namespace synthese
{
	namespace server
	{
		///////////////////////////////////////////////////////////
		/// Represents a single connection from a client.
		/// @ingroup m15
		///
		class HTTPConnection:
			public boost::enable_shared_from_this<HTTPConnection>,
			private boost::noncopyable
		{
			public:
			/// Construct a connection with the given io_service.
			explicit HTTPConnection(
				boost::asio::io_service& io_service,
				void (*handler)(const HTTPRequest& request, HTTPReply& reply)
			);

			/// Get the socket associated with the connection.
			boost::asio::ip::tcp::socket& socket();

			/// Start the first asynchronous operation for the connection.
			void start();

			private:
			/// Handle completion of a read operation.
			void handle_read(const boost::system::error_code& e,
				std::size_t bytes_transferred);

			/// Handle completion of a write operation.
			void handle_write(const boost::system::error_code& e);

			/// Strand to ensure the connection's handlers are not called concurrently.
			boost::asio::io_service::strand strand_;

			/// Socket for the connection.
			boost::asio::ip::tcp::socket socket_;

			/// Buffer for incoming data.
			boost::array<char, 8192> buffer_;

			/// The incoming request.
			HTTPRequest request_;

			/// The parser for the incoming request.
			HTTPRequestParser request_parser_;

			/// The reply to be sent back to the client.
			HTTPReply reply_;

			void (*handler_)(const HTTPRequest& request, HTTPReply& reply);
		};

		typedef boost::shared_ptr<HTTPConnection> connection_ptr;

	} // namespace server
} // namespace http

#endif // HTTP_SERVER3_CONNECTION_HPP
