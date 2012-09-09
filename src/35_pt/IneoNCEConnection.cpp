
/** IneoNCEConnection class implementation.
	@file IneoNCEConnection.cpp

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

#include "IneoNCEConnection.hpp"

#include "Exception.h"
#include "Log.h"

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/iostreams/stream.hpp>

using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio::ip;
using namespace std;

namespace synthese
{
	namespace pt
	{
		boost::shared_ptr<IneoNCEConnection> IneoNCEConnection::_theConnection;
		

		void IneoNCEConnection::InitThread()
		{
			_theConnection.reset(new IneoNCEConnection);

			// Attempt a connection
			// Get a list of endpoints corresponding to the server name.
			asio::io_service io_service;
			tcp::resolver resolver(io_service);
			tcp::resolver::query query(_theConnection->_nceAddress, _theConnection->_ncePort);
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
			{
				throw boost::system::system_error(error);
			}

			// Création du buffer de réception // (3)
			while(true)
			{
				boost::array<char, 128> buf;
				boost::system::error_code error;
				string s;
				while(error != boost::asio::error::eof)
				{
					size_t bytes_transferred(socket.read_some(boost::asio::buffer(buf), error));
					std::copy(buf.begin(), buf.begin()+bytes_transferred, std::back_inserter(s));

					stringstream reply;
					ptime now(second_clock::local_time());
					reply <<
						"<IdReply><Type>SYNTHESE</Type><Id>1</Id>" <<
						"<Date>" << now.date().day() << "/" << now.date().month() << "/" << now.date().year() << "</Date>" <<
						"<Heure>" << now.time_of_day() << "</Heure>" <<
						"</IdReply>\n";

					boost::asio::write(socket, boost::asio::buffer(reply.str()));
				}

				XMLNode node(ParseInput(s));


				util::Log::GetInstance().info(
					s
				);
			}


		}



		IneoNCEConnection::IneoNCEConnection():
			_nceAddress("192.168.0.1"),
			_ncePort("6201"),
			_status(offline)
		{}



		XMLNode IneoNCEConnection::ParseInput(
			const std::string& xml
		){
			XMLResults results;
			XMLNode allNode = XMLNode::parseString(xml.c_str(), NULL, &results);
			if (results.error != eXMLErrorNone)
			{
				throw Exception("Invalid XML");
			}
			return allNode;
		}
}	}

