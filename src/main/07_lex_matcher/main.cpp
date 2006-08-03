#include "07_lex_matcher/LexicalMatcher.h"

#include "01_util/Conversion.h"
#include "01_util/Exception.h"
#include "01_util/Log.h"


#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


using synthese::util::Log;
using synthese::util::Conversion;


namespace po = boost::program_options;


int main( int argc, char **argv )
{
    std::string txtfile;

    po::options_description desc("Allowed options");
    desc.add_options()
	("help", "produce this help message")
	("txtfile", po::value<std::string>(&txtfile)->default_value ("./input.txt"), "Lexical matcher entries txt file (one entry per line)");
	
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    if (vm.count("help")) {
	std::cout << desc << std::endl;
	return 1;
    }

    // const boost::filesystem::path& workingDir = boost::filesystem::initial_path();
    // Log::GetInstance ().info ("Working dir  = " + workingDir.string ());

    try
    {
    }
    catch (synthese::util::Exception& ex)
    {
	Log::GetInstance ().fatal ("Exit!", ex);
    }
}

