////////////////////////////////////////////////////////////////////////////////
/// SYNTHESE main file.
///	@file main.cpp
///	@author Marc Jambert
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

// At first to avoid the Windows bug "WinSock.h has already been included"
//#include "ServerModule.h"

#ifdef VLD
#include <vld.h>
#endif

#include "Exception.h"
//#include "Log.h"
//#include "Factory.h"
//#include "ModuleClass.h"
//#include "Language.hpp"
//#include "DBModule.h"
//#include "15_server/version.h"

#include <csignal>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sys/types.h> // umask
#include <sys/stat.h> // umask
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//#include "includes.cpp.inc"

#include <dlfcn.h>

using namespace boost;
using namespace std;
//using namespace synthese::util;
//using namespace synthese::db;
//using namespace synthese::server;

namespace po = boost::program_options;

boost::filesystem::path* pidFile (0);

void quit(bool doExit = true);

typedef void (*sighandler_t)(int);
static void setSigHandlers(sighandler_t handler)
{
	std::signal (SIGINT, handler);
	std::signal (SIGTERM, handler);
	std::signal (SIGILL, handler);

#ifndef WIN32
	std::signal(SIGPIPE, handler);
#endif	
}
volatile sig_atomic_t fatal_error_in_progress = 0;

void sig_INT_handler(int sig)
{
	// Since this handler is established for more than one kind of signal, 
	// it might still get invoked recursively by delivery of some other kind
	// of signal.  Use a static variable to keep track of that.
	if (fatal_error_in_progress)
		raise (sig);
	fatal_error_in_progress = 1;
	
	// Catch INT signal and close server properly with exit.
	// This allows profiling info to be dumped.
	//Log::GetInstance ().info ("Caught signal no. " + lexical_cast<string>(sig));

	// Ignore if we crash in the stop procedure
	setSigHandlers(SIG_DFL);

	quit(false);
	
	// Now reraise the signal.  We reactivate the signal's
	// default handling, which is to terminate the process.
	// We could just call exit or abort,
	// but reraising the signal sets the return status
	// from the process correctly.
	raise (sig);
}


#ifndef WIN32

void sig_PIPE_handler(int sig)
{
	// Catch SIGPIPE and ignore it. We do not want the program to
	// die on a broken pipe error, which is well detected at socket level.
	//Log::GetInstance ().info ("Ignoring broken pipe.");
}



pid_t daemonize ()
{
#if 0
	pid_t pid;
	if (getppid () == 1) return getpid();
	pid = fork ();
	if (pid < 0)
	{
		//Log::GetInstance ().fatal ("fork() failed !");
		exit (1);
	}
	if (pid > 0)
	{
		// Wait for PID file to exist
		while ((pidFile == 0) || boost::filesystem::exists (*pidFile) == false)
		{
			this_thread::sleep(posix_time::milliseconds(100));
		}
		exit(0);
	}

	umask(022);
	if (setsid () < 0)
	{
		//Log::GetInstance ().fatal ("setsid() failed !");
		exit (1);
	}

	if (chdir ("/") < 0)
	{
		//Log::GetInstance ().fatal ("chdir(\"/\") failed !");
		exit (1);
	}

	return getpid();
#endif
	return 0;
}

#endif

void ensureWritablePath (const boost::filesystem::path& path, bool removeOnSuccess)
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


filesystem::path createCompletePath (const std::string& s)
{
	boost::filesystem::path path (s);
	return boost::filesystem::absolute (path, boost::filesystem::initial_path());
}


void quit(bool doExit)
{
#if 0
	// End all threads
	ServerModule::KillAllHTTPThreads(false);
	ServerModule::End();
	ServerModule::KillAllThreads();

	// Terminate all modules
	vector<boost::shared_ptr<ModuleClass> > modules(Factory<ModuleClass>::GetNewCollection());
	BOOST_REVERSE_FOREACH(const boost::shared_ptr<ModuleClass> module, modules)
	{
		Log::GetInstance ().info ("Terminating module " + module->getFactoryKey() + "...");
		module->end();
	}

	// Last chance cleaning actions can be added here as well ...
	// Delete PID file
	if(pidFile)
		boost::filesystem::remove (*pidFile);

	Log::GetInstance ().info ("Exit!");

	if(doExit)
	{
		exit(0);
	}
#endif
}


int main( int argc, char **argv )
{
#ifdef VLD
	VLDDisable();
#endif

	setSigHandlers(sig_INT_handler);

#ifdef WIN32
	// Useful for attaching debugger at startup.
	if (::getenv("SYNTHESE_PAUSE"))
		::system("pause");
#endif

	try
	{
		{
			std::string dbConnString;
			std::string pidf;
			std::string logf;
			std::vector<std::string> params;


			// To define several params on the command line, the syntax is --param name1=val1 --param name2=val2 ....

			po::options_description desc("Allowed options");
			desc.add_options()
				("help", "produce this help message")
				("version", "display the version number and exit")
				("dbconn", po::value<std::string>(&dbConnString)->default_value(std::string ("sqlite://")),
				 "Database connection string, using format <backend>://<backend_specific_parameters> "
				 "(see backend documentation for the meaning of backend specific parameters).")
#ifndef WIN32
				("daemon", "Run server in daemon mode")
#endif
				("logfile", po::value<std::string>(&logf)->default_value (std::string ("-")), "Log file path or - for standard output)")
#ifndef WIN32
				("pidfile", po::value<std::string>(&pidf)->default_value (std::string ("s3_server.pid")), "PID file ( - = no pid file )")
#endif
				("param", po::value<std::vector<std::string> >(&params), "Default parameters values (if not defined in db)");

			po::variables_map vm;
			po::store(po::parse_command_line(argc, argv, desc), vm);
			po::notify(vm);
#if 0
			if (vm.count("version"))
			{
				std::cout << "SYNTHESE " << SYNTHESE_VERSION << " " <<
					SYNTHESE_REVISION <<
					" (" << SYNTHESE_BRANCH << " - " << SYNTHESE_BUILD_DATE << ")" <<
#ifdef WITH_MYSQL
					" With MYSQL" <<
#else
					" Without MYSQL" <<
#endif
					std::endl;

				return 1;
			}
#endif
			if (vm.count("help"))
			{
				std::cout << desc << std::endl;
				return 1;
			}
#ifndef WIN32
			bool daemonMode (vm.count("daemon") != 0);
#endif
#if 0
			ModuleClass::Parameters defaultParams;
			for (std::vector<std::string>::const_iterator it = params.begin ();
				it != params.end (); ++it)
			{
				int index = it->find ("=");

				std::string paramName (it->substr (0, index));
				std::string paramValue (it->substr (index+1));

				defaultParams.insert (std::make_pair (paramName, paramValue));
			}
#endif
#ifndef WIN32
			pid_t pid = getpid ();
			if(pidf != "-")
			{
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
				ensureWritablePath (*pidFile, true);
			}
#endif

			boost::filesystem::path logFile (createCompletePath (logf));

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
					//Log::GetInstance ().setOutputStream (logStream);
				}
			}



#ifndef WIN32
			if (daemonMode)
			{
				pid = daemonize ();
			}
			//sLog::GetInstance ().info ("Process PID = " + lexical_cast<string>(pid) + (daemonMode ? " (daemon mode)" : ""));
#endif
//#include "generated.cpp.inc"
			namespace fs = boost::filesystem;
			fs::path someDir("/opt/rcs/synthese3/lib");
			fs::directory_iterator end_iter;
			typedef std::vector<fs::path> result_set_t;
			result_set_t modulelib_set;

			if ( fs::exists(someDir) && fs::is_directory(someDir))
			{
				for( fs::directory_iterator dir_iter(someDir) ; dir_iter != end_iter ; ++dir_iter)
				{
					if (fs::is_regular_file(dir_iter->status()) )
					{
						cout << *dir_iter << endl;
						modulelib_set.push_back(*dir_iter);
					}
				}
			}


			for( result_set_t::const_iterator dir_iter = modulelib_set.begin(); dir_iter != modulelib_set.end() ; ++dir_iter)
			{
				// open the library
				cout << "Opening " << *dir_iter << "...\n";
				void* handle = dlopen(dir_iter->c_str(), RTLD_LAZY | RTLD_GLOBAL);

				if (!handle) {
					cerr << "Cannot open library: " << dlerror() << '\n';
					return 1;
				}

				// load the symbol
				cout << "Loading symbol moduleRegister...\n";
				typedef void (*moduleRegister_t)();
				moduleRegister_t moduleRegister = (moduleRegister_t) dlsym(handle, "moduleRegister");
				if (!moduleRegister) {
					cerr << "Cannot load symbol 'moduleRegister': " << dlerror() << '\n';
					dlclose(handle);
					return 1;
				}

				cout << "Calling moduleRegister...\n";
				moduleRegister();
			}
			const boost::filesystem::path& workingDir = boost::filesystem::current_path();
			//Log::GetInstance ().info ("Working dir  = " + workingDir.string ());

#if 0
			// Should be done in the module register
			synthese::Language::Populate();
			ModuleClass::SetDefaultParameters (defaultParams);
			DBModule::SetConnectionString(dbConnString);
#endif
			// Initialize modules
			//		if (Factory<ModuleClass>::size() == 0)
			//			throw std::exception("No registered module !");

#if 0
			vector<boost::shared_ptr<ModuleClass> > modules(Factory<ModuleClass>::GetNewCollection());
			BOOST_FOREACH(const boost::shared_ptr<ModuleClass> module, modules)
			{
				//Log::GetInstance ().info ("Pre-initializing module " + module->getFactoryKey() + "...");
				module->preInit();
			}

			BOOST_FOREACH(const boost::shared_ptr<ModuleClass> module, modules)
			{
				//Log::GetInstance ().info ("Initializing module " + module->getFactoryKey() + "...");
				module->init();
			}

			BOOST_FOREACH(const boost::shared_ptr<ModuleClass> module, modules)
			{
				//Log::GetInstance ().info ("Starting module " + module->getFactoryKey() + "...");
				module->start();
			}
#endif

#ifndef WIN32
			// Create the real PID file
			if(pidf != "-")
			{
				std::ofstream os (pidFile->string ().c_str (), std::ios_base::out);
				os << pid << std::endl;
				os.close ();
			}

			if (daemonMode)
			{
				// redirect I/O streams to /dev/null
				freopen ("/dev/null", "r", stdin);
				freopen ("/dev/null", "w", stdout);
				freopen ("/dev/null", "w", stderr);
			}
#endif

#ifdef VLD
			VLDEnable();
#endif

			// We pass the data to ServerModule to avoid having it being recompiled
			// on each version.h change (which triggers many links behind it).
#if 0
			ServerModule::InitRevisionInfo(
				SYNTHESE_VERSION,
				SYNTHESE_REVISION,
				SYNTHESE_BRANCH,
				SYNTHESE_BUILD_DATE,
				SYNTHESE_SVN_URL
			);
			ServerModule::RunHTTPServer();
			ServerModule::Wait();
#endif
			quit(false);
		}

		exit(0);
	}
	catch (std::exception& e)
	{
//		Log::GetInstance ().fatal (std::string ("Fatal error : ") + e.what ());
	}
	catch (...)
	{
//		Log::GetInstance ().fatal ("Unexpected exception.");
	}
	exit(1);
}
