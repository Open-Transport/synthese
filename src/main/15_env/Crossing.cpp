#include "Crossing.h"


#include <assert.h>

namespace synthese
{
	namespace env
	{


	    Crossing::Crossing (const uid& key,
				const City* city)
		: ConnectionPlace (key, "", city, CONNECTION_TYPE_ROADROAD, 0)
	    {
	    }
	    

	    
	    Crossing::~Crossing ()
	    {
		
	    }

	}
}
