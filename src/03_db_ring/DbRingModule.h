#ifndef SYNTHESE_DBRING_MODULE_H
#define SYNTHESE_DBRING_MODULE_H

#include "02_db/DbModuleClass.h"

#include "01_util/FactorableTemplate.h"

namespace synthese
{
	/** @defgroup m03 03 Database synchronisation ring.
		@ingroup m0

	@{
	*/


	/** 03 Database ring namespace.
	*/
	namespace dbring
	{

	    class Node;

		class DbRingModule : public util::FactorableTemplate<db::DbModuleClass, DbRingModule>
	    {
	    private:
		
		static Node* _Node;
		
	    public:
		
		void preInit ();
		void initialize();
		
		static Node* GetNode ();
		
		/** Called whenever a parameter registered by this module is changed
		 */
		static void ParameterCallback (const std::string& name, 
					       const std::string& value);
		   
		virtual std::string getName() const;
	    };
	    
	}
	
	/** @} */
	
}
#endif

