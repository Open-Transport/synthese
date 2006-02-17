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

  typedef enum { 
    UNKNOWN_ROAD_TYPE, 
    MOTORWAY, /* autoroute */
    MEDIAN_STRIPPED_ROAD,  /* route a chaussees separees (terre plein) */
    PRINCIPLE_AXIS, /* axe principal (au sens rue) */
    SECONDARY_AXIS, /* axe principal (au sens rue) */
    BRIDGE, /* pont */
    STREET, /* rue */
    PEDESTRIAN_STREET, /* rue pietonne */
    ACCESS_ROAD, /* bretelle */
    PRIVATE_WAY, /* voie privee */
    PEDESTRIAN_PATH, /* chemin pieton */
    TUNNEL, /* tunnel */
    HIGHWAY /* route secondaire */
  } RoadType;

    

private:
  

  const std::string _name;
  const RoadType _type;
  const std::string _discriminant;
  cCommune* _town;
  RoadChunkVector _chunks;

public:

  Road(Topography* topography,
       int key,
       const std::string& name,
       const RoadType& type,
       const std::string& discriminant,
       cCommune* town, 
       const std::vector<RoadChunk*>& chunks);
	
  virtual ~Road();

  const std::string& getName () const { return _name; }
  const RoadType& getType () const { return _type; }
  const std::string& getDiscriminant () const { return _discriminant; }
  cCommune* getTown () const { return _town; }
  
  const RoadChunkVector& getChunks () const { return _chunks; }

  const RoadChunk* findMostPlausibleChunkForNumber (const Address::AddressNumber& number) const;
  

};

}

#endif /*ROAD_H_*/
