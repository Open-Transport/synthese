#include "00_tcp/TcpClientSocket.h"
#include "00_tcp/SocketException.h"
#include "00_tcp/Constants.h"

#include "01_util/iostreams/Compression.h"
#include "01_util/Conversion.h"
#include "Exception.h"
#include "01_util/Log.h"
#include "01_util/threads/Thread.h"


#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <csignal>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


#if defined(UNIX)
  #include <unistd.h>
#endif


#if defined(_WIN32) || defined(WIN32)
# include <io.h>
#else
/* Make sure isatty() has a prototype.
*/
extern int isatty();
#endif






using namespace synthese::util;
using namespace synthese::tcp;
using namespace synthese::db;


namespace po = boost::program_options;


void sig_INT_handler(int sig)
{
    // Catch INT signal and close server properly with exit.
    // This allows profiling info to be dumped.
    Log::GetInstance ().info ("Caught signal no. " + Conversion::ToString (sig));

    // Last chance cleaning actions can be added here as well ...

    Log::GetInstance ().info ("Exit!");
    exit (0);
}



int main( int argc, char **argv )
{
    std::signal(SIGINT, sig_INT_handler);

    // Check if stdin is interactive or a redirection.
    bool isInteractive = isatty(0);

    std::string host;
    int port;

    po::options_description desc("Allowed options");

    desc.add_options()
	("help", "produce this help message")
	("host", po::value<std::string>(&host)->default_value ("localhost"), "SQLite db server host")
	("port", po::value<int>(&port)->default_value (3592), "SQLite db server port")
        ("without-replication", "Disable ring replication for all updates");


    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
	std::cout << desc << std::endl;
	return 1;
    }

    bool withoutReplication (vm.count("without-replication") > 0);

    Log::GetInstance ().info ("Connecting " + host + ":" + Conversion::ToString (port));

    char buf[1024*64];



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
    boost::iostreams::stream<TcpClientSocket> cliSocketStream (clientSock);

    Log::GetInstance ().info ("Connected.");

    // Wait for the welcome message...
    std::stringstream message;
    cliSocketStream.getline (buf, sizeof(buf));

    // R = Replication
    // L = Local node only
    cliSocketStream << (withoutReplication ? 'L' : 'R');

    message << buf;

    if (isInteractive)
	std::cout << message.str () << std::endl;

    try
    {

	if (isInteractive == false)
	{
	    // boost::iostreams::copy (std::cin, reply);
	    Compression::ZlibCompress (std::cin, cliSocketStream);

	    std::stringstream reply;
	    Compression::ZlibDecompress (cliSocketStream, reply);

	    // Keeps first two characters as error code.
	    std::string errorCode;
	    errorCode += ((char) reply.get ());
	    errorCode += ((char) reply.get ());

	    if (errorCode == "00")
	    {
		// Everything went fine. Do not dump the answer in non-interactive mode
	    }
	    else
	    {
		boost::iostreams::copy (reply, std::cerr);
		std::cerr << std::endl;
	    }

	}
	else
	{
	    std::stringstream input;
	    char c;
	    std::cout << "? ";

	    int nbStatements;
	    bool processIt (false);
	    while (!std::cin.eof ())
	    {
		std::cin.get(c);
		input << c;
		if ((c == ';') && (SQLite::IsStatementComplete (input.str ())))
		{
		    Compression::ZlibCompress (input, cliSocketStream);

		    std::stringstream reply;
		    Compression::ZlibDecompress (cliSocketStream, reply);

		    // Keeps first two characters as error code.
		    std::string errorCode;
		    errorCode += ((char) reply.get ());
		    errorCode += ((char) reply.get ());

		    if (errorCode == "00")
		    {
			// Everything went fine. Dump the answer
			boost::iostreams::copy (reply, std::cout);
			std::cout << std::endl;
		    }
		    else
		    {
			// Error!
			boost::iostreams::copy (reply, std::cerr);
			std::cerr << std::endl;
		    }
		    ++nbStatements;

		    // Clear temporary stringstream
		    input.str ("");
		    std::cout << "? ";
		}
	    }

	}
    }

    catch (synthese::Exception& ex)
    {
	Log::GetInstance ().fatal ("Exit!", ex);
    }

    cliSocketStream.close ();


}


