#ifndef SYNTHESE_ENV_ROAD_H
#define SYNTHESE_ENV_ROAD_H

#include <string>

#include "01_util/Registrable.h"
#include "01_util/UId.h"

#include "AddressablePlace.h"
#include "Path.h"



namespace synthese
{
namespace env
{

class City;
class RoadChunk;


class Road : 
    public synthese::util::Registrable<uid,Road>, 
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
  
  RoadType _type;

public:

  Road (const uid& id,
	const std::string& name,
	const City* city,
        const RoadType& type);

  virtual ~Road();


  //! @name Getters/Setters
  //@{
  bool hasReservationRule () const;
  const ReservationRule* getReservationRule () const;

  const RoadType& getType () const;
  void setType (const RoadType& type);
  //@}
    

  //! @name Query methods.
  //@{
  //@}

  //! @name Update methods.
  //@{

  //@}

};


}
}

#endif 
