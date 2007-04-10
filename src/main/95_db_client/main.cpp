

#include "00_tcp/TcpClientSocket.h"
#include "00_tcp/SocketException.h"


#include "01_util/Conversion.h"
#include "01_util/Exception.h"
#include "01_util/Log.h"
#include "01_util/Thread.h"

#include <boost/iostreams/stream.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <csignal>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace synthese::util;
using namespace synthese::tcp;


namespace po = boost::program_options;


void sigint_handler(int sig)
{
    
    std::cerr << "handling signal no. " << sig << "\n";
    exit (0);
}




const char ETB (23);

int main( int argc, char **argv )
{
    std::signal(SIGINT, sigint_handler);

    std::string host;
    int port;
    po::options_description desc("Allowed options");

    desc.add_options()
	("help", "produce this help message")
	("host", po::value<std::string>(&host)->default_value ("localhost"), "SQLite db server host")
	("port", po::value<int>(&port)->default_value (3592), "SQLite db server port"); 

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    if (vm.count("help")) {
	std::cout << desc << std::endl;
	return 1;
    }

    Log::GetInstance ().info ("Connecting " + host + ":" + Conversion::ToString (port));

    char buf[4096];
    // No timeout !
    int timeout = 0;
    TcpClientSocket clientSock (host, port, timeout);
		
    while (clientSock.isConnected () == false)
    {
	clientSock.tryToConnect ();
	Thread::Sleep (500);
    }
    
    // The client is connected.

    // Create commodity stream:
    boost::iostreams::stream<TcpClientSocket> cliSocketStream;
    cliSocketStream.open (clientSock);

    Log::GetInstance ().info ("Connected.");

    // Wait for the welcome message...
    std::stringstream message;
    cliSocketStream.getline (buf, 4096);

    message << buf;

    std::cout << message.str () << std::endl;

    std::string input;
    try
    {
	std::cout << "? ";
	// Wait for input...
	while (std::cin.getline (buf, 4096, ';'))
	{
	    std::string input (buf);

	    cliSocketStream << input << ETB;
	    cliSocketStream.flush ();

	    // Wait for reply
	    cliSocketStream.getline (buf, 4096, ETB);
	    
	    std::string answer (buf);
	    
	    // Keeps first two characters as error code.
	    std::string errorCode (answer.substr (0, 2));
	    answer = answer.substr (2);

	    if (answer == "00")
	    {
		// Everything went fine. Dump the answer
		std::cout << answer << std::endl;
		
	    }
	    else 
	    {
		// Error!
		std::cerr << answer << std::endl;
	    }

	    // std::cout << "Answer received : " << answer << std::endl;
	    
	    input = "";
	    std::cout << "? ";
	} 

    }
    catch (synthese::util::Exception& ex)
    {
	Log::GetInstance ().fatal ("Exit!", ex);
    }
    cliSocketStream.close ();

    
}


