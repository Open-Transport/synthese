

/** main implementation.
	@file main.cpp

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

#include <string>
#include <map>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "01_util/Conversion.h"
#include "01_util/Exception.h"
#include "01_util/Log.h"
#include "01_util/Factory.h"
#include "01_util/ModuleClass.h"
#include "01_util/Thread.h"
#include "01_util/ThreadManager.h"

#include "02_db/DbModuleClass.h"

#include "30_server/ServerModule.h"

// included auto generated code
#include "includes.cpp.inc"

using namespace synthese::util;
using namespace synthese::db;

using synthese::server::ServerModule;

namespace po = boost::program_options;


int main( int argc, char **argv )
{
	// included auto generated code
	#include "generated.cpp.inc"

    std::string db;
    std::vector<std::string> params;

    // To define several params on the command line, the syntax is --param name1=val1 --param name2=val2 ....

    po::options_description desc("Allowed options");
    desc.add_options()
	("help", "produce this help message")
	("db", po::value<std::string>(&db)->default_value (std::string ("./synthese.db3")), "SQLite database file")
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
    
    ThreadManager::Instance ()->run ();
    
}


