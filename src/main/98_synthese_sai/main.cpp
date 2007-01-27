
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

#include "30_server/ServerModule.h"

// included auto generated code
#include "includes.cpp.inc"

using namespace synthese::util;

using synthese::server::ServerModule;

namespace po = boost::program_options;


int main( int argc, char **argv )
{
	// included auto generated code
	#include "generated.cpp.inc"

    std::string db;

    po::options_description desc("Allowed options");
    desc.add_options()
	("help", "produce this help message")
	("db", po::value<std::string>(&db)->default_value ("./synthese.db3"), "SQLite database file");
	
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);    
    
    if (vm.count("help"))
	{
		std::cout << desc << std::endl;
		return 1;
    }

    const boost::filesystem::path& workingDir = boost::filesystem::initial_path();
    Log::GetInstance ().info ("Working dir  = " + workingDir.string ());

    try
    {
	ServerModule::setDatabasePath(db);

	// Initialize modules
	if (Factory<ModuleClass>::size() == 0)
	    throw Exception("No registered module !");
	
	for (Factory<ModuleClass>::Iterator it = Factory<ModuleClass>::begin(); 
	     it != Factory<ModuleClass>::end(); ++it)
	{
	    Log::GetInstance ().info ("Initializing module " + it.getKey() + "...");
	    it->setDatabasePath(db);
	    it->initialize();
	}


	// Infinite loop... to be replaced by some control on the thread manager.
	while (true) 
	{
	    Thread::Sleep (2000);
	}

    }
    catch (synthese::util::Exception& ex)
    {
		Log::GetInstance ().fatal ("Exit!", ex);
    }
}


