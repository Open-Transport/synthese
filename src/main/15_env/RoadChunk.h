#ifndef SYNTHESE_ENV_ROADCHUNK_H
#define SYNTHESE_ENV_ROADCHUNK_H


#include "Edge.h"



namespace synthese
{
namespace env
{

    class Address;
    class Road;



/** Association class between road and address.
A road chunk is always delimited by two adresses, with no other
address in between.
These delimiting addresses can correspond to :
- a crossing address
- a physical stop address
- a public place address

 @ingroup m15
*/
class RoadChunk : public Edge 
{
private:

    const Road* _road;
    const Address* _fromAddress;

public:

  RoadChunk (const Road* road,
	     const Address* fromAddress,
	     const EdgeType& type);
  
  virtual ~RoadChunk ();


  //! @name Getters/Setters
  //@{
  const Path* getParentPath () const;
  const Vertex* getFromVertex () const;
  double getMetricOffset () const;
  //@}
    

  //! @name Query methods.
  //@{
  //@}

};


}
}

#endif 
