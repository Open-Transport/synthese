
/** EMail class implementation.
	@file EMail.cpp

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

#include "EMail.h"

#include "Base64.hpp"

#include <boost/foreach.hpp>
#include <boost/asio.hpp>
#define CRLF "\r\n"                 // carriage-return/line feed pair

using boost::asio::ip::tcp;
using namespace std;
using namespace boost;

namespace synthese
{
	namespace util
	{
		EMail::EMail(
			const std::string& smtpServer,
			const std::string smtpPort /*= "mail" */
		):	_smtpServer(smtpServer),
			_smtpPort(smtpPort)
		{
		}



		void EMail::setFormat( EMail::Format value )
		{
			_format = value;
		}



		void EMail::setContent( const std::string& value )
		{
			_content = value;
		}



		void EMail::addRecipient( const std::string& email,
			string name
		){
			_recipients.push_back(make_pair(email, name));
		}



		void EMail::setSender( const std::string& value )
		{
			_sender = value;
		}



		void EMail::setSubject( const std::string& value )
		{
			_subject = value;
		}




		void EMail::send() const
		{
		   struct Local
		   {
				static void _Process(
					boost::asio::ip::tcp::socket& socket,
					boost::asio::streambuf& request,
					boost::asio::streambuf& response
				){
					boost::system::error_code error;
					boost::asio::write(socket, request);
					boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error);
					if (error) throw boost::system::system_error(error);
				}
			};

			boost::asio::io_service io_service;

			// Get a list of endpoints corresponding to the server name.
			tcp::resolver resolver(io_service);
			tcp::resolver::query query(_smtpServer, _smtpPort);
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			tcp::resolver::iterator end;

			// Try each endpoint until we successfully establish a connection.
			tcp::socket socket(io_service);
			boost::system::error_code error = boost::asio::error::host_not_found;
			while (error && endpoint_iterator != end)
			{
				socket.close();
				socket.connect(*endpoint_iterator++, error);
			}
			if (error)
				throw boost::system::system_error(error);

			// Read the response status line.
			boost::asio::streambuf response;
			boost::asio::streambuf request;
			std::ostream request_stream(&request);

			// Receive initial response from SMTP server
			boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error);
			if (error) throw boost::system::system_error(error);

			// Send HELO server.com
			request_stream << "HELO " << _smtpServer << CRLF;
			Local::_Process(socket, request, response);

			// Loop on recipients
			BOOST_FOREACH(const _Recipients::value_type& recipient, _recipients)
			{
				// Send MAIL FROM: <sender@mydomain.com>
				request_stream << "MAIL FROM:<" << _sender << ">" << CRLF;
				Local::_Process(socket, request, response);

				// Send RCPT TO: <receiver@domain.com>
				request_stream << "RCPT TO:<" << recipient.first << ">" << CRLF;
				Local::_Process(socket, request, response);

				// Send DATA
				request_stream << "DATA" << CRLF;
				Local::_Process(socket, request, response);

				request_stream <<
					"Subject: =?UTF-8?B?"<< base64::encode(_subject) <<"?="<< CRLF <<
					"From: ";
				if(_senderName.empty())
				{
					request_stream << _sender;
				}
				else
				{
					request_stream << _senderName << " <" << _sender << ">";
				}
				request_stream << CRLF;

				request_stream << "To: ";
				if(recipient.second.empty())
				{
					request_stream << recipient.first;
				}
				else
				{
					request_stream << "=?UTF-8?B?" << base64::encode(recipient.second) << "?= <" << recipient.first << ">";
				}
				request_stream << CRLF;
		
				if(_format == EMAIL_HTML)
				{
					request_stream <<
						"MIME-version: 1.0" << CRLF <<
						"Content-type: text/html; charset=utf-8" << CRLF
					;
				}

				request_stream << _content << CRLF << "." << CRLF;
				Local::_Process(socket, request, response);
			}

			// Send QUIT
			request_stream << "QUIT" << CRLF;
			Local::_Process(socket, request, response);
		}



		void EMail::setSenderName( const std::string& value )
		{
			_senderName = value;
		}
	}
}
