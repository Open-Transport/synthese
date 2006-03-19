#include "RoadChunk.h"
#include "Road.h"



namespace synthese
{
namespace env
{


RoadChunk::RoadChunk (const Road* road)
    : Edge ()
    , _road (road)
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
RoadChunk::getFrom () const
{
    return 0;
}




const Vertex* 
RoadChunk::getTo () const
{
    return 0;
}




}
}
