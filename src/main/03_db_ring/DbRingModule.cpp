#include "00_tcp/TcpService.h"

#include "03_db_ring/DbRingModule.h"
#include "03_db_ring/DbCommandThreadExec.h"
#include "03_db_ring/Node.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"
#include "01_util/threads/ManagedThread.h"

#include <iostream>

using namespace synthese::util;


// temp
#include "NodeInfoTableSync.h"



namespace synthese
{
	namespace dbring
	{

	    Node* DbRingModule::_Node = 0;


	    void DbRingModule::preInit ()
	    {
		RegisterParameter ("db_port", "3592", &ParameterCallback);
		RegisterParameter ("dbring_node_id", "1", &ParameterCallback); // Default node id is 1 (just like before to keep compliant with existing UIDs for orphan nodes).

		
		NodeId nodeId = (NodeId) Conversion::ToInt (GetParameter ("dbring_node_id"));

		// Create node at preinit time because it has some data to synchronize with db
		_Node = new Node ((NodeId) nodeId);
	    }



	    void DbRingModule::initialize()
	    {
		int sqliteServicePort = Conversion::ToInt (GetParameter ("db_port"));
		
		bool autorespawn = false;

		ManagedThread* nodeThread = 
		    new ManagedThread (_Node, "dbring_node", 300, autorespawn);

		
		synthese::tcp::TcpService* service = 
		    synthese::tcp::TcpService::openService (sqliteServicePort, true);
		
		// Just one thread
		DbCommandThreadExec* dbCommandThreadExec = new DbCommandThreadExec (service);
		
		ManagedThread* dbCommandThread = 
		    new ManagedThread (dbCommandThreadExec, "db_command", 100, autorespawn);
		
		
	    }
	    
	    

	    Node*
	    DbRingModule::GetNode ()
	    {
		return _Node;
	    }


	    
	    void DbRingModule::ParameterCallback (const std::string& name, 
						  const std::string& value)
	    {
		if (name == "db_port") 
		{
		    // TODO : close and reopen service on the new port
		}
		else if (name == "dbring_node_id") 
		{
		    // Erreur critique ...
		}
		
	    }
	    
	}
}


