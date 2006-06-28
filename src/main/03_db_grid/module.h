#ifndef SYNTHESE_DBGRID_MODULE_H
#define SYNTHESE_DBGRID_MODULE_H

namespace synthese
{

	/** @defgroup m17 17 Database Grid management module.
		
	This module is aimed at keeping in sync a grid of database nodes.

	The following constraints must be satisfied :

	* Data synchronization/replication  follows a multi-master mode ie write operations
	can be executed on any grid connected node.

	* Data synchronization is asynchronous. Connected grid nodes
	keeps on working normally even if one or several other grid nodes are down.

	* Any disconnected node is still available locally for reading and will synchronize 
	its data later on reconnection. 

	* The synchronization protocol must prevent conflicts from happenning (taking into account
	connection statuses and network communication latency).

	* A new data grid node may be added at any time, without any extra manual configuration.
	
	* The synchronization protocol must be secure (message encryption).

	----------------------

	

	@{
	*/

	/** 03_db_grid namespace */
	namespace dbgrid
	{

	}

	/** @} */

}

#endif
