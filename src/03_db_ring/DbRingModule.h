#ifndef SYNTHESE_DBRING_MODULE_H
#define SYNTHESE_DBRING_MODULE_H

#include "ModuleClassTemplate.h"

namespace synthese
{
	/** @defgroup m101 101 Database synchronization ring.
		@ingroup m1
		
		When this module will run correctly, integrate it into 10_db

	@{
	*/


	/** 101 Database ring namespace.
	*/
	namespace dbring
	{

	    class Node;

		class DbRingModule:
			public server::ModuleClassTemplate<DbRingModule>
	    {
	    private:
		
			static Node* _Node;
		
	    public:
		
			static Node* GetNode ();
		
			/** Called whenever a parameter registered by this module is changed
			*/
			static void ParameterCallback (const std::string& name, 
							const std::string& value);
		};
	}
	
	/** @} */
	
}
#endif

