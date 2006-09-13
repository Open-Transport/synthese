#include "RoadChunk.h"
#include "Road.h"
#include "Address.h"



namespace synthese
{
namespace env
{


RoadChunk::RoadChunk (const uid& id,
		      const Address* fromAddress,
		      int rankInRoad,
		      bool isDeparture,
		      bool isArrival)
    : synthese::util::Registrable<uid,RoadChunk> (id)
    , Edge (isDeparture, isArrival, fromAddress->getRoad (), rankInRoad)
    , _fromAddress (fromAddress)
{
}

  
RoadChunk::~RoadChunk ()
{
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
