#include "Server.h"

#include "01_util/Log.h"


#include <iostream>
#include <boost/program_options.hpp>

using synthese::util::Log;


namespace po = boost::program_options;


int main( int argc, char **argv )
{
    //  int port = 3591;
    // int threads = 10;

//  boost program_options causes crashes with GCC 4.0

    int loglevel;
    int port;
    int threads;

    po::options_description desc("Allowed options");
    desc.add_options()
	("help", "produce this help message")
	("loglevel", po::value<int>(&loglevel)->default_value (1), "log level (0:debug ; 1:info ; 2:warn; 3:error; 4:fatal; 5:none)")
	("port", po::value<int>(&port)->default_value (3591), "TCP service port")
	("threads", po::value<int>(&threads)->default_value (10), "number of parallel threads")
	
	;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    if (vm.count("help")) {
	std::cout << desc << std::endl;
	return 1;
    }

    // Configure default log (default output is cout).
    synthese::util::Log::GetInstance ().setLevel (
	(synthese::util::Log::Level) loglevel);
    
    synthese::server::Server server (port, threads);
    server.run ();
}

