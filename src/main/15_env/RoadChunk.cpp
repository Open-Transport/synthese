#include "RoadChunk.h"
#include "Road.h"
#include "Address.h"



namespace synthese
{
namespace env
{


RoadChunk::RoadChunk (const Road* road,
		      const Address* fromAddress)
    : Edge ()
    , _road (road)
    , _fromAddress (fromAddress)
{
}

  
RoadChunk::~RoadChunk ()
{
}

    



const Path* 
RoadChunk::getParentPath () const
{
    return _road;
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
