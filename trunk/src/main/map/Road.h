#ifndef ROAD_H_
#define ROAD_H_

#include "Referrant.h"
#include "RoadChunk.h"
#include <string>
#include <vector>



namespace synmap
{


class Topography;



class Road : public Referrant
{
public:
	
private:
	
  const std::string _name;
  const std::string _discriminant;
  std::vector<const RoadChunk*> _chunks;

public:

  Road(Topography* topography,
       int key,
       const std::string& name,
       const std::string& discriminant,
       const std::vector<RoadChunk*>& chunks);
	
  virtual ~Road();

  const std::string& getName () const { return _name; }
  const std::string& getDiscriminant () const { return _discriminant; }

  const std::vector<const RoadChunk*>& getChunks () const { return _chunks; }

  const RoadChunk* findMostPlausibleChunkForNumber (const RoadChunk::AddressNumber& number) const;


};

}

#endif /*ROAD_H_*/
