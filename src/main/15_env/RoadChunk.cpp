#include "RoadChunk.h"
#include "Road.h"
#include "Address.h"



namespace synthese
{
namespace env
{


RoadChunk::RoadChunk (const uid& id,
		      const Address* fromAddress)
    : synthese::util::Registrable<uid,RoadChunk> (id)
    , Edge (EDGE_TYPE_PASSAGE)
    , _fromAddress (fromAddress)
{
}

  
RoadChunk::~RoadChunk ()
{
}

    



const Path* 
RoadChunk::getParentPath () const
{
    return _fromAddress->getRoad ();
}




const Vertex* 
RoadChunk::getFromVertex () const
{
    return _fromAddress;
}




double
RoadChunk::getMetricOffset () const
{
    return _fromAddress->getMetricOffset ();
}





}
}
