
/*	This file belongs to the SYNTHESE project (public transportation specialized software)
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

#include "01_util/Conversion.h"
#include "01_util/Exception.h"
#include "01_util/Log.h"
#include "01_util/Factory.h"
#include "01_util/ModuleClass.h"
#include "01_util/threads/Thread.h"
#include "01_util/threads/ThreadManager.h"

#include "02_db/DbModuleClass.h"

#include "15_env/CommercialLine.h"
#include "15_env/TridentExport.h"



#include <csignal>
#include <string>
#include <map>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <sstream>
#include <fstream>

#include "02_db/DBModule.inc.cpp"
#include "15_env/EnvModule.inc.cpp"



using namespace synthese::util;
using namespace synthese::db;
using namespace synthese::env;


namespace po = boost::program_options;


// Usage : launch binary with the proper sqlite db and type 
// e * 
// in order to export all lines




int main( int argc, char **argv )
{
    try 
    {
	// included auto generated code
#include "02_db/DBModule.gen.cpp"
#include "15_env/EnvModule.gen.cpp"

	std::string db;
	std::vector<std::string> params;

	// To define several params on the command line, the syntax is --param name1=val1 --param name2=val2 ....

	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help", "produce this help message")
	    ("db", po::value<std::string>(&db)->default_value (std::string ("./config.db3")), "SQLite database file")
	    ("param", po::value<std::vector<std::string> >(&params), "Default parameters values (if not defined in db)");
	 
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
    
	po::notify(vm);    
    
	if (vm.count("help"))
	{
	    std::cout << desc << std::endl;
	    return 1;
	}
    
	DbModuleClass::Parameters defaultParams;
	for (std::vector<std::string>::const_iterator it = params.begin (); 
	     it != params.end (); ++it)
	{
	    int index = it->find ("=");
	
	    std::string paramName (it->substr (0, index));
	    std::string paramValue (it->substr (index+1));

	    defaultParams.insert (std::make_pair (paramName, paramValue));
	}

	const boost::filesystem::path& workingDir = boost::filesystem::initial_path();
	Log::GetInstance ().info ("Working dir  = " + workingDir.string ());

	boost::filesystem::path dbpath (db, boost::filesystem::native);
	DbModuleClass::SetDefaultParameters (defaultParams);
	DbModuleClass::SetDatabasePath (dbpath);
    
	// Initialize modules
	if (Factory<ModuleClass>::size() == 0)
	    throw Exception("No registered module !");

    
	for (Factory<ModuleClass>::Iterator it = Factory<ModuleClass>::begin(); 
	     it != Factory<ModuleClass>::end(); ++it)
	{
	    Log::GetInstance ().info ("Pre-initializing module " + it.getKey() + "...");
	    it->preInit();
	}

	for (Factory<ModuleClass>::Iterator it = Factory<ModuleClass>::begin(); 
	     it != Factory<ModuleClass>::end(); ++it)
	{
	    Log::GetInstance ().info ("Initializing module " + it.getKey() + "...");
	    it->initialize();
	}


	char buf[4096];
	// Wait for input...
	while (std::cin.getline (buf, 4096))
	{
	    std::string input (buf);
	    
	    if (input == "") continue;
	    
	    if (input[0] == 'e')
	    {
		// e 34543546544574 ../trident.xml
		
		std::string cmd;
		std::string commercialLineId;
		std::string filepath ("trident.xml");

		std::stringstream ss (input);
		ss >> cmd;
		ss >> commercialLineId;
		ss >> filepath;

		
		if (commercialLineId == "*")
		{
		    for (CommercialLine::ConstIterator it = CommercialLine::Begin();
			 it != CommercialLine::End (); ++it)
		    {
			boost::shared_ptr<CommercialLine> cl = it->second;
			std::string name = cl->getName ();
			std::string filename ("trident");
			for (int i=0; i<name.length (); ++i) 
			{
			    filename += (name[i] == '/') ? '_' : name[i];
			}
			filename += ".xml";
			std::ofstream out (filename.c_str ());
			TridentExport::Export (out, cl->getKey ());
			out.close ();
			std::cout << "Commercial line " <<  cl->getName () << " exported to " << filepath << std::endl;
		    }
		}
		else
		{
		    uid cluid = Conversion::ToLongLong (commercialLineId);
		    std::ofstream out (filepath.c_str ());
		    TridentExport::Export (out, cluid);
		    out.close ();
		    std::cout << "Commercial line " << cluid << " exported to " << filepath << std::endl;
		}


		    
	    }
	    else if (input[0] == 'p')
	    {
		std::cout << "Commercial lines list :" << std::endl;
		
		for (CommercialLine::ConstIterator it = CommercialLine::Begin ();
		     it != CommercialLine::End (); ++it)
		{
		    boost::shared_ptr<CommercialLine> cl = it->second;
		    std::cout << cl->getKey () << "   " << cl->getName () << " " << cl->getLongName () << std::endl;
		}
		std::cout << std::endl;
	    }
	    
	} 

    }
    catch (std::exception& e)
    {
	Log::GetInstance ().fatal (std::string ("Fatal error : ") + e.what ());
    }
    catch (...)
    {
	Log::GetInstance ().fatal ("Unexpected exception.");
    }

}

