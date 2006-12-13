

#include "UId.h"




namespace synthese
{
namespace util
{
	    



uid 
encodeUId (int tableId, int gridId, int gridNodeId, long objectId)
{
    uid id (objectId);
    uid tmp = gridNodeId;
    id |= (tmp << 32);
    tmp = gridId;
    id |= (tmp << 42);
    tmp = tableId;
    id |= (tmp << 48);
    return id;
}



	    
int 
decodeTableId (const uid& id)
{
    return (int) ((id & 0xFFFF000000000000LL) >> 48);
}


int 
decodeGridId (const uid& id)
{
    return (int) ((id & 0x0000FF0000000000LL) >> 42);
}


int 
decodeGridNodeId (const uid& id)
{
    return (int) ((id & 0x000000FF00000000LL) >> 32);
}



long
decodeObjectId (const uid& id)
{
    return (long) (id & 0x00000000FFFFFFFFLL);
}








}
}


