#ifndef ROAD_H_
#define ROAD_H_

#include "Referrant.h"
#include "RoadChunk.h"
#include <string>
#include <vector>
#include <set>


class cCommune;


namespace synmap
{


class Topography;


class Road : public Referrant
{
public:
	
  typedef std::vector<const RoadChunk*> RoadChunkVector;

private:
  

  const std::string _name;
  const std::string _discriminant;
  const cCommune* _town;
  RoadChunkVector _chunks;

public:

  Road(Topography* topography,
       int key,
       const std::string& name,
       const std::string& discriminant,
       const cCommune* town, 
       const std::vector<RoadChunk*>& chunks);
	
  virtual ~Road();

  const std::string& getName () const { return _name; }
  const std::string& getDiscriminant () const { return _discriminant; }
  const cCommune* getTown () const { return _town; }
  
  const RoadChunkVector& getChunks () const { return _chunks; }

  const RoadChunk* findMostPlausibleChunkForNumber (const Address::AddressNumber& number) const;
  

};

}

#endif /*ROAD_H_*/
