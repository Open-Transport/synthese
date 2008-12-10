
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

#include "30_server/ServerModule.h"


#include <csignal>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include <fcntl.h> // umask...

#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


// included auto generated code
#include "includes.cpp.inc"

using namespace synthese::util;
using namespace synthese::db;

using synthese::server::ServerModule;

namespace po = boost::program_options;


boost::filesystem::path* pidFile (0);


void sig_INT_handler(int sig)
{
    // Catch INT signal and close server properly with exit.
    // This allows profiling info to be dumped.
    Log::GetInstance ().info ("Caught signal no. " + Conversion::ToString (sig));

    // Last chance cleaning actions can be added here as well ...
    // Delete PID file
    boost::filesystem::remove (*pidFile);

    Log::GetInstance ().info ("Exit!");
    exit (0);
}

#ifndef WIN32

void sig_PIPE_handler(int sig)
{
    // Catch SIGPIPE and ignore it. We do not want the program to 
    // die on a broken pipe error, which is well detected at socket level.
    Log::GetInstance ().info ("Ignoring broken pipe.");
    
}



pid_t daemonize ()
{
    pid_t pid;
    if (getppid () == 1) return getpid();
    pid = fork ();
    if (pid < 0)
    {
	Log::GetInstance ().fatal ("fork() failed !");
	exit (1);
    }
    if (pid > 0) 
    {
	// Wait for PID file to exist
	while ((pidFile == 0) || boost::filesystem::exists (*pidFile) == false)
	{
	    Thread::Sleep (100);
	}
	exit (0);
    }
    
    umask(022);
    if (setsid () < 0)
    {
	Log::GetInstance ().fatal ("setsid() failed !");
	exit (1);
    }

    if (chdir ("/") < 0)
    {
	Log::GetInstance ().fatal ("chdir(\"/\") failed !");
	exit (1);
    }

    return getpid();
}

#endif

void 
ensureWritablePath (const boost::filesystem::path& path, bool removeOnSuccess)
{
    std::ofstream tos (path.string ().c_str (), std::ios_base::app);
    if (tos.good () == false)
    {
	std::cerr << "Cannot open " << path.string () << " for writing." << std::endl;
	exit (1);
    }
    else
    {
	tos.close ();
	if (removeOnSuccess) boost::filesystem::remove (*pidFile);
    }
}


boost::filesystem::path 
createCompletePath (const std::string& s)
{
    boost::filesystem::path path (s, boost::filesystem::native);
    return boost::filesystem::complete (path, boost::filesystem::initial_path());
}



int main( int argc, char **argv )
{
    std::signal (SIGINT, sig_INT_handler);
    std::signal (SIGTERM, sig_INT_handler);

#ifndef WIN32
	std::signal(SIGPIPE, sig_PIPE_handler);
#endif

    try 
    {
	std::string db;
	std::string pidf;
	std::string logf;
	std::vector<std::string> params;

	// To define several params on the command line, the syntax is --param name1=val1 --param name2=val2 ....

	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help", "produce this help message")
	    ("db", po::value<std::string>(&db)->default_value (std::string ("config.db3")), "SQLite database file")
#ifndef WIN32
	    ("daemon", "Run server in daemon mode")
#endif        
	    ("logfile", po::value<std::string>(&logf)->default_value (std::string ("-")), "Log file path or - for standard output)")
#ifndef WIN32
	    ("pidfile", po::value<std::string>(&pidf)->default_value (std::string ("s3_server.pid")), "PID file")
#endif        
#ifdef DEBUG
	    ("monothread", "Enable monothread emulation")
#endif        
	    ("param", po::value<std::vector<std::string> >(&params), "Default parameters values (if not defined in db)");
	 
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);    
    
	if (vm.count("help"))
	{
	    std::cout << desc << std::endl;
	    return 1;
	}
#ifndef WIN32
	bool daemonMode (vm.count("daemon"));
#endif        
#ifdef DEBUG
	bool monothread (vm.count("monothread"));
    if (monothread)
    {
        ThreadManager::SetMonothreadEmulation (monothread);
    }
#endif        
    
	DbModuleClass::Parameters defaultParams;
	for (std::vector<std::string>::const_iterator it = params.begin (); 
	     it != params.end (); ++it)
	{
	    int index = it->find ("=");
	
	    std::string paramName (it->substr (0, index));
	    std::string paramValue (it->substr (index+1));

	    defaultParams.insert (std::make_pair (paramName, paramValue));
	}

#ifndef WIN32
	pid_t pid = getpid ();

	// Check if a daemon instance is already running (PID file existence)
	pidFile = new boost::filesystem::path (createCompletePath (pidf));
	if (boost::filesystem::exists (*pidFile) == true)
	{
	    std::ifstream is (pidFile->string ().c_str (), std::ios_base::in);
	    is >> pid;
	    is.close ();
	    std::cerr << "Process s3_server is already running with PID " << pid << "." << std::endl;
	    exit (1);
	}
#endif        

	boost::filesystem::path dbpath (createCompletePath (db));
	boost::filesystem::path logFile (createCompletePath (logf));

#ifndef WIN32
	ensureWritablePath (*pidFile, true);
#endif        
	ensureWritablePath (dbpath, false);
	std::ostream* logStream = &std::cout;
	if (logf != "-")
	{
	    ensureWritablePath (logFile, false);
	    // Create log file output stream
	    logStream = new std::ofstream (logFile.string ().c_str (), std::ios_base::app);
	    if (logStream->good () == false)
	    {
		std::cerr << "Cannot open " << logFile.string () << " for writing." << std::endl;
		exit (1);
	    }
	    else
	    {
		Log::GetInstance ().setOutputStream (logStream);
	    }
	}



#ifndef WIN32
	if (daemonMode)
	{
	    pid = daemonize ();
	}
	Log::GetInstance ().info ("Process PID = " + Conversion::ToString (pid) + (daemonMode ? " (daemon mode)" : ""));
#endif        

	// included auto generated code
#include "generated.cpp.inc"



	const boost::filesystem::path& workingDir = boost::filesystem::current_path();
	Log::GetInstance ().info ("Working dir  = " + workingDir.string ());

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
    
	for (Factory<ModuleClass>::Iterator iti = Factory<ModuleClass>::begin(); 
	     iti != Factory<ModuleClass>::end(); ++iti)
	{
	    Log::GetInstance ().info ("Initializing module " + iti.getKey() + "...");
	    iti->initialize();
	}


#ifndef WIN32
	// Create the real PID file
	std::ofstream os (pidFile->string ().c_str (), std::ios_base::out);
	os << pid << std::endl;
	os.close ();

	if (daemonMode)
	{
	    // redirect I/O streams to /dev/null
	    freopen ("/dev/null", "r", stdin);
	    freopen ("/dev/null", "w", stdout);
	    freopen ("/dev/null", "w", stderr);
	}
#endif    

	ThreadManager::Instance ()->run ();
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


