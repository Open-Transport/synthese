#ifndef SYNTHESE_ENV_ROAD_H
#define SYNTHESE_ENV_ROAD_H

#include <string>

#include "01_util/Registrable.h"
#include "AddressablePlace.h"
#include "Path.h"



namespace synthese
{
namespace env
{

class City;
class RoadChunk;


class Road : 
    public synthese::util::Registrable<int,Road>, 
    public AddressablePlace,
    public Path
    

{
public:
	
  typedef enum { 
    ROAD_TYPE_UNKNOWN, 
    ROAD_TYPE_MOTORWAY, /* autoroute */
    ROAD_TYPE_MEDIAN_STRIPPED_ROAD,  /* route a chaussees separees (terre plein) */
    ROAD_TYPE_PRINCIPLE_AXIS, /* axe principal (au sens rue) */
    ROAD_TYPE_SECONDARY_AXIS, /* axe principal (au sens rue) */
    ROAD_TYPE_BRIDGE, /* pont */
    ROAD_TYPE_STREET, /* rue */
    ROAD_TYPE_PEDESTRIAN_STREET, /* rue pietonne */
    ROAD_TYPE_ACCESS_ROAD, /* bretelle */
    ROAD_TYPE_PRIVATE_WAY, /* voie privee */
    ROAD_TYPE_PEDESTRIAN_PATH, /* chemin pieton */
    ROAD_TYPE_TUNNEL, /* tunnel */
    ROAD_TYPE_HIGHWAY /* route secondaire */
  } RoadType;
    

private:
  
  const RoadType _type;
  const std::vector<const RoadChunk*> _chunks;

public:

  Road (const int& id,
	const std::string& name,
	const City* city,
        const RoadType& type,
	const std::vector<const RoadChunk*>& chunks);

  virtual ~Road();


  //! @name Getters/Setters
  //@{
  bool hasReservationRule () const;
  const ReservationRule* getReservationRule () const;

  int getEdgesCount () const;
  const Edge* getEdge (int index) const;

  const RoadType& getType () const;
  //@}
    

  //! @name Query methods.
  //@{
  //@}

};


}
}

#endif 
