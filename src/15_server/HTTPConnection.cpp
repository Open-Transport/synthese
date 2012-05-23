//
// connection.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "HTTPConnection.hpp"

#include <vector>
#include <boost/bind.hpp>

namespace synthese
{
	namespace server
	{

		HTTPConnection::HTTPConnection(
			boost::asio::io_service& io_service,
			void (*handler)(const HTTPRequest& request, HTTPReply& reply)
		):	strand_(io_service),
			socket_(io_service),
			handler_(handler)
		{
		}



		boost::asio::ip::tcp::socket& HTTPConnection::socket()
		{
			return socket_;
		}



		void HTTPConnection::start()
		{
			socket_.async_read_some(boost::asio::buffer(buffer_),
				strand_.wrap(
					boost::bind(&HTTPConnection::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)));
		}



		void HTTPConnection::handle_read(
			const boost::system::error_code& e,
			std::size_t bytes_transferred
		){
			if (!e)
			{
				boost::tribool result;
				boost::tie(result, boost::tuples::ignore) = request_parser_.parse(
					request_, buffer_.data(), buffer_.data() + bytes_transferred);

				if (result)
				{
					request_.ipaddr = socket_.remote_endpoint().address().to_string();
					(*handler_)(request_, reply_);
					boost::asio::async_write(socket_, reply_.to_buffers(),
						strand_.wrap(
							boost::bind(&HTTPConnection::handle_write, shared_from_this(),
							boost::asio::placeholders::error)));
				}
				else if (!result)
				{
					reply_ = HTTPReply::stock_reply(HTTPReply::bad_request);
					boost::asio::async_write(socket_, reply_.to_buffers(),
						strand_.wrap(
							boost::bind(&HTTPConnection::handle_write, shared_from_this(),
							boost::asio::placeholders::error)));
				}
				else
				{
					socket_.async_read_some(boost::asio::buffer(buffer_),
						strand_.wrap(
							boost::bind(&HTTPConnection::handle_read, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred)));
				}
			}

			// If an error occurs then no new asynchronous operations are started. This
			// means that all shared_ptr references to the connection object will
			// disappear and the object will be destroyed automatically after this
			// handler returns. The connection class's destructor closes the socket.
		}



		void HTTPConnection::handle_write(const boost::system::error_code& e)
		{
			if (!e)
			{
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
			}

			// No new asynchronous operations are started. This means that all shared_ptr
			// references to the connection object will disappear and the object will be
			// destroyed automatically after this handler returns. The connection class's
			// destructor closes the socket.
		}

	} // namespace server
} // namespace http
