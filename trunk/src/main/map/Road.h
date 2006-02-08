#ifndef ROAD_H_
#define ROAD_H_

#include "Referrant.h"
#include "RoadChunk.h"
#include <string>
#include <vector>
#include <set>



namespace synmap
{


class Topography;
class PhysicalStop;



class Road : public Referrant
{
public:
	
  typedef std::vector<const RoadChunk*> RoadChunkVector;
  typedef std::pair<RoadChunkVector, const PhysicalStop*> PathToPhysicalStop;

private:
  

  const std::string _name;
  const std::string _discriminant;
  const int _cityKey;
  RoadChunkVector _chunks;

public:

  Road(Topography* topography,
       int key,
       const std::string& name,
       const std::string& discriminant,
       const int cityKey, // TODO change this and create a city class
       const std::vector<RoadChunk*>& chunks);
	
  virtual ~Road();

  const std::string& getName () const { return _name; }
  const std::string& getDiscriminant () const { return _discriminant; }
  int getCityKey () const { return _cityKey; }
  
  const RoadChunkVector& getChunks () const { return _chunks; }

  const RoadChunk* findMostPlausibleChunkForNumber (const RoadChunk::AddressNumber& number) const;
  std::set< PathToPhysicalStop >
    findPathsToPhysicalStops (RoadChunk::AddressNumber addressNumber, 
				    double distance) const;


};

}

#endif /*ROAD_H_*/
