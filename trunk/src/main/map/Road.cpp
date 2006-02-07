#include "Road.h"
#include "RoadChunk.h"


namespace synmap
{

Road::Road(Topography* topography,
	   int key,
	   const std::string& name,
	   const std::string& discriminant,
	   const std::vector<RoadChunk*>& chunks)
  : Referrant (topography, key)
    , _name (name)
    , _discriminant (discriminant)
{
  for (std::vector<RoadChunk*>::const_iterator iter (chunks.begin ());
       iter != chunks.end (); ++iter) {
    RoadChunk* chunk = *iter;
    chunk->setRoad (this);
    _chunks.push_back (chunk);
  }

}



Road::~Road()
{
}



}
