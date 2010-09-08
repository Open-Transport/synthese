
/** DbRingModule class implementation.
	@file DbRingModule.cpp

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

#include "DbRingModule.h"
#include "DbCommandThreadExec.h"
#include "03_db_ring/Node.h"

#include "01_util/Conversion.h"
#include "01_util/Log.h"
#include "01_util/threads/ManagedThread.h"

#include "00_tcp/TcpService.h"


#include <iostream>

using namespace synthese::util;


// temp
#include "NodeInfoTableSync.h"



namespace synthese
{
	template<> const std::string util::FactorableTemplate<server::ModuleClass, dbring::DbRingModule>::FACTORY_KEY("03_db_ring");
	
	namespace dbring
	{
	    Node* DbRingModule::_Node = 0;
	}
	
	namespace server
	{
		template<> const string ModuleClassTemplate::NAME("Synchronisation base de données répartie");


	    template<> void ModuleClassTemplate<DbRingModule>::PreInit()
	    {
			RegisterParameter ("db_port", "3592", &ParameterCallback);
			RegisterParameter ("dbring_node_id", "1", &ParameterCallback); // Default node id is 1 (just like before to keep compliant with existing UIDs for orphan nodes).
			RegisterParameter ("dbring_authority", "0", &ParameterCallback);
	
			
			NodeId nodeId = (NodeId) Conversion::ToInt (GetParameter ("dbring_node_id"));
			bool isAuthority = Conversion::ToBool (GetParameter ("dbring_authority"));
			
			// Create node at preinit time because it has some data to synchronize with db,
			// mostly node infos.
			_Node = new Node ((NodeId) nodeId, isAuthority);
	    }



	    template<> void ModuleClassTemplate<DbRingModule>::Init()
	    {
			int sqliteServicePort = Conversion::ToInt (GetParameter ("db_port"));
			
			bool autorespawn = false;
	
			ManagedThread* nodeThread = 
				new ManagedThread (_Node, "dbring_node", 100, autorespawn);
	
			
			synthese::tcp::TcpService* service = 
				synthese::tcp::TcpService::openService (sqliteServicePort, true);
			
			// Just one thread
			DbCommandThreadExec* dbCommandThreadExec = new DbCommandThreadExec (service);
			
			ManagedThread* dbCommandThread = 
				new ManagedThread (dbCommandThreadExec, "db_command", 100, autorespawn);
	    }


		template<> void ModuleClassTemplate<DbRingModule>::End()
		{
		}
	}


	namespace dbring
	{
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


