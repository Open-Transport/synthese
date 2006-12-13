#ifndef SYNTHESE_UTIL_UID_H
#define SYNTHESE_UTIL_UID_H

typedef unsigned long long int uid;

namespace synthese
{

    
    namespace util
    {
	
	/** 
	    Encodes a universal id.
	    @param tableId (2 bytes)
	    @param gridId (4 bits)
	    @param gridNodeId (12 bits)
	    @param objectId (4 bytes)

	 */
	uid encodeUId (int tableId, int gridId, int gridNodeId, long objectId);

	int decodeTableId (const uid& id);
	int decodeGridId (const uid& id);
	int decodeGridNodeId (const uid& id);
	long decodeObjectId (const uid& id);
	    

    }


}

#endif

