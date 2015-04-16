
/** 92 RS485 client main file.
	@file 92_rs485_client/main.cpp
	@ingroup m92

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

#include <boost/bind.hpp>
#include <boost/asio.hpp>

//#ifdef WIN32
//	#include "Windows.h"
//#endif

#include "01_util/Log.h"

#include "HTTPRequestParser.hpp"
#include "HTTPRequest.hpp"

#include "MacAddressGetter.h"
#include "XmlParser.h"

#include "CPUGetWiredScreensFunction.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>

#define MAX_QUERY_SIZE 4096
#define STATUS_MESSAGE_SIZE 17

	/** @defgroup m92 92 RS485 Client
		@ingroup m9

		Le client RS485 assure en service continu les opérations suivantes :
			- demande du contenu au serveur, en spécifiant la clé de l'afficheur, ce qui détermine à la fois le protocole et le contenu
			- stockage du contenu reçu
			- transmission du contenu au port RS485
			- attente minute suivante

		Lorsque l'architecture matérielle le nécessite, le client RS485 doit gérer l'alimentation de plusieurs dispositifs branchés en parallèle sur le même port. La boucle de fonctionnement est alors la suivante :
			- demande des contenus au serveur, en spécifiant la clé de chaque afficheur
			- stockage des contenus reçus
			- transmission des contenus au port RS485, afficheur par afficheur
			- attente minute suivante

		@image html 1195572.2.000.png
		@image latex 1195572_1_000.eps "Schéma d'architecture cible d'une installation RS485" height=10cm

		@{
	*/



using namespace synthese::util;
using namespace std;
using boost::asio::ip::tcp;
using namespace synthese::server;
using namespace boost;
using namespace synthese::departure_boards;


std::string SendToDisplay(const std::string& comPort, const std::string& text)
{
	HANDLE hCom(
		CreateFile(
			comPort.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,    // must be opened with exclusive-access
			NULL, // no security attributes
			OPEN_EXISTING, // must use OPEN_EXISTING
			0,    // not overlapped I/O
			NULL  // hTemplate must be NULL for comm devices
	)	);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		Log::GetInstance ().fatal ("Error while creating comm file " + lexical_cast<string>(GetLastError()));
		return string();
	}

	// Build on the current configuration, and skip setting the size
	// of the input and output buffers with SetupComm.
	DCB dcb;
	BOOL fSuccess(
		GetCommState(hCom, &dcb)
	);
	if (!fSuccess)
	{
		// Handle the error.
		Log::GetInstance ().fatal ("Error while getting comm state " + lexical_cast<string>(GetLastError()));
		CloseHandle(hCom);
		return string();
	}

	COMMTIMEOUTS lpct;
	lpct.ReadIntervalTimeout = 200;
	lpct.ReadTotalTimeoutMultiplier = 200;
	lpct.ReadTotalTimeoutConstant = 0;
	lpct.WriteTotalTimeoutMultiplier = 200;
	lpct.WriteTotalTimeoutConstant = 0;

	fSuccess = SetCommTimeouts (hCom, &lpct);

	// Fill in DCB: 57,600 bps, 8 data bits, no parity, and 1 stop bit.
	dcb.BaudRate = CBR_9600;      // set the baud rate
	dcb.ByteSize = 8;             // data size, xmit, and rcv
	dcb.Parity = NOPARITY;      // parity bit
	dcb.StopBits = ONESTOPBIT;    // one stop bit
	fSuccess = SetCommState(hCom, &dcb);
	if (!fSuccess)
	{
		// Handle the error.
		Log::GetInstance ().fatal ("SetCommState failed with error " + lexical_cast<string>(GetLastError()));
		CloseHandle(hCom);
		return string();
	}

	Log::GetInstance ().info ("Serial port " + comPort + " successfully reconfigured.");

	// Transmit each character
	BOOST_FOREACH(char bufptr, text)
	{
		fSuccess = TransmitCommChar (hCom, bufptr);
	}

	// Read is updated with the number of bytes read
	char buf[MAX_QUERY_SIZE];
	DWORD readComm = 0;
	fSuccess = ReadFile (hCom, buf, STATUS_MESSAGE_SIZE, &readComm, NULL);
	if (!fSuccess)
	{
		Log::GetInstance ().error ("Error while reading status ! Returned status message will be empty or incomplete.");
	}
	Log::GetInstance ().info(lexical_cast<string>(readComm) + " bytes read :");

	// Create status message to be sent back to server.
	// It will be sent the next time that the client will ask for update.
	std::stringstream status;
	for (DWORD i=0; i<readComm; ++i)
	{
		status << std::hex << std::setw (2) << std::setfill ('0') << ((int) buf[i]);
	}
	Log::GetInstance ().info ("Status : " + status.str ());

	CloseHandle(hCom);

	return status.str();
}



std::string SendToServer(
	const string& server,
	const string& port,
	const string& url
){
	try
	{
		stringstream received;

		Log::GetInstance ().info ("Connecting " + server + ":" + port);

		// Get a list of endpoints corresponding to the server name.
		asio::io_service io_service;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(server, port);
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

		// Form the request. We specify the "Connection: close" header so that the
		// server will close the socket after transmitting the response. This will
		// allow us to treat all data up until the EOF as the content.
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "GET " << url << " HTTP/1.0\r\n";
		request_stream << "Host: " << server << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";

		// Send the request.
		boost::asio::write(socket, request);

		// Read the response status line.
		boost::asio::streambuf response;
		boost::asio::read_until(socket, response, "\r\n");

		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			std::cout << "Invalid response\n";
			return string();
		}
		if (status_code != 200)
		{
			std::cout << "Response returned with status code " << status_code << "\n";
			return string();
		}

		// Read the response headers, which are terminated by a blank line.
		boost::asio::read_until(socket, response, "\r\n\r\n");

		// Process the response headers.
		vector<string> headers;
		string header;
		while (std::getline(response_stream, header) && header != "\r")
			headers.push_back(header);

		// Write whatever content we already have to output.
		if (response.size() > 0)
			received << &response;

		// Read until EOF, writing data to output as we go.
		while (boost::asio::read(socket, response,
			boost::asio::transfer_at_least(1), error))
			received << &response;
		if (error != boost::asio::error::eof)
			throw boost::system::system_error(error);

		return received.str();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
		return string();
	}

}

typedef map<RegistryKeyType, string> ScreensMap;


void exec_thread(
	bool autoconf,
	ScreensMap& screens,
	const string& server,
	const string& port
){
	try
	{
		// Reads configuration from the server
		if(autoconf)
		{
			const string macAddress(MacAddressGetter::GetMACaddress());

			Log::GetInstance ().info ("Discovering displays list for MAC address : " + macAddress);

			screens.clear();
			string received(
				SendToServer(
				server,
				port,
				"/synthese3/admin?fonction=CPUGetWiredScreensFunction&ma=" + macAddress
				)	);

			if (received.empty ())
			{
				Log::GetInstance ().warn ("Received empty displays list ! Maybe a server crash ?");
			}
			else
			{
				Log::GetInstance ().info ("Received displays list : " + received);
			}

			XMLNode screensNode = XMLNode::parseString (received.c_str(), CPUGetWiredScreensFunction::DISPLAY_SCREENS_XML_TAG.c_str());
			int screensNumber(screensNode.nChildNode(CPUGetWiredScreensFunction::DISPLAY_SCREEN_XML_TAG.c_str()));
			for(int screenRank(0); screenRank < screensNumber; ++screenRank)
			{
				XMLNode screenNode(screensNode.getChildNode(CPUGetWiredScreensFunction::DISPLAY_SCREEN_XML_TAG.c_str(), screenRank));
				const string comPort("COM" + string(screenNode.getAttribute(CPUGetWiredScreensFunction::DISPLAY_SCREEN_COMPORT_XML_FIELD.c_str())));
				const RegistryKeyType screenId(lexical_cast<RegistryKeyType>(screenNode.getAttribute(CPUGetWiredScreensFunction::DISPLAY_SCREEN_ID_XML_FIELD.c_str())));

				screens.insert(make_pair(screenId, comPort));
			}
		}

		// Display on each screen
		BOOST_FOREACH(const ScreensMap::value_type& it, screens)
		{
			const RegistryKeyType screenId(it.first);
			const string& comm(it.second);

			string command(
				SendToServer(
				server, port,
				"/synthese3/admin?fonction=tdg&date=A&tb=" + lexical_cast<string>(screenId)
				)	);


			if (command.empty ())
			{
				Log::GetInstance ().warn ("Received empty command ! Maybe a server crash ?");
				continue;
			}

			Log::GetInstance ().info ("Received command : " + command);

			if ( (comm.substr(0, 3) == "COM") || (comm.substr (0, 3) == "com") )
			{
				string status(SendToDisplay(comm, command));

				if(!status.empty())
					SendToServer(server, port, "/synthese3/admin?fonction=tds&tb=" + lexical_cast<string>(screenId) + "&status=" + status);
			}

			this_thread::sleep(posix_time::millisec(500));
		}
	}
	catch(boost::thread_interrupted)
	{
		Log::GetInstance ().warn ("Thread interrupted due to a timeout.");
	}
	catch(...)
	{
		Log::GetInstance ().warn ("Unhandled exception.");
	}
}



/** Main function of the RS485 Client binary.
	@param argc Number of execution parameters
	@param argv Execution parameters array
	@ingroup m92
*/
int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		Log::GetInstance ().fatal ("Invalid number of arguments");
		exit(-1);
	}

	const string server(argv[1]);
	const string port =(argc >= 3 ? argv[2] : "80");
	bool autoconf(argc < 4);

	ScreensMap screens;
	if(!autoconf)
	{
		for(int nbclients=0; nbclients<argc-4; nbclients++)
		{
			screens.insert(
				make_pair(
					lexical_cast<RegistryKeyType>(argv[nbclients+4]),
					string(argv[3])
			)	);
		}
	}


	while(true)
	{
		thread theThread(
			boost::bind(
				&exec_thread,
				autoconf,
				screens,
				server,
				port
		)	);

		theThread.timed_join(posix_time::seconds(59));

		theThread.interrupt();

		this_thread::sleep(posix_time::seconds(60));
	}
}


/** @} */
