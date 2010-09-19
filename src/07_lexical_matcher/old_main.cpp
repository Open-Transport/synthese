#include "07_lex_matcher/LexicalMatcher.h"

#include "01_util/Conversion.h"
#include "Exception.h"
#include "01_util/Log.h"

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


using synthese::util::Log;
using synthese::util::Conversion;
using namespace synthese::lexical_matcher;


namespace po = boost::program_options;


int old_main( int argc, char **argv )
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

    boost::filesystem::path txtFile (txtfile, boost::filesystem::native);
    if (boost::filesystem::exists (txtFile) == false)
    {
	Log::GetInstance ().fatal ("Input file " + txtFile.string () + " does not exist.");
	return 1;
    }
    
    Log::GetInstance ().info ("Initializing matcher with data from " + txtFile.string ());

    bool ignoreCase (true);
    bool ignoreWordOrder (true);
    bool ignoreWordSpacing (true);
    TranslationMap translationMap = FrenchTranslationMap ();
    std::string separatorCharacters ("-,;.' &()");

    int nbMatches (10);

    LexicalMatcher<int> matcher (ignoreCase, 
				 ignoreWordOrder, 
				 ignoreWordSpacing, 
				 translationMap,
				 separatorCharacters);

    // Parse txt file and initialize lexical matcher
    // ...
    // Dump the image data
    std::ifstream ifs (txtFile.string().c_str ());
    char buf[4096];
    while (ifs) 
    {
        ifs.getline (buf, 4096);
	matcher.add (buf, 0);
    }
    
    Log::GetInstance ().info ("Initialization done");

    try
    {
	std::cout << "? ";
	// Wait for input...
	while (std::cin.getline (buf, 4096))
	{
	    std::string input (buf);
	    if (input.empty ()) break;
	    
	    LexicalMatcher<int>::MatchResult result = matcher.bestMatches (input, nbMatches);
	    std::cout << std::endl;
	    for (LexicalMatcher<int>::MatchResult::iterator it = result.begin ();
		 it != result.end (); ++it)
	    {
		std::cout << it->score << "\t" << it->key << std::endl;		
	    }
	    std::cout << std::endl;
	    std::cout << "? ";
	}
    }
    catch (synthese::Exception& ex)
    {
	Log::GetInstance ().fatal ("Exit!", ex);
    }
    
}


