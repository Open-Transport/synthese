
/** 92 RS485 client main file.
	@file 92_rs485_client/main.cpp
	@ingroup m92

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include <boost/asio.hpp>

#ifdef WIN32
	#include "Windows.h"
#endif

#include "01_util/threads/Thread.h"

#include "Conversion.h"
#include "01_util/Log.h"

#include "HTTPRequestParser.hpp"
#include "HTTPRequest.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include <boost/thread/thread.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/foreach.hpp>

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

/** Main function of the RS485 Client binary.
	@param argc Number of execution parameters
	@param argv Execution parameters array
	@ingroup m92
*/ 
int main(int argc, char* argv[])
{

    vector<string> codes;
    vector<int> outdate;
    int nbclients;
    DCB dcb;
    HANDLE hCom;
    BOOL fSuccess;

    // get parameters 
    if(argc < 4) 
	{
		Log::GetInstance ().fatal ("Invalid number of arguments");
		exit(-1);
	}

    string server(argv[1]);
    string port = argv[2];
    string comm(argv[3]);
    bool useCOM (false);
    if ( (comm.substr (0, 3) == "COM") || (comm.substr (0, 3) == "com") )
    {
		useCOM = true;
    }


    //memset(codes, MAX_CLIENTS, sizeof(char*));
    for(nbclients=0; nbclients<argc-4; nbclients++)
	{
		codes.push_back(argv[nbclients+4]);
		outdate.push_back(0);
    }

    if (useCOM)
    {
		hCom = CreateFile( comm.c_str(),
		   GENERIC_READ | GENERIC_WRITE,
		   0,    // must be opened with exclusive-access
		   NULL, // no security attributes
		   OPEN_EXISTING, // must use OPEN_EXISTING
		   0,    // not overlapped I/O
		   NULL  // hTemplate must be NULL for comm devices
		);
		
		if (hCom == INVALID_HANDLE_VALUE) 
		{
			Log::GetInstance ().fatal ("Error while creating comm file " + Conversion::ToString (GetLastError()));
			exit(1);
		}
		
		// Build on the current configuration, and skip setting the size
		// of the input and output buffers with SetupComm.
		fSuccess = GetCommState(hCom, &dcb);
		if (!fSuccess) 
		{
			// Handle the error.
			Log::GetInstance ().fatal ("Error while getting comm state " + Conversion::ToString (GetLastError()));
			exit(2);
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
			Log::GetInstance ().fatal ("SetCommState failed with error " + Conversion::ToString (GetLastError()));
			exit(3);
		}
		
		Log::GetInstance ().info ("Serial port " + std::string (comm) + " successfully reconfigured.");
    }
	    

    char buf[MAX_QUERY_SIZE];
    DWORD readComm = 0;

	while(true)
	{

		time_t now;
		struct tm *hms;
		int stamp;
		time(&now);
		hms = localtime(&now);
		stamp = hms->tm_min;


		for(int client=0; client<nbclients; client++)
		{
			stringstream received;
			try
			{

				if(outdate[client] == stamp) continue;

				//if(!SetCommState(hCom, &dcb))
				//    fichier << "erreur reinit port com" << endl;

				Log::GetInstance ().info ("Connecting " + std::string (server) 
					+ std::string (":") + Conversion::ToString (port));

				boost::asio::io_service io_service;

				// Get a list of endpoints corresponding to the server name.
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
				request_stream << "GET " << "/synthese3/admin?fonction=tdg&date=A&tb=" << codes[client] 
				<< " HTTP/1.0\r\n";
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
					continue;
				}
				if (status_code != 200)
				{
					std::cout << "Response returned with status code " << status_code << "\n";
					continue;
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
			}
			catch (std::exception& e)
			{
				std::cout << "Exception: " << e.what() << "\n";
				continue;
			}


			if (received.str().empty ())
			{
				Log::GetInstance ().warn ("Received empty command ! Maybe a server crash ?");
				outdate[client] = hms->tm_min;
			} 
			else
			{
				Log::GetInstance ().info ("Received command : " + received.str());

				time(&now);
				hms = localtime(&now);

				if (useCOM)
				{
					BOOST_FOREACH(char bufptr, received.str())
					{
						fSuccess = TransmitCommChar (hCom, bufptr);
					}
				}

				outdate[client] = hms->tm_min;

				if (useCOM)
				{
					// Read is updated with the number of bytes read
					fSuccess = ReadFile (hCom, buf, STATUS_MESSAGE_SIZE, &readComm, NULL); 
					if (!fSuccess)
					{
						Log::GetInstance ().error ("Error while reading status ! Returned status message will be empty or incomplete."); 
					}

					// Create status message to be sent back to server. 
					// It will be sent the next time that the client will ask for update.
					std::stringstream status;
					for (int i=0; i<STATUS_MESSAGE_SIZE; ++i)
					{
						status << std::hex << std::setw (2) << std::setfill ('0') << ((int) buf[i]);
					}
					Log::GetInstance ().info ("Status : " + status.str ());



					try
					{

						if(outdate[client] == stamp) continue;

						//if(!SetCommState(hCom, &dcb))
						//    fichier << "erreur reinit port com" << endl;

						Log::GetInstance ().info ("Connecting " + std::string (server) 
							+ std::string (":") + Conversion::ToString (port));

						boost::asio::io_service io_service;

						// Get a list of endpoints corresponding to the server name.
						tcp::resolver resolver(io_service);
						tcp::resolver::query query(server, "http");
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
						request_stream << "GET " << "/synthese3/admin?fonction=tds&tb=" << codes[client] 
						<< "&status=" << status.str () << " HTTP/1.0\r\n";
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
							continue;
						}
						if (status_code != 200)
						{
							std::cout << "Response returned with status code " << status_code << "\n";
							continue;
						}

						// Read the response headers, which are terminated by a blank line.
						boost::asio::read_until(socket, response, "\r\n\r\n");

						// Process the response headers.
						std::string header;
						while (std::getline(response_stream, header) && header != "\r")
							received << header << "\n";
						received << "\n";

						// Write whatever content we already have to output.
						if (response.size() > 0)
							received << &response;

						// Read until EOF, writing data to output as we go.
						while (boost::asio::read(socket, response,
							boost::asio::transfer_at_least(1), error))
							received << &response;
						if (error != boost::asio::error::eof)
							throw boost::system::system_error(error);

					}
					catch (std::exception& e)
					{
						std::cout << "Exception: " << e.what() << "\n";
					}
				} 
			}
		}
		Thread::Sleep(60000);
	}
}


/** @} */
