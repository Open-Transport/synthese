
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


#include <csignal>
#include <string>
#include <map>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <sstream>

#include "02_db/DBModule.inc.cpp"
#include "03_db_ring/DbRingModule.inc.cpp"
#include "03_db_ring/Node.h"



using namespace synthese::tcp;
using namespace synthese::util;
using namespace synthese::dbring;
using namespace synthese::db;


namespace po = boost::program_options;



class SickNode : public Node
{
private:


public:

    SickNode (const NodeId& id)
	: Node (id)
    {
    }



 protected:

/*
    bool recvToken (Token* token)
    {
	if (_deaf) return false;
	return Node::recvToken (token);
    }


    bool sendToken (Token* token)
    {
	if (_dumb) return false;
	return Node::sendToken (token);
	}

*/


};



/*

r create table toto (t);
 */



int main( int argc, char **argv )
{
    try 
    {
	// included auto generated code
#include "02_db/DBModule.gen.cpp"
#include "03_db_ring/DbRingModule.gen.cpp"

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
    
	NodeId nodeId = (NodeId) Conversion::ToInt (DbRingModule::GetParameter ("dbring_node_id"));
	
	// FORTEST :: just one ring right now
	// NodeInfo info (nodeId, 1, "localhost", 3592 + nodeId, false, OUTRING, -1);
	// NodeInfoTableSync::save (&info);
	// FORTEST


	char buf[4096];
	// Wait for input...
	while (std::cin.getline (buf, 4096))
	{
	    std::string input (buf);
	    
	    if (input == "") continue;
	    
	    if (input[0] == 'a')
	    {
		
		NodeId aid;
		std::string cmd;
		std::string ahost ("localhost");
		int aport (0);
		int auth (0);
		std::stringstream ss (input);
		RingId ringid = 0;
		ss >> cmd;
		ss >> aid;
		ss >> ringid;
		ss >> auth;
		ss >> ahost;
		ss >> aport;
		
		if (aport == 0) aport = 3592 + aid; 
		Log::GetInstance ().info ("Appending node " + Conversion::ToString (aid) + " " + ahost + ":" +
					  Conversion::ToString (aport));
		
		NodeInfo info (aid, ringid, ahost, aport, auth == 1 , OUTRING, -1);
		NodeInfoTableSync::save (&info);
		
		// DbRingModule::GetNode ()->setNodeInfo (aid, ringid, ahost, aport);
		
	    }
	    else if (input[0] == 'p')
	    {
		DbRingModule::GetNode ()->dump ();
	    }
	    else if (input[0] == 'r')
	    {
		std::cerr << "Query!" << std::endl;
		DbRingModule::GetNode ()->dump ();
		std::stringstream ss (input);
		std::string sql ("");
		sql = ss.str ().substr (2);
		DbRingModule::GetNode ()->execUpdate (sql);
	    }
	    
	    
	} 

//	ThreadManager::Instance ()->run ();
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

/*


int main( int argc, char **argv )
{
    std::string id (argv[1]);
    std::string host ("localhost");
    std::string port (Conversion::ToString (9990 + Conversion::ToInt (id)));
    if (argc >= 3)
    {
	host = argv[2];
	port = argv[3];
    }

    std::cerr << "Id =  " << id << std::endl;
    std::cerr << "Host =  " << host << std::endl;
    std::cerr << "Port =  " << port << std::endl;

    char buf[4096];
    SickNode* node = new SickNode ((NodeId) Conversion::ToInt (id));
    RingId ringid = 1;
    node->setNodeInfo ((NodeId) Conversion::ToInt (id), ringid, host, Conversion::ToInt (port));


    Thread thread (node, "node", 1000);

    thread.start ();

    // Wait for input...
    while (std::cin.getline (buf, 4096))
    {
	std::string input (buf);
	
	if (input == "") continue;

	if (input[0] == 'a')
	{
	    
	    NodeId aid;
	    std::string cmd;
	    std::string ahost ("localhost");
	    int aport (0);
	    std::stringstream ss (input);
	    ss >> cmd;
	    ss >> aid;
	    ss >> ahost;
	    ss >> aport;

	    if (aport == 0) aport = 9990 + aid; 
	    Log::GetInstance ().info ("Appending node " + Conversion::ToString (aid) + " " + ahost + ":" +
				      Conversion::ToString (aport));
	    node->setNodeInfo (aid, ringid, ahost, aport);

	}
	
    } 

}


*/
