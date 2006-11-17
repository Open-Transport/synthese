#include "70_server/Server.h"

#include "01_util/Conversion.h"
#include "01_util/Exception.h"
#include "01_util/Log.h"

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>



// included static initializations

#include "generated.cpp.inc"

//




using synthese::util::Log;
using synthese::util::Conversion;
using synthese::server::Server;


namespace po = boost::program_options;


int main( int argc, char **argv )
{
    std::string db;

    po::options_description desc("Allowed options");
    desc.add_options()
	("help", "produce this help message")
	("db", po::value<std::string>(&db)->default_value ("./synthese.db3"), "SQLite database file");
	
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    if (vm.count("help")) {
	std::cout << desc << std::endl;
	return 1;
    }

    const boost::filesystem::path& workingDir = boost::filesystem::initial_path();
    Log::GetInstance ().info ("Working dir  = " + workingDir.string ());

    synthese::server::Server server (db);
    synthese::server::Server::SetInstance (&server);


    try
    {
		synthese::server::Server::GetInstance ()->initialize ();
		synthese::server::Server::GetInstance ()->run ();
    }
    catch (synthese::util::Exception& ex)
    {
		Log::GetInstance ().fatal ("Exit!", ex);
    }
}

